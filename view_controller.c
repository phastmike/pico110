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
   vc->hmi = hmi;
   vc->radio = radio;
   vc->present = NULL;
   vc->exit_with_key = NULL;

   return vc;
}

void view_controller_destroy(view_controller_t *vc) {
   assert(vc != NULL);
   free(vc);
}

void view_controller_present(view_controller_t *vc) {
   assert(vc != NULL);
   if (vc->present) vc->present(vc);
}

void view_controller_exit_with_key_connect(view_controller_t *vc, view_controller_exit_with_key_cb_t callback) {
   assert(vc != NULL);
   vc->exit_with_key = callback;
}
