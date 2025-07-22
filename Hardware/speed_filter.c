#include "speed_filter.h"


#define SPEED_FILTER_LEN 5

// 左右轮滤波缓存与索引
static float speed_buffer_left[SPEED_FILTER_LEN] = {0};
static float speed_buffer_right[SPEED_FILTER_LEN] = {0};
static uint8_t speed_index_left = 0;
static uint8_t speed_index_right = 0;

// 最终滤波输出
static float filtered_speed_left = 0;
static float filtered_speed_right = 0;

/**
 * @brief 左轮速度滑动平均滤波
 * @param new_speed 左轮新速度（单位：m/s）
 * @return 滤波后的左轮速度（单位：m/s）
 */
float speed_filter_left(float new_speed)
{
    speed_buffer_left[speed_index_left] = new_speed;
    speed_index_left = (speed_index_left + 1) % SPEED_FILTER_LEN;

    float sum = 0;
    for (int i = 0; i < SPEED_FILTER_LEN; i++) {
        sum += speed_buffer_left[i];
    }
    filtered_speed_left = sum / SPEED_FILTER_LEN;
    return filtered_speed_left;
}

/**
 * @brief 右轮速度滑动平均滤波
 * @param new_speed 右轮新速度（单位：m/s）
 * @return 滤波后的右轮速度（单位：m/s）
 */
float speed_filter_right(float new_speed)
{
    speed_buffer_right[speed_index_right] = new_speed;
    speed_index_right = (speed_index_right + 1) % SPEED_FILTER_LEN;

    float sum = 0;
    for (int i = 0; i < SPEED_FILTER_LEN; i++) {
        sum += speed_buffer_right[i];
    }
    filtered_speed_right = sum / SPEED_FILTER_LEN;
    return filtered_speed_right;
}

/**
 * @brief 获取当前滤波后的左轮速度（m/s）
 */
float get_filtered_speed_left(void)
{
    return filtered_speed_left;
}

/**
 * @brief 获取当前滤波后的右轮速度（m/s）
 */
float get_filtered_speed_right(void)
{
    return filtered_speed_right;
}
