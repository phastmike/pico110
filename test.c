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
#include "tm1638.h"
#include "tune_step.h"
#include "rom_init.h"
#include "i2c0.h"
#include <pico/stdlib.h>
//#include "hardware/i2c.h"
//#include "hardware/irq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ADDED LED PIN
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// INIT EEPROM WITH CONTENTS
m110_t        *m110;
double         freq = 446.00625;
tune_step_t   *tune_step = NULL; 


/* Convert a frequency into a string.
 * Returns a newly allocated string,
 *  caller must free the string
 */
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

void radio_set_frequency(m110_t *m110, double freq_rx, double freq_tx) {
   m110_channel_frequencies_set(m110, 1, freq_rx, freq_tx);
   m110_channel_frequencies_set(m110, 2, freq_rx, freq_tx);
}

void display_show_frequency(tm1638_t *tm1638, double freq) {
   unsigned char *s = freq2string(freq);
   tm1638_show(tm1638, s, 0);
   free(s);
}

void display_show_eeprom_n_and_a(tm1638_t *tm1638, m110_t *m110) {
   //tm1638_clear(tm1638);
   unsigned char b1 = eeprom_raw_read_byte(m110_eeprom_get(m110), 0x22);
   unsigned char b2 = eeprom_raw_read_byte(m110_eeprom_get(m110), 0x23);
   unsigned char b3 = eeprom_raw_read_byte(m110_eeprom_get(m110), 0x24);
   unsigned int N = ((b1 & 2) << 8) + b2;
   unsigned int A = b3 >> 1;
   unsigned char *str = calloc(1, 8);
   sprintf(str, "N.%3dA.%3d",N,A);
   printf("str=[%s]\n", str);
   tm1638_show(tm1638, str, 0);
   free(str);
}

void display_show_tune_step(tm1638_t *tm1638, tune_step_t *tune_step) {
   unsigned char *str = calloc(1, 8);
   unsigned char *step_str = tune_step_get_as_string(tune_step);
   sprintf(str, "TS  %s",step_str);
   tm1638_show(tm1638, str, 0);
   free(str);
   free(step_str);
}

void display_show_brightness(tm1638_t *tm1638) {
   unsigned char b = tm1638_brightness_get(tm1638);
   if (b >= 0 && b <= 7) {
      unsigned char *str = calloc(1, 8);
      sprintf(str, "Bright %d", b);
      tm1638_show(tm1638, str, 0);
      free(str);
   }
}

void on_step_changed(tune_step_t *tune_step, void *user_data) {
   printf("Step Changed, userdata: %s, step is now at %s kHz\n", (char *) user_data, tune_step_get_as_string(tune_step));
}

// Main loop - initilises system and then loops while interrupts get on with processing the data
int main() {
   unsigned int keys;
   unsigned char opt_function_state = false;

   m110 = m110_new_with_data(rom_init);

   tune_step = tune_step_new();
   tune_step_on_changed_connect(tune_step, on_step_changed, "[This is userdata]");

   stdio_init_all();

   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   i2c0_init(0);

   tm1638_t *tm1638;
   tm1638 = tm1638_new(1, 2, 3, 0);

   unsigned char *si = freq2string(freq);
   tm1638_show(tm1638, si, 0);
   free(si);

   radio_set_frequency(m110, freq, freq);

   while (true) {
      tight_loop_contents();
      gpio_put(LED_PIN, 1);
      sleep_ms(50);
      keys = tm1638_keys(tm1638);
      sleep_ms(50);
       
      // Add 1 to offset F led
      tm1638_led(tm1638, tune_step_get_index(tune_step) + 1, 1);

      if (keys & 1) {
         opt_function_state = !opt_function_state; 
         tm1638_led(tm1638, 0, opt_function_state);

         if (!opt_function_state) {
            display_show_frequency (tm1638, freq);
            radio_set_frequency(m110, freq, freq);
         } else {
            display_show_tune_step(tm1638, tune_step);
         }
      } else if (keys & 2) {
         if (opt_function_state) {
            tm1638_led(tm1638, tune_step_get_index(tune_step) + 1, 0); 
            tune_step_next(tune_step);
            tm1638_led(tm1638, tune_step_get_index(tune_step) + 1, 0); 
            display_show_tune_step(tm1638, tune_step);
         } else {
            display_show_eeprom_n_and_a(tm1638, m110);
         }
      } else if (keys & 4) {
         tm1638_power_set(tm1638, !tm1638_power_get(tm1638));
         sleep_ms(50);
      } else if (keys & 8) {
         tm1638_show(tm1638, "Press  F", 0);
      } else if (keys & 16) {
         unsigned char b = tm1638_brightness_get(tm1638);
         b -= 1;
         tm1638_brightness_set(tm1638, b);
         display_show_brightness(tm1638);
      } else if (keys & 32) {
         unsigned char b = tm1638_brightness_get(tm1638);
         b += 1;
         tm1638_brightness_set(tm1638, b);
         display_show_brightness(tm1638);
      } else if (keys & 64) {
         freq -= tune_step_get_as_MHz(tune_step);
         display_show_frequency (tm1638,freq);
         radio_set_frequency(m110, freq, freq);
      } else if (keys & 128) {
         freq += tune_step_get_as_MHz(tune_step);
         display_show_frequency (tm1638,freq);
         radio_set_frequency(m110, freq, freq);
      } 
      gpio_put(LED_PIN, 0);
      sleep_ms(50);
   }
   return 0;
}
