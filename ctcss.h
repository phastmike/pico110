/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * ctcss.c
 * 
 * Continous Tone Coded Squelch System.
 * We cannot retrieve the tone from the eeprom correctly
 * due to minor Hz error with floating point.
 * One approach is to use the integer combinations or never
 * read and keep track of tones in RAM.
 *
 * José Miguel Fonte
 */

#ifndef _CTCSS_H_
#define _CTCSS_H_

#define CTCSS(x) ((ctcss_t *) x)

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   CTCSS_OFF,
   CTCSS_67_0, CTCSS_69_3,
   CTCSS_71_9, CTCSS_74_4, CTCSS_77_0, CTCSS_79_7,
   CTCSS_82_5, CTCSS_85_4, CTCSS_88_5,
   CTCSS_91_5, CTCSS_94_8, CTCSS_97_4,
   CTCSS_100_0, CTCSS_103_5, CTCSS_107_2,
   CTCSS_110_9, CTCSS_114_8, CTCSS_118_8,
   CTCSS_123_0, CTCSS_127_3,
   CTCSS_131_8, CTCSS_136_5,
   CTCSS_141_3, CTCSS_146_2,
   CTCSS_151_4, CTCSS_156_7, CTCSS_159_8,
   CTCSS_162_2, CTCSS_165_5, CTCSS_167_9,
   CTCSS_171_3, CTCSS_173_8, CTCSS_177_3, CTCSS_179_9,
   CTCSS_183_5, CTCSS_186_2, CTCSS_189_9,
   CTCSS_192_8, CTCSS_196_6, CTCSS_199_5,
   CTCSS_203_5, CTCSS_206_5,
   CTCSS_210_7, CTCSS_218_1,
   CTCSS_225_7, CTCSS_229_1,
   CTCSS_233_6,
   CTCSS_241_8,
   CTCSS_250_3, CTCSS_254_1 
} ctcss_tone_t;

typedef struct _ctcss_t ctcss_t;

typedef void (*ctcss_changed_cb_t)(ctcss_t *ctcss, void *user_data);

ctcss_t        *  ctcss_new();
ctcss_t        *  ctcss_new_with_tone(ctcss_tone_t tone);
void              ctcss_destroy(ctcss_t *ctcss);
char              ctcss_get_index(ctcss_t *ctcss);
void              ctcss_set_tone(ctcss_t *ctcss, ctcss_tone_t tone);
void              ctcss_next(ctcss_t *ctcss);
void              ctcss_prev(ctcss_t *ctcss);
double            ctcss_get_as_hz(ctcss_t *ctcss);
char           *  ctcss_get_as_string(ctcss_t *ctcss);
void              ctcss_on_changed_connect(ctcss_t * ctcss, ctcss_changed_cb_t callback, void *user_data);
void              ctcss_on_changed_clear(ctcss_t *ctcss);

#ifdef __cplusplus
}
#endif

#endif //_CTCSS_H_
