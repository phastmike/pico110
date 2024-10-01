/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * memory.c
 * 
 * A memory controller
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "memory_channel.h"

struct _memory_t {
   memory_channel_t *active_ch;
   memory_channel_t *ch[MEMORY_NUMBER_OF_CHANNELS];
};

memory_t *memory_new(void) {
   memory_t *mem = calloc(1, sizeof(memory_t));
   return mem;
}

void memory_init(memory_t *mem) {
   // Fill memories with PMR frequencies
   // and some local UHF ham repeaters/services
   mem->ch[0] = memory_channel_new_with(0,"PMR 1");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[0]), 446.00625);

   mem->ch[1] = memory_channel_new_with(1,"PMR 2");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[1]), 446.01875);

   mem->ch[2] = memory_channel_new_with(2,"PMR 3");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[2]), 446.03125);

   mem->ch[3] = memory_channel_new_with(3,"PMR 4");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[3]), 446.04375);
   
   mem->ch[4] = memory_channel_new_with(4,"PMR 5");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[4]), 446.05625);
   
   mem->ch[5] = memory_channel_new_with(5,"PMR 6");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[5]), 446.06875);
   
   mem->ch[6] = memory_channel_new_with(6,"PMR 7");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[6]), 446.08125);
   
   mem->ch[7] = memory_channel_new_with(7,"PMR 8");
   radio_channel_freq_set(RADIO_CHANNEL(mem->ch[7]), 446.09375);

   mem->ch[8] = memory_channel_new_with(8,"CQ0UGMR");
   radio_channel_t *rc = RADIO_CHANNEL(mem->ch[8]);
   radio_channel_ctcss_rx_set(rc, CTCSS_67_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_67_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);
   radio_channel_freq_set(rc, 439.100);

   mem->ch[9] = memory_channel_new_with(9,"CQ0UBRG");
   rc = RADIO_CHANNEL(mem->ch[9]);
   radio_channel_freq_set(rc, 438.800);
   radio_channel_ctcss_rx_set(rc, CTCSS_67_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_67_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   mem->ch[10] = memory_channel_new_with(10,"AMARANTE");
   rc = RADIO_CHANNEL(mem->ch[10]);
   radio_channel_freq_set(rc, 438.700);
   radio_channel_ctcss_rx_set(rc, CTCSS_123_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_123_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   mem->ch[11] = memory_channel_new_with(11,"VIANA");
   rc = RADIO_CHANNEL(mem->ch[11]);
   radio_channel_freq_set(rc, 438.650);
   radio_channel_ctcss_rx_set(rc, CTCSS_67_0);
   radio_channel_ctcss_tx_set(rc, CTCSS_67_0);
   radio_channel_dup_set(rc, DUP_DOWN);
   radio_channel_shift_set(rc, 7.6);

   mem->ch[12] = memory_channel_new_with(12,"AUCHAN");
   rc = RADIO_CHANNEL(mem->ch[12]);
   radio_channel_freq_set(rc, 444.3625);
   radio_channel_ctcss_rx_set(rc, CTCSS_107_2);
   radio_channel_ctcss_tx_set(rc, CTCSS_107_2);
   radio_channel_dup_set(rc, DUP_UP);
   radio_channel_shift_set(rc, 5.0);

   mem->active_ch = mem->ch[0];
}

memory_channel_t  *memory_active_ch_get(memory_t *mem) {
   assert(mem != NULL);
   return mem->active_ch;
}

void memory_active_ch_set(memory_t *mem, memory_channel_t *ch) {
   assert(ch != NULL);
   assert(mem != NULL);
   mem->active_ch = ch;
}

int memory_active_ch_get_id(memory_t *mem) {
   assert(mem != NULL);
   return memory_channel_id_get(memory_active_ch_get(mem));
}

memory_channel_t *memory_get_ch(memory_t *mem, int n) {
   assert(mem != NULL);
   assert(n >= 0 && n <= MEMORY_NUMBER_OF_CHANNELS);
   return mem->ch[n];
}

memory_channel_t *memory_next(memory_t *mem) {
   assert(mem != NULL);

   int id_now;
   int id_iter;
   char found = 0;

   id_now = memory_active_ch_get_id(mem);

   for (id_iter = id_now + 1; id_iter < MEMORY_NUMBER_OF_CHANNELS; id_iter++) {
      if (memory_get_ch(mem, id_iter) != NULL) {
         mem->active_ch = memory_get_ch(mem, id_iter);
         return mem->active_ch;
      }
   }

   for (id_iter = 0; id_iter < id_now; id_iter++) {
      if (memory_get_ch(mem, id_iter) != NULL) {
         mem->active_ch = memory_get_ch(mem,id_iter);
         return mem->active_ch;
      }
   }

   return NULL;
}

memory_channel_t *memory_prev(memory_t *mem) {
   assert(mem != NULL);

   int id_now;
   int id_iter;
   char found = 0;

   id_now = memory_active_ch_get_id(mem);

   for (id_iter = id_now - 1; id_iter >= 0; id_iter--) {
      if (memory_get_ch(mem, id_iter) != NULL) {
         mem->active_ch = memory_get_ch(mem, id_iter);
         return mem->active_ch;
      }
   }

   for (id_iter = MEMORY_NUMBER_OF_CHANNELS - 1 ; id_iter >= id_now; id_iter--) {
      if (memory_get_ch(mem, id_iter) != NULL) {
         mem->active_ch = memory_get_ch(mem,id_iter);
         return mem->active_ch;
      }
   }

   return NULL;
}

void memory_destroy(memory_t *mem) {
   assert(mem != NULL);
   for (int i = 0; i < MEMORY_NUMBER_OF_CHANNELS; i++) {
      if (mem->ch[i] != NULL) free (mem->ch[i]);
   }

   free(mem);
}
