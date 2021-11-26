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

#include <stdio.h>
#include <stdlib.h>
#include "ctcss.h"

#define CTCSS_NTONES          51

struct _ctcss_t {
   char index;

   void (*changed)(ctcss_t *ctss, void *user_data);
   void *changed_user_data;
};


// set as static
double ctcss_tones[] = {
   0,
   67.0, 69.3,
   71.9, 74.4, 77.0, 79.7,
   82.5, 85.4, 88.5,
   91.5, 94.8, 97.4,
   100.0, 103.5, 107.2,
   110.9, 114.8, 118.8,
   123.0, 127.3,
   131.8, 136.5,
   141.3, 146.2,
   151.4, 156.7, 159.8,
   162.2, 165.5, 167.9,
   171.3, 173.8, 177.3, 179.9,
   183.5, 186.2, 189.9,
   192.8, 196.6, 199.5,
   203.5, 206.5,
   210.7, 218.1,
   225.7, 229.1,
   233.6,
   241.8,
   250.3, 254.1 
};



ctcss_t *  ctcss_new() {
   ctcss_t *ctcss = calloc(1, sizeof(ctcss_t));
   if (ctcss == NULL) return NULL;
   
   ctcss->index = CTCSS_OFF;
   ctcss->changed = NULL;
   ctcss->changed_user_data = NULL;

   return ctcss;
}

ctcss_t * ctcss_new_with_tone(ctcss_tone_t tone) {
   ctcss_t *ctcss = ctcss_new();
   ctcss->index = tone;

   return ctcss;
}

void ctcss_destroy(ctcss_t *ctcss) {
   if (ctcss != NULL) free(ctcss);
}

char ctcss_get_index(ctcss_t *ctcss) {
   return ctcss->index;
}

void ctcss_set_tone(ctcss_t *ctcss, ctcss_tone_t tone) {
   ctcss->index = tone;

   if (ctcss->changed != NULL) {
      ctcss->changed(ctcss, ctcss->changed_user_data);
   }
}

void ctcss_next(ctcss_t *ctcss) {
   if (ctcss_get_index(ctcss) == CTCSS_NTONES - 1) {
      ctcss->index = 0;
   } else {
      ctcss->index += 1;
   }

   if (ctcss->changed != NULL) {
      ctcss->changed(ctcss, ctcss->changed_user_data);
   }
}

void ctcss_prev(ctcss_t *ctcss) {
   if (ctcss_get_index(ctcss) == 0) {
      ctcss->index = CTCSS_NTONES - 1;
   } else {
      ctcss->index -=1;
   }

   if (ctcss->changed != NULL) {
      ctcss->changed(ctcss, ctcss->changed_user_data);
   }
}


double ctcss_get_as_hz(ctcss_t *ctcss) {
   return ctcss_tones[ctcss_get_index(ctcss)];
}

char *ctcss_get_as_string(ctcss_t *ctcss) {
   unsigned char *string = calloc(1, 6);

   if (ctcss_get_index(ctcss) == 0) {
      sprintf(string, " Off");
   } else {
      sprintf(string, "%5.1f", ctcss_get_as_hz(ctcss));
   }

   return string;
}

void ctcss_on_changed_connect(ctcss_t * ctcss, ctcss_changed_cb_t callback, void *user_data) {
   if (callback != NULL) {
      ctcss->changed = callback;
      ctcss->changed_user_data = user_data;
   }
}

void ctcss_on_changed_clear(ctcss_t *ctcss) {
   ctcss->changed = NULL;
   ctcss->changed_user_data = NULL;
}
