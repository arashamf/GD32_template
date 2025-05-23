#ifndef spi_H
#define spi_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------//
#include "gd32f30x.h"

//-----------------------------------------------------------------//
void spi0_config(void);
void spi1_config(void);
void spi_write_byte (uint8_t );
void spi_write_buffer (uint8_t * , uint16_t );

#ifdef __cplusplus
}
#endif

#endif
