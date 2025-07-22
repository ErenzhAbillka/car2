#ifndef __HCSR04_H
#define __HCSR04_H



// 最大最小PWM定义（根据你的电机控制能力调整）
#define MAX_PWM     1000
#define MIN_PWM     -1000

// 目标距离（单位：mm）
#define TARGET_DISTANCE_MM 200.0f

// PID结构体定义
typedef struct
{
    float Kp;
    float Ki;
    float Kd;

    float prev_error;
    float integral;
} PID_TypeDef;

// 外部变量声明
extern PID_TypeDef Distance_PID;
extern float base_v;

// 函数声明
float constrain_float(float value, float min, float max);
void Distance_PID_Control(void);
void HC_SR04_Init(void);
int16_t sonar_mm(void);
float sonar(void);

#endif
