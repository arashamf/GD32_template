//-------------------------------------------------------------------------------------//
#include "encoder.h"
#include "typedef.h"
#include "gpio.h"
#include "OLED.h"
#include "tim.h"
#include <stdio.h>

//-------------------------------------------------------------------------------------//
#define MAX_CNT_TIMEOUT   15

//-------------------------------------------------------------------------------------//
uint8_t val_old = 0, val_new = 0; //значения битов энкодера
uint8_t flag_dir = 0, flag_rev = 0; //флаг направления поворота
uint8_t state_machine = SM_WAIT; //стадия конечного автомата
//uint8_t flag_end_delay = 0; //флаг окончания таймаута
uint32_t ms_delay = 10; //продолжительность таймаута
uint8_t cnt_timeout = 0; //количество таймаутов ожидания изменения показаний энкодера
int32_t enc_count = 0; //счётчик щелчков энкодера

//-----------------------------------------------------------------------------//
uint8_t read_gray_code_from_encoder(void ) 
{ 
    volatile uint8_t value = 0; 
    volatile uint8_t flag = 1;

    if((flag = gpio_input_bit_get(ENC_GPIO_Port, ENC_IN1_PIN)) == RESET) 
    { value |= (1<<1); }

    if((flag = gpio_input_bit_get(ENC_GPIO_Port, ENC_IN2_PIN)) == RESET) 
    { value |= (1<<0); }

  return value; 
}

