/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * key.c
 * 
 * Represents a key
 * 
 *
 * José Miguel Fonte
 */

#include "key.h"
#include <stdlib.h>
#include <pico/stdlib.h>

struct _hmi_key_t {
   hmi_key_id_t id;
   unsigned char active;
   void (*on_key_press_event)(hmi_key_t *key, hmi_key_id_t key_id, void *user_data);;
   void *on_press_user_data;
   void (*on_key_release_event)(hmi_key_t *key, hmi_key_id_t key_id, void *user_data);;
   void *on_release_user_data;
};

hmi_key_t * hmi_key_new(hmi_key_id_t id) {
   hmi_key_t *key = (hmi_key_t *) calloc(1, sizeof(hmi_key_t));
   assert(key != NULL);
   key->id = id;
   key->active = 0;

   return key;
}

void hmi_key_destroy(hmi_key_t *key) {
   assert(key != NULL);
   hmi_key_on_press_event_disconnect(key);
   hmi_key_on_release_event_disconnect(key);
   if (key) free(key);
}

hmi_key_id_t hmi_key_get_id(hmi_key_t *key) {
   assert(key != NULL);
   return key->id;
}

unsigned char hmi_key_get_active(hmi_key_t *key) {
   assert(key != NULL);
   return key->active != 0;
}

void hmi_key_set_active(hmi_key_t *key, unsigned char active) {
   assert(key != NULL);
   if (active != 0 && key->active == 0) {
      key->active = active;
      if (key->on_key_press_event) {
         key->on_key_press_event(key, key->id, key->on_press_user_data);
      }
   } else if (active == 0 && key->active != 0) {
      key->active = active;
      if (key->on_key_release_event) {
         key->on_key_release_event(key, key->id, key->on_release_user_data);
      }
   }
}

void hmi_key_on_press_event_connect(hmi_key_t *key, hmi_key_on_event_cb_t callback, void *user_data) {
   assert(key != NULL);
   key->on_key_press_event = callback;
   key->on_press_user_data = user_data;
}

void hmi_key_on_press_event_disconnect(hmi_key_t *key) {
   assert(key != NULL);
   key->on_key_press_event = NULL;
   key->on_press_user_data = NULL;
}

void hmi_key_on_release_event_connect(hmi_key_t *key, hmi_key_on_event_cb_t callback, void *user_data) {
   assert(key != NULL);
   key->on_key_release_event = callback;
   key->on_release_user_data = user_data;
}

void hmi_key_on_release_event_disconnect(hmi_key_t *key) {
   assert(key != NULL);
   key->on_key_release_event = NULL;
   key->on_release_user_data = NULL;
}
