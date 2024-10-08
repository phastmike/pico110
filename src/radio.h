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

#define RADIO_NUMBER_OF_CHANNELS    20

/* Could find a finer solution for the variants
 * Or fine tune for each end binary generated, 
 * whatever suits, Frequency in MHz 
 */
#define RADIO_FREQ_MIN              ((double) 430.000)
#define RADIO_FREQ_MAX              ((double) 450.000)

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
void              radio_init(radio_t *radio);
radio_channel_t * radio_get_vfo(radio_t *radio);
void              radio_set_vfo(radio_t *radio, radio_channel_t *radio_channel);
radio_channel_t * radio_get_active_channel(radio_t *radio);
void              radio_set_active_channel(radio_t *radio, radio_channel_t *radio_channel);
radio_mode_t      radio_get_mode(radio_t *radio);
void              radio_set_mode(radio_t *radio, radio_mode_t mode);
void              radio_commit_radio_channel(radio_t *radio, radio_channel_t *radio_channel);
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
void              radio_scan_set(radio_t *radio, bool scan);
bool              radio_scan_get(radio_t *radio);
void              radio_get_size(void);

#ifdef __cplusplus
}
#endif

#endif //_RADIO_H_
