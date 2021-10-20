/*
 *
 *
 */

#include "eeprom.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define EEPROM_SIZE 128

struct _eeprom {
   uint16_t addr;
   uint8_t mem[EEPROM_SIZE];
};

eeprom * eeprom_new(void) {
   return (eeprom *) calloc(1, sizeof(eeprom));
}

// DANGEROUS
eeprom * eeprom_new_with_data(uint8_t *data) {
   eeprom *retval;
   retval = (eeprom *) calloc(1, sizeof(eeprom));
   memcpy(retval, data, EEPROM_SIZE);

   return retval;
}
