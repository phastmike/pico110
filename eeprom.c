/*
 *
 *
 */

#include "eeprom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define EEPROM_SIZE 128

struct _eeprom_t {
   uint16_t addr;
   uint8_t mem[EEPROM_SIZE];
};

eeprom_t * eeprom_new(void) {
   eeprom_t *eeprom = (eeprom_t *) calloc(1, sizeof(eeprom_t));
   eeprom->addr = 0;
   return eeprom;
}

// DANGEROUS
eeprom_t * eeprom_new_with_data(uint8_t *data) {
   assert(data != NULL); // eventually return NULL

   eeprom_t *eeprom = eeprom_new();;
   memcpy(eeprom->mem, data, EEPROM_SIZE);

   return eeprom;
}

uint16_t eeprom_addr_get(eeprom_t *eeprom) {
   return eeprom->addr;
}

void eeprom_addr_set(eeprom_t *eeprom, uint16_t addr) {
   assert(addr >= 0 && addr < EEPROM_SIZE);

   eeprom->addr = addr;
}

void eeprom_addr_inc(eeprom_t *eeprom) {
   assert(eeprom != NULL);

   uint16_t addr = eeprom_addr_get(eeprom);
   addr++;

   if (addr > EEPROM_SIZE) {
      eeprom_addr_set(eeprom, 0);
   } else {
      eeprom_addr_set(eeprom, addr);
   }
}

uint8_t eeprom_read_byte(eeprom_t *eeprom) {
   assert(eeprom != NULL);

   uint8_t value;

   value = eeprom->mem[eeprom_addr_get(eeprom)];
   eeprom_addr_inc(eeprom);
   
   return value;
}

void eeprom_write_byte(eeprom_t *eeprom, uint8_t value) {
   assert(eeprom != NULL);
   assert(value >= 0 && value < EEPROM_SIZE);

   eeprom->mem[eeprom_addr_get(eeprom)] = value;
   eeprom_addr_inc(eeprom);
}
