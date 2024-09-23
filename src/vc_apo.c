/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_apo.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_apo.h"
#include "view_controller.h"

struct _vc_apo_t {
   view_controller_t vc;
};

// delegates
void vc_apo_show(view_controller_t *vc);
void vc_apo_present(view_controller_t *vc);

vc_apo_t *vc_apo_new(hmi_t *hmi, radio_t *radio) {
   vc_apo_t *this = (vc_apo_t *) calloc (1, sizeof(vc_apo_t));
   view_controller_init(VIEW_CONTROLLER(this), hmi, radio);
   VIEW_CONTROLLER(this)->show = vc_apo_show;
   VIEW_CONTROLLER(this)->present = vc_apo_present;
   return this;
}

void vc_apo_show(view_controller_t *vc) {
   assert(vc != NULL);
   char *string = (char *) calloc (1,9);
   sprintf(string, "Scr   On");
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_apo_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   hmi_display_set_enabled(VIEW_CONTROLLER(user_data)->hmi, !hmi_display_get_enabled(VIEW_CONTROLLER(user_data)->hmi));
}

void vc_apo_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   hmi_display_set_enabled(VIEW_CONTROLLER(user_data)->hmi, !hmi_display_get_enabled(VIEW_CONTROLLER(user_data)->hmi));
}

void vc_apo_on_press_generic_exit(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   hmi_t *hmi = HMI(VIEW_CONTROLLER(user_data)->hmi);

   if (!hmi_display_get_enabled(hmi)) return;

   view_controller_t *vc = VIEW_CONTROLLER(user_data);
   if (vc->exit_with_key) vc->exit_with_key(vc, key);
}

/* VIEW CONTROLLER present method */

void vc_apo_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_apo_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_apo_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_apo_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_apo_on_press_up_event, vc);

   vc_apo_show(vc);
}
