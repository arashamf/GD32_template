#ifndef gpio_H
#define gpio_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------------------//
#include "gd32f30x.h"

//--------------------------------------------------------------------------------//

#define MCO_GPIO_RCU        RCU_GPIOA
#define MCO_GPIO_PORT       GPIOA
#define MCO_Pin             GPIO_PIN_8

#define KEY_RCU             RCU_GPIOA
#define KEY_PORT            GPIOA
#define KEY_PIN             GPIO_PIN_0

#define LED_RCU             RCU_GPIOB
#define LED_PORT            GPIOB
#define LED_Pin             GPIO_PIN_2

#define DISP_GPIO_RCU       RCU_GPIOA
#define DISP_GPIO_Port      GPIOA
#define RST_Pin             GPIO_PIN_2
#define DC_Pin              GPIO_PIN_3
#define CS_Pin              GPIO_PIN_1

#define SPI0_GPIO_RCU       RCU_GPIOA
#define SPI0_GPIO_Port      GPIOA
#define MOSI0_Pin           GPIO_PIN_7
#define MISO0_Pin           GPIO_PIN_6
#define CLK0_Pin            GPIO_PIN_5

#define SPI1_GPIO_RCU       RCU_GPIOB
#define SPI1_GPIO_Port      GPIOB
#define MOSI1_Pin           GPIO_PIN_15
#define MISO1_Pin           GPIO_PIN_14
#define CLK1_Pin            GPIO_PIN_13

#define ENC_GPIO_RCU        RCU_GPIOB
#define ENC_GPIO_Port       GPIOB
#define ENC_IN2_PIN         GPIO_PIN_4
#define ENC_IN1_PIN         GPIO_PIN_6

#define USART_GPIO_RCU          RCU_GPIOA
#define USART_GPIO_Port         GPIOA
#define USART_Tx_Pin            GPIO_PIN_9
#define USART_Rx_Pin            GPIO_PIN_10

#define I2C_GPIO_RCU        RCU_GPIOB
#define I2C_GPIO_Port       GPIOB
#define I2C_SCL_Pin         GPIO_PIN_6
#define I2C_SDA_Pin         GPIO_PIN_7

#define LED(x)  ((x) ? (gpio_bit_set(LED_PORT, LED_Pin)) : (gpio_bit_reset(LED_PORT, LED_Pin)))
#define TOOGLE_LED() (LED(!(gpio_output_bit_get(LED_PORT, LED_Pin))))
#define LCD_CS(x) ((x) ? (gpio_bit_reset(DISP_GPIO_Port, CS_Pin)) : (gpio_bit_set(DISP_GPIO_Port, CS_Pin)))
#define LCD_DC(x) ((x) ? (gpio_bit_set(DISP_GPIO_Port, DC_Pin)) : (gpio_bit_reset(DISP_GPIO_Port, DC_Pin)))
#define LCD_RST(x) ((x) ? (gpio_bit_set(DISP_GPIO_Port, RST_Pin)) : (gpio_bit_reset(DISP_GPIO_Port, RST_Pin)))

#define ON      1
#define OFF     0

//--------------------------------------------------------------------------------//
void gpio_config(void);
void SPI0_gpio_init (void);
void SPI1_gpio_init (void);
void I2C_gpio_config(void);
void ENC_gpio_init (void);
void usart0_gpio_init (void);

#ifdef __cplusplus
}
#endif

#endif
