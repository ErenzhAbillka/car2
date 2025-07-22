#include "stm32f10x.h"
#include "Delay.h"
#include "Motor.h"



#define Echo GPIO_Pin_6		//HC-SR04模块的Echo脚接GPIOB6
#define Trig GPIO_Pin_5		//HC-SR04模块的Trig脚接GPIOB5
#define MAX_PWM     1000    // PWM最大值，根据你PWM配置调整
#define MIN_PWM     -1000   // 若允许反转，否则设置为0
#define TARGET_DISTANCE_MM 200.0f  // 目标距离：200mm = 20cm



extern float base_v;
uint64_t time=0;			//声明变量，用来计时
uint64_t time_end=0;		//声明变量，存储回波信号时间

void HC_SR04_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	//启用GPIOB的外设时钟	
	GPIO_InitTypeDef GPIO_InitStructure;					//定义结构体
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		//设置GPIO口为推挽输出
	GPIO_InitStructure.GPIO_Pin = Trig;						//设置GPIO口5
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//设置GPIO口速度50Mhz
	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化GPIOB
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;			//设置GPIO口为下拉输入模式
	GPIO_InitStructure.GPIO_Pin = Echo;						//设置GPIO口6
	GPIO_Init(GPIOB,&GPIO_InitStructure);					//初始化GPIOB
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,0);						//输出低电平
	Delay_us(15);											//延时15微秒
}

int16_t sonar_mm(void)									//测距并返回单位为毫米的距离结果
{
	uint32_t Distance,Distance_mm = 0;
	GPIO_WriteBit(GPIOB,Trig,1);						//输出高电平
	Delay_us(15);										//延时15微秒
	GPIO_WriteBit(GPIOB,Trig,0);						//输出低电平
	while(GPIO_ReadInputDataBit(GPIOB,Echo)==0);		//等待低电平结束
	time=0;												//计时清零
	while(GPIO_ReadInputDataBit(GPIOB,Echo)==1);		//等待高电平结束
	time_end=time;										//记录结束时的时间
	if(time_end/100<38)									//判断是否小于38毫秒，大于38毫秒的就是超时，直接调到下面返回0
	{
		Distance=(time_end*346)/2;						//计算距离，25°C空气中的音速为346m/s
		Distance_mm=Distance/100;						//因为上面的time_end的单位是10微秒，所以要得出单位为毫米的距离结果，还得除以100
	}
	return Distance_mm;									//返回测距结果
}

float sonar(void)										//测距并返回单位为米的距离结果
{
	uint32_t Distance,Distance_mm = 0;
	float Distance_m=0;
	GPIO_WriteBit(GPIOB,Trig,1);					//输出高电平
	Delay_us(15);
	GPIO_WriteBit(GPIOB,Trig,0);					//输出低电平
	while(GPIO_ReadInputDataBit(GPIOB,Echo)==0);
	time=0;
	while(GPIO_ReadInputDataBit(GPIOB,Echo)==1);
	time_end=time;
	if(time_end/100<38)
	{
		Distance=(time_end*346)/2;
		Distance_mm=Distance/100;
		Distance_m=Distance_mm/1000;
	}
	return Distance_m;
}

void TIM3_IRQHandler(void)			//更新中断函数，用来计时，每10微秒变量time加1
{									// https://blog.zeruns.tech
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)		//获取TIM3定时器的更新中断标志位
	{
		time++;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);			//清除更新中断标志位
	}
}



// PID控制器结构体
typedef struct
{
    float Kp;
    float Ki;
    float Kd;

    float prev_error;
    float integral;
} PID_TypeDef;

// 初始化PID参数
PID_TypeDef Distance_PID = {
    .Kp = 1.0f,
    .Ki = 0.05f,
    .Kd = 0.3f,
    .prev_error = 0.0f,
    .integral = 0.0f
};




/**
  * @brief 限制数值在[min, max]之间
  */
float constrain_float(float value, float min, float max)
{
    if (value < min) return min;
    else if (value > max) return max;
    else return value;
}

/**
  * @brief 距离PID控制函数
  * @note 通过 sonar_mm() 获取当前距离，控制 base_v 输出PWM
  */
void Distance_PID_Control(void)
{
    int Distance_mm = sonar_mm();  // 当前测量距离（单位mm）
    float error = Distance_mm - TARGET_DISTANCE_MM;

    // 积分项
    Distance_PID.integral += error;

    // 微分项
    float derivative = error - Distance_PID.prev_error;

    // PID 输出
    float output = Distance_PID.Kp * error +
                   Distance_PID.Ki * Distance_PID.integral +
                   Distance_PID.Kd * derivative;

    // 更新前一次误差
    Distance_PID.prev_error = error;

    // PWM输出限幅
    base_v = constrain_float(output, MIN_PWM, MAX_PWM);

    // 控制电机
    if (base_v >= 0)
    {
        Motor_leftSetSpeed((int)base_v);  // 正转，速度前进
				Motor_rightSetSpeed((int)base_v);
    }

}


