#include "stm32f10x.h"                  // Device header

/**
  * 函    数：右编码器初始化（使用TIM3）
  * 参    数：无
  * 返 回 值：无
  */
void Encoder_R_Init(void)
{
    // 开启时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA, ENABLE);

    // GPIO 初始化，PA8和PA9作为输入，上拉输入
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 定时器时基初始化
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65535;  // 计数最大值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;   // 不分频
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // 输入捕获初始化
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);

    // 配置TIM1通道1
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    // 配置TIM1通道2
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM1, &TIM_ICInitStructure);

    // 配置编码器接口模式
    TIM_EncoderInterfaceConfig(TIM1,
                              TIM_EncoderMode_TI12,
                              TIM_ICPolarity_Rising,
                              TIM_ICPolarity_Rising);

    // 使能TIM1计数器
    TIM_Cmd(TIM1, ENABLE);

    // TIM1是高级定时器，启用主输出使能
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}



/**
  * 函    数：左侧编码器初始化（使用TIM3）
  * 参    数：无
  * 返 回 值：无
  */
void Encoder_L_Init(void)
{
    /*开启时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //开启TIM3的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); //开启GPIOB和AFIO时钟（需禁用JTAG）

    /*禁用JTAG功能，释放PB4（TIM3_CH1）*/
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);        //禁用JTAG，保留SWD调试

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;          //PB4(TIM3_CH1)和PB5(TIM3_CH2)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);                          //将PB4和PB5初始化为上拉输入

    /*时基单元初始化*/
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;               //16位计数器最大值
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;                //不分频
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);

    /*输入捕获初始化*/
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;                         //滤波器参数
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICFilter = 0xF;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    /*编码器接口配置*/
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

    /*TIM使能*/
    TIM_Cmd(TIM3, ENABLE);                                          //使能TIM3
}



/**
  * 函    数：获取左侧编码器的增量值
  * 参    数：无
  * 返 回 值：自上此调用此函数后，左侧编码器的增量值
  */
int16_t encoder_L_get(void)
{
    int16_t Temp;
    Temp = TIM_GetCounter(TIM3);  // 改为读取TIM3
    TIM_SetCounter(TIM3, 0);
    return Temp;
}


/**
  * 函    数：获取右侧编码器的增量值
  * 参    数：无
  * 返 回 值：自上此调用此函数后，右侧编码器的增量值
  */
int16_t encoder_R_get(void)
{
    int16_t Temp;
    Temp = TIM_GetCounter(TIM1);  // 改为读取TIM1
    TIM_SetCounter(TIM1, 0);
    return Temp;
}