//-----------------------------------------------------------------------------//
void read_data_encoder(void)
{
  //for (uint32_t count = 0; count < 500000; count++)
  { 
    {
      val_new = read_gray_code_from_encoder(); //чтение данных энкодера

      switch (state_machine) //проверка номера стадии конечного автомата
      {
        case SM_WAIT: //стадия ожидания поворота энкодера
        if (flag_end_delay == 0) //если таймаут истёк
        {
          if (val_new != val_old) //если показания энкодера изменились
	        { 
            if (val_new == PULSE_STATE_FIRST_REV) 
            {
              state_machine = SM_FIRST; //переход на следующую стадию автомата
              flag_rev = 1; //флаг реверсного поворота энкодера

              #ifdef __USE_DBG
		            sprintf (DBG_buffer,  "1WAIT,flag_rev=%u\r\n", val_new);
		            DBG_PutString(DBG_buffer);
	            #endif
            }
            else
            {
              if (val_new == PULSE_STATE_FIRST_DIR)
              {
                state_machine = SM_FIRST; //переход на следующую стадию автомата
                flag_dir = 1; //флаг прямого поворота энкодера

                #ifdef __USE_DBG
		              sprintf (DBG_buffer,  "1WAIT,flag_dir=%u\r\n", val_new);
		              DBG_PutString(DBG_buffer);
	              #endif
              }
              else 
              {
                #ifdef __USE_DBG
                  sprintf (DBG_buffer,  "1RESET,new=%u,old=%u\r\n",  val_new, val_old);
		              DBG_PutString(DBG_buffer);
	              #endif

                flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
                state_machine = SM_RESET;  //переход в начальную стадию конечного автомата
              }
            }
            cnt_timeout = 0; //сброс счётчика таймаутов
            val_old = val_new; //сохранение текущего показания энкодера
            delay_ms (ms_delay); //начало таймаута
          }
        }
        break;

        case SM_FIRST: //стадия средней части импульса энкодера
          if (flag_end_delay == 0) //если таймаут истёк
          {
            if (val_new == val_old)
	          { 
              cnt_timeout++;
              if (cnt_timeout > MAX_CNT_TIMEOUT)
              {
                cnt_timeout = 0;  //сброс счётчика таймаутов
                flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
                state_machine = SM_RESET;
              }
              else
              { delay_ms (ms_delay); }//ожидание изменений показаний энкодера 
            }       
          else
          { 
            #ifdef __USE_DBG
              sprintf (DBG_buffer,  "2FIRST,val_new=%u\r\n",  val_new);
		          DBG_PutString(DBG_buffer);
	          #endif

            cnt_timeout = 0; //сброс счётчика таймаутов             
            val_old = val_new; //сохранение текущего показания энкодера
            if (val_new == PULSE_STATE_SECOND) //если данные от экодера соотвествуют ожидаемым
            { state_machine = SM_NULL; } //переход на следующую стадию автомата
            else //если данные от энкодера не соотвествуют ожидаемым
            {
              flag_rev = flag_dir = 0;
              state_machine = SM_RESET; //переход на стадию сброса показаний энкодера
            }
            delay_ms (ms_delay);  //начало таймаута ожидания изменений показаний энкодера 
          }
        }
        break;        

        case SM_NULL: //стадия последней части импульса энкодера
          if (flag_end_delay == 0) //если таймаут истёк
          {
            if (val_new == val_old) //если показания энкодера не изменилось
	          { 
              if (cnt_timeout > MAX_CNT_TIMEOUT) //если количество таймаутов ожидания изменения показаний энкодера превышено
              {
                cnt_timeout = 0;  //сброс счётчика таймаутов
                flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
                state_machine = SM_RESET; //переход в начальную стадию конечного автомата
              }
              else
              {  delay_ms (ms_delay); }//начало таймаута ожидания изменений показаний энкодера 
            }       
            else
            { 
              #ifdef __USE_DBG
                sprintf (DBG_buffer,  "3NULL,val_new=%u\r\n",  val_new);
		            DBG_PutString(DBG_buffer);
	            #endif

              if (val_new ==  PULSE_STATE_LAST_REV) //если данные от экодера соотвествуют ожидаемым
              {
                if (flag_rev == 1) //если установлен флаг реверсного поворота энкодера
                { 
                  state_machine = SM_LAST;  //переход на следующую стадию автомата
                  delay_ms (ms_delay); //начало таймаута
                }              
              }
              else
              {
                if (val_new == PULSE_STATE_LAST_DIR) //если данные от экодера соотвествуют ожидаемым
                {
                  if (flag_dir == 1) //если установлен флаг прямого поворота энкодера
                  {  
                    state_machine = SM_LAST;  //переход на следующую стадию автомата
                    delay_ms (ms_delay); //начало таймаута
                  }
                }
                else //если данные от энкодера не соотвествуют ожидаемым
                {
                  flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
                  state_machine = SM_RESET;  //переход на стадию сброса показаний энкодера
                }
              }
              val_old = val_new; //сохранение текущего показания энкодера
              cnt_timeout = 0; //сброс счётчика таймаутов
            }
          }
          break;

        case  SM_LAST: //проверка стадии импульса энкодера по умолчанию
          if (flag_end_delay == 0) //если таймаут истёк
          {
            if (val_new == val_old) //если показания энкодера не изменилось
	          { 
              if (cnt_timeout > MAX_CNT_TIMEOUT) //если количество таймаутов ожидания изменения показаний энкодера превышено
              {
                cnt_timeout = 0;  //сброс счётчика таймаутов
                flag_rev = flag_dir = 0; 
                state_machine = SM_RESET; //переход к начальной стадии конечного автомата
              }
              else
              {  delay_ms (ms_delay); }//таймаут ожидания изменений показаний энкодера 
            }       
            else
            {
              #ifdef __USE_DBG
                sprintf (DBG_buffer,  " 4LAST,val_new=%u\r\n",  val_new);
		            DBG_PutString(DBG_buffer);
	            #endif
                
              if (flag_rev == 1) //если установлен флаг реверсного поворота энкодера
              {
                enc_count--;  
                OLED_Clear(NONE_INVERTED);
                snprintf(lcd_buf, 25,"-cnt=%ld, %x, %x, %x", enc_count, val_new, val_old, val_old | val_new);
                OLED_DrawStr(lcd_buf, 15, 25);
                OLED_UpdateScreen();
              }
              else
              {
                if (flag_dir == 1) //если установлен флаг прямого поворота энкодера
                {
                  enc_count++;  
                  OLED_Clear(NONE_INVERTED);
                  snprintf(lcd_buf, 25,"+cnt=%ld, %x, %x, %x", enc_count, val_new, val_old, val_old | val_new);
                  OLED_DrawStr(lcd_buf, 15, 25);
                  OLED_UpdateScreen(); 
                } 
              } 
              flag_rev = flag_dir = 0; //сброс флагов поворота энкодера
              val_old = val_new; //сохранение текущего показания энкодера
              cnt_timeout = 0; //сброс счётчика таймаутов
              state_machine = SM_WAIT; //переход к начальной стадии конечного автомата
            }            
          } 
          break;  

        case  SM_RESET: //стадия ожидания сброса показанрий энкодера в состояние по умолчанию
          if (flag_end_delay == 0) //если таймаут истёк
          {
            if (val_new != val_old) //если показания энкодера изменились
            {
              if (val_new == PULSE_STATE_DEFAULT) //если энкодер в состоянии по умолчанию (поворот закончен)
	            { 
                state_machine = SM_WAIT; //переход к начальной стадии конечного автомата
                  
                #ifdef __USE_DBG
                  sprintf (DBG_buffer,  "END_RESET,new=%u,old=%u\r\n",  val_new, val_old);
		              DBG_PutString(DBG_buffer);
	              #endif
              }
              else
              {
                #ifdef __USE_DBG
                  sprintf (DBG_buffer,  "CONT_RESET,new=%u,old=%u\r\n",  val_new, val_old);
		              DBG_PutString(DBG_buffer);
	              #endif
              }
              val_old = val_new; //сохранение текущего показания энкодера
            }               
            delay_ms (ms_delay); //начало таймаута
          }
          break;

        default:
          break;         
      }
    }
  }
}

