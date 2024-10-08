/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_txadmit.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_txadmit.h"
#include "view_controller.h"

struct _vc_txadmit_t {
   view_controller_t vc;
};

void vc_txadmit_show(view_controller_t *vc);
void vc_txadmit_present(view_controller_t *vc);

vc_txadmit_t *vc_txadmit_new(hmi_t *hmi, radio_t *radio) {
   vc_txadmit_t *vc_txadmit = (vc_txadmit_t *) calloc(1, sizeof(vc_txadmit_t)); 
   view_controller_init(VIEW_CONTROLLER(vc_txadmit), hmi, radio);
   VIEW_CONTROLLER(vc_txadmit)->show = vc_txadmit_show;
   VIEW_CONTROLLER(vc_txadmit)->present = vc_txadmit_present;
   return vc_txadmit;
}

char * tx_admit_to_string(tx_admit_t txa) {
   char *string = (char *) calloc (1, 9);

   switch (txa) {
      case TXADMIT_MONITOR:
         sprintf(string, "  MO");
         break;
      case TXADMIT_ALWAYS:
         sprintf(string, "  AL");
         break;
      case TXADMIT_NO_CARRIER:
         sprintf(string, "  NC");
         break;
      case TXADMIT_PL_NO_CARRIER:
         sprintf(string, "NC.PL");
         break;
      default:
         sprintf(string, " Err");
         break; 
   }
   
   return string;
}

void vc_txadmit_show(view_controller_t *vc) {
   assert(vc != NULL);
   char *string = (char *) calloc (1,9);
   tx_admit_t txa = radio_channel_tx_admit_get(radio_get_active_channel(vc->radio));
   char *str_txa = tx_admit_to_string(txa);
   sprintf(string, "TAD %s", str_txa);
   hmi_display_text(vc->hmi, 0, string);
   free(string);
   free(str_txa);
}

/* EVENTS */

void vc_txadmit_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_tx_admit_down(VIEW_CONTROLLER(user_data)->radio);
}

void vc_txadmit_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_tx_admit_up(VIEW_CONTROLLER(user_data)->radio);
}

void vc_txadmit_on_press_generic_exit(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   hmi_keys_disconnect(VIEW_CONTROLLER(user_data)->hmi);
   view_controller_leave(VIEW_CONTROLLER(user_data), key);
}

/* VIEW CONTROLLER present method */

void vc_txadmit_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_txadmit_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_txadmit_on_press_generic_exit, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_txadmit_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_txadmit_on_press_up_event, vc);
}
