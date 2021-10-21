/*
 *
 *
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <pico/stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _eeprom_t eeprom_t;

eeprom_t * eeprom_new();
eeprom_t * eeprom_new_with_data(uint8_t *data);

void       eeprom_addr_set(eeprom_t *eeprom, uint16_t addr);
uint8_t	  eeprom_read_byte(eeprom_t *eeprom);
void       eeprom_write_byte(eeprom_t *eeprom, uint8_t value);

#ifdef __cplusplus
}
#endif

#endif //_EEPROM_H_
