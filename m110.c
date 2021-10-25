/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * eeprom.c
 * 
 * simple eeprom emulation. behaviour done in i2c ISR 
 * 
 *
 * José Miguel Fonte
 */

#include "m110.h"
#include <stdlib.h>
#include <string.h>

//#include <stdint.h>
//#include <assert.h>
//#include <stdio.h>

struct _m110_t {
   eeprom_t *eeprom;

   float RX_FI;
   float PLL_REF_STEP;
   unsigned char CH1_INDEX;
   unsigned char CH2_INDEX;
   unsigned char PRESCALER;
   unsigned char TOT_INDEX;
   unsigned char REKEY_INDEX;
   unsigned char CHECKSUM_INDEX;
};


m110_t * m110_new(void) {
  m110_t *m110 = (m110_t *) calloc(1, sizeof(m110_t));
  m110->eeprom = eeprom_new();
  m110->RX_FI = 21.4;
  m110->PLL_REF_STEP = 6.25; // kHz
  m110->CH1_INDEX = 0x1b;
  m110->CH2_INDEX = 0x25;
  m110->PRESCALER = 127;
  m110->TOT_INDEX = 0x18;
  m110->REKEY_INDEX = 0x19;
  m110->CHECKSUM_INDEX = 0x0f;
  
  return m110;
}

m110_t * m110_new_with_data(unsigned char *data) {
   assert(data != NULL);

   m110_t *m110 = m110_new();
   assert(m110 != NULL);

   assert(m110->eeprom != NULL);
   free(m110->eeprom);

   m110->eeprom = eeprom_new_with_data(data);

   return m110;
}

m110_t * m110_new_with_eeprom(eeprom_t *eeprom) {
   assert(eeprom != NULL);
   m110_t *m110 = m110_new();
   m110_eeprom_set(m110, eeprom);

   return m110;
}

eeprom_t * m110_eeprom_get(m110_t *m110) {
   return m110->eeprom;
}

void m110_eeprom_set(m110_t *m110, eeprom_t *eeprom) {
   if (m110->eeprom != NULL) {
      free(m110->eeprom);
   }

   m110->eeprom = eeprom;
}

void m110_eeprom_set_checksum(m110_t *m110, unsigned char checksum) {
   assert(checksum >=0 && checksum <= 0xFF);

   eeprom_t *eeprom = m110_eeprom_get(m110);
   eeprom_raw_write_byte(eeprom, m110->CHECKSUM_INDEX, checksum);
}

void m110_eeprom_regenerate_checksum(m110_t *m110) {
   unsigned int sum, iter, value;
   //unsigned char iter;

   sum = 0;
   m110_eeprom_set_checksum(m110, 0xff);
   eeprom_t *eeprom = m110_eeprom_get(m110);
   for (iter = 0; iter <= 0x30; iter++) {
      sum+=eeprom_raw_read_byte(eeprom, iter);
   }
   
   value = sum & 0xff;
   m110_eeprom_set_checksum(m110, (0x100 - value) & 0xff);
}


void m110_channel_frequencies_set(m110_t *m110, unsigned char channel, double freq_mhz_rx, double freq_mhz_tx) {
   unsigned char index;
   
   if (channel == 1) {   
      index = m110->CH1_INDEX;
   } else if (channel = 2) {
      index = m110->CH2_INDEX;
   } else {
      return;
   }

   double ft;
   double freq_khz_rx;
   double freq_khz_tx;
   unsigned int N,A;

   eeprom_t *eeprom = m110_eeprom_get(m110);

   freq_mhz_rx -= 21.4;
   freq_khz_rx = freq_mhz_rx * 1000;
   ft = freq_khz_rx / m110->PLL_REF_STEP;
   N = ((unsigned int) (ft / m110->PRESCALER));
   A = ((unsigned int) ft % m110->PRESCALER) << 1;
   eeprom_raw_write_byte(eeprom, index+7, (eeprom_raw_read_byte(eeprom, index+7) & 0xfc) + ((N >> 8) & 0x03));
   eeprom_raw_write_byte(eeprom, index+8, N & 0xFF);
   eeprom_raw_write_byte(eeprom, index+9, A);

   freq_khz_tx = freq_mhz_tx * 1000;
   ft = freq_khz_tx / m110->PLL_REF_STEP;
   N = ((unsigned int) (ft / m110->PRESCALER));
   A = ((unsigned int) ft % m110->PRESCALER) << 1;
   eeprom_raw_write_byte(eeprom, index+2, (eeprom_raw_read_byte(eeprom, index+2) & 0xfc) + ((N >> 8) & 0x03));
   eeprom_raw_write_byte(eeprom, index+3, N & 0xFF);
   eeprom_raw_write_byte(eeprom, index+4, A);

   m110_eeprom_regenerate_checksum(m110);
}
