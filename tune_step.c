/* -*- Mode: Vala; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :                  */
/*
 * tune_step.c
 * 
 * Tune step, vfo frequency step.
 * Need various instances? Probably not... 
 *
 * José Miguel Fonte
 */

#include <stdio.h>
#include <stdlib.h>
#include "tune_step.h"

#define   FSTEP_6_25kHz  0.00625
#define   FSTEP_12_5kHz  0.0125
#define   FSTEP_25kHz    0.025
#define   FSTEP_50khz    0.05
#define   FSTEP_100kHz   0.1
#define   FSTEP_500kHz   0.5
#define   FSTEP_1MHz     1
#define   FSTEP_N        7

#define TUNE_STEP_DEFAULT_INDEX 1 // Array index for default step, 1 = 12.5khz

struct _tune_step_t {
   char index;

   void (*changed)(tune_step_t *tune_step, void *user_data);
   void *changed_user_data;
};


double tune_step_steps[] = {
   FSTEP_6_25kHz, FSTEP_12_5kHz, FSTEP_25kHz, FSTEP_50khz, FSTEP_100kHz,
   FSTEP_500kHz, FSTEP_1MHz
};

tune_step_t *  tune_step_new() {
   tune_step_t *tune_step = calloc(1, sizeof(tune_step_t));
   
   tune_step->index = TUNE_STEP_DEFAULT_INDEX;
   tune_step->changed = NULL;
   tune_step->changed_user_data = NULL;

   return tune_step;
}

void tune_step_destroy(tune_step_t *tune_step) {
   if (tune_step != NULL) free(tune_step);
}

char tune_step_get_index(tune_step_t *tune_step) {
   return tune_step->index;
}

void tune_step_next(tune_step_t *tune_step) {
   tune_step->index += 1;
   if (tune_step_get_index(tune_step) == FSTEP_N) {
      tune_step->index = 0;
   }

   if (tune_step->changed != NULL) {
      tune_step->changed(tune_step, tune_step->changed_user_data);
   }
}

void tune_step_prev(tune_step_t *tune_step) {
   tune_step->index -=1;
   if (tune_step_get_index(tune_step) == -1) {
      tune_step->index = FSTEP_N - 1;
   }

   if (tune_step->changed != NULL) {
      tune_step->changed(tune_step, tune_step->changed_user_data);
   }
}


double tune_step_get_as_MHz(tune_step_t *tune_step) {
   return tune_step_steps[tune_step_get_index(tune_step)];
}

unsigned char *tune_step_get_as_string(tune_step_t *tune_step) {
   unsigned char *string = calloc(1, 6);

   switch (tune_step_get_index(tune_step)) {
      case 0: sprintf(string, " 6.25");
              break;
      case 1: sprintf(string, " 12.5");
              break;
      case 2: sprintf(string, "  25");
              break;
      case 3: sprintf(string, "  50");
              break;
      case 4: sprintf(string, " 100");
              break;
      case 5: sprintf(string, " 500");
              break;
      case 6: sprintf(string, "1000");
              break;
      default: sprintf(string," Err");
              break;
   }

   return string;
}

void tune_step_on_changed_connect(tune_step_t * tune_step, void (*callback)(tune_step_t *, void *), void *user_data) {
   if (callback != NULL) {
      tune_step->changed = callback;
      tune_step->changed_user_data = user_data;
   }
}

void tune_step_on_changed_clear(tune_step_t *tune_step) {
   tune_step->changed = NULL;
   tune_step->changed_user_data = NULL;
}
