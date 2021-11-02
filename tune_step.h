/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * tune_step.h
 * 
 * Tune step, vfo frequency step.
 * Need various instances? Probably not... 
 *
 * José Miguel Fonte
 */

typedef struct _tune_step_t tune_step_t;

tune_step_t    *  tune_step_new();
void              tune_step_destroy(tune_step_t *tune_step);
char              tune_step_get_index(tune_step_t *tune_step);
void              tune_step_next(tune_step_t *tune_step);
void              tune_step_prev(tune_step_t *tune_step);
double            tune_step_get_as_MHz(tune_step_t *tune_step);
unsigned char *   tune_step_get_as_string(tune_step_t *tune_step);
void              tune_step_on_changed_connect(tune_step_t * tune_step, void (*callback)(tune_step_t *));
void              tune_step_on_changed_clear(tune_step_t *tune_step);
