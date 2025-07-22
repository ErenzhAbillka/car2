#ifndef __SPEED_FILTER_H
#define __SPEED_FILTER_H

#include "stdint.h"

/* 滤波长度 */
#define SPEED_FILTER_LEN 5

/* 左轮速度滑动平均滤波
 * @param new_speed 左轮新速度（单位：m/s）
 * @return 滤波后的左轮速度（单位：m/s）
 */
float speed_filter_left(float new_speed);

/* 右轮速度滑动平均滤波
 * @param new_speed 右轮新速度（单位：m/s）
 * @return 滤波后的右轮速度（单位：m/s）
 */
float speed_filter_right(float new_speed);

/* 获取当前滤波后的左轮速度（单位：m/s） */
float get_filtered_speed_left(void);

/* 获取当前滤波后的右轮速度（单位：m/s） */
float get_filtered_speed_right(void);

#endif /* __SPEED_FILTER_H */