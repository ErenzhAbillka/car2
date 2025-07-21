#include "stm32f10x.h"                  // STM32锟借�?�头锟侥硷拷
#include "Delay.h"                     // 锟斤拷时锟斤拷锟斤拷头锟侥硷�?

// 锟斤拷锟斤拷旒ｏ拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷吆锟绞憋拷锟斤拷叩锟紾PIO锟斤拷锟斤拷
#define Track_DAT	GPIO_Pin_4        // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷PA4
#define Track_SCL	GPIO_Pin_5        // 时锟斤拷锟斤拷锟斤拷锟斤拷PA5

uint8_t TrackN;   // 锟芥储锟斤拷取锟斤拷锟斤�?8位锟届迹锟斤拷锟斤拷锟斤拷状态锟街斤�?
float Err = 0;    // 锟届迹偏锟斤拷值锟斤拷锟斤拷锟斤拷PID偏锟斤拷锟斤拷锟�?

/**
 * @brief 锟斤拷�?�锟斤拷锟届迹锟斤拷锟斤拷锟斤拷GPIO锟剿匡拷
 *        锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷为锟斤拷锟斤拷锟斤拷锟�?，时锟斤拷锟斤拷锟斤拷锟斤拷为锟斤拷锟斤拷锟斤拷锟�
 */
void Track_Init(void)
{
    // 使锟斤拷GPIOA锟剿匡拷时锟斤拷
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIOStructure;
	
	// 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷PA4为锟斤拷锟斤拷锟斤拷锟斤�?
	GPIOStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIOStructure.GPIO_Pin = Track_DAT;
	GPIOStructure.GPIO_Speed = GPIO_Speed_50MHz;   // 锟斤拷锟斤拷锟劫度�?�拷锟斤拷锟斤拷锟斤拷影锟届，锟斤拷写锟斤拷锟斤拷习锟斤拷锟斤拷锟斤�?
	GPIO_Init(GPIOA, &GPIOStructure);
	
	// 锟斤拷锟斤拷时锟斤拷锟斤拷PA5为锟斤拷锟斤拷锟斤拷锟�
	GPIOStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOStructure.GPIO_Pin = Track_SCL;
	GPIOStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIOStructure);
}

/**
 * @brief 锟斤拷取锟届迹锟斤拷锟斤拷锟斤拷锟斤拷一位锟斤拷锟斤�?
 *        通锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�?ｏ拷锟斤拷取锟斤拷锟斤拷锟斤拷锟较�?�拷平锟斤拷锟轿筹拷一锟斤拷bit
 * @return 锟斤拷取锟侥碉拷锟斤拷位锟斤�?0锟斤�?1
 */
uint8_t Track_Read_bit(void)
{
	uint8_t bit = 0;
	
	GPIO_ResetBits(GPIOA, Track_SCL);    // 锟斤拷锟斤拷时锟斤拷锟�??ｏ拷准锟斤拷锟斤拷取
	bit = GPIO_ReadInputDataBit(GPIOA, Track_DAT);  // 锟斤拷取锟斤拷锟斤拷锟�??碉拷前锟斤拷平状�?
	GPIO_SetBits(GPIOA, Track_SCL);      // 锟斤拷锟斤拷时锟斤拷锟�??ｏ拷锟斤拷锟斤拷一锟斤拷时锟斤拷锟斤拷锟斤�?
	
	Delay_us(6);                        // 锟斤拷时6�?锟�??，锟斤拷证时锟斤拷锟饺�?�拷
	
	return bit;
}

/**
 * @brief 锟斤拷取锟届迹锟斤拷锟斤拷锟斤拷全锟斤拷8位锟斤拷锟斤�?
 *        锟斤拷锟轿�?�拷�?8锟斤拷位锟斤拷锟斤拷锟戒按位锟斤拷铣锟揭伙拷锟斤拷纸锟絋rackN
 * @param arr 指锟斤拷娲�锟斤拷取锟�?�节的憋拷锟斤拷指锟斤拷
 */
void Read_Track_DATA(uint8_t* arr)
{
	uint8_t n = 0;
	uint8_t strackarr[8] = {0};   // 锟斤拷时锟芥�?8位锟斤拷锟斤拷锟斤拷锟斤�?

	// �?锟斤拷锟斤拷�?8锟斤拷位
	for (n = 0; n < 8; n++)
	{
		strackarr[n] = Track_Read_bit();
	}

	// 8锟斤拷位锟斤拷铣锟揭伙拷锟斤拷纸冢锟阶�锟斤拷顺锟斤拷strackarr[0]锟斤拷锟斤拷锟轿伙拷锟絙it7锟斤�?
	TrackN = strackarr[7]      // bit0
	       + strackarr[6] * 2  // bit1
	       + strackarr[5] * 4  // bit2
	       + strackarr[4] * 8  // bit3
	       + strackarr[3] * 16 // bit4
	       + strackarr[2] * 32 // bit5
	       + strackarr[1] * 64 // bit6
	       + strackarr[0] * 128;// bit7

	*arr = TrackN;  // 通锟斤拷指锟�?返锟�?讹拷取�?
}

