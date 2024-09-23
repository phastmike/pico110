/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * test.c
 * 
 * Pico 110 Eeprom emulation.
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/stdlib.h>

#include "m110.h"
#include "radio.h"
#include "hmi.h"
#include "tune_step.h"
#include "i2c0.h"
#include "ctcss.h"
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
   eeprom_addr_set(EEPROM(m110), addr);
}

void on_i2c_write_byte(uint8_t byte, void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = M110(user_data);
   eeprom_serial_write_byte(EEPROM(m110), byte);
}

uint8_t on_i2c_read_byte(void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = M110(user_data);
   return eeprom_serial_read_byte(EEPROM(m110));
}


typedef enum {
   VMODE_FREQ,
   VMODE_FUNC
} vmode_t;

view_controller_t ** vcs;
int                  vc_id;
unsigned int         vcs_size;

void on_freq_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL);
   
   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1: 
         vc_id = 1;
         view = vcs[1];
         view_controller_present(view);
         break;
      case HMI_KEY_4:
         vc_id = 3;
         view = vcs[3];
         view_controller_present(view);
         break;
      case HMI_KEY_5:
         vc_id = 4;
         view = vcs[4];
         view_controller_present(view);
         break;
      case HMI_KEY_6:
         vc_id = 9;
         view = vcs[9];
         view_controller_present(view);
         break;
      default:
         break;
   }
} 

void on_brightness_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 2;
         view = vcs[2];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         //NEED SETTER & GETTER
         //((vc_freq_t *) vc)->func_enabled = false;
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_apo_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 3;
         view = vcs[3];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_tune_step_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 4;
         view = vcs[4];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_enc_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 5;
         view = vcs[5];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}


void on_dec_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 6;
         view = vcs[6];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_timeout_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 7;
         view = vcs[7];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_rekey_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 8;
         view = vcs[8];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_txadmit_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 9;
         view = vcs[9];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_dup_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 10;
         view = vcs[10];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_shift_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 11;
         view = vcs[11];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

void on_version_exit_with_key(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL && key != NULL);

   view_controller_t *view;

   switch(hmi_key_get_id(key)) {
      case HMI_KEY_1:
         vc_id = 1;
         view = vcs[1];
         view_controller_present(view);
         break;
      case HMI_KEY_2:
         vc_id = 0;
         view = vcs[0];
         view_controller_present(view);
         break;
      default:
         break;
   }
}

/* MAIN ENTRY POINT */

int main() {
   unsigned int keys;
   vmode_t  view_mode;

   stdio_init_all();

#ifdef DEBUG
   /* Startup for Debug
    * Waits 10 seconds to give time to dump output to console with:
    * cat /dev/ttyACM0
    */
   //sleep_ms(10000);
   sleep_ms(5000);
   printf("Start...\n");
#endif

   hmi_t *hmi = hmi_new();
   radio_t *radio = radio_new();

   i2c0_init(false); // init without clock streching
   i2c_on_addr_set_connect(on_i2c_addr_request,M110(radio));
   i2c_on_write_byte_connect(on_i2c_write_byte,M110(radio));
   i2c_on_read_byte_connect(on_i2c_read_byte,M110(radio));

   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   vc_intro_t *intro = vc_intro_new(hmi, radio);
   view_controller_present(VIEW_CONTROLLER(intro));
   view_controller_destroy(VIEW_CONTROLLER(intro));

   view_controller_t *vcs_array[] = {
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

   vc_id = 0;
   vcs = &vcs_array[0];
   vcs_size = sizeof(vcs_array)/sizeof(vcs_array[0]);
   view_mode = VMODE_FREQ;
   view_controller_present(vcs[0]);
   radio_mode_t previous_mode = RADIO_MODE_VFO;

   // Navigation logic is defined in these callbacks
   // Dont remember why I followed this route but remember thinking 
   // it was a quick workaround to a state machine that failed  :)
   view_controller_exit_with_key_connect(vcs[0], on_freq_exit_with_key);   
   view_controller_exit_with_key_connect(vcs[1], on_brightness_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[2], on_apo_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[3], on_tune_step_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[4], on_enc_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[5], on_dec_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[6], on_timeout_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[7], on_rekey_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[8], on_txadmit_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[9], on_dup_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[10], on_shift_exit_with_key); 
   view_controller_exit_with_key_connect(vcs[11], on_version_exit_with_key); 

   // MAINLOOP //

   long long unsigned int count = 0;
   while(true) {
      tight_loop_contents();
      
      if (radio_scan_get(radio)) {
         // FIXME: Needs border limit control if VFO mode
         // MR ONLY
         radio_radio_channel_up(radio);
         vc_id = 0;
         view_controller_present(VIEW_CONTROLLER(vcs[0]));
         sleep_ms(200);
      } 

      keys = hmi_keys_scan(hmi);
      view_controller_show (VIEW_CONTROLLER(vcs[vc_id]));
   }

   return 0;
}
