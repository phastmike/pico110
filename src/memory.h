/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * memory.h
 * 
 * A memory controller 
 *
 * José Miguel Fonte
 */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include "memory_channel.h"

#define MEMORY(x)                    ((memory_t *) x)

typedef struct _memory_t memory_t;

#define MEMORY_NUMBER_OF_CHANNELS    20

#ifdef __cplusplus
extern "C" {
#endif

memory_t *           memory_new(void);
void                 memory_init(memory_t *mem);
memory_channel_t *   memory_active_ch_get(memory_t *mem);
memory_channel_t *   memory_get_ch(memory_t *mem, int ch);
memory_channel_t *   memory_next(memory_t *mem);
memory_channel_t *   memory_prev(memory_t *mem);

#ifdef __cplusplus
}
#endif

#endif //_RADIO_H_
