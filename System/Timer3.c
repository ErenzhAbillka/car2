#include "stm32f10x.h"

/**
  * @brief TIM3 初始化（10ms定时中断）
  * @note 72MHz主频，预分频71，周期9999 => 10ms中断
  */
void TIM3_Init(void)
{
    // 1. 开启TIM3时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. 配置定时器基础参数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period = 9999;          // 计数10000次
    TIM_TimeBaseInitStructure.TIM_Prescaler = 71;         // 72MHz/(71+1)=1MHz计数频率
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    // 3. 使能更新中断
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    // 4. 配置NVIC中断优先级（优先级设置比TIM1低，避免影响）
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;          
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  // 抢占优先级比TIM1大（数字大，优先级低）
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 5. 启动定时器
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief TIM3 中断服务函数
  */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        // 这里写你的周期任务代码，如另一组编码器读取、速度计算或其他功能
    }
}
