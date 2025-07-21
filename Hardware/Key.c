#include "stm32f10x.h"
#include "Delay.h"

#define KEY_NUM     2       // 按键数量

#define KEY_PIN_1   GPIO_Pin_1
#define KEY_PIN_2   GPIO_Pin_11

#define KEY_PORT    GPIOB

// 状态定义
#define KEY_DOWN    1
#define KEY_UP      0

static uint8_t key_state[KEY_NUM] = {0};      // 上一轮的状态
static uint8_t key_flag[KEY_NUM]  = {0};      // 标志位（是否有短按）

/**
  * 按键初始化
  */
void Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = KEY_PIN_1 | KEY_PIN_2;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

/**
  * 按键状态扫描（非阻塞，放在主循环或定时中断中定期调用）
  */
void Key_Scan(void)
{
    uint8_t read_now[KEY_NUM];
    read_now[0] = GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN_1);
    read_now[1] = GPIO_ReadInputDataBit(KEY_PORT, KEY_PIN_2);

    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        if (key_state[i] == 1 && read_now[i] == 0) // 高->低，按下
        {
            Delay_ms(5);  // 消抖
            if (GPIO_ReadInputDataBit(KEY_PORT, (i == 0 ? KEY_PIN_1 : KEY_PIN_2)) == 0)
            {
                key_flag[i] = 1; // 短按有效
            }
        }
        key_state[i] = read_now[i];
    }
}

/**
  * 获取按键状态（返回 KEY_DOWN 表示该键被短按一次）
  */
uint8_t Key_Get(uint8_t key_id)
{
    if (key_id < 1 || key_id > KEY_NUM) return 0;
    if (key_flag[key_id - 1])
    {
        key_flag[key_id - 1] = 0;  // 清除标志
        return KEY_DOWN;
    }
    return KEY_UP;
}
