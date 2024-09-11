/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
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

#define M110_FREQ_MIN         403.0
#define M110_FREQ_MAX         470.0

#define M110_RX_FI            21.4
#define M110_PLL_REF_STEP     6.25
#define M110_CH1_INDEX        0x1b
#define M110_CH2_INDEX        0x25
#define M110_PRESCALER        127
#define M110_TOT_INDEX        0x18
#define M110_REKEY_INDEX      0x19
#define M110_CHECKSUM_INDEX   0x0f

struct _m110_t {
   eeprom_t *eeprom;
};


m110_t * m110_new(void) {
  m110_t *m110 = M110(calloc(1, sizeof(m110_t)));
  m110->eeprom = eeprom_new();
  assert(m110 != NULL);
  return m110;
}

// FIXME: Needs more safety
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

void m110_destroy(m110_t *m110) {
   assert(m110 != NULL);

   if (m110->eeprom != NULL) {
      free(m110->eeprom);
   }

   free(m110);
}

eeprom_t * m110_eeprom_get(m110_t *m110) {
   assert(m110 != NULL);
   return m110->eeprom;
}

void m110_eeprom_set(m110_t *m110, eeprom_t *eeprom) {
   assert(m110 != NULL);
   if (m110->eeprom != NULL) {
      free(m110->eeprom);
   }

   m110->eeprom = eeprom;
}

void m110_eeprom_set_checksum(m110_t *m110, unsigned char checksum) {
   assert(m110 != NULL);
   assert(checksum >=0 && checksum <= 0xFF);

   eeprom_t *eeprom = m110_eeprom_get(m110);
   eeprom_write_byte(eeprom, M110_CHECKSUM_INDEX, checksum);
}

void m110_eeprom_regenerate_checksum(m110_t *m110) {
   unsigned int sum, iter, value;

   assert(m110 != NULL);

   sum = 0;
   m110_eeprom_set_checksum(m110, 0xff);
   eeprom_t *eeprom = m110_eeprom_get(m110);
   for (iter = 0; iter <= 0x30; iter++) {
      sum+=eeprom_read_byte(eeprom, iter);
   }
   
   value = sum & 0xff;
   m110_eeprom_set_checksum(m110, (0x100 - value) & 0xff);
}

void m110_channel_frequencies_set(m110_t *m110, unsigned char channel, double freq_mhz_rx, double freq_mhz_tx) {
   unsigned char index;
   
   assert(m110 != NULL);

   // VALIDATE FREQUENCIES
   if (freq_mhz_rx < M110_FREQ_MIN || freq_mhz_tx < M110_FREQ_MIN ||
       freq_mhz_rx > M110_FREQ_MAX || freq_mhz_tx > M110_FREQ_MAX) {
      return;
   }

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return;
   }

   double ft;
   double freq_khz_rx;
   double freq_khz_tx;
   unsigned int N,A;

   eeprom_t *eeprom = m110_eeprom_get(m110);

   freq_mhz_rx -= M110_RX_FI;
   freq_khz_rx = freq_mhz_rx * 1000;
   ft = freq_khz_rx / M110_PLL_REF_STEP;
   N = ((unsigned int) (ft / M110_PRESCALER));
   A = ((unsigned int) ft % M110_PRESCALER) << 1;
   eeprom_write_byte(eeprom, index+7, (eeprom_read_byte(eeprom, index+7) & 0xfc) + ((N >> 8) & 0x03));
   eeprom_write_byte(eeprom, index+8, N & 0xFF);
   eeprom_write_byte(eeprom, index+9, A);

   freq_khz_tx = freq_mhz_tx * 1000;
   ft = freq_khz_tx / M110_PLL_REF_STEP;
   N = ((unsigned int) (ft / M110_PRESCALER));
   A = ((unsigned int) ft % M110_PRESCALER) << 1;
   eeprom_write_byte(eeprom, index+2, (eeprom_read_byte(eeprom, index+2) & 0xfc) + ((N >> 8) & 0x03));
   eeprom_write_byte(eeprom, index+3, N & 0xFF);
   eeprom_write_byte(eeprom, index+4, A);

   m110_eeprom_regenerate_checksum(m110);
}

void m110_ctcss_rx_set(m110_t *m110, unsigned char channel, double ctcss) {
   eeprom_t *eeprom;
   unsigned char index;
   double tmp, decimal;
   unsigned int tone_int;

   assert(m110 != NULL);
   
   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return;
   }

   tmp = ctcss / 0.016365413;
   tone_int = (int) tmp;
   decimal = tmp - tone_int;
   if (decimal >= 0.5) {
      tone_int +=1;
   } 

   eeprom = m110_eeprom_get(m110);
   eeprom_write_byte(eeprom, index+5, ((tone_int) >> 8) & 0xff);
   eeprom_write_byte(eeprom, index+6, (tone_int & 0xff));
   m110_eeprom_regenerate_checksum(m110);
}


