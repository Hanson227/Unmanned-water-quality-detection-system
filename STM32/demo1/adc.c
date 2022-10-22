#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>

static ADC_InitTypeDef          ADC_InitStructure;
static ADC_CommonInitTypeDef    ADC_CommonInitStructure;
static GPIO_InitTypeDef 		GPIO_InitStructure;

void ADC_init(void)
{
	//��ADCӲ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//�򿪶˿�AӲ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//��������Ϊģ���ź�ģʽ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//��������Ϊģ���ź�ģʽ
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//��λ����
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����Ϊ��������ģʽ
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//����ADCӲ��ʱ�Ӷ���Ƶ=84MHz/2=42mHZ
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//����Ϊ����Ҫӳ������
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���ڳ�����
    ADC_CommonInit(&ADC_CommonInitStructure);
 
	
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//�������ݾ���Ϊ12λ����
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//�رն�ͨ������
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//��������ת��
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���洢
    ADC_InitStructure.ADC_NbrOfConversion = 1;//��ͨ��ת��
    ADC_Init(ADC1, &ADC_InitStructure);
	
	//��ADC��ת���������
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);
	
	//ʹ��ADC
	ADC_Cmd(ADC1, ENABLE);
	
	
}

int  get_adc() 
{
	ADC_SoftwareStartConv(ADC1); //ʹ��ָ���� ADC1 �����ת����������
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������
	return ADC_GetConversionValue(ADC1); //�������һ�� ADC1 �������ת�����
}

int Get_Adc_Average(u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=get_adc();
		delay_ms(5);
	}
	return temp_val/times;
} 

