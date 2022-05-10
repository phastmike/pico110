/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_freq.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_freq.h"
#include "view_controller.h"

struct _vc_freq_t {
   view_controller_t *vc;
};

void vc_freq_present(view_controller_t *vc);

vc_freq_t *vc_freq_new(hmi_t *hmi, radio_t *radio) {
   vc_freq_t *vc_freq = (vc_freq_t *) view_controller_new(hmi, radio);
   VIEW_CONTROLLER(vc_freq)->present = vc_freq_present;
   return vc_freq;
}

unsigned char *freq2string(double freq) {
   unsigned char i, j;
   unsigned char *fstr, *rstr;
   fstr = (unsigned char *) calloc (1, 9);
   sprintf(fstr, "%f", freq);
   rstr = (unsigned char *) calloc (1, 11);
   for (i = 0, j = 0; i < 9; i++, j++) {
      if (i == 7) {
         rstr[j] = '.';
         j++;
      }
      rstr[j] = fstr[i];
   }
   free(fstr);
   return rstr;
}

void vc_freq_show(view_controller_t *vc) {
   assert(vc != NULL);
   radio_channel_t *rc = radio_get_active_channel(vc->radio);
   double freq = radio_channel_freq_rx_get(rc);
   unsigned char *s = freq2string(freq);
   hmi_display_text(vc->hmi, 0, s);
   free(s);

   if (radio_channel_dup_get(rc) == DUP_OFF) {
      hmi_led_set(vc->hmi, HMI_LED_DUP, HMI_LED_OFF);
   } else {
      hmi_led_set(vc->hmi, HMI_LED_DUP, HMI_LED_ON);
   }

   hmi_led_set(vc->hmi, HMI_LED_REV, radio_channel_get_rev(rc) ? HMI_LED_ON : HMI_LED_OFF);

   unsigned char mode = radio_get_mode(vc->radio);
   switch(mode) {
      case RADIO_MODE_VFO:
         hmi_led_set(vc->hmi, HMI_LED_MR, HMI_LED_OFF);
         break;
      case RADIO_MODE_MEMORY:
      default:
         hmi_led_set(vc->hmi, HMI_LED_MR, HMI_LED_ON);
         break;
   }
}

/* EVENTS */

void vc_freq_on_press_func_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_mode_t mode;

   mode = radio_get_mode(VIEW_CONTROLLER(user_data)->radio);

   switch(mode) {
      case RADIO_MODE_VFO:
         radio_set_mode(VIEW_CONTROLLER(user_data)->radio, RADIO_MODE_MEMORY);
         break;
      case RADIO_MODE_MEMORY:
      default:
         radio_set_mode(VIEW_CONTROLLER(user_data)->radio, RADIO_MODE_VFO);
         break;
   }

   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_radio_channel_down(VIEW_CONTROLLER(user_data)->radio);
   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   radio_radio_channel_up(VIEW_CONTROLLER(user_data)->radio);
   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_rev_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //if PTT OFF do
   //hmi_display_get_enabled ... should not happen
   //view_mode... a problem because it's outside of scope
   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   radio_channel_set_rev(rc, !radio_channel_get_rev(rc));
   radio_set_active_channel(VIEW_CONTROLLER(user_data)->radio, rc);
   hmi_led_set(VIEW_CONTROLLER(user_data)->hmi, HMI_LED_REV, radio_channel_get_rev(rc) ? HMI_LED_ON : HMI_LED_OFF);
   vc_freq_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_freq_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_led_set(vc->hmi, HMI_LED_FMENU, HMI_LED_ON);
   hmi_key_t *key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_down_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_up_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_3);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_func_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_6);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_rev_event, vc);
   vc_freq_show(vc);
}
