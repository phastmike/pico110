/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * key.h
 * 
 * Represents a key
 * 
 *
 * José Miguel Fonte
 */

#ifndef _KEY_H_
#define _KEY_H_

#define HMI_KEY(x) ((key_t *) (x))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
   HMI_KEY_1 =  1 << 0,
   HMI_KEY_2 =  1 << 1,
   HMI_KEY_3 =  1 << 2,
   HMI_KEY_4 =  1 << 3,
   HMI_KEY_5 =  1 << 4,
   HMI_KEY_6 =  1 << 5,
   HMI_KEY_7 =  1 << 6,
   HMI_KEY_8 =  1 << 7,
} hmi_key_id_t;

typedef struct _hmi_key_t hmi_key_t;
typedef void (*hmi_key_on_event_cb_t)(hmi_key_t *key, hmi_key_id_t key_id, void *user_data);

hmi_key_t      *	hmi_key_new(unsigned char id);
void			      hmi_key_destroy(hmi_key_t *key);
hmi_key_id_t      hmi_key_get_id(hmi_key_t *key);
unsigned char     hmi_key_get_active(hmi_key_t *key);
void              hmi_key_set_active(hmi_key_t *key, unsigned char active);
void			      hmi_key_on_press_event_connect(hmi_key_t *key, hmi_key_on_event_cb_t callback, void *user_data);
void			      hmi_key_on_press_event_disconnect(hmi_key_t *key);
void			      hmi_key_on_release_event_connect(hmi_key_t *key, hmi_key_on_event_cb_t callback, void *user_data);
void			      hmi_key_on_release_event_disconnect(hmi_key_t *key);

#ifdef __cplusplus
}
#endif

#endif //_KEY_H_
