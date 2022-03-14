/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_rekey.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_rekey.h"
#include "view_controller.h"

struct _vc_rekey_t {
   view_controller_t *vc;
};

void vc_rekey_present(view_controller_t *vc);

vc_rekey_t *vc_rekey_new(hmi_t *hmi, radio_t *radio) {
   vc_rekey_t *vc_rekey = (vc_rekey_t *) calloc (1, sizeof(vc_rekey_t));
   vc_rekey = (vc_rekey_t *) view_controller_new(hmi, radio);
   VIEW_CONTROLLER(vc_rekey)->present = vc_rekey_present;

   return vc_rekey;
}

void vc_rekey_show(view_controller_t *vc) {
   char *string = (char *) calloc (1,9);
   unsigned char rekey = m110_rekey_get(radio_get_m110(vc->radio));
   sprintf(string, "Pen %4d", rekey);
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_rekey_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_rekey_down(VIEW_CONTROLLER(user_data)->radio);
   vc_rekey_show(VIEW_CONTROLLER(user_data));
}

void vc_rekey_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_rekey_up(VIEW_CONTROLLER(user_data)->radio);
   vc_rekey_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_rekey_present(view_controller_t *vc) {
   hmi_key_t *key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_rekey_on_press_down_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_rekey_on_press_up_event, vc);
   vc_rekey_show(vc);
}
