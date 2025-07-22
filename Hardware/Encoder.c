#include "stm32f10x.h"  // Device header

/**
  * @brief  左轮编码器初始化（TIM3，PA6-CH1，PA7-CH2）
  * @note   使用编码器接口模式，读取双路正交编码器的脉冲计数
  *         PA6 → TIM3_CH1，PA7 → TIM3_CH2
  * @param  无
  * @retval 无
  */
void Encoder_Left_Init(void)
{
    /* 1. 开启时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);      // 开启 TIM3 定时器时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     // 开启 GPIOA 时钟

    /* 2. 初始化 GPIOA6/A7 为上拉输入模式（连接编码器 A/B 相） */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;             // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;    // PA6/PA7 对应 TIM3_CH1/CH2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 3. 定时器基本单元配置 */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 时钟不分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
    TIM_TimeBaseInitStructure.TIM_Period = 65535;                    // 自动重装值，最大值（16位）
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;                     // 不预分频（等效系统主频直接驱动）
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            // 重复计数，仅高级定时器使用
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    /* 4. 输入捕获配置（CH1/CH2均加滤波） */
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);                // 先初始化为默认值

    TIM_ICInitStructure.TIM_ICFilter = 0x0F;               // 设置最大滤波（15），滤除干扰

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;       // 配置 CH1
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;       // 配置 CH2
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    /* 5. 设置编码器接口模式 */
    TIM_EncoderInterfaceConfig(
        TIM3,
        TIM_EncoderMode_TI12,          // 使用 CH1 和 CH2 双通道编码器模式
        TIM_ICPolarity_Rising,         // 不反相
        TIM_ICPolarity_Rising          // 不反相
    );

    /* 6. 清零计数器并启动定时器 */
    TIM_SetCounter(TIM3, 0);  // 初始化 CNT 为 0
    TIM_Cmd(TIM3, ENABLE);    // 启动 TIM3
}

/**
  * @brief  获取左轮编码器的增量值（TIM3）
  * @note   每次调用会自动清零 TIM3 的计数器
  * @param  无
  * @retval int16_t 增量计数（-32768 ~ 32767）
  */
int16_t Encoder_Left_GetDelta(void)
{
    int16_t delta = (int16_t)TIM_GetCounter(TIM3);  // 读取当前 CNT
    TIM_SetCounter(TIM3, 0);                        // 清零 CNT，准备下一次读取
    return delta;
}
