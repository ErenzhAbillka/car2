#include "stm32f10x.h"                  // Device header

/**
  * @brief TIM4 初始化（10ms定时中断）
  * @note 72MHz主频，预分频71，周期9999 => 10ms中断
  */
	
void TIM4_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_InternalClockConfig(TIM4);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInitStructure.TIM_Prescaler = 7199;   // 72MHz / (7199+1) = 10kHz
    TIM_TimeBaseInitStructure.TIM_Period = 99;        // 计数100次 -> 10ms (100/10kHz = 10ms)

    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    TIM_Cmd(TIM4, ENABLE);
}
