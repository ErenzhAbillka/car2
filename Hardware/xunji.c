#include "stm32f10x.h"                  // STM32标准库头文件
#include "xunji.h"


extern int red_sensors[8];            // 外部声明红外传感器状态数组

int  Trackn = 0;                      // 传感器组合状态整数表示

int red_sensors[8] = {0};             // 传感器上次状态缓存
int comePointFlag = 0;                // 传感器状态变化标志（上升沿或下降沿检测）

/**
 * @brief 限幅函数，保证值在[min, max]范围内
 * @param value 输入值
 * @param min 最小值
 * @param max 最大值
 * @return 限幅后的值
 */
int constrain(int value, int min, int max) 
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

/**
 * @brief 循迹传感器GPIO初始化
 * 配置8路GPIO输入，上拉输入模式
 */
void Xunji_Init(void)
{
	/*开启GPIOA、GPIOB、GPIOC时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/*GPIO初始化配置 - 输入上拉*/
	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIOA的11、15、0号引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           // 输入上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_15|GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       // 速度50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// GPIOB的4、5号引脚
	GPIO_InitTypeDef GPIO_InitStructure1;
	GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;
	GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure1);

	// GPIOC的13、14、15号引脚
	GPIO_InitTypeDef GPIO_InitStructure2;
	GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure2);
}

/**
 * @brief 读取8路循迹传感器状态，更新传感器数组
 * @param read_senors 传入的8位数组指针，用于存储读取的状态（0或1）
 */
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

	// 将8个传感器状态按位组合成一个整数，方便状态判断和显示
	Trackn = read_senors[0] * 128 +read_senors[1]*64+ read_senors[2]*32 + 
		read_senors[3]*16+read_senors[4]*8+ read_senors[5]*4+ 
		read_senors[6]*2+ read_senors[7]*1;
}

// GPIO端口和引脚映射数组，方便循环读取
GPIO_TypeDef* Xunji_PIN_PORTS[8] = { GPIOC, GPIOA,GPIOA, GPIOA,GPIOB, GPIOB,GPIOC, GPIOC};
uint16_t Xunji_PIN_PINS[8] = {GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_11, GPIO_Pin_15,GPIO_Pin_5, GPIO_Pin_4, GPIO_Pin_13, GPIO_Pin_14};

/**
 * @brief 更新红外传感器状态，检测边沿变化
 * 
 * 根据当前采集的传感器状态和之前状态比较，检测传感器状态是否发生了上升沿或下降沿，
 * 并更新全局变量comePointFlag标志。
 */
void update_redSensor(void) 
{
    int sensors[8];  // 临时数组保存当前传感器状态

    // 逐个读取GPIO引脚状态，注意传感器输出可能是低电平有效，转换为0/1
    for (int i = 0; i < 8; i++)
    {
        // GPIO_ReadInputDataBit返回1表示高电平，传感器可能低电平有效
        // 因此这里用条件 ?: 转换为 0/1状态
        sensors[i] = GPIO_ReadInputDataBit(Xunji_PIN_PORTS[i], Xunji_PIN_PINS[i]) ? 0 : 1;
    }

    // 比较当前和上次状态，检测边沿
    for (int i = 0; i < 8; i++) 
	{
        if (sensors[i] == 0 && red_sensors[i] == 1) 
		{
            comePointFlag = 1; // 上升沿检测到，表示由1变0
        } 
		else if (sensors[i] == 1 && red_sensors[i] == 0) 
		{
            comePointFlag = -1; // 下降沿检测到，表示由0变1
        }
        red_sensors[i] = sensors[i]; // 更新缓存状态
    }
}


/**
 * @brief 小车循迹主控制函数
 * @param Motor_leftSetSpeed 左电机速度指针（调用者需传地址）
 * @param Motor_rightSetSpeed 右电机速度指针（调用者需传地址）
 * @param base_v 基础速度（整数，电机PWM基准速度）
 * @note 函数使用PID调节电机速度实现循迹
 */
void trait(int* Motor_leftSetSpeed, int* Motor_rightSetSpeed, int base_v) 
{
    static float last_error = 0;    // 上一次误差，用于微分计算
    static float integral = 0;      // 积分累计量，用于积分计算
    
    int wandao_line = 1;            // 弯道检测标志，默认检测到弯道（1表示直道）

	// 检查前3个传感器是否有检测到线，若有，认为是弯道（wendao_line = 0）
	for(int i = 0; i < 8; i++) 
	{
        if(red_sensors[i] != 0) 
		{
			i++;
			if(i>=3)
			{
                wandao_line = 0;   // 检测到弯道
                break;
			}
        }
    }
	 
	// 根据弯道检测调整基础速度
	if(wandao_line) {
        base_v = base_v + 15; // 弯道时提高基础速度
    } else {
        base_v = base_v;          // 直道时恢复正常速度
    }
		
    // 计算循迹误差（加权差法）
    float error = (red_sensors[7] - red_sensors[0]) * 7 + 
                  (red_sensors[6] - red_sensors[1]) * 5 +
                  (red_sensors[5] - red_sensors[2]) * 3 + 
                  (red_sensors[4] - red_sensors[3]) * 1;
    
    // PID参数定义
    float Kp = 0.8;  // 比例系数
    float Ki = 0.01; // 积分系数
    float Kd = 0.3;  // 微分系数
    
    // PID控制算法
    integral += error;                        // 积分累加
    float derivative = error - last_error;  // 误差微分
    float correction = Kp * error + Ki * integral + Kd * derivative; // PID输出
    last_error = error;                      // 更新上次误差
    
    // 计算左右电机最终速度（带限幅）
    *Motor_leftSetSpeed = constrain(base_v + correction, -100, 100);
    *Motor_rightSetSpeed = constrain(base_v - correction, -100, 100);
}
