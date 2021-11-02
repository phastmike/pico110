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
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rom_init.h"

// define I2C addresses to be used for this peripheral
#define I2C0_PERIPHERAL_ADDR 0x50

// GPIO pins to use for I2C
#define GPIO_SDA0 12
#define GPIO_SCK0 13

// ADDED LED PIN
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// INIT EEPROM WITH CONTENTS
m110_t      *m110;

double      freq = 446.00625;
tune_step_t *tune_step = NULL; 

/*
#define   FSTEP_6_25kHz  0.00625
#define   FSTEP_12_5kHz  0.0125
#define   FSTEP_25kHz    0.025
#define   FSTEP_50khz    0.05
#define   FSTEP_100kHz   0.1
#define   FSTEP_500kHz   0.5
#define   FSTEP_1MHz     1
#define   FSTEP_N        7

double fsteps[] = {
   FSTEP_6_25kHz, FSTEP_12_5kHz, FSTEP_25kHz, FSTEP_50khz, FSTEP_100kHz,
   FSTEP_500kHz, FSTEP_1MHz
};

char step = 1;

void step_next() {
   step += 1;
   if (step == FSTEP_N) {
      step = 0;
   }
}

void step_prev() {
   step -= 1;
   if (step == -1) {
      step = FSTEP_N - 1;
   }
}

double step_get() {
   return fsteps[step];
}

unsigned char *step_get_as_string() {
   unsigned char *string = calloc(1, 6);

   switch (step) {
      case 0: sprintf(string, " 6.25");
              break;
      case 1: sprintf(string, " 12.5");
              break;
      case 2: sprintf(string, "  25");
              break;
      case 3: sprintf(string, "  50");
              break;
      case 4: sprintf(string, " 100");
              break;
      case 5: sprintf(string, " 500");
              break;
      case 6: sprintf(string, "1000");
              break;
      default: sprintf(string," Err");
               break;
   }

   return string;
}
*/
void i2c0_irq_handler() {

   // Get interrupt status
   uint32_t status = i2c0->hw->intr_stat;

   // Check to see if we have received data from the I2C controller
   if (status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {

      // Read the data (this will clear the interrupt)
      uint32_t value = i2c0->hw->data_cmd;

      // Check if this is the 1st byte we have received
      if (value & I2C_IC_DATA_CMD_FIRST_DATA_BYTE_BITS) {

         // If so treat it as the address to use
         eeprom_addr_set(m110_eeprom_get(m110), (uint16_t)(value & I2C_IC_DATA_CMD_DAT_BITS));
         //eeprom_addr_set((eeprom_t *) m110, (uint16_t)(value & I2C_IC_DATA_CMD_DAT_BITS));
      } else {
         // If not 1st byte then store the data in the RAM
         // and increment the address to point to next byte
         eeprom_write_byte(m110_eeprom_get(m110), (uint8_t)(value & I2C_IC_DATA_CMD_DAT_BITS));
         //eeprom_write_byte((eeprom_t *) m110, (uint8_t)(value & I2C_IC_DATA_CMD_DAT_BITS));
      }
   }

   // Check to see if the I2C controller is requesting data from the RAM
   if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {

      // Write the data from the current address in RAM
      i2c0->hw->data_cmd = (uint32_t) eeprom_read_byte(m110_eeprom_get(m110));
      //i2c0->hw->data_cmd = (uint32_t) eeprom_read_byte((eeprom_t *) m110);

      // Clear the interrupt
      i2c0->hw->clr_rd_req;
   }
}

unsigned char *freq2string(double freq) {
   unsigned char i, j;
   unsigned char *ft, *string;
   ft = (unsigned char *) calloc (1, 9);
   sprintf(ft, "%f", freq);
   string = (unsigned char *) calloc (1, 11);
   for (i = 0, j = 0; i < 9; i++, j++) {
      if (i == 7) {
         string[j] = '.';
         j++;
      }
      string[j] = ft[i];
   }

   return string;
}


// Main loop - initilises system and then loops while interrupts get on with processing the data
int main() {
   m110 = m110_new_with_data(rom_init);

   tune_step = tune_step_new();

   stdio_init_all();

   gpio_init(LED_PIN);
   gpio_set_dir(LED_PIN, GPIO_OUT);

   // Setup I2C0 as slave (peripheral)
   i2c_init(i2c0, 100 * 1000);
   i2c_set_slave_mode(i2c0, true, I2C0_PERIPHERAL_ADDR);

   // Enable slave clock stretching
   //i2c0->hw->enable = 0;
   //hw_set_bits(&i2c0->hw->con, I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_BITS);
   //i2c0->hw->enable = 1;
   //-----------------------------------------------------------------

   // Setup GPIO pins to use and add pull up resistors
   gpio_set_function(GPIO_SDA0, GPIO_FUNC_I2C);
   gpio_set_function(GPIO_SCK0, GPIO_FUNC_I2C);
   gpio_pull_up(GPIO_SDA0);
   gpio_pull_up(GPIO_SCK0);

   // Enable the I2C interrupts we want to process
   i2c0->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);

   // Set up the interrupt handler to service I2C interrupts
   //irq_set_exclusive_handler(I2C0_IRQ, i2c0_irq_handler0);
   irq_set_exclusive_handler(I2C0_IRQ, i2c0_irq_handler);

   // Enable I2C interrupt
   irq_set_enabled(I2C0_IRQ, true);

   // Do nothing in main loop
   tm1638_t *tm1638;
   tm1638 = tm1638_new(1, 2, 3, 0);
   unsigned int keys;

   unsigned char *si = freq2string(freq);
   tm1638_show(tm1638, si, 0);
   free(si);
   m110_channel_frequencies_set(m110, 1, freq, freq);
   m110_channel_frequencies_set(m110, 2, freq, freq);

   unsigned char opt_function_state = false;

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
            unsigned char *si = freq2string(freq);
            tm1638_show(tm1638, si, 0);
            free(si);
            m110_channel_frequencies_set(m110, 1, freq, freq);
            m110_channel_frequencies_set(m110, 2, freq, freq);
         } else {
            unsigned char *str = calloc(1, 8);
            unsigned char *step_str = tune_step_get_as_string(tune_step);
            sprintf(str, "TS  %s", step_str);
            tm1638_show(tm1638, str, 0);
            free(str);
            free(step_str);
         }
      } else if (keys & 2) {
         if (opt_function_state) {
            tm1638_led(tm1638, tune_step_get_index(tune_step) + 1, 0); 
            tune_step_next(tune_step);
            tm1638_led(tm1638, tune_step_get_index(tune_step) + 1, 0); 
            unsigned char *str = calloc(1, 8);
            unsigned char *step_str = tune_step_get_as_string(tune_step);
            sprintf(str, "TS  %s",step_str);
            tm1638_show(tm1638, str, 0);
            free(str);
            free(step_str);
         } else {
            tm1638_clear(tm1638);
         }
      } else if (keys & 4) {
         tm1638_power_set(tm1638, !tm1638_power_get(tm1638));
         sleep_ms(50);
      } else if (keys & 8) {
         tm1638_show(tm1638, "Press F", 0);
      } else if (keys & 16) {
         unsigned char b = tm1638_brightness_get(tm1638);
         b -= 1;
         tm1638_brightness_set(tm1638, b);
         b = tm1638_brightness_get(tm1638);
         unsigned char *str = calloc(1, 8);
         sprintf(str, "Bright %d", b);
         tm1638_show(tm1638, str, 0);
      } else if (keys & 32) {
         unsigned char b = tm1638_brightness_get(tm1638);
         b += 1;
         tm1638_brightness_set(tm1638, b);
         b = tm1638_brightness_get(tm1638);
         unsigned char *str = calloc(1, 8);
         sprintf(str, "Bright %d", b);
         tm1638_show(tm1638, str, 0);
         free(str);
      } else if (keys & 64) {
         freq -= tune_step_get_as_MHz(tune_step);
         unsigned char *s = freq2string(freq);
         tm1638_show(tm1638, s, 0);
         free(s);
         m110_channel_frequencies_set(m110, 1, freq, freq);
         m110_channel_frequencies_set(m110, 2, freq, freq);
      } else if (keys & 128) {
         freq += tune_step_get_as_MHz(tune_step);
         unsigned char *s = freq2string(freq);
         tm1638_show(tm1638, s, 0);
         free(s);
         m110_channel_frequencies_set(m110, 1, freq, freq);
         m110_channel_frequencies_set(m110, 2, freq, freq);
      } 
      gpio_put(LED_PIN, 0);
      sleep_ms(50);
   }
   return 0;
}
