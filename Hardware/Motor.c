#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "MPU6050.h"

/* 电机初始化 */
void Motor_Init(void)
{
    /* 开启GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    /* 初始化 PA4 PA5 控制左轮方向，PA6 PA7 控制右轮方向 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    PWM_Init();  // 初始化 PWM
}

/* 设置左轮速度，范围：-100 ~ +100 */
void Motor_SetSpeed_L(int8_t Speed_L)
{
    if (Speed_L >= 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_4);    // 正转
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare_Left(Speed_L);       // 设置 PWM
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_4);  // 反转
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        PWM_SetCompare_Left(-Speed_L);      // 取绝对值
    }
}

/* 设置右轮速度，范围：-100 ~ +100 */
void Motor_SetSpeed_R(int8_t Speed_R)
{
    if (Speed_R >= 0)
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_6);    // 正转
        GPIO_ResetBits(GPIOA, GPIO_Pin_7);
        PWM_SetCompare_Right(Speed_R);
    }
    else
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_6);  // 反转
        GPIO_SetBits(GPIOA, GPIO_Pin_7);
        PWM_SetCompare_Right(-Speed_R);
    }
}





//void Move_stop(void)
//{
//	GPIO_ResetBits(GPIOA, GPIO_Pin_4);	//PA5置低电平，设置方向为正转
//	GPIO_ResetBits(GPIOA, GPIO_Pin_5);	//PA5置低电平，设置方向为正转
//	GPIO_ResetBits(GPIOA, GPIO_Pin_6);	//PA5置低电平，设置方向为正转
//	GPIO_ResetBits(GPIOA, GPIO_Pin_7);	//PA5置低电平，设置方向为正转
//}


//void turn_right_90(int16_t GZ, int16_t Target_right)
//{
//    if (GZ < Target_right)
//    {
//        Motor_SetSpeed1(28);				//设置直流电机的速度为速度变量
//				Motor_SetSpeed2(168);				//设置直流电机的速度为速度变量
//			
//    }
//    else
//    {
//			Move_stop();
//    }
//}
//void turn_back(int16_t GZ, int16_t Target_right)
//{
//    if ((GZ < Target_right) && (GZ > 0))
//    {
//        Motor_SetSpeed1(28);				//设置直流电机的速度为速度变量
//				Motor_SetSpeed2(168);				//设置直流电机的速度为速度变量
//    }
//    else if (( GZ> Target_right) || ( GZ < 0))
//    {
//        Move_stop();
//    }
//}

