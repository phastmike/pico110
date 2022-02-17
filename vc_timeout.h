/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * vc_timeout.h
 * 
 * A view controller for pico m110
 * 
 *
 * José Miguel Fonte
 */

#ifndef _VC_TIMEOUT_H_
#define _VC_TIMEOUT_H_

#include "hmi.h"
#include "radio.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _vc_timeout_t vc_timeout_t;

vc_timeout_t * vc_timeout_new(hmi_t *hmi, radio_t *radio);

#ifdef __cplusplus
}
#endif

#endif //_VC_TUNE_STEP_H_
