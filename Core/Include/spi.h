#ifndef spi_H
#define spi_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------//
#include "gd32f30x.h"

//-----------------------------------------------------------------//
#define LCD_BUFFER_SIZE  28

//-----------------------------------------------------------------//
void spi0_config(void);
void spi1_config(void);
void spi_write_byte (uint8_t );
void spi_write_buffer (uint8_t * , uint16_t );
void dma_config(void);
void spi_write_buffer_DMA (void);
void DMA0_Channel2_Callback(void);
//-----------------------------------------------------------------//
extern char lcd_buf[LCD_BUFFER_SIZE];

#ifdef __cplusplus
}
#endif

#endif
