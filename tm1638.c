/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * tm1638.c
 * 
 * TM1638 LED+KEY Module
 * 
 *
 * José Miguel Fonte
 */

#include "tm1638.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pico/stdlib.h>

#define TM1638_DATA_CMD    0x40
#define TM1638_ADDR_CMD    0xC0
#define TM1638_CTRL_CMD    0x80

#define TM1638_DISPL_ON    0x08
#define TM1638_READ_KEY    0x02
#define TM1638_FIX_ADDR    0x04

#define DWAIT              1

unsigned char tm1638_font[] = {
   0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,
   0x5E,0x79,0x71,0x3D,0x76,0x06,0x1E,0x76,0x38,0x55,0x54,0x3F,0x73,
   0x67,0x50,0x6D,0x78,0x3E,0x1C,0x2A,0x76,0x6E,0x5B,0x00,0x40,0x63
};

struct _tm1638_t {
   unsigned char on;
   unsigned char stb;
   unsigned char clk;
   unsigned char dio;
   unsigned char brightness; 
};

void tm1638_write_ctrl_cmd(tm1638_t *tm1638);

tm1638_t *tm1638_new(unsigned char stb, unsigned char clk, unsigned char dio, unsigned char brightness) {
   if (!(brightness >= 1 && brightness <= 8)) return NULL;

   tm1638_t *tm1638 = (tm1638_t *) calloc(1, sizeof(tm1638_t));

   tm1638->on = TM1638_DISPL_ON;
   tm1638->stb = stb;
   tm1638->clk = clk;
   tm1638->dio = dio;
   tm1638->brightness = brightness;

   gpio_init(tm1638->stb);
   gpio_set_dir(tm1638->stb, GPIO_OUT);
   gpio_init(tm1638->clk);
   gpio_set_dir(tm1638->clk, GPIO_OUT);
   gpio_init(tm1638->dio);
   gpio_set_dir(tm1638->dio, GPIO_OUT);

   gpio_put(tm1638->stb, 1);
   gpio_put(tm1638->clk, 1);
   gpio_put(tm1638->dio, 0);

   tm1638_clear(tm1638);
   tm1638_write_ctrl_cmd(tm1638);

   return tm1638;
}

void tm1638_destroy(tm1638_t *tm1638) {
   if (tm1638 != NULL) {
      free(tm1638);
   }
}

void tm1638_byte(tm1638_t *tm1638, unsigned char byte) {
   unsigned char i;
   
   for (i = 0; i < 8; i++) {
      gpio_put(tm1638->clk, 0);
      sleep_us(DWAIT);
      gpio_put(tm1638->dio, (byte >> i) & 1);
      sleep_us(DWAIT);
      gpio_put(tm1638->clk, 1);
   }
}

void tm1638_command(tm1638_t *tm1638, unsigned char cmd) { // convert to typedef enum
   gpio_put(tm1638->stb, 0);
   sleep_us(DWAIT);
   tm1638_byte(tm1638, cmd);
   sleep_us(DWAIT);
   gpio_put(tm1638->stb, 1);
}

void tm1638_write_data_cmd(tm1638_t *tm1638) {
   tm1638_command(tm1638, TM1638_DATA_CMD);
}

void tm1638_set_address(tm1638_t *tm1638, unsigned char addr) {
   // Check range for addr
   tm1638_byte(tm1638, TM1638_ADDR_CMD | addr);
}


void tm1638_write_ctrl_cmd(tm1638_t *tm1638) {
   tm1638_command(tm1638, TM1638_CTRL_CMD | tm1638->on | tm1638->brightness);
}

unsigned char tm1638_display_enabled_get(tm1638_t *tm1638) {
   return tm1638->on == TM1638_DISPL_ON;
}

void tm1638_display_enabled_set(tm1638_t *tm1638, unsigned char enabled) {
   if (enabled != 0) {
      tm1638->on = TM1638_DISPL_ON;
   } else {
      tm1638->on = 0;
   }
   tm1638_write_ctrl_cmd(tm1638);
}

unsigned char tm1638_brightness_get(tm1638_t *tm1638) {
   return tm1638->brightness + 1;
}

void tm1638_brightness_set(tm1638_t *tm1638, unsigned char brightness) {
   if (!(brightness >=1 && brightness <=8)) return;

   tm1638->brightness = brightness - 1;
   tm1638_write_ctrl_cmd(tm1638);
}

unsigned char tm1638_scan_keys(tm1638_t *tm1638) {
   unsigned char i;
   unsigned char pressed = 0;

   // need to init pin?
   gpio_set_dir(tm1638->dio, GPIO_IN);
   gpio_pull_up(tm1638->dio);
   for (i = 0; i < 8; i++) {
      gpio_put(tm1638->clk, 0);
      sleep_us(DWAIT);
      if (gpio_get(tm1638->dio)) {
         pressed |= 1 << i;
      }
      //sleep_us(DWAIT);
      gpio_put(tm1638->clk, 1);
   }
   gpio_set_dir(tm1638->dio, GPIO_OUT);
   return pressed;
}

