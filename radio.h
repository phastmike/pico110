/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * radio.h
 * 
 * A radio with all parameters 
 *
 * José Miguel Fonte
 */

#ifndef _RADIO_H_
#define _RADIO_H_

#include "m110.h"
#include "ctcss.h"
#include "radio_channel.h"

#define RADIO(x)                    ((radio_t *) x)

#define RADIO_NUMBER_OF_CHANNELS    40

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   RADIO_MODE_VFO,
   RADIO_MODE_MEMORY,
   RADIO_MODE_FUNC,
   RADIO_MODE_NMODES
} radio_mode_t;

typedef struct _radio_t radio_t;

radio_t *         radio_new(void);
radio_t *         radio_new_with_defaults(void);
m110_t *          radio_get_m110(radio_t *radio);
radio_channel_t * radio_get_active_channel(radio_t *radio);
void              radio_set_active_channel(radio_t *radio, radio_channel_t *radio_channel);
radio_mode_t      radio_get_mode(radio_t *radio);
void              radio_set_mode(radio_t *radio, radio_mode_t mode);
void              radio_commit_radio_channel(radio_t *radio, radio_channel_t *radio_channel);
ctcss_t *         radio_get_ctcss(radio_t *radio);
void              radio_set_ctcss(radio_t *radio, double ctcss);
char *            radio_tune_step_get(radio_t *radio);
void              radio_tune_step_down(radio_t *radio);
void              radio_tune_step_up(radio_t *radio);
void              radio_timeout_down(radio_t *radio);
void              radio_timeout_up(radio_t *radio);
void              radio_rekey_down(radio_t *radio);
void              radio_rekey_up(radio_t *radio);
void              radio_tx_admit_down(radio_t *radio);
void              radio_tx_admit_up(radio_t *radio);
void              radio_radio_channel_down(radio_t *radio);
void              radio_radio_channel_up(radio_t *radio);
void              radio_dup_down(radio_t *radio);
void              radio_dup_up(radio_t *radio);
void              radio_get_size(void);

#ifdef __cplusplus
}
#endif

#endif //_RADIO_H_
