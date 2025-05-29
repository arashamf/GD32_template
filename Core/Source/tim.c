//-------------------------------------------------------------------------------------//
#include "tim.h"
#include "gd32f30x.h"

//-------------------------------------------------------------------------------------//
#define     TIM_MS_DELAY            TIMER6
#define     RCU_TIMER_DELAY         RCU_TIMER6
#define     TIM_MS_IRQHandler       TIMER6_IRQHandler
#define     TIM_MS_DELAY_IRQn       TIMER6_IRQn 
//#define     TIM_MS_DELAY_IRQn       55
//-------------------------------------------------------------------------------------//
uint8_t flag_end_delay = 0; //флаг окончания таймаута

//-------------------------------------------------------------------------------------//
void timer_delay_init (void)
{
    timer_parameter_struct delay_tim_init;

    rcu_periph_clock_enable(RCU_TIMER_DELAY);
    timer_deinit(TIM_MS_DELAY);

    // TIMER configuration 
    delay_tim_init.prescaler         = (30000-1); //предделитель 30000
    delay_tim_init.alignedmode       = TIMER_COUNTER_EDGE;
    delay_tim_init.counterdirection  = TIMER_COUNTER_UP;
    delay_tim_init.period            = (4000-1);
    delay_tim_init.clockdivision     = TIMER_CKDIV_DIV1 ; //делитель частоты таймера
    delay_tim_init.repetitioncounter = 0;
    timer_init(TIM_MS_DELAY, &delay_tim_init);

    timer_interrupt_enable(TIM_MS_DELAY, TIMER_INT_UP);
    timer_interrupt_flag_clear(TIM_MS_DELAY, TIMER_INT_FLAG_UP);

    nvic_irq_enable(TIM_MS_DELAY_IRQn, 1, 1);
}


//-------------------------------------------------------------------------------------//
void delay_ms (uint16_t delay)
{
    flag_end_delay = 1;
    timer_counter_value_config(TIM_MS_DELAY, 0);
    timer_autoreload_value_config(TIM_MS_DELAY, 4*delay);
    timer_interrupt_flag_clear(TIM_MS_DELAY, TIMER_INT_FLAG_UP);
    timer_enable(TIM_MS_DELAY);

}

//-------------------------------------------------------------------------------------//
void timer_delay_callback (void)
{
	if (timer_interrupt_flag_get(TIM_MS_DELAY, TIMER_INT_FLAG_UP) == SET)
	{	
		timer_interrupt_flag_clear(TIM_MS_DELAY, TIMER_INT_FLAG_UP); 
		timer_disable(TIM_MS_DELAY); 
        flag_end_delay = 0;
	}
}

