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

typedef struct _eeprom eeprom;

eeprom * eeprom_new();
eeprom * eeprom_new_with_data(uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif //_EEPROM_H_
