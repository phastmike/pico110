/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * memory_channel.h
 * 
 * A memory channel with all parameters 
 *
 * José Miguel Fonte
 */

#ifndef _MEMORY_CHANNEL_H_
#define _MEMORY_CHANNEL_H_

#include "radio_channel.h"

#define MEMORY_CHANNEL(x) ((memory_channel_t *) x)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct       _memory_channel_t memory_channel_t;

memory_channel_t *   memory_channel_new(void);
memory_channel_t *   memory_channel_new_with(unsigned char id, unsigned char *name);
void                 memory_channel_destroy(memory_channel_t *memory_channel);
unsigned int         memory_channel_get_size();
int                  memory_channel_id_get(memory_channel_t *memory_channel);
char *               memory_channel_name_get(memory_channel_t *memory_channel);

#ifdef __cplusplus
}
#endif

#endif //_MEMORY_CHANNEL_H_
