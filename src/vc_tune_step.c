/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_tune_step.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_tune_step.h"
#include "view_controller.h"

struct _vc_tune_step_t {
   view_controller_t vc;
};

// delegates
void vc_tune_step_show(view_controller_t *vc);
void vc_tune_step_present(view_controller_t *vc);

vc_tune_step_t *vc_tune_step_new(hmi_t *hmi, radio_t *radio) {
   vc_tune_step_t *vc_tune_step = (vc_tune_step_t *) calloc(1, sizeof(vc_tune_step_t)); 
   view_controller_init(VIEW_CONTROLLER(vc_tune_step), hmi, radio);
   VIEW_CONTROLLER(vc_tune_step)->show = vc_tune_step_show;
   VIEW_CONTROLLER(vc_tune_step)->present = vc_tune_step_present;
   return vc_tune_step;
}

void vc_tune_step_show(view_controller_t *vc) {
   assert(vc != NULL);
   char *str_ts;
   char *string = (char *) calloc (1,9);
   str_ts = radio_tune_step_get(vc->radio); 
   sprintf(string, "TS  %s",str_ts);
   hmi_display_text(vc->hmi, 0, string);
   free(string);
   free(str_ts);
}

/* EVENTS */

void vc_tune_step_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_tune_step_down(VIEW_CONTROLLER(user_data)->radio);
}

void vc_tune_step_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_tune_step_up(VIEW_CONTROLLER(user_data)->radio);
}

void vc_tune_step_on_press_generic_exit(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   hmi_keys_disconnect(VIEW_CONTROLLER(user_data)->hmi);
   view_controller_leave(VIEW_CONTROLLER(user_data), key);
}

/* VIEW CONTROLLER present method */

void vc_tune_step_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_tune_step_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_tune_step_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_3);
   hmi_key_on_press_event_disconnect(key);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_tune_step_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_tune_step_on_press_up_event, vc);

   vc_tune_step_show(vc);
}
