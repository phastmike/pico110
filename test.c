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

// ADDED LED PIN
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// INIT EEPROM WITH CONTENTS
radio_t       *radio;
tune_step_t   *tune_step = NULL; 
ctcss_t       *ctcss = NULL;


/*
void display_show_eeprom_n_and_a(hmi_t *hmi, m110_t *m110) {
   unsigned char b1 = eeprom_raw_read_byte(m110_eeprom_get(m110), 0x22);
   unsigned char b2 = eeprom_raw_read_byte(m110_eeprom_get(m110), 0x23);
   unsigned char b3 = eeprom_raw_read_byte(m110_eeprom_get(m110), 0x24);
   unsigned int N = ((b1 & 2) << 8) + b2;
   unsigned int A = b3 >> 1;
   unsigned char *str = calloc(1, 8);
   sprintf(str, "N.%3dA.%3d",N,A);
   printf("str=[%s]\n", str);
   hmi_display_text(hmi, 0, str);
   free(str);
}
*/

void display_intro(hmi_t *hmi) {
   assert(hmi != NULL);

   hmi_display_text(hmi, 0, " RADIUS ");
   for(int j = 0; j < HMI_NUMBER_OF_KEYS; j++) {
      hmi_leds_set(hmi, 1 << j);
      sleep_ms(80);
   }

   hmi_display_text(hmi, 0, "PICO 110");
   for(int k = 0; k < HMI_NUMBER_OF_KEYS; k++) {
      hmi_leds_set(hmi, 0x80 >> k);
      sleep_ms(80);
   }

   hmi_leds_set(hmi, 0);

   hmi_display_text(hmi, 0, " CT1ENQ ");
   sleep_ms(1000);
}


void on_step_changed(tune_step_t *tune_step, void *user_data) {
   printf("Step Changed, userdata: %s, step is now at %s kHz\n", (char *) user_data, tune_step_get_as_string(tune_step));
}

void on_ctcss_changed(ctcss_t *ctcss, void *user_data) {
   printf("CTCSS Changed, index: %d tone %f (%s)\n", ctcss_get_index(ctcss), ctcss_get_as_hz(ctcss), ctcss_get_as_string(ctcss));
}

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
   //eeprom_addr_set(EEPROM(user_data), addr);
   eeprom_addr_set(m110_eeprom_get(m110), addr);
}

void on_i2c_write_byte(uint8_t byte, void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = (m110_t *) user_data;

   //eeprom_write_byte(EEPROM(user_data), byte);
   eeprom_write_byte(m110_eeprom_get(m110), byte);
}

uint8_t on_i2c_read_byte(void *user_data) {
   assert(user_data != NULL);
   m110_t *m110 = (m110_t *) user_data;

   //return eeprom_read_byte(EEPROM(user_data));
   return eeprom_read_byte(m110_eeprom_get(m110));

}

// Main loop - initilises system and then loops while interrupts get on with processing the data
int main() {
   unsigned int keys;
   unsigned char opt_function_state = false;
   unsigned char opt_function_ctcss = false;

   stdio_init_all();

   // startup for debug
   //sleep_ms(10000);
   //printf("Start...\n");
   //pause_iteration


   i2c0_init(false); // init without clock streching
   radio = radio_new_with_defaults();
   hmi_t *hmi = hmi_new();

   i2c_on_addr_set_connect(on_i2c_addr_request,radio_get_m110(radio));
   i2c_on_write_byte_connect(on_i2c_write_byte,radio_get_m110(radio));
   i2c_on_read_byte_connect(on_i2c_read_byte,radio_get_m110(radio));

   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   display_intro(hmi);

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
   };
   
   int vc_id = 0;

   view_controller_present(vcs[vc_id]);

   while(true) {
      tight_loop_contents();
      gpio_put(LED_PIN, 1);
      sleep_ms(80);
      
      keys = hmi_keys_scan(hmi);

      if (keys & 1 && hmi_display_get_enabled(hmi)) {
         if (vc_id == (sizeof(vcs)/sizeof(view_controller_t *)) - 1) vc_id = 0;
         else vc_id += 1;
         view_controller_present(vcs[vc_id]);
      } else if (keys & 2 && hmi_display_get_enabled(hmi)) {
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
