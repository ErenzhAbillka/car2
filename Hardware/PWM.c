#include "stm32f10x.h"                  // Device header

/**
  * @brief  PWM初始化（TIM2 → CH3: 左轮, CH4: 右轮）
  * @note   使用 TIM2_CH3（PA2）控制左轮，TIM2_CH4（PA3）控制右轮
  *         PWM频率 = 72MHz / PSC / (ARR + 1) = 72MHz / 36 / 100 ≈ 20kHz
  * @param  无
  * @retval 无
  */
void PWM_Init(void)
{
    /* 1. 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);      // 开启 TIM2 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     // 开启 GPIOA 时钟

    /* 2. 初始化 GPIOA2/A3 为复用推挽输出，用于 PWM 输出 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           // 复用推挽输出（用于PWM）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;    // PA2 → TIM2_CH3，PA3 → TIM2_CH4
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 定时器基础配置 */
    TIM_InternalClockConfig(TIM2);  // 使用内部时钟（默认就是内部，可省略）

    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    TIM_TimeBaseInitStructure.TIM_Period = 100 - 1;                  // ARR: 自动重装值，计数上限为99
    TIM_TimeBaseInitStructure.TIM_Prescaler = 36 - 1;                // 预分频，PSC=35，时钟频率变为 2MHz
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数器（高级定时器才用）
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);             // 初始化 TIM2 基本时基单元

    /* 4. 输出比较通道初始化为 PWM 模式 */
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);                         // 结构体初始化（赋默认值）
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;               // PWM模式1：当计数器小于CCR时输出有效
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;       // 输出极性：高电平有效
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;   // 使能输出
    TIM_OCInitStructure.TIM_Pulse = 0;                              // 初始占空比为0%

    TIM_OC3Init(TIM2, &TIM_OCInitStructure);    // 配置通道3（PA2 → 左轮PWM）
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);    // 配置通道4（PA3 → 右轮PWM）

    /* 5. 启动定时器 */
    TIM_Cmd(TIM2, ENABLE);  // 启动 TIM2
}

/**
  * @brief  设置左轮 PWM 输出（TIM2_CH3）
  * @param  Compare: PWM占空比值，范围 0~100
  * @note   占空比 = Compare / (ARR+1) = Compare / 100
  */
void PWM_SetLeftMotorPWM(uint16_t Compare)
{
    TIM_SetCompare3(TIM2, Compare);  // 设置 CCR3 值，对应 PA2 左轮
}

/**
  * @brief  设置右轮 PWM 输出（TIM2_CH4）
  * @param  Compare: PWM占空比值，范围 0~100
  * @note   占空比 = Compare / (ARR+1) = Compare / 100
  */
void PWM_SetRightMotorPWM(uint16_t Compare)
{
    TIM_SetCompare4(TIM2, Compare);  // 设置 CCR4 值，对应 PA3 右轮
}
