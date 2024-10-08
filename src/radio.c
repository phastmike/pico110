/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * radio.c
 * 
 * A radio with all parameters 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "radio.h"
#include "memory.h"
#include "rom_init.h"
#include "memory_channel.h"

struct _radio_t {
   m110_t m110;
   radio_channel_t *vfo;
   radio_channel_t *ch_ptr;   // points to working radio_channel, either vfo or a memory
   radio_mode_t mode;
   unsigned char memory_selected;
   memory_channel_t *memory[RADIO_NUMBER_OF_CHANNELS];
   bool scan_enabled;
   memory_t *mem;
};

radio_t *radio_new(void) {
   radio_t *r = (radio_t *) calloc (1, sizeof(radio_t));
   radio_init(r);
   return r;
}

void radio_init(radio_t *radio) {
   m110_init(M110(radio), rom_init);

   // Many of the following values can be backed up
   // so that the working state is preserved after power off

   radio_set_vfo(radio, radio_channel_new()); // Comes with defaults
   radio->mem = memory_new();
   memory_init(radio->mem);
   radio_set_mode(radio, RADIO_MODE_VFO);
   
   // Defaults to VFO;
   // also commits to m110

   radio->scan_enabled = false;
}

void radio_destroy(radio_t *radio) {
   assert(radio != NULL);

   //if (radio->m110) m110_destroy(radio->m110);
   if (radio->vfo)  radio_channel_destroy(radio->vfo);
   for (int i = 0; i < RADIO_NUMBER_OF_CHANNELS; i++) {
      if (radio->memory[i]) {
         memory_channel_destroy(radio->memory[i]);
      }
   }

   free(radio);
}

radio_channel_t * radio_get_vfo(radio_t *radio) {
   assert(radio != NULL);
   return radio->vfo;
}

void radio_set_vfo(radio_t *radio, radio_channel_t *radio_channel) {
   assert(radio != NULL);
   radio->vfo = radio_channel;
}

radio_channel_t * radio_get_active_channel(radio_t *radio) {
   assert(radio != NULL);
   return radio->ch_ptr;
}

void radio_set_active_channel(radio_t *radio, radio_channel_t *radio_channel) {
   assert(radio != NULL);

   if (radio_channel != NULL) {
      radio->ch_ptr = radio_channel;
      radio_commit_radio_channel(radio, radio_channel);
   }
}

radio_mode_t radio_get_mode(radio_t *radio) {
   assert(radio != NULL);
   return radio->mode;
}

void radio_set_mode(radio_t *radio, radio_mode_t mode) {
   assert(radio != NULL);
   radio->mode = mode;

   switch(mode) {
      case RADIO_MODE_VFO:
         radio_channel_set_rev(radio->vfo, REV_OFF);
         radio_channel_set_rev(radio_get_vfo(radio), REV_OFF);
         radio_set_active_channel(radio, radio_get_vfo(radio));
         break;
      case RADIO_MODE_MEMORY:
         radio_channel_t *rc = RADIO_CHANNEL(memory_active_ch_get(radio->mem));
         radio_channel_set_rev(rc, REV_OFF);
         radio_set_active_channel(radio, rc); 
         break;
      default:
         // Should not happen but if so, do nothing
         break;
   }
}

void radio_commit_radio_channel(radio_t *radio, radio_channel_t *radio_channel) {
   assert(radio != NULL);
   assert(radio_channel != NULL);

   tx_admit_t txa;
   double freq_rx, freq_tx;
   
   freq_rx = radio_channel_freq_rx_get(radio_get_active_channel(radio));
   freq_tx = radio_channel_freq_tx_get(radio_get_active_channel(radio));

   m110_t *m110 = M110(radio);

   m110_channel_frequencies_set(m110, 1, freq_rx, freq_tx);
   m110_channel_frequencies_set(m110, 2, freq_rx, freq_tx);
   
   m110_ctcss_rx_set(m110, 1, ctcss_get_as_double(radio_channel_ctcss_rx_get(radio_channel)));
   m110_ctcss_tx_set(m110, 1, ctcss_get_as_double(radio_channel_ctcss_tx_get(radio_channel)));
   m110_ctcss_rx_set(m110, 2, ctcss_get_as_double(radio_channel_ctcss_rx_get(radio_channel)));
   m110_ctcss_tx_set(m110, 2, ctcss_get_as_double(radio_channel_ctcss_tx_get(radio_channel)));

   txa = m110_tx_admit_get(m110, 1);
   m110_tx_admit_set(m110, 1, txa);
   m110_tx_admit_set(m110, 2, txa);
   
   // MISSING:
   // power
}

char *radio_tune_step_get(radio_t *radio) {
   assert(radio != NULL);
   return tune_step_get_as_string(radio_channel_tune_step_get(radio_get_vfo(radio)));
}
void radio_tune_step_down(radio_t *radio) {
   assert(radio != NULL);
   tune_step_prev(radio_channel_tune_step_get(radio_get_vfo(radio)));
}

void radio_tune_step_up(radio_t *radio) {
   assert(radio != NULL);
   tune_step_next(radio_channel_tune_step_get(radio_get_vfo(radio)));
}

void radio_timeout_down(radio_t *radio) {
   assert(radio != NULL);
   unsigned char timeout = m110_timeout_get(M110(radio));
   if (timeout > 0) timeout -= 1;
   m110_timeout_set(M110(radio), timeout);
}

