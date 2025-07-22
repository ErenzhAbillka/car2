#include "stm32f10x.h"                  // Device header
#include "Motor.h"

void car_init()
{
	Motor_Init();
}

void go_ahead()
{
	Motor_leftSetSpeed(10);
	Motor_rightSetSpeed(10);
}

void go_back()
{
		Motor_leftSetSpeed(-10);
	Motor_rightSetSpeed(-10);
}

void left_Turn()
{
		Motor_leftSetSpeed(0);
	Motor_rightSetSpeed(10);
}

void right_Turn()
{
		Motor_leftSetSpeed(10);
	  Motor_rightSetSpeed(0);
}

void self_left()
{
		Motor_leftSetSpeed(-10);
	  Motor_rightSetSpeed(10);
}

void self_right()
{
		Motor_leftSetSpeed(10);
	  Motor_rightSetSpeed(-10);
}

void stop()
{
		Motor_leftSetSpeed(0);
	  Motor_rightSetSpeed(0);
}