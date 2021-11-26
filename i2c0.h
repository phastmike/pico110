/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 3; tab-width: 3 -*- */
/* vim: set tabstop=3 softtabstop=3 shiftwidth=3 expandtab :               */
/*
 * i2c0.h
 * 
 * I2C Peripheral Controller0 routines
 *
 *
 * José Miguel Fonte
 */

#ifndef _I2C0_H_
#define _I2C0_H_

#ifdef __cplusplus
extern "C" {
#endif

void i2c0_init(unsigned char clock_streching_enabled);
void i2c_on_addr_set_connect(void (*callback)(uint16_t addr, void *user_data), void *user_data);
void i2c_on_write_byte_connect(void (*callback)(uint8_t byte, void *user_data), void *user_data);
void i2c_on_read_byte_connect(uint8_t (*callback)(void *user_data), void *user_data);

#ifdef __cplusplus
}
#endif

#endif //_I2C0_H_
