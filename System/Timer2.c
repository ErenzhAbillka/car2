#include "stm32f10x.h"

/**
  * @brief TIM1 初始化（10ms中断一次）
  * @note 72MHz主频时，Period=9999, Prescaler=719 => 10ms
  */
#include "stm32f10x.h"

/**
  * @brief  TIM1 初始化，10ms周期中断
  */
void TIM1_Init(void)
{
    // 1. 开启TIM1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // 2. 配置定时器基础参数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_Period = 9999;          // 自动重装载值，计数10000个tick（0~9999）
    TIM_TimeBaseInitStructure.TIM_Prescaler = 719;        // 预分频，72MHz/720=100kHz计数频率
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // 3. 使能更新中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    // 4. 配置中断优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;           // TIM1 更新中断号
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;    // 抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;           // 响应优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 5. 启动定时器主输出
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    // 6. 启动定时器计数
    TIM_Cmd(TIM1, ENABLE);
}

/**
  * @brief TIM1 更新中断服务函数
  */
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

        // 这里写你的周期任务代码，比如读取编码器、更新速度等
    }
}





