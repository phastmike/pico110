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


//#include "eeprom.h"
#include "m110.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include <stdio.h>

// define I2C addresses to be used for this peripheral
#define I2C0_PERIPHERAL_ADDR 0x50

// GPIO pins to use for I2C
#define GPIO_SDA0 12
#define GPIO_SCK0 13

// GPIO SWITCH
#define GPIO_SWITCH  9
#define GPIO_SWITCH2 8

// ADDED LED PIN
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

// ram_addr is the current address to be used when writing / reading the RAM
// N.B. the address auto increments, as stored in 8 bit value it automatically rolls round when reaches 255
uint8_t ram_addr = 0;

// ram is the storage for the RAM data
uint8_t ram[128] = {
   0x52, 0x53, 0x41, 0x31, 0x35, 0x32, 0x35, 0x45, 
   0x5a, 0x41, 0xdf, 0x02, 0x00, 0x00, 0x00, 0x67,
   0x58, 0x00, 0x50, 0x16, 0x81, 0x12, 0x01, 0x02,
   0x21, 0x02, 0x20, 0x02, 0x7c, 0xe6, 0x41, 0xa2,
   0x00, 0x00, 0x4e, 0x32, 0x6c, 0x02, 0x7c, 0xe6, 
   0x41, 0xa2, 0x13, 0x06, 0x4e, 0x32, 0x7c, 0x00,
};

// INIT EEPROM WITH CONTENTS
m110_t   *m110;

double   freq = 460.0;


// Interrupt handler implements the RAM
void i2c0_irq_handler0() {

    // Get interrupt status
    uint32_t status = i2c0->hw->intr_stat;

    // Check to see if we have received data from the I2C controller
    if (status & I2C_IC_INTR_STAT_R_RX_FULL_BITS) {

        // Read the data (this will clear the interrupt)
        uint32_t value = i2c0->hw->data_cmd;

        // Check if this is the 1st byte we have received
        if (value & I2C_IC_DATA_CMD_FIRST_DATA_BYTE_BITS) {

            // If so treat it as the address to use
            ram_addr = (uint8_t)(value & I2C_IC_DATA_CMD_DAT_BITS);

        } else {
            // If not 1st byte then store the data in the RAM
            // and increment the address to point to next byte
            ram[ram_addr] = (uint8_t)(value & I2C_IC_DATA_CMD_DAT_BITS);
            ram_addr++;
        }
    }

    // Check to see if the I2C controller is requesting data from the RAM
    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {

        // Write the data from the current address in RAM
        i2c0->hw->data_cmd = (uint32_t)ram[ram_addr];

        // Clear the interrupt
        i2c0->hw->clr_rd_req;

        // Increment the address
        ram_addr++;
    }
}


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

void gpio_callback(uint gpio, uint32_t events) {
    bool active;
    double new_freq;
    char dir_up[]="UP";
    char dir_down[]="DOWN";
    char *dir = NULL;

    //printf("** events = %d | gpioactive = %d\n", events, active);
    
    if ((events & 4) != 0 ) { // Low Edge
       // debounce
       uint64_t t1 = time_us_32();
       uint64_t t2 = t1;
       while (t2 - t1 < 100000) {
         t2 = time_us_32();
       }
       
       if (gpio == GPIO_SWITCH) {
          active = !gpio_get(GPIO_SWITCH);
          new_freq = freq+0.0125;
          dir = dir_up;
       } else if (gpio == GPIO_SWITCH2) {
          active = !gpio_get(GPIO_SWITCH2);
          new_freq = freq-0.0125;
          dir = dir_down;
       } else {
         return;
       }

       if (active) {
          freq = new_freq;
          printf("%s: freq = %.4f MHz\n", dir, freq);
          m110_channel_frequencies_set(m110, 1, freq, freq);
          m110_channel_frequencies_set(m110, 2, freq, freq);
       }
    }
}

// Main loop - initilises system and then loops while interrupts get on with processing the data
int main() {
    m110 = m110_new_with_data(ram);

    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(GPIO_SWITCH);
    gpio_set_dir(GPIO_SWITCH, GPIO_IN);
    gpio_pull_up(GPIO_SWITCH);

    gpio_set_irq_enabled_with_callback(GPIO_SWITCH, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    // 2
    gpio_init(GPIO_SWITCH2);
    gpio_set_dir(GPIO_SWITCH2, GPIO_IN);
    gpio_pull_up(GPIO_SWITCH2);

    gpio_set_irq_enabled_with_callback(GPIO_SWITCH2, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);


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
    while (true) {
        tight_loop_contents();

        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
    return 0;
}
