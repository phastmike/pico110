/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * hni.h
 * 
 * Human Machine Interface 
 * 
 *
 * José Miguel Fonte
 */

#ifndef _HMI_H_
#define _HMI_H_

#include "key.h"

#define HMI_NUMBER_OF_KEYS    8 
#define HMI(x)                ((hmi_t *) x)

#define HMI_LED_FMENU         0
#define HMI_LED_SCAN          1
#define HMI_LED_MR            2
#define HMI_LED_LOW           3
#define HMI_LED_DEC           4
#define HMI_LED_DUP           5
#define HMI_LED_SQ            6
#define HMI_LED_RTX           7

#ifdef __cplusplus
extern "C" {
#endif

typedef struct    _hmi_t hmi_t;

hmi_t *           hmi_new(void);
void              hmi_destroy(hmi_t *hmi);
unsigned char     hmi_display_get_enabled(hmi_t *hmi);
void              hmi_display_set_enabled(hmi_t *hmi, unsigned char enabled);
unsigned char     hmi_display_get_brightness(hmi_t *hmi);
void              hmi_display_set_brightness(hmi_t *hmi, unsigned char brightness);
void              hmi_display_text(hmi_t *hmi, unsigned char pos, char *text);
void              hmi_display_text_scroll(hmi_t *hmi, char *text, unsigned char speed, unsigned char direction);
void              hmi_display_text_clear(hmi_t *hmi);
unsigned char     hmi_keys_scan(hmi_t *hmi);
void              hmi_led_set(hmi_t *hmi, unsigned char pos, unsigned char status);
void              hmi_leds_set(hmi_t *hmi, unsigned char status);
hmi_key_t *       hmi_get_key(hmi_t *hmi, hmi_key_id_t id);

#ifdef __cplusplus
}
#endif

#endif //_HMI_H_
