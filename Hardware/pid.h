#ifndef __PID_H
#define __PID_H


typedef struct
{
    float SetValue; // 期望值 参考值
    float ActValue; // 实际值
    float KP; // 比例因子
    float KI; // 积分因子
    float KD; // 微分因子
    float ek; // 本级误差
    float ek_1; // 上一次
    float ek_2; // 上上次
    float ek_sum; // 误差累积
    float Sum_max; // 误差累和上限
    float Sum_min; // 误差累和下限
    float PIDmax; // max limit
    float PIDmin; // min limit
    float PIDout; // output
}PID;

extern PID speed;//速度
extern PID Angle;//角度
//extern PID Deviation;//陀螺仪阻尼补偿项

void PID_Init(void);
float PID_Control_Inc(PID* pid,int flag);
float PID_Control_Pos(PID* pid,int flag);
float Speed_PID_Count(float actvalue_sudu,float expected_sudu);
float Angle_PID_Count(float actvalue_angle,float expected_angle);
#endif