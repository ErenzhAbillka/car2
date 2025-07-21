#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "PID.h"
#include "math.h"
#include "MPU6050.h"
#include "Encoder.h"
#include "MyI2C.h"
#include "MPU6050_Reg.h"



// PID 控制器实例
PID Speed = {0};
PID Deviation = {0};
PID Angle = {0};

// 全局变量定义（确保只定义一次）
float Expected_Speed = 500.0f;
int16_t Difference_Speed = 0;
int16_t MPU_GyroZ = 0;

float KP2 = 9.0f;    // 偏差环比例
float KD2 = 5.0f;    // 偏差环微分

// 计算绝对值辅助函数
int16_t pid_abs(int16_t value)
{
    return (value >= 0) ? value : -value;
}

// PID 初始化函数
void PID_Init(void)
{
    Speed.KP = 0.5f * 0.8f;
    Speed.KI = 0;
    Speed.KD = 8.0f * 0.8f;
    Speed.PIDmax = 1500;
    Speed.PIDmin = -1500;
    Speed.PIDout = 0;

    Deviation.KP = KP2;
    Deviation.KI = 0;
    Deviation.KD = KD2;
    Deviation.PIDmax = duty_max;
    Deviation.PIDmin = duty_min;
    Deviation.PIDout = 0;

    Angle.KP = 6.75f;
    Angle.KI = 0.17f;
    Angle.KD = 0;
    Angle.PIDmax = 4000;
    Angle.PIDmin = -4000;
    Angle.PIDout = 0;
}

// 增量式PID控制（带限幅）
float PID_Control_Inc(PID* pid, int flag)
{
    float inc;
    pid->ek = pid->SetValue - pid->ActValue;

    inc = pid->KP * (pid->ek - pid->ek_1)
        + pid->KI * pid->ek
        + pid->KD * (pid->ek - 2 * pid->ek_1 + pid->ek_2);

    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;

    if (flag)
    {
        if (inc > pid->PIDmax) inc = pid->PIDmax;
        if (inc < pid->PIDmin) inc = pid->PIDmin;
    }

    pid->PIDout += inc;
    return inc;
}

// 位置式PID控制
float PID_Control_Pos(PID* pid, int flag)
{
    float pos;
    pid->ek = pid->SetValue - pid->ActValue;
    pid->ek_sum += pid->ek;

    pos = pid->KP * pid->ek + pid->KI * pid->ek_sum + pid->KD * (pid->ek - pid->ek_1);

    pid->ek_2 = pid->ek_1;
    pid->ek_1 = pid->ek;

    if (flag)
    {
        if (pos > pid->PIDmax) pos = pid->PIDmax;
        if (pos < pid->PIDmin) pos = pid->PIDmin;
    }

    pid->PIDout = pos;
    return pos;
}

// 偏差PID计算，带陀螺仪抗扰
void Deviation_PID_Count(float error)
{
    Deviation.ActValue = error;
    Deviation.SetValue = 0;

    Deviation.PIDout = KP2 * pid_abs((int16_t)error) * error
                      + PID_Control_Pos(&Deviation, 0)
                      - KD2 * MPU_GyroZ;
}

// 速度PID计算，增量式
void Speed_PID_Count(void)
{
    Speed.ActValue = (float)(encoder_R_get() + encoder_L_get()) * 0.5f;
    Speed.SetValue = Expected_Speed;

    Speed.ek = Speed.SetValue - Speed.ActValue;

    Speed.PIDout += (Speed.ek * Speed.KP
                  + (Speed.ek - Speed.ek_1) * Speed.KD
                  + Speed.KI * (Speed.ek - 2 * Speed.ek_1 + Speed.ek_2));

    Speed.ek_2 = Speed.ek_1;
    Speed.ek_1 = Speed.ek;

    if (Speed.PIDout > Speed.PIDmax) Speed.PIDout = Speed.PIDmax;
    if (Speed.PIDout < Speed.PIDmin) Speed.PIDout = Speed.PIDmin;
}

// 角度PID计算
void Angle_PID_Count(float deviation)
{
    Angle.SetValue = deviation;
    Angle.ActValue = (float)MPU_GyroZ;

    PID_Control_Inc(&Angle, 1);

    if (Angle.PIDout > Angle.PIDmax) Angle.PIDout = Angle.PIDmax;
    if (Angle.PIDout < Angle.PIDmin) Angle.PIDout = Angle.PIDmin;
}

// 更新 MPU6050 Z轴陀螺仪值
void MPU6050_UpdateGyroZ(void)
{
    int16_t accX, accY, accZ;
    int16_t gyroX, gyroY, gyroZ;

    MPU6050_GetData(&accX, &accY, &accZ, &gyroX, &gyroY, &gyroZ);
    MPU_GyroZ = gyroZ;
}
