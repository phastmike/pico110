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

struct _view_controller_t {
   hmi_t    * hmi;
   radio_t  * radio;
   
   void (*present)(view_controller_t *vc);
};

view_controller_t *  view_controller_new(hmi_t *hmi, radio_t *radio);
void                 view_controller_present(view_controller_t *vc);


#ifdef __cplusplus
}
#endif

#endif //_VIEW_CONTROLLER_H_