void radio_timeout_up(radio_t *radio) {
   assert(radio != NULL);
   unsigned char timeout = m110_timeout_get(M110(radio));
   if (timeout < 255) timeout += 1;
   m110_timeout_set(M110(radio), timeout);
}

void radio_rekey_down(radio_t *radio) {
   assert(radio != NULL);
   unsigned char rekey = m110_rekey_get(M110(radio));
   if (rekey > 0) rekey -= 1;
   m110_rekey_set(M110(radio), rekey);
}

void radio_rekey_up(radio_t *radio) {
   assert(radio != NULL);
   unsigned char rekey = m110_rekey_get(M110(radio));
   if (rekey < 255) rekey += 1;
   m110_rekey_set(M110(radio), rekey);
}

void radio_tx_admit_down(radio_t *radio) {
   assert(radio != NULL);
   radio_channel_t *rc = radio_get_active_channel(radio);
   tx_admit_t txa = radio_channel_tx_admit_get(rc);

   switch (txa) {
      case TXADMIT_MONITOR:
         txa = TXADMIT_PL_NO_CARRIER;
         break;
      case TXADMIT_ALWAYS:
         txa = TXADMIT_MONITOR;
         break;
      case TXADMIT_NO_CARRIER:
         txa = TXADMIT_ALWAYS;
         break;
      case TXADMIT_PL_NO_CARRIER:
         txa = TXADMIT_NO_CARRIER;
         break;
      default:
         return;
   }

   radio_channel_tx_admit_set(rc, txa);
   radio_set_active_channel(radio, rc);
}

void radio_tx_admit_up(radio_t *radio) {
   assert(radio != NULL);
   radio_channel_t *rc = radio_get_active_channel(radio);
   tx_admit_t txa = radio_channel_tx_admit_get(rc);
   
   switch (txa) {
      case TXADMIT_MONITOR:
         txa = TXADMIT_ALWAYS;
         break;
      case TXADMIT_ALWAYS:
         txa = TXADMIT_NO_CARRIER;
         break;
      case TXADMIT_NO_CARRIER:
         txa = TXADMIT_PL_NO_CARRIER;
         break;
      case TXADMIT_PL_NO_CARRIER:
         txa = TXADMIT_MONITOR;
         break;
      default:
         return;
   }

   radio_channel_tx_admit_set(rc, txa);
   radio_set_active_channel(radio, rc);
}

void radio_radio_channel_down(radio_t *radio) {
   assert(radio != NULL);

   // Must check repeater mode, etc...
   // must move that logic elsewhere

   if (radio->mode == RADIO_MODE_MEMORY) { 
      radio_set_active_channel(radio,RADIO_CHANNEL(memory_next(radio->mem)));
   } else if (radio->mode == RADIO_MODE_VFO) {
      radio_channel_t *rc = radio_get_active_channel(radio);
      if (radio_channel_get_rev(rc) == REV_ON) {
         radio_channel_set_rev(rc, REV_OFF);
      }
      // lower band limits here
      double freq = radio_channel_freq_rx_get(rc) - tune_step_get_as_MHz(radio_channel_tune_step_get(rc)); 
      if (freq >= RADIO_FREQ_MIN) {
         radio_channel_freq_rx_set(rc, freq);
         // What about tx
      } else {
         radio_channel_freq_rx_set(rc, RADIO_FREQ_MAX);
      }
      radio_set_active_channel(radio, rc);
   }
}

void radio_radio_channel_up(radio_t *radio) {
   assert(radio != NULL);

   if (radio->mode == RADIO_MODE_MEMORY) { 
      radio_set_active_channel(radio,RADIO_CHANNEL(memory_prev(radio->mem)));
   } else if (radio->mode == RADIO_MODE_VFO) {
      radio_channel_t *rc = radio_get_active_channel(radio);
      if (radio_channel_get_rev(rc) == REV_ON) {
         radio_channel_set_rev(rc, REV_OFF);
      }
      // upper band limits here
      double freq = radio_channel_freq_rx_get(rc) + tune_step_get_as_MHz(radio_channel_tune_step_get(rc)); 
      if (freq <= RADIO_FREQ_MAX) {
         radio_channel_freq_rx_set(rc, freq);
         // What about tx
      } else {
         radio_channel_freq_rx_set(rc, RADIO_FREQ_MIN);
      }
      radio_set_active_channel(radio, rc);
   }
}

void radio_dup_down(radio_t *radio) {
   assert(radio != NULL);

   dup_t dup = radio_channel_dup_get(radio->ch_ptr);
   
   if (dup == DUP_DOWN) {
     dup = DUP_UP; 
   } else {
     dup--;
   }

   radio_channel_dup_set(radio->ch_ptr, dup);
}

void radio_dup_up(radio_t *radio) {
   assert(radio != NULL);

   dup_t dup = radio_channel_dup_get(radio->ch_ptr);
   
   if (dup == DUP_UP) {
     dup = DUP_DOWN; 
   } else {
     dup++;
   }

   radio_channel_dup_set(radio->ch_ptr, dup);
}


void radio_scan_set(radio_t *radio, bool scan) {
   assert (radio != NULL);
   radio->scan_enabled = scan;
}

bool radio_scan_get(radio_t *radio) {
   assert (radio != NULL);
   return radio->scan_enabled; 
}

void radio_get_size(void) {
   printf("SizeOf(radio_channel_t) = %d\n", radio_channel_get_size());
   printf("SizeOf(memory_channel_t) = %d\n", memory_channel_get_size());
   printf("SizeOf(radio_t) = %d\n", sizeof(radio_t));
}

