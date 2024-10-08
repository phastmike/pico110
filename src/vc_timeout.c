/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_timeout.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_timeout.h"
#include "view_controller.h"

struct _vc_timeout_t {
   view_controller_t vc;
};

void vc_timeout_show(view_controller_t *vc);
void vc_timeout_present(view_controller_t *vc);

vc_timeout_t *vc_timeout_new(hmi_t *hmi, radio_t *radio) {
   vc_timeout_t *vc_timeout = (vc_timeout_t *) calloc(1, sizeof(vc_timeout_t)); 
   view_controller_init(VIEW_CONTROLLER(vc_timeout), hmi, radio);
   VIEW_CONTROLLER(vc_timeout)->show = vc_timeout_show;
   VIEW_CONTROLLER(vc_timeout)->present = vc_timeout_present;
   return vc_timeout;
}

void vc_timeout_show(view_controller_t *vc) {
   assert(vc != NULL);
   char *string = (char *) calloc (1,9);
   unsigned char tot = m110_timeout_get(M110(vc->radio));
   sprintf(string, "ToT %4d",tot * 5);
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_timeout_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_timeout_down(VIEW_CONTROLLER(user_data)->radio);
}

void vc_timeout_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_timeout_up(VIEW_CONTROLLER(user_data)->radio);
}

void vc_timeout_on_press_generic_exit(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   hmi_keys_disconnect(VIEW_CONTROLLER(user_data)->hmi);
   view_controller_leave(VIEW_CONTROLLER(user_data), key);
}

/* VIEW CONTROLLER present method */

void vc_timeout_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_timeout_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_timeout_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_timeout_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_timeout_on_press_up_event, vc);
}
