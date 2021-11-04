/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * m110.h
 * 
 * Routines for m110 eeprom emulation.
 * 
 *
 * José Miguel Fonte
 */

#ifndef _M110_H_
#define _M110_H_

#include "eeprom.h"
//#include <pico/stdlib.h>
//#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _m110_t m110_t;

m110_t      *  m110_new(void);
m110_t      *  m110_new_with_data(unsigned char *data);
m110_t      *  m110_new_with_eeprom(eeprom_t *eeprom);

eeprom_t    *  m110_eeprom_get(m110_t *m110);
void           m110_eeprom_set(m110_t *m110, eeprom_t *eeprom);
void           m110_channel_frequencies_set(m110_t *m110, unsigned char channel, double freq_mhz_rx, double freq_mhz_tx);
void           m110_ctcss_rx_set(m110_t *m110, unsigned char channel, double ctcss);
void           m110_ctcss_tx_set(m110_t *m110, unsigned char channel, double ctcss);
unsigned int   m110_timeout_get(m110_t *m110);
void           m110_timeout_set(m110_t *m110, unsigned char time_in_multiples_of_5_seconds);
unsigned int   m110_rekey_get(m110_t *m110);
void           m110_rekey_set(m110_t *m110, unsigned char time_in_seconds);
unsigned char  m110_channel_low_power_get(m110_t *m110, unsigned char channel);
void           m110_channel_low_power_set(m110_t *m110, unsigned char channel, unsigned char low_power);
unsigned char  m110_channel_monitor_enabled_get(m110_t *m110, unsigned char channel);
void           m110_channel_monitor_enabled_set(m110_t *m110, unsigned char channel, unsigned char enabled);

#ifdef __cplusplus
}
#endif

#endif //_M110_H_
