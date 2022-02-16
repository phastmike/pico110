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
#include "rom_init.h"
#include "memory_channel.h"

struct _radio_t {
   m110_t *m110;
   radio_channel_t *vfo;
   radio_channel_t *ch_ptr;   // points to working radio_channel, either vfo or a memory
   radio_mode_t mode;
   unsigned char memory_selected;
   memory_channel_t *memory[RADIO_NUMBER_OF_CHANNELS];
};

radio_t *radio_new(void) {
   return (radio_t *) calloc (1, sizeof(radio_t));
}

radio_t *radio_new_with_defaults(void) {
   radio_t *radio    = radio_new();
   radio->m110       = m110_new_with_data(rom_init);

   // Many of the following values can be backed up
   // so that the working state is preserved after power off

   radio->mode       = RADIO_MODE_VFO;

   radio->vfo        = radio_channel_new(); // comes with defaults

   // Init pointers to NULL - Empty memories
   for (int i = 0; i < RADIO_NUMBER_OF_CHANNELS; i++) {
      radio->memory[i] = NULL;
   }

   // Fill memories with PMR frequencies
   radio->memory[0]  = memory_channel_new_with(1,"PMR 1",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[0]), 446.00625);
   radio->memory[1]  = memory_channel_new_with(2,"PMR 2",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[1]), 446.01875);
   radio->memory[2]  = memory_channel_new_with(3,"PMR 3",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[2]), 446.03125);
   radio->memory[3]  = memory_channel_new_with(4,"PMR 4",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[3]), 446.04375);
   radio->memory[4]  = memory_channel_new_with(5,"PMR 5",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[4]), 446.05625);
   radio->memory[5]  = memory_channel_new_with(6,"PMR 6",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[5]), 446.06875);
   radio->memory[6]  = memory_channel_new_with(7,"PMR 7",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[6]), 446.08125);
   radio->memory[7]  = memory_channel_new_with(8,"PMR 8",radio_channel_new());
   radio_channel_freq_set(memory_channel_get_radio_channel(radio->memory[7]), 446.09375);

   radio->memory[8]  = memory_channel_new_with(9,"CQ0UGMR",radio_channel_new());
   radio_channel_t *rc = memory_channel_get_radio_channel(radio->memory[8]);
   radio_channel_freq_set(rc, 439.100);
   radio_channel_ctcss_rx_set(rc, CTCSS_67_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_67_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   radio->memory[9]  = memory_channel_new_with(10,"CQ0UBRG",radio_channel_new());
   rc = memory_channel_get_radio_channel(radio->memory[9]);
   radio_channel_freq_set(rc, 438.800);
   radio_channel_ctcss_rx_set(rc, CTCSS_67_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_67_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   radio->memory[10]  = memory_channel_new_with(11,"AMARANTE",radio_channel_new());
   rc = memory_channel_get_radio_channel(radio->memory[10]);
   radio_channel_freq_set(rc, 438.700);
   radio_channel_ctcss_rx_set(rc, CTCSS_123_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_123_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   radio->memory[11]  = memory_channel_new_with(12,"VIANA",radio_channel_new());
   rc = memory_channel_get_radio_channel(radio->memory[11]);
   radio_channel_freq_set(rc, 438.650);
   radio_channel_ctcss_rx_set(rc, CTCSS_67_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_67_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   radio->memory[12]  = memory_channel_new_with(13,"AUCHAN",radio_channel_new());
   rc = memory_channel_get_radio_channel(radio->memory[12]);
   radio_channel_freq_set(rc, 444.3625);
   radio_channel_ctcss_rx_set(rc, CTCSS_107_2);
   radio_channel_ctcss_tx_set(rc, CTCSS_107_2);
   radio_channel_dup_set(rc, DUP_UP);
   radio_channel_shift_set(rc, 5.0);

   radio->memory_selected = 0;

   // Defaults to VFO;
   // also commits to m110
   radio_set_active_channel(radio, radio->vfo); 

   return radio;
}

void radio_destroy(radio_t *radio) {
   assert(radio != NULL);

   if (radio->m110) m110_destroy(radio->m110);
   if (radio->vfo)  radio_channel_destroy(radio->vfo);
   for (int i = 0; i < RADIO_NUMBER_OF_CHANNELS; i++) {
      if (radio->memory[i]) {
         memory_channel_destroy(radio->memory[i]);
      }
   }

   free(radio);
}

m110_t * radio_get_m110(radio_t *radio) {
   return radio->m110;
}

radio_channel_t * radio_get_active_channel(radio_t *radio) {
   assert(radio != NULL);
   return radio->ch_ptr;
}

void radio_set_active_channel(radio_t *radio, radio_channel_t *radio_channel) {
   assert(radio != NULL);
   assert(radio_channel != NULL);

   //ctcss_on_changed_clear(radio_channel_ctcss_rx_get(radio_get_active_channel(radio)));
   //ctcss_on_changed_clear(radio_channel_ctcss_tx_get(radio_get_active_channel(radio)));

   radio->ch_ptr = radio_channel;

   radio_commit_radio_channel(radio, radio_channel);
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
         radio_set_active_channel(radio, radio->vfo);
         break;
      case RADIO_MODE_MEMORY:
         radio_set_active_channel(radio, memory_channel_get_radio_channel(radio->memory[radio->memory_selected]));
         break;
      default:
         // Should not happen abut if so, do nothing
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

   m110_t *m110 = radio_get_m110(radio);

   m110_channel_frequencies_set(m110, 1, freq_rx, freq_tx);
   m110_channel_frequencies_set(m110, 2, freq_rx, freq_tx);
   
   m110_ctcss_rx_set(m110, 1, ctcss_get_as_hz(radio_channel_ctcss_rx_get(radio_channel)));
   m110_ctcss_tx_set(m110, 1, ctcss_get_as_hz(radio_channel_ctcss_tx_get(radio_channel)));
   m110_ctcss_rx_set(m110, 2, ctcss_get_as_hz(radio_channel_ctcss_rx_get(radio_channel)));
   m110_ctcss_tx_set(m110, 2, ctcss_get_as_hz(radio_channel_ctcss_tx_get(radio_channel)));

   txa = m110_tx_admit_get(m110, 1);
   m110_tx_admit_set(m110, 1, txa);
   m110_tx_admit_set(m110, 2, txa);
   
   // MISSING:
   // power
}

ctcss_t *radio_get_ctcss(radio_t *radio) {
   assert(radio != NULL);

   switch (radio->mode) {
      case RADIO_MODE_VFO:
         return radio_channel_ctcss_rx_get(radio->vfo);
      case RADIO_MODE_MEMORY:
         return radio_channel_ctcss_rx_get(memory_channel_get_radio_channel(radio->memory[radio->memory_selected]));
      default:
         break;
   }

   return NULL;
}

void radio_set_ctcss(radio_t *radio, double ctcss) {
   assert(radio != NULL);

   /*
   switch (radio->mode) {
      case RADIO_MODE_VFO:
         return radio_channel_ctcss_rx_set( rx_get(radio->vfo);
      case RADIO_MODE_MEMORY:
         return radio_channel_ctcss_rx_get(memory_channel_get_radio_channel(radio->memory[radio->memory_selected]));
      default:
         break;
   }

   return NULL;
   */

   m110_t *m110 = radio_get_m110(radio);
   m110_ctcss_rx_set(m110, 1, ctcss);
   m110_ctcss_rx_set(m110, 2, ctcss);
   m110_ctcss_tx_set(m110, 1, ctcss);
   m110_ctcss_tx_set(m110, 2, ctcss);
//   radio_channel_ctcss_rx_set
}

char *radio_tune_step_get(radio_t *radio) {
   assert(radio != NULL);
   return tune_step_get_as_string(radio_channel_tune_step_get(radio->vfo));
}
void radio_tune_step_down(radio_t *radio) {
   assert(radio != NULL);
   tune_step_prev(radio_channel_tune_step_get(radio->vfo));
}

void radio_tune_step_up(radio_t *radio) {
   assert(radio != NULL);
   tune_step_next(radio_channel_tune_step_get(radio->vfo));
}

void radio_timeout_down(radio_t *radio) {
   assert(radio != NULL);
   unsigned char timeout = m110_timeout_get(radio_get_m110(radio));
   if (timeout > 0) timeout -= 1;
   m110_timeout_set(radio_get_m110(radio), timeout);
}

void radio_timeout_up(radio_t *radio) {
   assert(radio != NULL);
   unsigned char timeout = m110_timeout_get(radio_get_m110(radio));
   if (timeout < 255) timeout += 1;
   m110_timeout_set(radio_get_m110(radio), timeout);
}

void radio_rekey_down(radio_t *radio) {
   assert(radio != NULL);
   unsigned char rekey = m110_rekey_get(radio_get_m110(radio));
   if (rekey > 0) rekey -= 1;
   m110_rekey_set(radio_get_m110(radio), rekey);
}

void radio_rekey_up(radio_t *radio) {
   assert(radio != NULL);
   unsigned char rekey = m110_rekey_get(radio_get_m110(radio));
   if (rekey < 255) rekey += 1;
   m110_rekey_set(radio_get_m110(radio), rekey);
}

void radio_tx_admit_down(radio_t *radio) {
   assert(radio != NULL);
   //tx_admit_t txa = m110_tx_admit_get(radio_get_m110(radio), 1);
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

   //m110_tx_admit_set(radio_get_m110(radio), 1, txa);
   //m110_tx_admit_set(radio_get_m110(radio), 2, txa);

   radio_channel_tx_admit_set(rc, txa);
   radio_set_active_channel(radio, rc);
}

void radio_tx_admit_up(radio_t *radio) {
   assert(radio != NULL);
   //tx_admit_t txa = m110_tx_admit_get(radio_get_m110(radio), 1);
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

   //m110_tx_admit_set(radio_get_m110(radio), 1, txa);
   //m110_tx_admit_set(radio_get_m110(radio), 2, txa);
   
   radio_channel_tx_admit_set(rc, txa);
   radio_set_active_channel(radio, rc);
}

void radio_radio_channel_down(radio_t *radio) {
   assert(radio != NULL);

   // Must check repeater mode, etc...
   // must move that logic elsewhere

   int i,j;
   int count;
   int found = -1;
  
   if (radio->mode == RADIO_MODE_MEMORY) { 
      for (j = radio->memory_selected - 1; j >= 0 && found == -1; j--) {
         if (radio->memory[j] != NULL) {
            found = j;
         }
      }

      if (found == -1) { 
         for (j = RADIO_NUMBER_OF_CHANNELS - 1; j > radio->memory_selected && found == -1; j--) {
            if (radio->memory[j]) {
               found = j;
            }
         }
      }
      
      if (found != -1) {
         radio->memory_selected = found;
         radio_channel_set_rev(radio_get_active_channel(radio), REV_OFF);
         radio_set_active_channel(radio, memory_channel_get_radio_channel(radio->memory[radio->memory_selected]));
      } 
   } else if (radio->mode == RADIO_MODE_VFO) {
      radio_channel_freq_rx_set(radio_get_active_channel(radio), radio_channel_freq_rx_get(radio_get_active_channel(radio)) - tune_step_get_as_MHz(radio_channel_tune_step_get(radio_get_active_channel(radio))));
      radio_set_active_channel(radio, radio->vfo);
   }
}

void radio_radio_channel_up(radio_t *radio) {
   int i,j;
   int count;
   int found = -1;

   assert(radio != NULL);
  
   if (radio->mode == RADIO_MODE_MEMORY) { 

      for (j = radio->memory_selected + 1; j < RADIO_NUMBER_OF_CHANNELS && found == -1; j++) {
         if (radio->memory[j] != NULL) {
            found = j;
         }
      }

      if (found == -1) { 
         for (j = 0 ; j <= radio->memory_selected - 1 && found == -1; j++) {
            if (radio->memory[j]) {
               found = j;
            }
         }
      }
      
      if (found != -1) {
         radio->memory_selected = found;
         radio_set_active_channel(radio, memory_channel_get_radio_channel(radio->memory[radio->memory_selected]));
      }
   } else if (radio->mode == RADIO_MODE_VFO) {
      radio_channel_freq_rx_set(radio_get_active_channel(radio), radio_channel_freq_rx_get(radio_get_active_channel(radio)) + tune_step_get_as_MHz(radio_channel_tune_step_get(radio_get_active_channel(radio))));
      radio_set_active_channel(radio, radio->vfo);
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

void radio_get_size(void) {
   printf("SizeOf(radio_channel_t) = %d\n", radio_channel_get_size());
   printf("SizeOf(memory_channel_t) = %d\n", memory_channel_get_size());
   printf("SizeOf(radio_t) = %d\n", sizeof(radio_t));
}
