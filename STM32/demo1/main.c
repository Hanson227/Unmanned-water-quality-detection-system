#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "Ph.h"
#include "adc.h"
#include <stdio.h>

unsigned char Tx[20];
static GPIO_InitTypeDef GPIO_InitStructure;
static NVIC_InitTypeDef NVIC_InitStructure;
static USART_InitTypeDef USART_InitStructure;

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

	//��PAӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//�򿪴���1Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//����PA9��PA10Ϊ���ù���ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //��9 10������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//�๦��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//��������������������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//������Ӧ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		//û��ʹ������������

	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//��PA9��PA10�������ӵ�����1��Ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

	//���ô���1��ز����������ʡ���У��λ��8λ����λ��1��ֹͣλ......
	USART_InitStructure.USART_BaudRate = baud;										//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 8λ����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ��������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�����շ�����
	USART_Init(USART1, &USART_InitStructure);

	//���ô���1���жϴ�������������һ���ֽڴ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	//���ô���1���ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//ʹ�ܴ���1����
	USART_Cmd(USART1, ENABLE);
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

	//����־λ
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		//��������
		d = USART_ReceiveData(USART1);

		//�����յ������ݣ�������PC
		USART_SendData(USART1, d);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			;

		//��ձ�־λ
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}
