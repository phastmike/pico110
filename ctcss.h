/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
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

typedef struct _ctcss_t ctcss_t;

typedef void (*ctcss_changed_cb_t)(ctcss_t *ctcss, void *user_data);

ctcss_t        *  ctcss_new();
void              ctcss_destroy(ctcss_t *ctcss);
char              ctcss_get_index(ctcss_t *ctcss);
void              ctcss_next(ctcss_t *ctcss);
void              ctcss_prev(ctcss_t *ctcss);
double            ctcss_get_as_hz(ctcss_t *ctcss);
unsigned char *   ctcss_get_as_string(ctcss_t *ctcss);
void              ctcss_on_changed_connect(ctcss_t * ctcss, ctcss_changed_cb_t callback, void *user_data);
void              ctcss_on_changed_clear(ctcss_t *ctcss);
