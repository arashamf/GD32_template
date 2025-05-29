//-------------------------------------------------------------------------------------//
#include "gpio.h"

//-------------------------------------------------------------------------------------//
void gpio_config(void) 
{
    rcu_periph_clock_enable(RCU_AF);     // enable  clock 
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE); //JTAG-DP disabled and SW-DP enabled 
    
    rcu_periph_clock_enable(LED_RCU);
    gpio_init(LED_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LED_Pin); // configure led pin as output 
    
    rcu_periph_clock_enable(MCO_GPIO_RCU);
    gpio_init(MCO_GPIO_PORT, GPIO_MODE_AF_PP , GPIO_OSPEED_50MHZ, MCO_Pin); //MCO pin

    ENC_gpio_init ();

}

//-----------------------configure SPI0 GPIO: SCK/PA5, MOSI/PA7-----------------------//
void SPI0_gpio_init (void) 
{
    rcu_periph_clock_enable(DISP_GPIO_RCU);
    rcu_periph_clock_enable(SPI0_GPIO_RCU);
    gpio_pin_remap_config(GPIO_SPI0_REMAP, DISABLE); 
    gpio_init(SPI0_GPIO_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_10MHZ, CLK0_Pin | MOSI0_Pin); //PA7-MOSI, PA5-SCK
    gpio_init(SPI0_GPIO_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_10MHZ, MISO0_Pin); //PA6-MISO

    gpio_init(DISP_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ, RST_Pin | DC_Pin | CS_Pin);
    LCD_CS(OFF) ;
    LCD_DC(OFF) ;
    LCD_RST(OFF) ;
}

//------------------------configure I2c GPIO: SCL/PB6, SDA/PB7------------------------//
void I2C_gpio_config(void)
{
    gpio_pin_remap_config(GPIO_I2C0_REMAP, DISABLE); 
    rcu_periph_clock_enable(I2C_GPIO_RCU);
    gpio_init(I2C_GPIO_Port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, I2C_SDA_Pin | I2C_SCL_Pin); //PB6-SCL, PB7-SDA
}

//-----------------------configure SPI1 GPIO: SCK/PB13, MOSI/PB15-----------------------//
void SPI1_gpio_init (void) 
{
    rcu_periph_clock_enable(DISP_GPIO_RCU);
    rcu_periph_clock_enable(SPI1_GPIO_RCU);
    gpio_init(SPI1_GPIO_Port , GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15); //PB15-MOSI, PB13-SCK
    gpio_init(SPI1_GPIO_Port , GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14); //PB14-MISO
    gpio_init(DISP_GPIO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RST_Pin | DC_Pin | CS_Pin);
    LCD_CS(OFF) ;
    LCD_DC(OFF) ;
    LCD_RST(OFF) ;
}

//-----------------------configure ENC GPIO: PB4, PB6-----------------------//
void ENC_gpio_init (void) 
{
    rcu_periph_clock_enable(USART_GPIO_RCU);
    gpio_init(ENC_GPIO_Port, GPIO_MODE_IPD, GPIO_OSPEED_10MHZ, ENC_IN2_PIN | ENC_IN1_PIN); 
}

//-----------------------configure UASRT0 GPIO: Tx:PA9, Rx:PA10-----------------------//
void usart0_gpio_init (void) 
{
    rcu_periph_clock_enable(USART_GPIO_RCU);
    gpio_pin_remap_config(GPIO_USART0_REMAP, DISABLE); 
    gpio_init(USART_GPIO_Port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);         //PA9-USARTx_Tx
    gpio_init(USART_GPIO_Port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);  //PA10-USARTx_Rx
}
