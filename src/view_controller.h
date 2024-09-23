/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * view_controller.h
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */


#ifndef _VIEW_CONTROLLER_H_
#define _VIEW_CONTROLLER_H_

#define VIEW_CONTROLLER(x) ((view_controller_t *) x)

#include "hmi.h"
#include "radio.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _view_controller_t view_controller_t;
typedef void (*view_controller_present_cb_t)(view_controller_t *vc);
typedef void (*view_controller_exit_with_key_cb_t)(view_controller_t *vc, hmi_key_t *key);

struct _view_controller_t {
   hmi_t *    hmi;
   radio_t *  radio;
   
   void (*show)(view_controller_t *vc);
   void (*present)(view_controller_t *vc);
   void (*exit_with_key)(view_controller_t *vc, hmi_key_t *key);
};

view_controller_t *  view_controller_new(hmi_t *hmi, radio_t *radio);
void                 view_controller_destroy(view_controller_t *vc);
void                 view_controller_init(view_controller_t *vc, hmi_t *hmi, radio_t *radio);
void                 view_controller_show(view_controller_t *vc);
void                 view_controller_present(view_controller_t *vc);
void                 view_controller_exit_with_key_connect(view_controller_t *vc, view_controller_exit_with_key_cb_t callback); 

#ifdef __cplusplus
}
#endif

#endif //_VIEW_CONTROLLER_H_
