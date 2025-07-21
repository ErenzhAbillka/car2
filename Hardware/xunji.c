#include "stm32f10x.h"                  // Device header


extern int red_sensors[8];


int red_sensors[8] = {0}; // 上次传感器状态
int comePointFlag = 0;      // 传感器状态变化标志

int constrain(int value, int min, int max) 
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void Xunji_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//开启GPIOA的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_15|GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure1);
  
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure2);
	
}

void read_sensors(int*read_senors)
{
	read_senors[0]=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15);
	read_senors[1]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
	read_senors[2]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11);
	read_senors[3]=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15);
	read_senors[4]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5);
	read_senors[5]=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
	read_senors[6]=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13);
	read_senors[7]=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_14);
	
}

GPIO_TypeDef* Xunji_PIN_PORTS[8] = { GPIOC, GPIOA,GPIOA, GPIOA,GPIOB, GPIOB,GPIOC, GPIOC};
uint16_t Xunji_PIN_PINS[8] = {GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_11, GPIO_Pin_15,GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_13, GPIO_Pin_14};

void update_redSensor(void) 
{
    int sensors[8];  // 改为与硬件实际一致
    for (int i = 0; i < 8; i++)
  {
        // 使用标准外设库函数读取 GPIO 状态
        sensors[i] = GPIO_ReadInputDataBit(Xunji_PIN_PORTS[i], Xunji_PIN_PINS[i]) ? 0 : 1;
  }

    for (int i = 0; i < 8; i++) 
	{
        if (sensors[i] == 0 && red_sensors[i] == 1) 
				{
            comePointFlag = 1; // 上升沿
        } else if (sensors[i] == 1 && red_sensors[i] == 0) {
            comePointFlag = -1; // 下降沿
        }
        red_sensors[i] = sensors[i]; // 更新状态
   }
}

void trait(int* Motor_leftSetSpeed, int* Motor_rightSetSpeed, int base_v) 
{
    int speed = (red_sensors[7] - red_sensors[0]) * 7 + 
                (red_sensors[6] - red_sensors[1]) * 5 +
                (red_sensors[5] - red_sensors[2]) * 3 + 
                (red_sensors[4] - red_sensors[3]) * 1;

    int max_speed = 100;  // 最大速度
    int min_speed = -100; // 最小速度

    *Motor_leftSetSpeed = constrain(base_v + speed *2, min_speed, max_speed);
    *Motor_rightSetSpeed = constrain(base_v -speed *2, min_speed, max_speed);
}


float calculate_line_deviation(int sensors[8]) {
    // 根据传感器值计算偏差，权重可根据传感器位置定义
    // 这里是示例权重，可以调整
    float weights[8] = {-3.5f, -2.5f, -1.5f, -0.5f, 0.5f, 1.5f, 2.5f, 3.5f};
    float deviation = 0;
    int active_count = 0;

    for (int i = 0; i < 8; i++) {
        if (sensors[i]) {
            deviation += weights[i];
            active_count++;
        }
    }

    if (active_count > 0)
        return deviation / active_count;
    else
        return 0;  // 传感器无检测时，偏差为0或者上一次偏差
}

