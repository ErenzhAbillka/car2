#ifndef __XUNJI_H
#define __XUNJI_H

#include "stm32f10x.h"

// 传感器状态数组，外部声明
extern int red_sensors[8];

// 传感器组合状态（整型）
extern int Trackn;

// 传感器状态变化标志（上升沿/下降沿）
extern int comePointFlag;

/**
 * @brief 限幅函数，保证值在[min, max]范围内
 * @param value 输入值
 * @param min 最小值
 * @param max 最大值
 * @return 限幅后的值
 */
int constrain(int value, int min, int max);

/**
 * @brief 初始化循迹传感器GPIO口
 */
void Xunji_Init(void);

/**
 * @brief 读取8路循迹传感器状态，更新传感器数组
 * @param read_senors 指向长度为8的整型数组，用于存储读取的状态
 */
void read_sensors(int* read_senors);

/**
 * @brief 更新红外传感器状态，检测状态边沿
 */
void update_redSensor(void);

/**
 * @brief 小车循迹主控制函数，基于PID调节电机速度
 * @param Motor_leftSetSpeed 指向左电机速度变量的指针（PWM值）
 * @param Motor_rightSetSpeed 指向右电机速度变量的指针（PWM值）
 * @param base_v 基础速度（整数）
 */
void trait(int* Motor_leftSetSpeed, int* Motor_rightSetSpeed, int base_v);

#endif /* __XUNJI_H */
