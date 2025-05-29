#include "usart.h"
#include "gpio.h"

//---------------------------------------------------------------------//
#define   DBG_UART      USART0

//---------------------------------------------------------------------//
char DBG_buf[30];

//-----------------------------for MicroLIB-----------------------------//
int fputc(int ch, FILE *f)
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
	usart_data_transmit(USART0, (uint8_t)ch);
	while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	return ch;
}

//-------------------------------for GNU-------------------------------//
int _write (int fd, char *pBuffer, int size)
{ 
	for (int i = 0; i < size; i++)
	{
		usart_data_transmit(USART0, (uint8_t)pBuffer[i]);
	  while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	}
	return size;
}

//---------------------------------------------------------------------//
void usart0_config(void)
{
    usart0_gpio_init ();

    rcu_periph_clock_enable(RCU_USART0);     // enable USART clock 

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
    usart_enable(USART0);
}

//---------------------------------------------------------------------//
int DBG_put_msg (char *pBuffer, int size)
{ 
	for (int i = 0; i < size; i++)
	{
		usart_data_transmit(DBG_UART, (uint8_t)pBuffer[i]);
	  while(RESET == usart_flag_get(DBG_UART, USART_FLAG_TBE));
	}
	return size;
}
