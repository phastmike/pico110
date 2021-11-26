/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_dup.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "key.h"
#include "vc_dup.h"
#include "view_controller.h"
#include "radio_channel.h"

struct _vc_dup_t {
   view_controller_t *vc;
   dup_t dup;
};

void vc_dup_present(view_controller_t *vc);

vc_dup_t *vc_dup_new(hmi_t *hmi, radio_t *radio) {
   vc_dup_t *vc_dup = (vc_dup_t *) calloc (1, sizeof(vc_dup_t));
   vc_dup = (vc_dup_t *) view_controller_new(hmi, radio);
   vc_dup->dup = radio_channel_dup_get(radio_get_active_channel(radio));
   VIEW_CONTROLLER(vc_dup)->present = vc_dup_present;

   return vc_dup;
}

void vc_dup_show(view_controller_t *vc) {
   char *string = (char *) calloc (1,9);
   switch (((vc_dup_t *) vc)->dup) {
      case DUP_DOWN:
         sprintf(string, "DUP    -");
         break;
      case DUP_OFF:
         sprintf(string, "DUP  OFF");
         break;
      case DUP_UP:
         sprintf(string, "DUP   UP");
   }
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_dup_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_timeout_down(VIEW_CONTROLLER(user_data)->radio);
   vc_dup_show(VIEW_CONTROLLER(user_data));
}

void vc_dup_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_timeout_up(VIEW_CONTROLLER(user_data)->radio);
   vc_dup_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_dup_present(view_controller_t *vc) {
   hmi_key_t *key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_dup_on_press_down_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_dup_on_press_up_event, vc);
   vc_dup_show(vc);
}
