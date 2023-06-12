/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * test.c
 * 
 * Pico 110 Eeprom emulation.
 *
 * José Miguel Fonte
 */

#include "m110.h"
#include "radio.h"
#include "hmi.h"
#include "tune_step.h"
#include "i2c0.h"
#include "ctcss.h"
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tm1638.h"
#include "view_controller.h"
#include "vc_brightness.h"
#include "vc_tune_step.h"
#include "vc_timeout.h"
#include "vc_rekey.h"
#include "vc_txadmit.h"
#include "vc_freq.h"
#include "vc_dup.h"
#include "vc_enc.h"
#include "vc_dec.h"
#include "vc_version.h"
#include "vc_shift.h"
#include "vc_apo.h"
#include "vc_pll_na.h"
#include "vc_intro.h"

//#define DEBUG // uncomment for initial delay

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void on_i2c_addr_request(uint16_t addr, void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = M110(user_data);
   eeprom_addr_set(m110_eeprom_get(m110), addr);
}

void on_i2c_write_byte(uint8_t byte, void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = M110(user_data);
   eeprom_write_byte(m110_eeprom_get(m110), byte);
}

uint8_t on_i2c_read_byte(void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = M110(user_data);
   return eeprom_read_byte(m110_eeprom_get(m110));
}

// MAINLOOP //

typedef enum {
   VMODE_FREQ,
   VMODE_FUNC
} vmode_t;

int main() {
   unsigned int keys;
   vmode_t  view_mode;

   stdio_init_all();

#ifdef DEBUG
   /* Startup for Debug
    * Waits 10 seconds to give time to dump output to console with:
    * cat /dev/ttyACM0
    */
   sleep_ms(10000);
   printf("Start...\n");
#endif


   radio_t *radio = radio_new_with_defaults();
   hmi_t *hmi = hmi_new();

   i2c0_init(false); // init without clock streching
   i2c_on_addr_set_connect(on_i2c_addr_request,radio_get_m110(radio));
   i2c_on_write_byte_connect(on_i2c_write_byte,radio_get_m110(radio));
   i2c_on_read_byte_connect(on_i2c_read_byte,radio_get_m110(radio));

   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   vc_intro_t *intro = vc_intro_new(hmi, radio);
   view_controller_present(VIEW_CONTROLLER(intro));
   view_controller_destroy(VIEW_CONTROLLER(intro));

   view_controller_t *vcs[] = {
      VIEW_CONTROLLER(vc_freq_new(hmi, radio)),
      VIEW_CONTROLLER(vc_brightness_new(hmi, radio)),
      VIEW_CONTROLLER(vc_apo_new(hmi, radio)),
      VIEW_CONTROLLER(vc_tune_step_new(hmi, radio)),
      VIEW_CONTROLLER(vc_enc_new(hmi, radio)),
      VIEW_CONTROLLER(vc_dec_new(hmi, radio)),
      VIEW_CONTROLLER(vc_timeout_new(hmi, radio)),
      VIEW_CONTROLLER(vc_rekey_new(hmi, radio)),
      VIEW_CONTROLLER(vc_txadmit_new(hmi, radio)),
      VIEW_CONTROLLER(vc_dup_new(hmi, radio)),
      VIEW_CONTROLLER(vc_shift_new(hmi, radio)),
      VIEW_CONTROLLER(vc_version_new(hmi, radio)),
      //VIEW_CONTROLLER(vc_pll_na_new(hmi, radio)),
   };
   
   int vc_id = 0;
   view_mode = VMODE_FREQ;
   view_controller_present(vcs[vc_id]);
   radio_mode_t previous_mode = RADIO_MODE_VFO;

   while(true) {
      tight_loop_contents();
      //gpio_put(LED_PIN, 1);
      //sleep_ms(80);
      
      //
      // Needs some time control
      // and control for scan press and key to stop
      //
      if (radio_scan_get(radio) && view_mode == VMODE_FREQ) {
         // VFO + MR
         // FIXME: Needs border limit control
         //radio_radio_channel_up(radio);
         
         // MR ONLY
         if (radio_get_mode(radio) == RADIO_MODE_MEMORY) {
            radio_radio_channel_up(radio);
         }
      } 

      if (radio_get_mode(radio) != RADIO_MODE_MEMORY) {
         radio_scan_set(radio, false);
         hmi_led_set(hmi, HMI_LED_SCAN, HMI_LED_OFF);
      }

      keys = hmi_keys_scan(hmi);

      /*
      if (keys & HMI_KEY_1 && hmi_display_get_enabled(hmi)) {
         if (view_mode == VMODE_FREQ) {
            hmi_led_set(hmi, HMI_LED_FMENU, HMI_LED_ON); // Already set in view_controller  
            view_mode = VMODE_FUNC;
            previous_mode = radio_get_mode(radio);
            radio_set_mode(radio, RADIO_MODE_FUNC); 
            radio_scan_set(radio, false);
            hmi_led_set(hmi, HMI_LED_SCAN, HMI_LED_OFF);
         } else if (view_mode == VMODE_FUNC) {
            if (vc_id == (sizeof(vcs)/sizeof(view_controller_t *)) - 1) {
               vc_id = 0;
               view_mode = VMODE_FREQ;
               radio_set_mode(radio, previous_mode); 
               hmi_led_set(hmi, HMI_LED_FMENU, HMI_LED_OFF);
            } else {
               vc_id += 1;
            }
            view_controller_present(vcs[vc_id]);
         }
         
      } */ /*else*/ if (keys & HMI_KEY_2 && hmi_display_get_enabled(hmi) && view_mode == VMODE_FUNC) {
         vc_id = 0;
         view_controller_present(vcs[vc_id]);
         view_mode = VMODE_FREQ;
         radio_set_mode(radio, previous_mode); 
         hmi_led_set(hmi, HMI_LED_FMENU, HMI_LED_OFF);
         radio_scan_set(radio, false);
         hmi_led_set(hmi, HMI_LED_SCAN, HMI_LED_OFF);
      } else if (keys & HMI_KEY_2 && hmi_display_get_enabled(hmi) && radio_get_mode(radio) == RADIO_MODE_MEMORY) {
         radio_scan_set(radio, !radio_scan_get(radio));
         hmi_led_set(hmi, HMI_LED_SCAN, radio_scan_get(radio));
      } /*else if (keys & HMI_KEY_4 && hmi_display_get_enabled(hmi) && view_mode == VMODE_FREQ && vc_id == 0) {
         radio_channel_t *rc = radio_get_active_channel(radio);
         radio_channel_low_power_set(rc, !radio_channel_low_power_get(rc));
         radio_set_active_channel(radio, rc);
         hmi_led_set(hmi, HMI_LED_LOW, radio_channel_low_power_get(rc) ? HMI_LED_ON : HMI_LED_OFF);
      } */else if (keys & HMI_KEY_4 && hmi_display_get_enabled(hmi) && view_mode == VMODE_FUNC && vc_id == 0) {
        vc_id = 3;
        view_controller_present(vcs[vc_id]);
      } else if (keys & HMI_KEY_5 && hmi_display_get_enabled(hmi) && view_mode == VMODE_FUNC && vc_id == 0) {
        vc_id = 4;
        view_controller_present(vcs[vc_id]);
      } else if (keys & HMI_KEY_6 && hmi_display_get_enabled(hmi) && view_mode == VMODE_FUNC && vc_id == 0) {
        vc_id = 9;
        view_controller_present(vcs[vc_id]);
      } else {
        view_controller_present(vcs[vc_id]);
      }

      //gpio_put(LED_PIN, 0);
      sleep_ms(80);
   }

   return 0;
}
