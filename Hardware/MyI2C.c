#include "stm32f10x.h"                  // Device header
#include "Delay.h"   

//函数名称,端口号的替换
void MyI2C_W_SCL(uint8_t BitValue)   //W代表写
{	
	GPIO_WriteBit (GPIOB,GPIO_Pin_10,(BitAction )BitValue );//BitAction是枚举类型,表示这个位是高电平还是低电平
	Delay_us(10); 		//时序的延迟
}

void MyI2C_W_SDA(uint8_t BitValue)
{	
	GPIO_WriteBit (GPIOB,GPIO_Pin_11,(BitAction )BitValue );//BitAction是枚举类型,表示这个位是高电平还是低电平
	Delay_us(10); 
}

uint8_t MyI2C_R_SDA(void)//R代表读
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
	Delay_us(10); 
	return BitValue ;		//返回读到SDA线的电平
	
}

void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  //开漏输出 
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits (GPIOB, GPIO_Pin_10 | GPIO_Pin_11);

}

//起始条件
void MyI2C_Start(void)
{
	MyI2C_W_SDA (1);	//SDA释放
	MyI2C_W_SCL (1);	//SCL释放
	MyI2C_W_SDA (0);	//先拉低SDA 
	MyI2C_W_SCL (0);	//在拉低SCL
}

//终止条件
void MyI2C_Stop(void)
{
	MyI2C_W_SDA (0);	//先拉低SDA
	MyI2C_W_SCL (1);	//再释放SCL
	MyI2C_W_SDA (1);	//再释放SDA
}

//发送一个字节
void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;		//定义一个迭代变量
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));		//用按位与的方式,取出数据的某一位或某几位,>>表示右移,括号确保优先级
		MyI2C_W_SCL(1);//释放SCL
		MyI2C_W_SCL(0);//拉低SCL 
	}
}

//接受一个字节
uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);		//主机开始读取数据
		if (MyI2C_R_SDA() == 1){Byte |= (0x80 >> i);}		
		MyI2C_W_SCL(0);		//从机放入下一个数据
	}
	return Byte;
}

//发送应答
void MyI2C_SendAck(uint8_t AckBit)
{
	MyI2C_W_SDA(AckBit);		//主机把AckBit放到SDA上
	MyI2C_W_SCL(1);			//从机读取应答
	MyI2C_W_SCL(0);			//进入下一个时序单元
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}








