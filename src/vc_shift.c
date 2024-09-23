/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_shift.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_shift.h"
#include "view_controller.h"

struct _vc_shift_t {
   view_controller_t vc;
};

void vc_shift_show(view_controller_t *vc);
void vc_shift_present(view_controller_t *vc);

vc_shift_t *vc_shift_new(hmi_t *hmi, radio_t *radio) {
   vc_shift_t *vc_shift = (vc_shift_t *) calloc(1, sizeof(vc_shift_t)); 
   view_controller_init(VIEW_CONTROLLER(vc_shift), hmi, radio);
   VIEW_CONTROLLER(vc_shift)->show = vc_shift_show;
   VIEW_CONTROLLER(vc_shift)->present = vc_shift_present;
   return vc_shift;
}

void vc_shift_show(view_controller_t *vc) {
   assert(vc != NULL);
   char *string = (char *) calloc (1,9);
   double shift = radio_channel_shift_get(radio_get_active_channel(vc->radio)); 
   sprintf(string, "SFT %5.01f", shift);
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_shift_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   double shift = radio_channel_shift_get(radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio)); 
   shift -= 0.1;
   radio_channel_shift_set(radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio), shift);
   vc_shift_show(VIEW_CONTROLLER(user_data));
}

void vc_shift_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   double shift = radio_channel_shift_get(radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio)); 
   shift += 0.1;
   radio_channel_shift_set(radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio), shift);
   vc_shift_show(VIEW_CONTROLLER(user_data));
}


void vc_shift_on_press_generic_exit(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   view_controller_t *vc = VIEW_CONTROLLER(user_data);
   if (vc->exit_with_key) vc->exit_with_key(vc, key);
}

/* VIEW CONTROLLER present method */

void vc_shift_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_shift_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_shift_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_shift_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_shift_on_press_up_event, vc);

   vc_shift_show(vc);
}