void tm1638_clear(tm1638_t *tm1638) {
   unsigned char i;

   tm1638_write_data_cmd(tm1638);
   gpio_put(tm1638->stb, 0);
   sleep_us(DWAIT);
   tm1638_set_address(tm1638, 0);
   for(i = 0; i < 16; i++) {
      tm1638_byte(tm1638, 0x00);
   } 
   sleep_us(DWAIT);
   gpio_put(tm1638->stb, 1);
}

void tm1638_write(tm1638_t *tm1638, unsigned char *data, unsigned int size, unsigned char pos) {
   unsigned int i;

   if (!(pos >=0 && pos <= 15)) return;

   tm1638_write_data_cmd(tm1638);
   gpio_put(tm1638->stb, 0);
   sleep_us(DWAIT);
   tm1638_set_address(tm1638, pos);
   for(i = 0; i < size; i++) {
      tm1638_byte(tm1638, *(data + i));
   } 
   sleep_us(DWAIT);
   gpio_put(tm1638->stb, 1);
}

void tm1638_led(tm1638_t *tm1638, unsigned char pos, unsigned char value) {
   tm1638_write(tm1638, &value, 1, (pos << 1) + 1);
}

void tm1638_leds(tm1638_t *tm1638, unsigned char value) {
   unsigned char i;
   unsigned char pos;

   tm1638_write_data_cmd(tm1638);
   pos = 1;
   for(i = 0; i < 8; i++) {
      gpio_put(tm1638->stb, 0);
      sleep_us(DWAIT);
      tm1638_set_address(tm1638, pos);
      tm1638_byte(tm1638, (value >> i) & 1);
      pos +=2;
      sleep_us(DWAIT);
      gpio_put(tm1638->stb, 1);
   }
}

unsigned char tm1638_keys(tm1638_t *tm1638) {
   unsigned char i;
   unsigned int keys;

   keys = 0;
   gpio_put(tm1638->stb, 0);
   sleep_us(DWAIT);
   tm1638_byte(tm1638, TM1638_DATA_CMD | TM1638_READ_KEY);
   for(i = 0; i < 4; i++) {
      keys |= tm1638_scan_keys(tm1638) << i;
   }
   //sleep_us(DWAIT);
   gpio_put(tm1638->stb, 1);
   return keys;
}

void tm1638_segments(tm1638_t *tm1638, unsigned char *segments, unsigned int size, unsigned char pos) {
   unsigned int i;
   if (!(pos >= 0 && pos <= 7)) return;

   tm1638_write_data_cmd(tm1638);
   for(i = 0; i < size; i++) {
      gpio_put(tm1638->stb, 0);
      sleep_us(DWAIT);
      tm1638_set_address(tm1638, pos << 1);
      tm1638_byte(tm1638, segments[i]);
      pos += 1;
      gpio_put(tm1638->stb, 1);
      sleep_us(DWAIT);
   }
}

unsigned char tm1638_encode_char(unsigned char byte) {
   if (byte == 32) {
      return tm1638_font[36];                // Space
   } else if (byte == 42) {
      return tm1638_font[38];                // star/degree
   } else if (byte == 45) {
      return tm1638_font[37];                // dash
   } else if (byte >= 65 && byte <= 90) {
      return tm1638_font[byte-55];           // uppercase A-Z
   } else if (byte >= 97 && byte <= 122) {
      return tm1638_font[byte-87];           // lowercase a-z
   } else if (byte >= 48 && byte <= 57) {
      return tm1638_font[byte-48];
   } else {
      return tm1638_font[36];                // Not found. (Space)
   }
}

unsigned char * tm1638_encode_string(unsigned char *string, unsigned int *new_len) {
   unsigned int i,j;
   unsigned int len;
   unsigned int nlen;
   unsigned int ndots;
   unsigned char *nstr;
   
   //calculate number of dots, nlen = len -ndots
   len = strlen(string);
   for(i = 0, ndots = 0; i < len; i++) {
      if (string[i] == '.') {
         ndots++;
      }
   }

   nlen = len - ndots;
   nstr = (unsigned char *) calloc (1, nlen);

   for (i = 0, j = 0; j < len; j++) {
      if (string[j] != '.') {
         nstr[i] = tm1638_encode_char(string[j]);
         i++;
      } else {
         nstr[i-1] |= 0x80;
      }
   }
   
   if (new_len != NULL) *new_len = nlen;

   return nstr;
}

void tm1638_show(tm1638_t *tm1638, unsigned char pos, unsigned char *string) {
   unsigned char *nstr;
   unsigned int new_len;

   if (!(pos >= 0 && pos <= 7)) return;

   nstr = tm1638_encode_string(string, &new_len);
   tm1638_segments(tm1638, nstr, new_len, pos);
   free(nstr);
}
