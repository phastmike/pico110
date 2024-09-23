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
#include <string.h>
#include "memory_channel.h"

struct _memory_channel_t {
   radio_channel_t radio_channel;
   unsigned char id;
   unsigned char *name;
};

memory_channel_t * memory_channel_new(void) {
   memory_channel_t *memory_channel = calloc(1,sizeof(memory_channel_t));
   radio_channel_init(RADIO_CHANNEL(memory_channel));
   return memory_channel;
}

memory_channel_t * memory_channel_new_with(unsigned char id, unsigned char *name) {
   memory_channel_t *memory_channel = memory_channel_new();
   memory_channel->id = id;
   memory_channel->name = calloc(1, strlen(name));
   memory_channel->name = strcpy(memory_channel->name, name) ;
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

int memory_channel_id_get(memory_channel_t *memory_channel) {
   assert(memory_channel != NULL);
   return memory_channel->id;
}

char * memory_channel_name_get(memory_channel_t *memory_channel) {
   assert(memory_channel != NULL);
   return memory_channel->name;
}
