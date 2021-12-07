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

#define MEMORY_CHANNEL(x) ((memory_t *) x)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct       _memory_channel_t memory_channel_t;

memory_channel_t *   memory_channel_new(void);
memory_channel_t *   memory_channel_new_with(unsigned char id, unsigned char *name, radio_channel_t *radio_channel);
void                 memory_channel_destroy(memory_channel_t *memory_channel);
radio_channel_t  *   memory_channel_get_radio_channel(memory_channel_t *memory_channel);
unsigned int         memory_channel_get_size();

#ifdef __cplusplus
}
#endif

#endif //_MEMORY_CHANNEL_H_
