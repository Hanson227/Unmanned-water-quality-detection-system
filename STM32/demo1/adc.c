#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>

static ADC_InitTypeDef          ADC_InitStructure;
static ADC_CommonInitTypeDef    ADC_CommonInitStructure;
static GPIO_InitTypeDef 		GPIO_InitStructure;

void ADC_init(void)
{
	//打开ADC硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//打开端口A硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	//配置引脚为模拟信号模式
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//配置引脚为模拟信号模式
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//配置为独立工作模式
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//配置ADC硬件时钟二分频=84MHz/2=42mHZ
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//配置为不需要映射数据
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//调节抽样点
    ADC_CommonInit(&ADC_CommonInitStructure);
 
	
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//调整数据精度为12位精度
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;//关闭多通道采样
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//持续进行转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐存储
    ADC_InitStructure.ADC_NbrOfConversion = 1;//单通道转换
    ADC_Init(ADC1, &ADC_InitStructure);
	
	//将ADC与转换器相关联
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);
	
	//使能ADC
	ADC_Cmd(ADC1, ENABLE);
	
	
}

int  get_adc() 
{
	ADC_SoftwareStartConv(ADC1); //使能指定的 ADC1 的软件转换启动功能
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束
	return ADC_GetConversionValue(ADC1); //返回最近一次 ADC1 规则组的转换结果
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

