/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_enc.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_enc.h"
#include "view_controller.h"

struct _vc_enc_t {
   view_controller_t vc;
};

void vc_enc_show(view_controller_t *vc);
void vc_enc_present(view_controller_t *vc);

vc_enc_t *vc_enc_new(hmi_t *hmi, radio_t *radio) {
   vc_enc_t *vc_enc = (vc_enc_t *) calloc(1, sizeof(vc_enc_t));
   view_controller_init(VIEW_CONTROLLER(vc_enc), hmi, radio);
   VIEW_CONTROLLER(vc_enc)->show = vc_enc_show;
   VIEW_CONTROLLER(vc_enc)->present = vc_enc_present;
   return vc_enc;
}

void vc_enc_show(view_controller_t *vc) {
   assert(vc != NULL);
   ctcss_t * enc = radio_channel_ctcss_tx_get(radio_get_active_channel(vc->radio));
   char *string = (char *) calloc (1,9);
   sprintf(string, "Enc %s", ctcss_get_as_string(enc));
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_enc_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   ctcss_prev(radio_channel_ctcss_tx_get(rc));
   radio_set_active_channel(VIEW_CONTROLLER(user_data)->radio, rc);
   vc_enc_show(VIEW_CONTROLLER(user_data));
}

void vc_enc_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   ctcss_next(radio_channel_ctcss_tx_get(rc));
   radio_set_active_channel(VIEW_CONTROLLER(user_data)->radio, rc);
   vc_enc_show(VIEW_CONTROLLER(user_data));
}

void vc_enc_on_press_generic_exit(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   view_controller_t *vc = VIEW_CONTROLLER(user_data);
   if (vc->exit_with_key) vc->exit_with_key(vc, key);
}

/* VIEW CONTROLLER present method */

void vc_enc_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_enc_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_enc_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_enc_on_press_down_event, vc);
   
   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_enc_on_press_up_event, vc);

   vc_enc_show(vc);
}
