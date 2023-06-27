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
   view_controller_t vc;
   bool func_enabled;
};

void vc_freq_present(view_controller_t *vc);

vc_freq_t *vc_freq_new(hmi_t *hmi, radio_t *radio) {
   vc_freq_t *vc_freq = (vc_freq_t *) calloc(1, sizeof(vc_freq_t)); 
   VIEW_CONTROLLER(vc_freq)->hmi = hmi;
   VIEW_CONTROLLER(vc_freq)->radio = radio;
   VIEW_CONTROLLER(vc_freq)->present = vc_freq_present;
   vc_freq->func_enabled = false;
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
   hmi_led_set(vc->hmi, HMI_LED_FMENU, ((vc_freq_t *)vc)->func_enabled ? HMI_LED_ON : HMI_LED_OFF);

   unsigned char mode = radio_get_mode(vc->radio);
   switch(mode) {
      case RADIO_MODE_VFO:
         hmi_led_set(vc->hmi, HMI_LED_MR, HMI_LED_OFF);
         break;
      case RADIO_MODE_MEMORY:
         hmi_led_set(vc->hmi, HMI_LED_MR, HMI_LED_ON);
         break;
      default:
         //hmi_led_set(vc->hmi, HMI_LED_MR, HMI_LED_ON);
         break;
   }

   vc_freq_t * view = (vc_freq_t *) vc;
   
   if (view->func_enabled) {
      radio_scan_set(vc->radio, false);
      //hmi_led_set(vc->hmi, HMI_LED_SCAN, HMI_LED_OFF);
   }

   if (radio_scan_get(vc->radio)) {
      hmi_led_set(vc->hmi, HMI_LED_SCAN, HMI_LED_ON);
   } else {
      hmi_led_set(vc->hmi, HMI_LED_SCAN, HMI_LED_OFF);
   }

}

/* EVENTS */
/* FIXME: The following actions should check if PTT is active
 * and return if so as these are not allowed while transmitting
 */

void vc_freq_on_press_vm_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   radio_t *radio = RADIO(VIEW_CONTROLLER(user_data)->radio);
   radio_mode_t mode = radio_get_mode(radio);

   switch(mode) {
      case RADIO_MODE_VFO:
         radio_set_mode(radio, RADIO_MODE_MEMORY);
         break;
      case RADIO_MODE_MEMORY:
         radio_scan_set(radio, false);
         radio_set_mode(radio, RADIO_MODE_VFO);
      default:
         break;
   }

   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_radio_channel_down(VIEW_CONTROLLER(user_data)->radio);
   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   radio_radio_channel_up(VIEW_CONTROLLER(user_data)->radio);
   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_rev_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //if PTT OFF do
   //hmi_display_get_enabled ... should not happen
   //view_mode... a problem because it's outside of scope
   assert(key != NULL && user_data != NULL);

   vc_freq_t *view = (vc_freq_t *) user_data;
   view_controller_t *vc = VIEW_CONTROLLER(user_data);

   if (view->func_enabled) {
      if (vc->exit_with_key) vc->exit_with_key(vc, key);
      return;
   }

   if (radio_get_mode(VIEW_CONTROLLER(user_data)->radio) == RADIO_MODE_FUNC) return;

   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   radio_channel_set_rev(rc, !radio_channel_get_rev(rc));
   radio_set_active_channel(VIEW_CONTROLLER(user_data)->radio, rc);
   hmi_led_set(VIEW_CONTROLLER(user_data)->hmi, HMI_LED_REV, radio_channel_get_rev(rc) ? HMI_LED_ON : HMI_LED_OFF);
   vc_freq_show(VIEW_CONTROLLER(user_data));
}

void vc_freq_on_press_scan_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   hmi_t *hmi = VIEW_CONTROLLER(user_data)->hmi;
   if (!hmi_display_get_enabled(hmi)) return;

   vc_freq_t *view = (vc_freq_t *) user_data;

   if (view->func_enabled) {
      view->func_enabled = false;
      vc_freq_show(VIEW_CONTROLLER(user_data));
      return;
   }

   radio_t *radio = RADIO(VIEW_CONTROLLER(user_data)->radio);
   if (radio_get_mode(radio) != RADIO_MODE_MEMORY) return;
   radio_scan_set(radio, !radio_scan_get(radio));
   hmi_led_set(HMI(VIEW_CONTROLLER(user_data)->hmi), HMI_LED_SCAN, radio_scan_get(radio));  // HMI_LED_OFF

      /*
       * else if (keys & HMI_KEY_2 && hmi_display_get_enabled(hmi) && view_mode == VMODE_FUNC) {
         vc_id = 0;
         view_controller_present(vcs[vc_id]);
         view_mode = VMODE_FREQ;
         radio_set_mode(radio, previous_mode); 
         hmi_led_set(hmi, HMI_LED_FMENU, HMI_LED_OFF);
         radio_scan_set(radio, false);
         hmi_led_set(hmi, HMI_LED_SCAN, HMI_LED_OFF);
      */
}

void vc_freq_on_press_fmenu_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   vc_freq_t *view = (vc_freq_t *) user_data;
   view_controller_t *vc = VIEW_CONTROLLER(user_data);

   if (view->func_enabled) {
      if (vc->exit_with_key) vc->exit_with_key(vc, key);
   } else {
      view->func_enabled = ! view->func_enabled;
      vc_freq_show(VIEW_CONTROLLER(user_data));
   }
}

void vc_freq_on_press_power_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
         radio_t *radio = VIEW_CONTROLLER(user_data)->radio;
         radio_channel_t *rc = radio_get_active_channel(radio);

         vc_freq_t *view = (vc_freq_t *) user_data;
         view_controller_t *vc = VIEW_CONTROLLER(user_data);

         if (view->func_enabled) {
            if (vc->exit_with_key) vc->exit_with_key(vc,key);
            return;
         }

         radio_channel_low_power_set(rc, !radio_channel_low_power_get(rc));
         radio_set_active_channel(radio, rc);
         hmi_led_set(VIEW_CONTROLLER(user_data)->hmi, HMI_LED_LOW, radio_channel_low_power_get(rc) ? HMI_LED_ON : HMI_LED_OFF);
}

void vc_freq_on_press_sql_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   vc_freq_t *view = (vc_freq_t *) user_data;
   view_controller_t *vc = VIEW_CONTROLLER(user_data);

   if (view->func_enabled) {
      if (vc->exit_with_key) vc->exit_with_key(vc, key);
   } else {
      // DO something with SQL key
   }

}

/* VIEW CONTROLLER present method */

void vc_freq_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);
   //hmi_led_set(vc->hmi, HMI_LED_FMENU, HMI_LED_ON);

   hmi_key_t *key;

   key = hmi_get_key(vc->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_freq_on_press_fmenu_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_freq_on_press_scan_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_3);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_vm_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_4);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_power_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_5);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_sql_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_6);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_rev_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_down_event, vc);

   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_up_event, vc);

   vc_freq_show(vc);
}
