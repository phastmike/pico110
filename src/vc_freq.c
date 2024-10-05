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

/* Private function prototypes */

void vc_freq_show(view_controller_t *vc);
void vc_freq_present(view_controller_t *vc);
void vc_freq_init(vc_freq_t *this, hmi_t *hmi, radio_t *radio);

/****************************************************************************/

vc_freq_t *vc_freq_new(hmi_t *hmi, radio_t *radio) {
   vc_freq_t *this = (vc_freq_t *) calloc(1, sizeof(vc_freq_t)); 
   vc_freq_init(this, hmi, radio);
   return this;
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
   if (rc == NULL) return;
   double freq = radio_channel_freq_rx_get(rc);
   unsigned char *s = freq2string(freq);
   hmi_display_text(vc->hmi, 0, s);
   free(s);

   hmi_led_set(vc->hmi, HMI_LED_REV, radio_channel_get_rev(rc) ? HMI_LED_ON : HMI_LED_OFF);

   hmi_led_set(vc->hmi, HMI_LED_FMENU, ((vc_freq_t *)vc)->func_enabled ? HMI_LED_ON : HMI_LED_OFF);
   hmi_led_set(vc->hmi, HMI_LED_DUP, radio_channel_dup_get(rc) == DUP_OFF ? HMI_LED_OFF : HMI_LED_ON);
   hmi_led_set(vc->hmi, HMI_LED_MR, radio_get_mode(vc->radio) == RADIO_MODE_VFO ? HMI_LED_OFF : HMI_LED_ON);
   hmi_led_set(vc->hmi, HMI_LED_LOW, radio_channel_low_power_get(rc) ? HMI_LED_ON : HMI_LED_OFF);

   /* Hmmm why?  */
   if (VC_FREQ(vc)->func_enabled) {
      radio_scan_set(vc->radio, false);
   }

   hmi_led_set(vc->hmi, HMI_LED_SCAN, radio_scan_get(vc->radio) ? HMI_LED_ON : HMI_LED_OFF);
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
}

void vc_freq_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   assert(VIEW_CONTROLLER(user_data)->radio != NULL);
   radio_radio_channel_down(VIEW_CONTROLLER(user_data)->radio);
}

void vc_freq_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);
   assert(VIEW_CONTROLLER(user_data)->radio != NULL);
   radio_radio_channel_up(VIEW_CONTROLLER(user_data)->radio);
}

void vc_freq_on_press_rev_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //if PTT OFF do
   //hmi_display_get_enabled ... should not happen
   //view_mode... a problem because it's outside of scope
   assert(key != NULL && user_data != NULL);

   if (VC_FREQ(user_data)->func_enabled) {
      view_controller_leave(VIEW_CONTROLLER(user_data), key);
      return;
   }

   if (radio_get_mode(VIEW_CONTROLLER(user_data)->radio) == RADIO_MODE_FUNC) return;

   radio_channel_t *rc = radio_get_active_channel(VIEW_CONTROLLER(user_data)->radio);
   radio_channel_set_rev(rc, !radio_channel_get_rev(rc));
}

void vc_freq_on_press_scan_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   if (VC_FREQ(user_data)->func_enabled) {
      VC_FREQ(user_data)->func_enabled = false;
      return;
   }

   radio_t *radio = RADIO(VIEW_CONTROLLER(user_data)->radio);
   if (radio_get_mode(radio) != RADIO_MODE_MEMORY) return;
   radio_scan_set(radio, !radio_scan_get(radio));
}

void vc_freq_on_press_fmenu_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   if (VC_FREQ(user_data)->func_enabled) {
      view_controller_leave(VIEW_CONTROLLER(user_data), key);
      return;
   }

   VC_FREQ(user_data)->func_enabled = !VC_FREQ(user_data)->func_enabled;
}

void vc_freq_on_press_power_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
         radio_t *radio = VIEW_CONTROLLER(user_data)->radio;
         radio_channel_t *rc = radio_get_active_channel(radio);

         if (VC_FREQ(user_data)->func_enabled) {
            view_controller_leave(VIEW_CONTROLLER(user_data), key);
            return;
         }

         radio_channel_low_power_set(rc, !radio_channel_low_power_get(rc));
}

void vc_freq_on_press_sql_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   assert(key != NULL && user_data != NULL);

   vc_freq_t *view = (vc_freq_t *) user_data;
   view_controller_t *vc = VIEW_CONTROLLER(user_data);

   if (VC_FREQ(user_data)->func_enabled) {
      view_controller_leave(VIEW_CONTROLLER(user_data), key);
      return;
   } else {
      // DO something with SQL key
   }

}

void vc_freq_keyb_set_cb(vc_freq_t *vc) {
   assert (vc != NULL && VIEW_CONTROLLER(vc)->hmi != NULL && VIEW_CONTROLLER(vc)->radio != NULL);

   hmi_keys_disconnect(VIEW_CONTROLLER(vc)->hmi);

   hmi_key_t *key;

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_1);
   hmi_key_on_release_event_connect(key, vc_freq_on_press_fmenu_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_2);
   hmi_key_on_release_event_connect(key, vc_freq_on_press_scan_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_3);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_vm_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_4);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_power_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_5);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_sql_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_6);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_rev_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_down_event, VIEW_CONTROLLER(vc));

   key = hmi_get_key(VIEW_CONTROLLER(vc)->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_freq_on_press_up_event, VIEW_CONTROLLER(vc));
   
}

void vc_freq_init(vc_freq_t *this, hmi_t *hmi, radio_t *radio) {
   assert(this != NULL);
   view_controller_init(VIEW_CONTROLLER(this), hmi, radio);
   VIEW_CONTROLLER(this)->show = vc_freq_show;
   VIEW_CONTROLLER(this)->present = vc_freq_present;
   this->func_enabled = false;
}

/* VIEW CONTROLLER present method */

void vc_freq_present(view_controller_t *vc) {
   assert(vc != NULL);
   vc_freq_keyb_set_cb(VC_FREQ(vc));
   vc_freq_show(vc);
}
