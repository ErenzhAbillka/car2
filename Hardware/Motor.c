#include "stm32f10x.h"                  // Device header
#include "PWM.h"

/**
  * @brief  电机控制引脚和PWM初始化
  * @note   控制引脚使用 GPIOB 的 12~15，PWM 通道由 PWM_Init 初始化
  * @param  无
  * @retval 无
  */
void Motor_Init(void)
{
    // 开启 GPIOB 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // 初始化 PB12~PB15 为推挽输出（用于电机方向控制）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始化 PWM（用于电机速度控制）
    PWM_Init();
}

/**
  * @brief  设置左电机转速与方向
  * @param  Speed: 速度值，范围 -100 ~ +100。正值表示正转，负值表示反转。
  * @retval 无
  */
void Motor_leftSetSpeed(int8_t Speed)
{
    if (Speed >= 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);   // PB14 高电平
        GPIO_ResetBits(GPIOB, GPIO_Pin_15); // PB15 低电平 -> 正转
        PWM_SetLeftMotorPWM(Speed);            // 设置 PWM 占空比（通道3）
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_15); // PB15 低电平
        GPIO_SetBits(GPIOB, GPIO_Pin_14);   // PB14 高电平 -> 反转
        PWM_SetLeftMotorPWM(-Speed);           // 取反后作为 PWM 值
    }
}

/**
  * @brief  设置右电机转速与方向
  * @param  Speed: 速度值，范围 -100 ~ +100。正值表示正转，负值表示反转。
  * @retval 无
  */
void Motor_rightSetSpeed(int8_t Speed)
{
    if (Speed >= 0)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_12);   // PB12 高电平
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); // PB13 低电平 -> 正转
        PWM_SetRightMotorPWM(Speed);            // 设置 PWM 占空比（通道4）
    }
    else
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_13); // PB13 低电平
        GPIO_SetBits(GPIOB, GPIO_Pin_12);   // PB12 高电平 -> 反转
        PWM_SetRightMotorPWM(-Speed);           // 取反后作为 PWM 值
    }
}
