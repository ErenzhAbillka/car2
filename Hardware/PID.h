#ifndef _PID_H
#define _PID_H

#include <stdint.h>  // 使用标准整数类型

// PID 输出限幅宏定义
#define duty_max  70
#define duty_min  -70

// PID 控制器结构体
typedef struct
{
    float SetValue;   // 期望值（目标值）
    float ActValue;   // 实际值（反馈值）

    float KP;         // 比例系数
    float KI;         // 积分系数
    float KD;         // 微分系数

    float ek;         // 当前误差
    float ek_1;       // 上一次误差
    float ek_2;       // 上上次误差
    float ek_sum;     // 误差累积（积分）

    float PIDmax;     // PID输出最大限制
    float PIDmin;     // PID输出最小限制

    float PIDout;     // PID输出值
} PID;


// 全局变量声明（实际定义放在 pid.c）
extern PID Speed;              // 速度环PID控制器实例
extern PID Deviation;          // 偏差环PID控制器实例
extern PID Angle;              // 角度环PID控制器实例

extern float Expected_Speed;   // 期望速度，单位自定义
extern int16_t Difference_Speed;  // 编码器速度差

extern int16_t MPU_GyroZ;      // MPU6050陀螺仪Z轴角速度值

extern float KP2;              // 偏差环比例系数
extern float KD2;              // 偏差环微分系数


// 计算绝对值的辅助函数
int16_t pid_abs(int16_t value);

// 初始化PID参数（设置KP，KI，KD，限幅等）
void PID_Init(void);

// 增量式PID控制函数
// pid: PID控制器指针
// flag: 是否启用限幅（1启用，0不启用）
// 返回增量输出值
float PID_Control_Inc(PID* pid, int flag);

// 位置式PID控制函数
// pid: PID控制器指针
// flag: 是否启用限幅（1启用，0不启用）
// 返回当前位置输出值
float PID_Control_Pos(PID* pid, int flag);

// 速度环PID计算，增量式，需外部调用
void Speed_PID_Count(void);

// 偏差环PID计算，带陀螺仪抗扰，需外部调用
void Deviation_PID_Count(float error);

// 角度环PID计算，需外部调用
void Angle_PID_Count(float deviation);

// 更新MPU6050陀螺仪Z轴数据，刷新全局变量MPU_GyroZ
void MPU6050_UpdateGyroZ(void);

#endif
