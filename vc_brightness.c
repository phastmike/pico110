/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_brightness.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_brightness.h"
#include "view_controller.h"

struct _vc_brightness_t {
   view_controller_t vc;
};

void vc_brightness_present(view_controller_t *vc);

vc_brightness_t *vc_brightness_new(hmi_t *hmi, radio_t *radio) {
   vc_brightness_t *vc_brightness = (vc_brightness_t *) calloc(1, sizeof(vc_brightness_t));
   VIEW_CONTROLLER(vc_brightness)->hmi = hmi;
   VIEW_CONTROLLER(vc_brightness)->radio = radio;
   VIEW_CONTROLLER(vc_brightness)->present = vc_brightness_present;
   return vc_brightness;
}

void vc_brightness_show(view_controller_t *vc) {
   assert(vc != NULL);
   char *string = (char *) calloc (1,9);
   unsigned char brightness = hmi_display_get_brightness(VIEW_CONTROLLER(vc)->hmi);
   sprintf(string, "Bright %d", brightness);
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_brightness_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   unsigned char brightness = hmi_display_get_brightness(VIEW_CONTROLLER(user_data)->hmi);
   hmi_display_set_brightness(HMI(VIEW_CONTROLLER(user_data)->hmi), brightness - 1);  
   vc_brightness_show(VIEW_CONTROLLER(user_data));
}

void vc_brightness_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   unsigned char brightness = hmi_display_get_brightness(VIEW_CONTROLLER(user_data)->hmi);
   hmi_display_set_brightness(HMI(VIEW_CONTROLLER(user_data)->hmi), brightness + 1);  
   vc_brightness_show(VIEW_CONTROLLER(user_data));
}

void vc_brightness_on_press_scan_back(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   view_controller_t *vc = VIEW_CONTROLLER(user_data);
   if (vc->exit_with_key) vc->exit_with_key(vc, key);
}

void vc_brightness_on_press_fmenu(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   view_controller_t *vc = VIEW_CONTROLLER(user_data);
   if (vc->exit_with_key) vc->exit_with_key(vc, key);
}

/* VIEW CONTROLLER present method */

void vc_brightness_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_led_set(vc->hmi, HMI_LED_FMENU, HMI_LED_ON);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_press_event_connect(key, vc_brightness_on_press_fmenu, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_press_event_connect(key, vc_brightness_on_press_scan_back, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_brightness_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_brightness_on_press_up_event, vc);

   vc_brightness_show(vc);
}
