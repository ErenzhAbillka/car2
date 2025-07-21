#include "stm32f10x.h"
#include "pwm.h"

/**
  * 函    数：PWM初始化（TIM2 通道2 -> 左轮，通道3 -> 右轮）
  * 参    数：无
  * 返 回 值：无
  */
void PWM_Init(void)
{
    // 开启外设时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	// TIM2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// PA端口

    // 初始化 GPIOA.2 -> CH3，GPIOA.1 -> CH2
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // TIM2_CH2 -> 左轮
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // TIM2_CH3 -> 右轮
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置定时器为内部时钟
    TIM_InternalClockConfig(TIM2);

    // 配置时基单元（PWM频率 = 72M / (PSC+1) / (ARR+1)）
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;   // 72MHz / 720 = 100kHz
    TIM_TimeBaseStructure.TIM_Period = 100 - 1;      // PWM频率 = 100kHz / 100 = 1kHz
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置输出通道：PWM1，初始值为 0
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC2Init(TIM2, &TIM_OCInitStructure); // CH2 -> 左轮
    TIM_OC3Init(TIM2, &TIM_OCInitStructure); // CH3 -> 右轮

    // 使能定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**
  * 函    数：设置左轮 PWM 占空比
  * 参    数：Compare 占空比（0~100）
  */
void PWM_SetCompare_Left(uint16_t Compare)
{
    TIM_SetCompare2(TIM2, Compare); // TIM2_CH2 -> PA1 -> 左轮
}

/**
  * 函    数：设置右轮 PWM 占空比
  * 参    数：Compare 占空比（0~100）
  */
void PWM_SetCompare_Right(uint16_t Compare)
{
    TIM_SetCompare3(TIM2, Compare); // TIM2_CH3 -> PA2 -> 右轮
}
