/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * test.c
 * 
 * simple eeprom emulation. behaviour done in i2c ISR 
 * 
 *
 * José Miguel Fonte
 */

/**
 * Example program for basic use of pico as an I2C peripheral (previously known as I2C slave)
 * 
 * This example allows the pico to act as a 256byte RAM
 * 
 * Author: Graham Smith (graham@smithg.co.uk)
 * https://forums.raspberrypi.com/viewtopic.php?f=144&t=304074
 */


// Usage:
//
// When writing data to the pico the first data byte updates the current address to be used when writing or reading from the RAM
// Subsequent data bytes contain data that is written to the ram at the current address and following locations (current address auto increments)
//
// When reading data from the pico the first data byte returned will be from the ram storage located at current address
// Subsequent bytes will be returned from the following ram locations (again current address auto increments)
//
// N.B. if the current address reaches 255, it will autoincrement to 0 after next read / write

#include "m110.h"
#include "radio.h"
#include "hmi.h"
#include "tune_step.h"
#include "i2c0.h"
#include "ctcss.h"
#include <pico/stdlib.h>
//#include "hardware/i2c.h"
//#include "hardware/irq.h"
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

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void pause_iteration(void) {
   while (true) {
      tight_loop_contents();
      gpio_put(LED_PIN, 1);
      sleep_ms(50);
      gpio_put(LED_PIN, 0);
      sleep_ms(50);
      printf("\titeration...\n");
   }
}

void on_i2c_addr_request(uint16_t addr, void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = (m110_t *) user_data;
   eeprom_addr_set(m110_eeprom_get(m110), addr);
}

void on_i2c_write_byte(uint8_t byte, void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = (m110_t *) user_data;
   eeprom_write_byte(m110_eeprom_get(m110), byte);
}

uint8_t on_i2c_read_byte(void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = (m110_t *) user_data;
   return eeprom_read_byte(m110_eeprom_get(m110));
}

// MAINLOOP //

int main() {
   unsigned int keys;

   stdio_init_all();

   // startup for debug
   //sleep_ms(10000);
   //printf("Start...\n");
   //pause_iteration


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

   view_controller_present(vcs[vc_id]);

   while(true) {
      tight_loop_contents();
      gpio_put(LED_PIN, 1);
      sleep_ms(80);
      
      keys = hmi_keys_scan(hmi);

      if (keys & HMI_KEY_1 && hmi_display_get_enabled(hmi)) {
         if (vc_id == (sizeof(vcs)/sizeof(view_controller_t *)) - 1) vc_id = 0;
         else vc_id += 1;
         view_controller_present(vcs[vc_id]);
      } else if (keys & HMI_KEY_2 && hmi_display_get_enabled(hmi)) {
         vc_id = 0;
         view_controller_present(vcs[vc_id]);
      }

      gpio_put(LED_PIN, 0);
      sleep_ms(80);
   }

   /*
   vc_txadmit_t *vcta = vc_txadmit_new(hmi, radio);
   view_controller_present(VIEW_CONTROLLER(vcta));
   */

   return 0;
}