/**
 * @brief 锟斤拷锟捷轨迹锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷状态锟斤拷TrackN锟斤拷锟斤拷锟截讹拷应锟侥轨迹偏锟斤拷值Err
 *        通锟斤拷switch锟结构匹锟戒常锟斤拷锟届迹锟斤拷锟斤拷锟斤拷状态锟斤拷锟斤拷锟斤拷锟接︼拷锟狡�锟斤拷�?
 * @param car_state 锟斤拷前锟斤拷锟斤拷状态锟斤拷影锟斤拷偏锟斤拷值锟侥�?�拷锟斤�?
 * @return 锟届迹偏锟斤拷值锟斤拷锟斤拷位锟皆讹拷锟藉（锟斤拷锟斤拷PID锟斤拷锟狡ｏ�?
 */
float Track_Err(uint16_t car_state)
{
	switch(TrackN)
	{
		case 0xe7:	// 1110 0111锟斤拷锟斤拷示锟斤拷锟斤拷锟节轨迹锟斤拷锟侥ｏ拷锟斤拷锟�?0
			Err = 0;
			break;
		case 0xcf:	// 1100 1111锟斤拷偏锟揭ｏ拷锟斤拷锟斤拷锟�6
			Err = 6.0;
			break;
		case 0x9f:	// 1001 1111锟斤拷偏锟揭革拷锟斤拷锟斤拷锟斤拷锟�?9
			Err = 9.0;
			break;
		case 0x3f:	// 0011 1111锟斤拷锟斤拷锟揭ｏ拷锟斤拷锟斤拷锟�12
			Err = 12.0;
			break;
		case 0xf3:	// 1111 0011锟斤拷偏锟斤拷锟斤拷罡�6
			Err = -6.0;
			break;
		case 0xf9:	// 1111 1001锟斤拷偏锟斤拷锟斤拷锟斤拷锟筋负9
			Err = -9.0;
			break;
		case 0xfc:	// 1111 1100锟斤拷锟斤拷锟斤拷锟斤拷罡�12
			Err = -12.0;
			break;
		
		case 0xef:	// 1110 1111锟斤拷锟斤拷�?偏锟�?ｏ拷锟斤拷锟�?2.5
			Err = 2.5;
			break;
		case 0xdf:	// 1101 1111锟斤拷偏锟揭ｏ拷锟斤拷锟�?4
			Err = 4.0;
			break;
		case 0xbf:	// 1011 1111锟斤拷偏锟揭ｏ拷锟斤拷锟�?7.5
			Err = 7.5;
			break;
		case 0x7f:	// 0111 1111锟斤拷锟斤拷锟揭ｏ拷锟斤拷锟�?12
			Err = 12.0;
			break;
		case 0xf7:	// 1111 0111锟斤拷锟斤拷�?偏锟斤拷锟斤拷罡�?2.5
			Err = -2.5;
			break;
		case 0xfb:	// 1111 1011锟斤拷偏锟斤拷锟斤拷罡�4
			Err = -4.0;
			break;
		case 0xfd:	// 1111 1101锟斤拷偏锟斤拷锟斤拷罡�7.5
			Err = -7.5;
			break;
		case 0xfe:	// 1111 1110锟斤拷锟斤拷锟斤拷锟斤拷罡�12
			Err = -12.0;
			break;
		
//		case 0x8f:	// 1000 1111
//			Err = 8.0;
//			break;
		case 0x1f:	// 0001 1111锟斤拷锟斤拷偏锟斤拷锟斤拷锟�?7
			Err = 7.0;
			break;
		case 0xf8:	// 1111 1000锟斤拷锟斤拷偏锟斤拷锟斤拷锟�?-7
			Err = -7.0;
			break;
	}

	// 锟斤拷锟捷�?�拷锟斤拷状态锟斤拷锟斤拷偏锟筋，锟斤拷止锟斤拷某些状态时锟斤拷罘斤拷虿缓锟斤拷锟�?
	if(car_state == 0x1012 || car_state == 0x1013)
	{
		if(Err <= 0)  // 锟斤拷锟狡�锟斤拷为锟斤拷锟斤拷锟�?�，强锟狡癸拷锟斤拷
			Err = 0;
	}
	else if(car_state == 0x1022 || car_state == 0x1023)
	{
		if(Err >= 0)  // 锟斤拷锟狡�锟斤拷为锟斤拷锟斤拷锟�?�，强锟狡癸拷锟斤拷
			Err = 0;
	}

//	// 锟斤拷锟斤拷注锟酵达拷锟斤拷为锟斤拷锟捷筹拷状态微锟斤拷偏锟筋，锟缴革拷锟斤拷锟斤拷�?�锟斤拷锟斤�?
//	if(car_state == 0x1013)
//	{
//		Err = Err + 5.0;
//	}
//	else if(car_state == 0x1023)
//	{
//		Err = Err - 5.0;
//	}

	return Err;  // 锟斤拷锟�?硷拷锟斤拷锟狡�锟斤拷�?
}
