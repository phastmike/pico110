/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * memory_channel.c
 * 
 * A memory channel with all parameters 
 *
 * José Miguel Fonte
 */


#include <stdio.h>
#include <stdlib.h>
#include "memory_channel.h"

struct _memory_channel_t {
   radio_channel_t radio_channel;
   unsigned char *name;
   unsigned char id;
};

memory_channel_t * memory_channel_new(void) {
   return (memory_channel_t *) calloc(1,sizeof(memory_channel_t));
}

memory_channel_t * memory_channel_new_with(unsigned char id, unsigned char *name) {
   memory_channel_t *memory_channel = memory_channel_new();
   memory_channel->id = id;
   memory_channel->name = name;
   radio_channel_set_defaults(RADIO_CHANNEL(memory_channel));
   return memory_channel;
}

void memory_channel_destroy(memory_channel_t *memory_channel) {
   assert(memory_channel != NULL);

   if (memory_channel->name) free(memory_channel->name);
   free(memory_channel);
}


unsigned int memory_channel_get_size() {
   return sizeof(memory_channel_t);
}
