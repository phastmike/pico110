/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * hni.c
 * 
 * Human Machine Interface 
 * 
 *
 * José Miguel Fonte
 */

#include "tm1638.h"
#include "hmi.h"
#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>

struct _hmi_t {
   tm1638_t       *  tm1638;
   unsigned char     led_status;
   unsigned char     key_status;
   hmi_key_t      *  key[HMI_NUMBER_OF_KEYS];
};

hmi_t * hmi_new(void) {
   hmi_t *hmi = (hmi_t *) calloc(1, sizeof(hmi_t));
   assert(hmi != NULL);
   hmi->tm1638 = tm1638_new(1, 2, 3, 1);
   hmi->led_status = 0x0;
   hmi->key_status = 0x0;
   for (int i = 0; i < HMI_NUMBER_OF_KEYS; i++) {
      hmi->key[i] = hmi_key_new((hmi_key_id_t) (1 << i));
   }

   return hmi;
}

void hmi_destroy(hmi_t *hmi) {
   assert(hmi != NULL);

   for (int i = 0; i < HMI_NUMBER_OF_KEYS; i++) {
      hmi_key_destroy(hmi->key[i]);
   }

   free(hmi);
}

unsigned char hmi_display_get_enabled(hmi_t *hmi) {
   assert(hmi != NULL);

   return tm1638_display_enabled_get(hmi->tm1638);
}

void hmi_display_set_enabled(hmi_t *hmi, unsigned char enabled) {
    assert(hmi != NULL); 

    tm1638_display_enabled_set(hmi->tm1638, enabled);
}

unsigned char hmi_display_get_brightness(hmi_t *hmi) {
   assert(hmi != NULL);

   return tm1638_brightness_get(hmi->tm1638);
}

void hmi_display_set_brightness(hmi_t *hmi, unsigned char brightness) {
   assert(hmi != NULL);

   tm1638_brightness_set(hmi->tm1638, brightness);
}

void hmi_display_text(hmi_t *hmi, unsigned char pos, char *text) {
   assert(hmi != NULL);

   tm1638_show(hmi->tm1638, pos, text);
}

void hmi_display_text_scroll(hmi_t *hmi, char *text, unsigned char speed, unsigned char direction) {
   assert(hmi != NULL);

   // Think something here
}

void hmi_display_text_clear(hmi_t *hmi) {
   assert (hmi != NULL);
   
   tm1638_clear(hmi->tm1638);
}

/* Explain how read_keys, id, indices and values relate */

unsigned char hmi_keys_scan(hmi_t *hmi) {
   assert(hmi != NULL);

   unsigned char keys_read;

   // Do some debounce
   keys_read = tm1638_keys(hmi->tm1638);
   
   if (keys_read == hmi->key_status) return keys_read;

   sleep_ms(80);

   keys_read = tm1638_keys(hmi->tm1638);

   if (keys_read != hmi->key_status) {
      hmi->key_status = keys_read;
      for (int i = 0; i < HMI_NUMBER_OF_KEYS; i++) {
         hmi_key_set_active(hmi->key[i], (keys_read >> i) & 1);
      }
   }

   return keys_read;
}

void hmi_led_set(hmi_t *hmi, unsigned char pos, hmi_led_status_t status) {
   assert(hmi != NULL);

   unsigned char led_status;

   if (status) {
      led_status = (1 << pos);
      hmi->led_status |= led_status;
   } else {
      led_status = (1 << pos);
      hmi->led_status &= ~led_status;
   }
   tm1638_led(hmi->tm1638, pos, status);
}

void hmi_leds_set(hmi_t *hmi, unsigned char status) {
   assert(hmi != NULL);
   hmi->led_status = status;
   tm1638_leds(hmi->tm1638, status);
}

hmi_key_t *hmi_get_key(hmi_t *hmi, hmi_key_id_t id) {
   assert(hmi != NULL);

   /*
   if (id < 0 || id >= HMI_NUMBER_OF_KEYS) return NULL;

   return hmi->key[id];
   */
   
   for (int i = 0; i < HMI_NUMBER_OF_KEYS; i++) {
      if (hmi_key_get_id(hmi->key[i]) == id) {
         return hmi->key[i];
      }
   }

   return NULL;
}

void hmi_keys_disconnect(hmi_t *hmi) {
   assert(hmi != NULL);

   for (int i = 0; i < HMI_NUMBER_OF_KEYS; i++) {
      hmi_key_on_press_event_disconnect(hmi->key[i]);
      hmi_key_on_release_event_disconnect(hmi->key[i]);
   }
}
