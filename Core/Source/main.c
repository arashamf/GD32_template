//---------------------------------------------------------------------------//
#include "gd32f30x.h"
#include <stdio.h>
#include "systick.h"
#include "encoder.h"
#include "usart.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "OLED.h"
#include "typedef.h"

//---------------------------------------------------------------------------//
#define ISP_ADDR ((uint32_t)0x1FFFF000)
//#define MAX_CNT_TIMEOUT   15
//---------------------------------------------------------------------------//
FlagStatus sw_led = SET;

//---------------------------------------------------------------------------//
void adc_config(void);
void jump_to_app(uint32_t addr);

//---------------------------------------------------------------------------//
int main(void) 
{
#ifdef HID_IAP
    // Soft :GD32 MCU HID IAP Programmer, Bootloader Size :8KB 
  // if use HID bootloader ,set ROM start addr 0x8002000 && set vector 
  // if not use HID bootloader ,set ROM start addr 0x8000000 && Comment out the following code 
  // nvic_vector_table_set(NVIC_VECTTAB_FLASH,0x2000);
    nvic_vector_table_set(NVIC_VECTTAB_FLASH,0x2000);
#endif
    rcu_all_reset_flag_clear();
    SystemInit ();
    SystemCoreClockUpdate ();
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2); //2 bits for pre-emption priority 2 bits for subpriority
    gpio_config();
    usart0_config();
    dma_config();
    spi0_config();
    OLED_init();
    timer_delay_init ();

    rcu_ckout0_config(RCU_CKOUT0SRC_CKSYS); //вывод системной частоты на вывод MCO

	FontSet(Segoe_UI_Eng_12);
    snprintf(lcd_buf, 20,"enc_count=%ld", enc_count);
    OLED_DrawStr(lcd_buf, 5, 5);	//
    OLED_UpdScreen_DMA();

    LED(ON);

    while (1) 
    {
        for (uint32_t count = 0; count < 500000; count++)
        {
            read_data_encoder();
        }
        TOOGLE_LED();
    }

}

//---------------------------------------------------------------------------//
void jump_to_app(uint32_t addr) {
    uint32_t app_address;
    void *(*application)(void);

    /* test if user code is programmed starting from address */
    if (((*(__IO uint32_t *) addr) & 0x2FFE0000) == 0x20000000) {
        app_address = *(__IO uint32_t *) (addr + 4);
        application = (void *(*)(void)) app_address;

        /* initialize user application's stack pointer */
        __set_MSP(*(__IO uint32_t *) addr);

        /* jump to application */
        application();
    }
}

//---------------------------------------------------------------------------//
int _read(int fd, char *pBuffer, int size) {
    for (int i = 0; i < size; i++) {
        while (usart_flag_get(USART0, USART_FLAG_RBNE) == 1) {
        }

        pBuffer[i] = usart_data_receive(USART0);
    }
    return size;
}

extern int  _end;

//---------------------------------------------------------------------------//
caddr_t _sbrk ( int incr )
{
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if (heap == NULL) {
        heap = (unsigned char *)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return (caddr_t) prev_heap;
}

//---------------------------------------------------------------------------//
int link(char *old, char *new) {
    return -1;
}

//---------------------------------------------------------------------------//
int _close(int file)
{
    return -1;
}

//---------------------------------------------------------------------------//
int _fstat(int file, struct stat *st)
{
    //st->st_mode = S_IFCHR;
    return 0;
}

//---------------------------------------------------------------------------//
int _isatty(int file)
{
    return 1;
}

//---------------------------------------------------------------------------//
int _lseek(int file, int ptr, int dir)
{
    return 0;
}

//---------------------------------------------------------------------------//
void abort(void)
{
    /* Abort called */
    while(1);
}