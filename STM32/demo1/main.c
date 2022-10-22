#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "adc.h"
#include <stdio.h>

static GPIO_InitTypeDef GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static USART_InitTypeDef USART_InitStructure;
static GPIO_InitTypeDef GPIO_InitStructure;


// 局部变量，用于保存转换计算后的电压值
float ADC_ConvertedValueLocal;
unsigned char Tx[20]; //无线发送缓存

#pragma import(__use_no_semihosting_swi)
struct __FILE
{
	int handle; /* Add whatever you need here */
};
FILE __stdout;
FILE __stdin;
int fputc(int ch, FILE *f)
{

	USART_SendData(USART1, ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;
	USART_ClearFlag(USART1, USART_FLAG_TXE);

	return ch;
}

void usart1_init(uint32_t baud)
{

	//打开PA硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//打开串口1硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//配置PA9和PA10为复用功能模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //第9 10根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//多功能模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出，增加输出电流能力。
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//高速响应
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//没有使能上下拉电阻

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//将PA9和PA10引脚连接到串口1的硬件
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	//配置串口1相关参数：波特率、无校验位、8位数据位、1个停止位......
	USART_InitStructure.USART_BaudRate = baud;										//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 1个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;								//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//允许收发数据
	USART_Init(USART1, &USART_InitStructure);

	//配置串口1的中断触发方法：接收一个字节触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	//配置串口1的中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//使能串口1工作
	USART_Cmd(USART1, ENABLE);
}

void PH_Value_Conversion()
{
	float PH_Value = 0;
	ADC_ConvertedValueLocal = (float)Get_Adc_Average(20)*(3.3/4096); // 读取转换的AD值
	PH_Value = -5.7541f * ADC_ConvertedValueLocal + 15.654f;

	if (PH_Value <= 0.0f)
	{
		PH_Value = 0.0f;
	}
	if (PH_Value >= 14.0f)
	{
		PH_Value = 14.0f;
	}

	/*显示电压*/
	Tx[0] = (int)(PH_Value * 100) / 1000 + '0';
	Tx[1] = (int)(PH_Value * 100) % 1000 / 100 + '0';
	Tx[2] = '.';
	Tx[3] = (int)(PH_Value * 100) % 100 / 10 + '0';
	Tx[4] = (int)(PH_Value * 100) % 10 + '0';
}

int main(void)
{
	usart1_init(115200);
	ADC_init();
	printf("It is ADC test\n");
	while (1)
	{
		PH_Value_Conversion();
		printf("%s\n", Tx);
		delay_ms(1000);
	}
}

void USART1_IRQHandler(void)
{
	uint8_t d;

	//检测标志位
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		//接收数据
		d = USART_ReceiveData(USART1);

		//将接收到的数据，返发给PC
		USART_SendData(USART1, d);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			;

		//清空标志位
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
