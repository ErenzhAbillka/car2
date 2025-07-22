#include "stm32f10x.h" // STM32标准库头文件

/**
 * @brief 主程序入口
 * @note 初始化所有模块，进入死循环等待中断和处理按键等
 */
int main(void)
{
	// 初始化各硬件模块
	OLED_Init();		  // OLED显示屏初始化
	// Timer_Init();		  // 定时器初始化（含中断配置）
	Encoder_Left_Init();  // 左轮编码器初始化
	Encoder_Right_Init(); // 右轮编码器初始化
	Motor_Init();		  // 电机驱动初始化
	mpu_dmp_init();		  // MPU6050传感器初始化（含DMP）
	Xunji_Init();		  // 循迹传感器初始化
	Key_Init();			  // 按键初始化
	LED_Init();			  // 指示灯初始化
	Buzzer_Init();		  // 蜂鸣器初始化
	PID_Init();			  // PID控制参数初始化

	// 在OLED第一行显示固定标签
	OLED_ShowString(1, 1, "Track:");

	while (1)
	{
		process();
	}
}


