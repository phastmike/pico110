/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_intro.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_intro.h"
#include "view_controller.h"

struct _vc_intro_t {
   view_controller_t *vc;
};

void vc_intro_present(view_controller_t *vc);

vc_intro_t *vc_intro_new(hmi_t *hmi, radio_t *radio) {
   vc_intro_t *vc_intro = (vc_intro_t *) view_controller_new(hmi, radio);
   VIEW_CONTROLLER(vc_intro)->present = vc_intro_present;

   return vc_intro;
}

void vc_intro_show(view_controller_t *vc) {
   assert(vc != NULL);
   assert(vc->hmi != NULL);

   hmi_display_text(vc->hmi, 0, " RADIUS ");
   for(int j = 0; j < HMI_NUMBER_OF_KEYS; j++) {
      hmi_leds_set(vc->hmi, 1 << j);
      sleep_ms(80);
   }

   hmi_display_text(vc->hmi, 0, "PICO 110");
   for(int k = 0; k < HMI_NUMBER_OF_KEYS; k++) {
      hmi_leds_set(vc->hmi, 0x80 >> k);
      sleep_ms(80);
   }

   hmi_leds_set(vc->hmi, 0);

   hmi_display_text(vc->hmi, 0, " CT1ENQ ");
   sleep_ms(1000);
}

/* EVENTS */

void vc_intro_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //radio_timeout_down(VIEW_CONTROLLER(user_data)->radio);
   vc_intro_show(VIEW_CONTROLLER(user_data));
}

void vc_intro_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //radio_timeout_up(VIEW_CONTROLLER(user_data)->radio);
   vc_intro_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_intro_present(view_controller_t *vc) {
   //hmi_key_t *key = hmi_get_key(vc->hmi, HMI_KEY_7);
   //hmi_key_on_press_event_connect(key, vc_intro_on_press_down_event, vc);
   //key = hmi_get_key(vc->hmi, HMI_KEY_8);
   //hmi_key_on_press_event_connect(key, vc_intro_on_press_up_event, vc);
   vc_intro_show(vc);
}
