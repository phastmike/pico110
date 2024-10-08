/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * view_controller.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdlib.h>
#include "view_controller.h"

#include <stdio.h>

view_controller_t *view_controller_new(hmi_t *hmi, radio_t *radio) {
   view_controller_t *vc = (view_controller_t *) calloc (1, sizeof(view_controller_t));
   view_controller_init(vc, hmi, radio);
   return vc;
}

void view_controller_destroy(view_controller_t *vc) {
   assert(vc != NULL);
   free(vc);
}

void view_controller_init(view_controller_t *vc, hmi_t *hmi, radio_t *radio) {
   assert(vc != NULL);
   assert(hmi != NULL && radio != NULL); // redundant*

   vc->hmi = hmi;
   vc->radio = radio;
   vc->present = NULL;
   vc->exit_with_key = NULL;
}

void view_controller_show(view_controller_t *vc) {
   assert(vc != NULL);
   if (vc->show) vc->show(vc);
}

void view_controller_present(view_controller_t *vc) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi);
   if (vc->present) vc->present(vc);
}

void view_controller_leave(view_controller_t *vc, hmi_key_t *key) {
   assert(vc != NULL);
   hmi_keys_disconnect(vc->hmi); // redundant here or at present - rule out
   if (vc->exit_with_key) vc->exit_with_key(vc, key);
} 

void view_controller_exit_with_key_connect(view_controller_t *vc, view_controller_exit_with_key_cb_t callback) {
   assert(vc != NULL);
   vc->exit_with_key = callback;
}
