/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * radio_channel.c
 * 
 * A radio frequency/channel with all parameters 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "radio_channel.h"

#define RADIO_CHANNEL_DEFAULT_RX_FREQUENCY   446.00625
#define RADIO_CHANNEL_DEFAULT_TX_FREQUENCY   446.00625

/*
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
*/

radio_channel_t * radio_channel_new(void) {
   radio_channel_t *radio_channel = (radio_channel_t *) calloc(1,sizeof(radio_channel_t)); 

   radio_channel_set_defaults(radio_channel);

   /*
   radio_channel->freq_rx = RADIO_CHANNEL_DEFAULT_RX_FREQUENCY;
   radio_channel->freq_tx = RADIO_CHANNEL_DEFAULT_TX_FREQUENCY;
   radio_channel->ctcss_rx = ctcss_new();
   radio_channel->ctcss_tx = ctcss_new();
   radio_channel->shift = 7.6;                  // radio_channel->freq_rx - radio_channel->freq_tx; 
   radio_channel->dup = DUP_OFF; 
   radio_channel->rev = REV_OFF;
   radio_channel->tx_admit = TXADMIT_ALWAYS; 
   radio_channel->low_power = 0;
   radio_channel->tune_step = tune_step_new();
   //tune_step_next(radio_channel->tune_step);    // Set ts as 25kHz
   */

   return radio_channel;
}

void radio_channel_destroy(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   if (radio_channel->ctcss_rx)  ctcss_destroy(radio_channel->ctcss_rx);
   if (radio_channel->ctcss_tx)  ctcss_destroy(radio_channel->ctcss_tx);
   if (radio_channel->tune_step) tune_step_destroy(radio_channel->tune_step);
   free(radio_channel);
}

void radio_channel_set_defaults(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   radio_channel->freq_rx = RADIO_CHANNEL_DEFAULT_RX_FREQUENCY;
   radio_channel->freq_tx = RADIO_CHANNEL_DEFAULT_TX_FREQUENCY;
   radio_channel->ctcss_rx = ctcss_new();
   radio_channel->ctcss_tx = ctcss_new();
   radio_channel->shift = 7.6;                  // radio_channel->freq_rx - radio_channel->freq_tx; 
   radio_channel->dup = DUP_OFF; 
   radio_channel->rev = REV_OFF;
   radio_channel->tx_admit = TXADMIT_ALWAYS; 
   radio_channel->low_power = 0;
   radio_channel->tune_step = tune_step_new();
   //tune_step_next(radio_channel->tune_step);    // Set ts as 25kHz
}

void radio_channel_copy(radio_channel_t *dest, radio_channel_t *src) {
   assert(dest != NULL && src != NULL);
   *(dest) = *(src);
}

double radio_channel_freq_rx_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->freq_rx;
}

void radio_channel_freq_rx_set(radio_channel_t *radio_channel, double freq) {
   assert(radio_channel != NULL);
   radio_channel->freq_rx = freq; 
}

double radio_channel_freq_tx_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->freq_tx;
}

void radio_channel_freq_tx_set(radio_channel_t *radio_channel, double freq) {
   assert(radio_channel != NULL);
   radio_channel->freq_tx = freq; 
}

static void radio_channel_freq_tx_recalc(radio_channel_t * radio_channel) {
   assert(radio_channel != NULL);

   switch(radio_channel_dup_get(radio_channel)) {
      case DUP_DOWN:
         radio_channel_freq_tx_set(radio_channel, radio_channel_freq_rx_get(radio_channel) - radio_channel_shift_get(radio_channel));
         break;
      case DUP_UP:
         radio_channel_freq_tx_set(radio_channel, radio_channel_freq_rx_get(radio_channel) + radio_channel_shift_get(radio_channel));
         break;
      case DUP_OFF:
      default:
         radio_channel_freq_tx_set(radio_channel, radio_channel_freq_rx_get(radio_channel));
         break;
   }
}

void radio_channel_freq_set(radio_channel_t *radio_channel, double freq) {
   assert(radio_channel != NULL);

   radio_channel_freq_rx_set(radio_channel, freq);

   radio_channel_freq_tx_recalc(radio_channel);
   /*
   switch(radio_channel->dup) {
      case DUP_DOWN:
         radio_channel_freq_tx_set(radio_channel, freq - radio_channel->shift);
         break;
      case DUP_UP:
         radio_channel_freq_tx_set(radio_channel, freq + radio_channel->shift);
         break;
      case DUP_OFF:
      default:
         radio_channel_freq_tx_set(radio_channel, freq);
         break;
   }
   */
}

ctcss_t *radio_channel_ctcss_rx_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->ctcss_rx;
}

void radio_channel_ctcss_rx_set(radio_channel_t *radio_channel, ctcss_tone_t tone) {
   assert(radio_channel != NULL);
   //verify if valid tone !?
   ctcss_set_tone(radio_channel->ctcss_rx, tone); 
}

ctcss_t *radio_channel_ctcss_tx_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->ctcss_tx;
}

void radio_channel_ctcss_tx_set(radio_channel_t *radio_channel, ctcss_tone_t tone) {
   assert(radio_channel != NULL);
   //verify if valid tone !?
   ctcss_set_tone(radio_channel->ctcss_tx, tone); 
}

double radio_channel_shift_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->shift;
}

void radio_channel_shift_set(radio_channel_t *radio_channel, double shift) {
   assert(radio_channel != NULL);
   if (shift >= 0 && shift <= 10) {
      radio_channel->shift = shift;
      radio_channel_freq_tx_recalc(radio_channel);
   }
}

dup_t radio_channel_dup_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->dup;
}

void radio_channel_dup_set(radio_channel_t *radio_channel, dup_t dup) {
   assert(radio_channel != NULL);
   radio_channel->dup = dup;
   radio_channel_freq_tx_recalc(radio_channel);
}

tx_admit_t radio_channel_tx_admit_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->tx_admit;
}

void radio_channel_tx_admit_set(radio_channel_t *radio_channel, tx_admit_t txadmit) {
   assert(radio_channel != NULL);
   // could validate txadmit
   radio_channel->tx_admit = txadmit;
}

unsigned char radio_channel_low_power_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->low_power;
}

void radio_channel_low_power_set(radio_channel_t *radio_channel, unsigned char low_power) {
   assert(radio_channel != NULL);
   assert(low_power == 0 || low_power == 1);

   radio_channel->low_power = low_power;
}

tune_step_t *radio_channel_tune_step_get(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->tune_step;
}

unsigned int radio_channel_get_size(void) {
   return sizeof(radio_channel_t);
}

rev_t radio_channel_get_rev(radio_channel_t *radio_channel) {
   assert(radio_channel != NULL);
   return radio_channel->rev;
}

void radio_channel_set_rev(radio_channel_t *radio_channel, rev_t rev) {
   assert(radio_channel != NULL);
   assert (rev == REV_OFF || rev == REV_ON);

   rev_t rc_rev = radio_channel_get_rev(radio_channel);

   // may have problem here if channel has shift but no dup?
   if (rc_rev != rev && radio_channel_shift_get(radio_channel) != 0 && radio_channel_dup_get(radio_channel) != DUP_OFF) {
      radio_channel->rev = rev;
      radio_channel_freq_set(radio_channel, radio_channel_freq_tx_get(radio_channel)); // Must set frequency before setting dup
      if (radio_channel_dup_get(radio_channel) == DUP_UP) {
         radio_channel_dup_set(radio_channel, DUP_DOWN);
      } else {
         radio_channel_dup_set(radio_channel, DUP_UP);
      }
   }
}
