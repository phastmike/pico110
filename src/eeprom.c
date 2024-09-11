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

#include <stdlib.h>
#include <string.h>
#include "eeprom.h"


eeprom_t * eeprom_new(void) {
   eeprom_t *eeprom = (eeprom_t *) calloc(1, sizeof(eeprom_t));
   assert(eeprom != NULL);
   return eeprom;
}

eeprom_t * eeprom_new_with_data(unsigned char *data) {
   assert(data != NULL);
   eeprom_t *eeprom = eeprom_new();;
   assert(eeprom != NULL);
   eeprom_init(eeprom, data);
   return eeprom;
}

void eeprom_destroy(eeprom_t *eeprom) {
   assert(eeprom != NULL);
   free(eeprom);
}

// FIXME: Needs a fix if more generic
void eeprom_init(eeprom_t *eeprom, unsigned char *data) {
   assert(eeprom != NULL);
   eeprom_addr_set(eeprom, 0);
   
   // DANGEROUS : data must be of size EEPROM_SIZE
   if (data != NULL) {
      memcpy(eeprom->mem, data, EEPROM_SIZE);
   }
}

unsigned int eeprom_addr_get(eeprom_t *eeprom) {
   return eeprom->addr;
}

void eeprom_addr_set(eeprom_t *eeprom, unsigned int addr) {
   assert(addr >= 0 && addr < EEPROM_SIZE);

   eeprom->addr = addr;
}

void eeprom_addr_inc(eeprom_t *eeprom) {
   assert(eeprom != NULL);

   unsigned int addr = eeprom_addr_get(eeprom);

   addr++;

   if (addr >= EEPROM_SIZE) {
      eeprom_addr_set(eeprom, 0);
   } else {
      eeprom_addr_set(eeprom, addr);
   }
}

unsigned char eeprom_read_byte(eeprom_t *eeprom, unsigned int addr) {
   assert(addr >= 0 && addr < EEPROM_SIZE);
   return eeprom->mem[addr];
} 

void eeprom_write_byte(eeprom_t *eeprom, unsigned int addr, unsigned char value) {
   assert(addr >= 0 && addr < EEPROM_SIZE);
   assert(value >= 0 && value <= 0xFF);
   eeprom->mem[addr] = value;
} 

unsigned char eeprom_serial_read_byte(eeprom_t *eeprom) {
   assert(eeprom != NULL);

   unsigned char value;

   value = eeprom->mem[eeprom_addr_get(eeprom)];
   eeprom_addr_inc(eeprom);
   
   return value;
}

void eeprom_serial_write_byte(eeprom_t *eeprom, unsigned char value) {
   assert(eeprom != NULL);
   assert(value >= 0 && value <= 0xFF);

   eeprom->mem[eeprom_addr_get(eeprom)] = value;
   eeprom_addr_inc(eeprom);
}
