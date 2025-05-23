#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------//
#include "gd32f30x.h"

//-----------------------------------------------------------------//
void i2c_config(void);
void i2c_write_bytes (uint8_t , uint8_t * , uint8_t );
void i2c_write_buffer_8bit_registr (uint8_t , uint8_t , uint8_t * , uint8_t );
void i2c_write_buffer_16bit_registr (uint8_t , uint16_t , uint8_t * , uint8_t );
#ifdef __cplusplus
}
#endif

#endif
