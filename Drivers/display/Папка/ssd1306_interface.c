
/* Includes ------------------------------------------------------------------*/
#include "ssd1306_interface.h"
#include "spi.h"
#include "gpio.h"

/* Private define ------------------------------------------------------------*/
#define SSD1306_SPI_TIMEOUT                                	0xFF

/* Private typedef -----------------------------------------------------------*/
extern SSD1306_State SSD1306_state;

/* Private variables ---------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void ssd1306_SendCommand(uint8_t type_command)
{
	LCD_DC(OFF); 
	spi_write_byte (type_command);
}

/*----------------------------------------------------------------------------*/
void ssd1306_SendByteData(uint8_t data)
{
	LCD_DC(ON); 
	spi_write_byte (data);
}

/*----------------------------------------------------------------------------*/
void ssd1306_SendDataBuffer(uint8_t *data, uint16_t data_size)
{
	#ifdef __SPI_DMA_MODE
	spi_write_buffer_DMA();
	#else
	LCD_DC(ON);
	CS(ON);
	spi_write_buffer (data, data_size);
	CS(OFF);
	#endif
}

