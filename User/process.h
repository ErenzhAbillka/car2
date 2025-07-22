#ifndef __TRACK_H__
#define __TRACK_H__

#include "Delay.h"
#include "OLED.h"
#include "Timer.h"
#include "Encoder.h"  // 左轮编码器接口头文件
#include "Encoder1.h" // 右轮编码器接口头文件
#include "Motor.h"
#include "car.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "xunji.h" // 循迹传感器相关头文件
// #include "key.h"
// #include "led.h"
#include "Buzzer.h"
#include "pid.h"
#include "speed_filter.h" // 速度滤波接口
#include "HCSR04.h"		  // 速度滤波接口

// 宏定义部分，根据你的硬件参数配置
#define WHEEL_CIRCUMFERENCE 0.188f // 轮子周长，单位米，示例为直径0.06m的轮子
#define ENCODER_PPR 500			   // 编码器每转脉冲数（Pulses Per Revolution）
#define SAMPLE_TIME 0.01f		   // 采样时间，单位秒（对应定时器中断周期，比如10ms）
#define RIGHTWHEELPWM   500
#define LEFTWHEELPWM    500
#define STRAIGHT 0
#define TURN     1

void FollowAndTrack_Control(void);
float CalculateSpeed(int16_t encoder_count);
void process();
void processKeyJudgment(int Key);
void processJudgment(int choose, int targetRound);
void processTurn(int choose);

#endif

