#ifndef TIM_H
#define TIM_H

//--------------------------------------------------------------------------------//
#include "gd32f30x.h"

//--------------------------------------------------------------------------------//
void timer_delay_init (void);
void delay_ms (uint16_t );

extern uint8_t flag_end_delay;
#endif