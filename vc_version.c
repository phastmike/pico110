/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_version.c
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "hmi_key.h"
#include "vc_version.h"
#include "view_controller.h"

struct _vc_version_t {
   view_controller_t *vc;
};

void vc_version_present(view_controller_t *vc);

vc_version_t *vc_version_new(hmi_t *hmi, radio_t *radio) {
   vc_version_t *vc_version = (vc_version_t *) view_controller_new(hmi, radio);
   VIEW_CONTROLLER(vc_version)->present = vc_version_present;
   return vc_version;
}

void vc_version_show(view_controller_t *vc) {
   char *string = (char *) calloc (1,9);
   sprintf(string, "Rel  0.1.5");
   hmi_display_text(vc->hmi, 0, string);
   free(string);
}

/* EVENTS */

void vc_version_on_press_down_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //vc_version_show(VIEW_CONTROLLER(user_data));
}

void vc_version_on_press_up_event(hmi_key_t *key, hmi_key_id_t key_id, void *user_data) {
   //vc_version_show(VIEW_CONTROLLER(user_data));
}

/* VIEW CONTROLLER present method */

void vc_version_present(view_controller_t *vc) {
   hmi_key_t *key = hmi_get_key(vc->hmi, HMI_KEY_7);
   hmi_key_on_press_event_connect(key, vc_version_on_press_down_event, vc);
   key = hmi_get_key(vc->hmi, HMI_KEY_8);
   hmi_key_on_press_event_connect(key, vc_version_on_press_up_event, vc);
   vc_version_show(vc);
}
