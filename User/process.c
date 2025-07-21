#include "process.h"

// 实际速度（滤波后），单位米/秒
float Actual_Speed_L = 0, Actual_Speed_R = 0;
// 编码器计数值
int encL, encR;
// 速度差，声明为外部变量，定义在其他文件中
extern int16_t Difference_Speed;

// 存储轨迹传感器数据（8位）
uint8_t track_sensor_data = 0;
// 轨迹偏差值（用于PID偏差输入）
float line_err = 0.0f;

/**
 * @brief 设置目标速度
 * @param speed 目标速度，单位根据PID设置（一般为编码器计数）
 */
void Set_Target_Speed(float speed)
{
    Expected_Speed = speed;
    Speed.SetValue = speed;
}

/**
 * @brief 限幅函数，限制value在[min, max]范围内
 * @param value 输入值
 * @param min 最小值
 * @param max 最大值
 * @return 限幅后的值
 */
int constrain(int value, int min, int max)
{
    if(value < min)
        return min;
    else if(value > max)
        return max;
    else
        return value;
}


void processNo_Init()
{
    MPU6050_UpdateGyroZ(); // 更新陀螺仪Z轴角速度，用于姿态控制或补偿

    encL = encoder_L_get(); // 读取左编码器计数
    encR = encoder_R_get(); // 读取右编码器计数

    Difference_Speed = encL - encR; // 计算左右轮速度差，用于偏差PID控制

    // 读取轨迹传感器数据（8位二进制数据，表示传感器状态）
    Read_Track_DATA(&track_sensor_data);

    // 根据轨迹传感器数据计算轨迹偏差，car_state传入0x0000（可根据需要调整）
    line_err = Track_Err(0x0000);

    // 速度PID控制，实际速度取左右编码器计数平均值
    Speed.ActValue = (float)(encL + encR) * 0.5f;
    Speed.SetValue = Expected_Speed;
    Speed_PID_Count();

    // 偏差PID控制，输入轨迹偏差，用于左右轮差速调节
    Deviation_PID_Count(line_err);

    // 计算最终PWM输出值（速度PID输出 + 偏差PID输出）
    int16_t pwmL = (int16_t)(Speed.PIDout + Deviation.PIDout);
    int16_t pwmR = (int16_t)(Speed.PIDout - Deviation.PIDout);

    // 限制PWM输出在有效范围内，防止过载或饱和
    pwmL = constrain(pwmL, duty_min, duty_max);
    pwmR = constrain(pwmR, duty_min, duty_max);

    // 发送PWM给左右电机驱动
    Motor_SetSpeed_L(pwmL);
    Motor_SetSpeed_R(pwmR);

    // OLED显示调试信息
    OLED_ShowString(1, 1, "Enc_L:");
    OLED_ShowNum(2, 1, encL, 5);

    OLED_ShowString(3, 1, "Enc_R:");
    OLED_ShowNum(4, 1, encR, 5);

    OLED_ShowString(5, 1, "Err:");
    OLED_ShowNum(6, 1, (int)(line_err * 10), 4); // 轨迹偏差放大10倍显示

    Delay_ms(50); // 延时50ms，控制循环周期及显示刷新速率
}

void processNo1()
{
}

void processNo2()
{
}

void processNo3()
{
}

/**
 * @brief 定时器4中断服务程序，周期性计算轮速
 */
void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  // 清除中断标志

        // 读取左轮编码器计数，并清零计数器，准备下一周期计数
        int16_t encoder_count_L = TIM_GetCounter(TIM3);
        TIM_SetCounter(TIM3, 0);

        // 读取右轮编码器计数，并清零计数器
        int16_t encoder_count_R = TIM_GetCounter(TIM1);
        TIM_SetCounter(TIM1, 0);

        // 计算轮子转数 = 脉冲数 / 脉冲每圈数
        float revol_L = encoder_count_L / PULSE_PER_ROUND;
        float revol_R = encoder_count_R / PULSE_PER_ROUND;

        // 计算轮子滚动距离 = 转数 * 轮子周长 (2πr)
        float dist_L = revol_L * 2 * 3.1416f * WHEEL_RADIUS;
        float dist_R = revol_R * 2 * 3.1416f * WHEEL_RADIUS;

        // 计算速度 = 距离 / 时间，带滤波处理获得平滑速度
        Actual_Speed_L = speed_filter_left(dist_L / DT);
        Actual_Speed_R = speed_filter_right(dist_R / DT);
    }
}