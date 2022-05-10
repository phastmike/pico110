/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_dec.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_dec.h"
#include "view_controller.h"

struct _vc_dec_t {
   view_controller_t *vc;
};

void vc_dec_present(view_controller_t *vc);

vc_dec_t *vc_dec_new(hmi_t *hmi, radio_t *radio) {
   vc_dec_t *vc_dec = (vc_dec_t *) view_controller_new(hmi, radio);
   VIEW_CONTROLLER(vc_dec)->present = vc_dec_present;
   return vc_dec;
}

void vc_dec_show(view_controller_t *vc) {
   ctcss_t * dec = radio_channel_ctcss_rx_get(radio_get_active_channel(vc->radio));
   char *string = (char *) calloc (1,9);
   sprintf(string, "Dec %s", ctcss_get_as_string(dec));
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_dec_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   ctcss_prev(radio_channel_ctcss_rx_get(rc));
   radio_set_active_channel(VIEW_CONTROLLER(user_data)->radio, rc);
   vc_dec_show(VIEW_CONTROLLER(user_data));
}

void vc_dec_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   ctcss_next(radio_channel_ctcss_rx_get(rc));
   radio_set_active_channel(VIEW_CONTROLLER(user_data)->radio, rc);
   vc_dec_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_dec_present(view_controller_t *vc) {
   hmi_key_t *key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_dec_on_press_down_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_dec_on_press_up_event, vc);
   vc_dec_show(vc);
}
