/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * i2c0.c
 * 
 * I2C Peripheral Controller0 routines
 *
 *
 * José Miguel Fonte
 */

#include "m110.h"
#include <pico/stdlib.h>
#include "hardware/i2c.h"
#include "hardware/irq.h"

// define I2C addresses to be used for this peripheral
#define I2C0_PERIPHERAL_ADDR 0x50

// GPIO pins to use for I2C
#define GPIO_SDA0 12
#define GPIO_SCK0 13

extern m110_t *m110;

void i2c0_irq_handler(void) {

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

void i2c0_init(unsigned char clock_streching_enabled) {
   // Setup I2C0 as slave (peripheral)
   i2c_init(i2c0, 100 * 1000);
   i2c_set_slave_mode(i2c0, true, I2C0_PERIPHERAL_ADDR);

   // Enable slave clock stretching
   if (clock_streching_enabled) {
      i2c0->hw->enable = 0;
      hw_set_bits(&i2c0->hw->con, I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_BITS);
      i2c0->hw->enable = 1;
   }

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
}
