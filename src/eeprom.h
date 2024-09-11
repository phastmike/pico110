/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * eeprom.h
 * 
 * simple eeprom emulation. behaviour done in i2c ISR 
 * 
 *
 * José Miguel Fonte
 */


#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <pico/stdlib.h> 

#define EEPROM(x) ((eeprom_t *) x)

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _eeprom_t eeprom_t;

eeprom_t *     eeprom_new();
eeprom_t *     eeprom_new_with_data(unsigned char *data);

void           eeprom_addr_set(eeprom_t *eeprom, unsigned int addr);
unsigned int   eeprom_addr_get(eeprom_t *eeprom);
unsigned char  eeprom_read_byte(eeprom_t *eeprom, unsigned int addr);
void           eeprom_write_byte(eeprom_t *eeprom, unsigned int addr, unsigned char value);
unsigned char  eeprom_serial_read_byte(eeprom_t *eeprom);
void           eeprom_serial_write_byte(eeprom_t *eeprom, unsigned char value);

#ifdef __cplusplus
}
#endif

#endif //_EEPROM_H_
