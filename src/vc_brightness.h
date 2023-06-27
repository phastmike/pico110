
#ifndef _VC_BRIGHTNESS_H_
#define _VC_BRIGHTNESS_H_

#include "hmi.h"
#include "radio.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct _vc_brightness_t vc_brightness_t;

vc_brightness_t * vc_brightness_new(hmi_t *hmi, radio_t *radio);

#ifdef __cplusplus
}
#endif

#endif //_VC_BRIGHTNESS_H_
