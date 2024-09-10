/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * radio_channel.c
 * 
 * TM1638 LED+KEY Module
 * 
 *
 * José Miguel Fonte
 */

#ifndef _RADIO_CHANNEL_H_
#define _RADIO_CHANNEL_H_

#include "m110.h"
#include "ctcss.h"
#include "tune_step.h"

#define RADIO_CHANNEL(x) ((radio_channel_t *) x)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   DUP_DOWN  = -1,
   DUP_OFF   =  0,
   DUP_UP    =  1
} dup_t;

typedef enum {
   REV_OFF = 0,
   REV_ON  = 1
} rev_t;

typedef struct _radio_channel_t radio_channel_t;

struct _radio_channel_t {
   double            freq_rx;
   double            freq_tx;
   ctcss_t *         ctcss_rx;
   ctcss_t *         ctcss_tx;
   double            shift;     // rx-tx
   dup_t             dup;
   rev_t             rev;
   tx_admit_t        tx_admit;
   unsigned char     low_power;
   tune_step_t *     tune_step;
};

radio_channel_t * radio_channel_new(void);
void              radio_channel_destroy(radio_channel_t *radio_channel);
void              radio_channel_init(radio_channel_t *radio_channel);
void              radio_channel_copy(radio_channel_t *dest, radio_channel_t *src);
double            radio_channel_freq_rx_get(radio_channel_t *radio_channel);
void              radio_channel_freq_rx_set(radio_channel_t *radio_channel, double freq);
double            radio_channel_freq_tx_get(radio_channel_t *radio_channel);
void              radio_channel_freq_tx_set(radio_channel_t *radio_channel, double freq);
void              radio_channel_freq_set(radio_channel_t *radio_channel, double freq);
ctcss_t *         radio_channel_ctcss_rx_get(radio_channel_t *radio_channel);
void              radio_channel_ctcss_rx_set(radio_channel_t *radio_channel, ctcss_tone_t tone);
ctcss_t *         radio_channel_ctcss_tx_get(radio_channel_t *radio_channel);
void              radio_channel_ctcss_tx_set(radio_channel_t *radio_channel, ctcss_tone_t tone);
double            radio_channel_shift_get(radio_channel_t *radio_channel);
void              radio_channel_shift_set(radio_channel_t *radio_channel, double shift);
dup_t             radio_channel_dup_get(radio_channel_t *radio_channel);
void              radio_channel_dup_set(radio_channel_t *radio_channel, dup_t dup);
tx_admit_t        radio_channel_tx_admit_get(radio_channel_t *radio_channel);
void              radio_channel_tx_admit_set(radio_channel_t *radio_channel, tx_admit_t txadmit);
unsigned char     radio_channel_low_power_get(radio_channel_t *radio_channel);
void              radio_channel_low_power_set(radio_channel_t *radio_channel, unsigned char low_power);
tune_step_t *     radio_channel_tune_step_get(radio_channel_t *radio_channel);
unsigned int      radio_channel_get_size(void);
rev_t             radio_channel_get_rev(radio_channel_t *radio_channel);
void              radio_channel_set_rev(radio_channel_t *radio_channel, rev_t rev);

#ifdef __cplusplus
}
#endif

#endif //_RADIO_CHANNEL_H_
