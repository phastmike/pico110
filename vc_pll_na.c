/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_pll_na.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "eeprom.h"
#include "vc_pll_na.h"
#include "view_controller.h"

struct _vc_pll_na_t {
   view_controller_t *vc;
};

void vc_pll_na_present(view_controller_t *vc);

vc_pll_na_t *vc_pll_na_new(hmi_t *hmi, radio_t *radio) {
   vc_pll_na_t *vc_pll_na = (vc_pll_na_t *) view_controller_new(hmi, radio);
   VIEW_CONTROLLER(vc_pll_na)->present = vc_pll_na_present;
   return vc_pll_na;
}

void vc_pll_na_show(view_controller_t *vc) {
   assert (vc != NULL);
   unsigned char b1 = eeprom_raw_read_byte(m110_eeprom_get(radio_get_m110(vc->radio)), 0x22);
   unsigned char b2 = eeprom_raw_read_byte(m110_eeprom_get(radio_get_m110(vc->radio)), 0x23);
   unsigned char b3 = eeprom_raw_read_byte(m110_eeprom_get(radio_get_m110(vc->radio)), 0x24);
   unsigned int N = ((b1 & 2) << 8) + b2;
   unsigned int A = b3 >> 1;
   unsigned char *str = calloc(1, 8);
   sprintf(str, "N.%3dA.%3d",N,A);
   hmi_display_text(vc->hmi, 0, str);
   free(str);
}

/* EVENTS - key commands can be removed */

void vc_pll_na_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   vc_pll_na_show(VIEW_CONTROLLER(user_data));
}

void vc_pll_na_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   vc_pll_na_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_pll_na_present(view_controller_t *vc) {
   assert (vc != NULL);
   vc_pll_na_show(vc);
}
