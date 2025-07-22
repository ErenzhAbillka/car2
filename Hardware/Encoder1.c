#include "stm32f10x.h"  // Device header

/**
  * @brief  右轮编码器初始化（使用 TIM1，PA8-CH1，PA9-CH2）
  * @note   使用编码器接口模式，读取正交编码器脉冲
  *         PA8 → TIM1_CH1，PA9 → TIM1_CH2
  * @param  无
  * @retval 无
  */
void Encoder_Right_Init(void)
{
    /* 1. 开启时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);      // 开启 TIM1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     // 开启 GPIOA 时钟

    /* 2. 配置 GPIOA8/A9 为上拉输入（编码器 A/B 相） */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;             // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;    // PA8:CH1, PA9:CH2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 配置定时器基本参数 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 时钟不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    TIM_TimeBaseInitStructure.TIM_Period = 65535;                    // 最大计数值（16位）
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;                     // 不预分频
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数器，仅高级定时器使用
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    /* 4. 输入捕获滤波器设置（CH1 & CH2） */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);      // 初始化结构体默认值
    TIM_ICInitStructure.TIM_ICFilter = 0x0F;     // 最大滤波，抗干扰

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    /* 5. 编码器接口模式配置 */
    TIM_EncoderInterfaceConfig(
        TIM1,
        TIM_EncoderMode_TI12,          // 编码器模式，双通道
        TIM_ICPolarity_Rising,         // CH1 不反相
        TIM_ICPolarity_Rising          // CH2 不反相
    );

    /* 6. 清零计数器并启动定时器 */
    TIM_SetCounter(TIM1, 0);      // 初始化 CNT 为 0
    TIM_Cmd(TIM1, ENABLE);        // 启动 TIM1
}

/**
  * @brief  获取右轮编码器的增量值（TIM1）
  * @note   每次调用后会自动清零 CNT
  * @param  无
  * @retval int16_t 编码器增量（-32768 ~ 32767）
  */
int16_t Encoder_Right_GetDelta(void)
{
    int16_t delta = (int16_t)TIM_GetCounter(TIM1);  // 读取 CNT 值
    TIM_SetCounter(TIM1, 0);                        // 清零 CNT
    return delta;
}

