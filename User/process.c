#include "process.h"

// 全局变量
int sensors1[8];   // 传感器状态缓存数组
int stop_flag = 0; // 停车标志计数，用于判断是否停车
int KeyNum = 0;	   // 按键编号
int confirm = 0;   // 驱动确认标志，1使能驱动，0禁止
int MODE = 0;	   // 工作模式变量（备用）
int num = 0;	   // 按键切换变量
int left_pwm, right_pwm; // ?

int startFlag; // 启动标志位
int turn = 0;	// 转弯标志位		
int key;

float base_v = 53;		 // 巡线基础速度（可调）
float leftSpeed = 0.0f;	 // 左轮滤波后速度，单位m/s
float rightSpeed = 0.0f; // 右轮滤波后速度，单位m/s

uint64_t numlen(uint64_t num) // 计算数字的长度
{
	uint64_t len = 1;	   // 初始长度为1
	for (; num > 9; ++len) // 判断num是否大于9，否则长度+1
		num /= 10;		   // 使用除法进行运算，直到num小于1
	return len;			   // 返回长度的值
}

void FollowAndTrack_Control(void)
{
	// 获取距离
	int Distance_mm = sonar_mm();
	if (Distance_mm < 50 || Distance_mm > 1000)
		Distance_mm = TARGET_DISTANCE_MM;

	// 跟车 PID
	float error = Distance_mm - TARGET_DISTANCE_MM;
	Distance_PID.integral += error;
	float derivative = error - Distance_PID.prev_error;
	float follow_adjust = Distance_PID.Kp * error +
						  Distance_PID.Ki * Distance_PID.integral +
						  Distance_PID.Kd * derivative;
	Distance_PID.prev_error = error;

	// 组合速度，限幅
	float adjusted_speed = base_v + follow_adjust;
	adjusted_speed = constrain_float(adjusted_speed, 100, MAX_PWM);

	// 巡线 PID，控制左右轮 PWM
	trait(&left_pwm, &right_pwm, (int)adjusted_speed);

	// 控制左右轮
	Motor_leftSetSpeed(right_pwm);
	Motor_rightSetSpeed(left_pwm);
}

/**
 * @brief 计算轮子的线速度
 * @param encoder_count 编码器在采样周期内的计数值（增量）
 * @return 计算出的线速度，单位米/秒(m/s)
 */
float CalculateSpeed(int16_t encoder_count)
{
	// 速度 = (脉冲数/编码器分辨率) / 采样时间 * 轮子周长
	// 计算轮子每秒转速，再换算成线速度
	return ((float)encoder_count / ENCODER_PPR) / SAMPLE_TIME * WHEEL_CIRCUMFERENCE;
}

// main
void process()
{
    processInit();
	processKeyJudgment(key);
}

void processInit()
{
		int Distance_mm = sonar_mm();		   // 获取距离测量结果，单位毫米（mm）
		int Distance_m = Distance_mm / 1000;   // 转换为米（m）为单位，将整数部分放入Distance_m
		int Distance_m_p = Distance_mm % 1000; // 转换为米（m）为单位，将小数部分放入Distance_m_p

		read_sensors(sensors1); // 读取传感器的值
}

// choose 为 0 时走直线
// choose 为 1 时左拐
void processTurn(int choose)
{
    switch (choose)
    {
    case 0:
        /* code */
        Motor_leftSetSpeed(LEFTWHEELPWM);
        Motor_rightSetSpeed(RIGHTWHEELPWM);
        break;
    case 1:
        /* code */
        Motor_leftSetSpeed(0);
        Motor_rightSetSpeed(RIGHTWHEELPWM);
        Delay_ms(50);
        turn = 0;
        break;
    default:
        break;
    }
}

void processKeyJudgment(int Key)
{
    if (Key == 0)
    {
        /* code */
        processJudgment(0, 1); 
    }
    if (Key == 1)
    {
        /* code */
        processJudgment(0, 2);
    }
    // if (Key == 2)
    // {
    //     /* code */
        
    // }
    // if (Key == 3)
    // {
    //     /* code */
        
    // }
}

// 功能：单一内外圈指定圈数运动
void processJudgment(int choose, int targetRound)
{
    int alreadyStarted = 0;
    int roundCounter = 0; 
    // 起点检测（TrackN == 全低） && 尚未起步
    if (Trackn == 0xFF && startFlag == 0)
    {
        startFlag = 1;  // 起点第一次检测
        alreadyStarted = 0;
    }

    // 如果已经从起点离开并绕了一圈又回来，再次检测 TrackN == 0xFF → 增加圈数
    if (Trackn == 0xFF && startFlag == 1 && alreadyStarted == 1)
    {
        roundCounter++;
        alreadyStarted = 0; // 防止重复加
    }

    // 小车离开起点区域（用于下次计圈）
    if (Trackn != 0xFF && startFlag == 1)
    {
        alreadyStarted = 1;
    }

    // 达到目标圈数，停车或触发事件
    if (roundCounter >= targetRound)
        return;

    // 弯道检测逻辑保留
    if (Trackn > 0x0F)
    {
        switch (choose)
        {
        case 0:
			processTurn(0);	
            break;
        case 1:
            turn = 1;
			processTurn(turn);
            break;
        default:
            break;
        }
    }
}

void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) // 判断是否是TIM4的更新事件触发的中断
	{
		update_redSensor(); // 更新传感器的值
		Key_Tick();			// 非阻塞式按键
		if (stop_flag < 10)
		{
			FollowAndTrack_Control();
		}
		else
		{
			Motor_leftSetSpeed(0);
			Motor_rightSetSpeed(0);
		}

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // 清除TIM4更新事件的中断标志位
	}
}