void m110_ctcss_tx_set(m110_t *m110, unsigned char channel, double ctcss) {
   eeprom_t *eeprom;
   unsigned char index;
   double tmp, decimal;
   unsigned int tone_int;
   
   assert(m110 != NULL);

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return;
   }

   tmp = ctcss / 0.125233645;
   tone_int = (int) tmp;
   decimal = tmp - tone_int;
   if (decimal >= 0.5) {
      tone_int +=1;
   } 

   eeprom = m110_eeprom_get(m110);
   eeprom_write_byte(eeprom, index, ((tone_int) >> 8) & 0xff);
   eeprom_write_byte(eeprom, index+1, (tone_int & 0xff));
   m110_eeprom_regenerate_checksum(m110);
}

unsigned char m110_timeout_get(m110_t *m110) {
   if (m110 == NULL) return 0;
   return (eeprom_read_byte(m110_eeprom_get(m110), M110_TOT_INDEX));
}

void m110_timeout_set(m110_t *m110, unsigned char time) {
   assert(m110 != NULL);
   /* Value is multiple of 5 seconds */
   if (!(time >= 0 && time <= 255)) return;

   eeprom_write_byte(m110_eeprom_get(m110), M110_TOT_INDEX, time);
   m110_eeprom_regenerate_checksum(m110);
}

unsigned char m110_rekey_get(m110_t *m110) {
   assert(m110 != NULL);
   return eeprom_read_byte(m110_eeprom_get(m110), M110_REKEY_INDEX);
}

void m110_rekey_set(m110_t *m110, unsigned char time) {
   assert(m110 != NULL);
   if (!(time >= 0 && time <= 0xff)) return;

   eeprom_write_byte(m110_eeprom_get(m110), M110_REKEY_INDEX, time);
   m110_eeprom_regenerate_checksum(m110);

}

unsigned char m110_channel_low_power_get(m110_t *m110, unsigned char channel) {
   // Labeled internally as clockshift but seems to also control the power - needs testing
   // Binary value, If active, then it's low power
   unsigned char index;

   assert(m110 != NULL);

   if (channel != 1 && channel != 2) return 2;

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return 2;
   }

   return (eeprom_read_byte(m110_eeprom_get(m110), index + 7) & 0x80) != 0;
}

void m110_channel_low_power_set(m110_t *m110, unsigned char channel, unsigned char low_power) {
   // Clockshift, if active is low power
   unsigned char index, value;

   assert(m110 != NULL);

   if (channel != 1 && channel != 2) return;

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return;
   }
   
   if (low_power == 0) {
      value = eeprom_read_byte(m110_eeprom_get(m110), index + 7);
      eeprom_write_byte(m110_eeprom_get(m110), index + 7, value & 0x7F);
   } else {
      value = eeprom_read_byte(m110_eeprom_get(m110), index + 7);
      eeprom_write_byte(m110_eeprom_get(m110), index + 7, value | 0x80);
   }
}

unsigned char m110_channel_monitor_enabled_get(m110_t *m110, unsigned char channel) {
   // Labeled internally as clockshift but seems to also control the power - needs testing
   // Binary value, If active, then it's low power
   unsigned char index;

   assert(m110 != NULL);

   if (channel != 1 && channel != 2) return 2;

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return 2;
   }

   return (eeprom_read_byte(m110_eeprom_get(m110), index + 7) & 0x08) != 0;
}

void m110_channel_monitor_enabled_set(m110_t *m110, unsigned char channel, unsigned char enabled) {
   // Clockshift, if active is low power
   unsigned char index, value;

   assert(m110 != NULL);

   if (channel != 1 && channel != 2) return;

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return;
   }
   
   if (enabled == 0) {
      value = eeprom_read_byte(m110_eeprom_get(m110), index + 7);
      eeprom_write_byte(m110_eeprom_get(m110), index + 7, value & 0xF7);
   } else {
      value = eeprom_read_byte(m110_eeprom_get(m110), index + 7);
      eeprom_write_byte(m110_eeprom_get(m110), index + 7, value | 0x08);
   }
}

tx_admit_t m110_tx_admit_get(m110_t *m110, unsigned char channel) {
   unsigned char index;

   assert(m110 != NULL);

   if (channel != 1 && channel != 2) return TXADMIT_MONITOR;

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return TXADMIT_MONITOR;
   }

   return (((eeprom_read_byte(m110_eeprom_get(m110), index + 2)) >> 3) & 0x7);
}

void m110_tx_admit_set(m110_t *m110, unsigned char channel, tx_admit_t txadmit) {
   unsigned char index;

   assert(m110 != NULL);
   if (channel != 1 && channel != 2) return;
   if (txadmit != TXADMIT_MONITOR && txadmit != TXADMIT_ALWAYS &&
       txadmit != TXADMIT_NO_CARRIER && txadmit != TXADMIT_PL_NO_CARRIER) return;

   if (channel == 1) {   
      index = M110_CH1_INDEX;
   } else if (channel = 2) {
      index = M110_CH2_INDEX;
   } else {
      return;
   }

   unsigned char fbits = eeprom_read_byte(m110_eeprom_get(m110), index + 2);
   fbits &= 0x03;
   eeprom_write_byte(m110_eeprom_get(m110), index + 2, 0xc4 + (txadmit << 3) + fbits);
}
