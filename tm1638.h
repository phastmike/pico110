/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * tm1638.h
 * 
 * TM1638 LED+KEY Module
 * 
 *
 * José Miguel Fonte
 */

#ifndef _TM1638_H_
#define _TM1638_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _tm1638_t tm1638_t;

tm1638_t      *tm1638_new(unsigned char stb, unsigned char clk, unsigned char dio, unsigned char brightness);
void           tm1638_destroy(tm1638_t *tm1638);
void           tm1638_clear(tm1638_t *tm1638);
unsigned char  tm1638_power_get(tm1638_t *tm1638);
void           tm1638_power_set(tm1638_t *tm1638, unsigned char onoff);
unsigned char  tm1638_brightness_get(tm1638_t *tm1638);
void           tm1638_brightness_set(tm1638_t *tm1638, unsigned char brightness);
void           tm1638_led(tm1638_t *tm1638, unsigned char pos, unsigned char value);
void           tm1638_leds(tm1638_t *tm1638, unsigned char value);
unsigned int   tm1638_keys(tm1638_t *tm1638);
void           tm1638_show(tm1638_t *tm1638, unsigned char *string, unsigned char pos);

#ifdef __cplusplus
}
#endif

#endif //_TM1638_H_
