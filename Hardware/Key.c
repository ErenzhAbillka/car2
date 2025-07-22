#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t Key_Num;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)//返回状态
{
	uint8_t Temp;
	if (Key_Num)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	return 0;
}

uint8_t Key_GetState(void)//得到状态
{
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		return 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		return 2;
	}
	return 0;
}

void Key_Tick(void) //按键定时器 
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;//现在的Key_num值，以及上次的值
	
	Count ++;
	if (Count >= 10)//20ms进行一次，主定时器是1ms
	{
		Count = 0;
		
		PrevState = CurrState;//上一次的值
		CurrState = Key_GetState();//这一次的值
		
		if (CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
}
//按下去之前是前面Key_GetState(void)返回的函数，按下去之后值就变成了0 所以这边Key_Num = PrevState;