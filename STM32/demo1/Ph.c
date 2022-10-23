#include <stm32f4xx.h>
#include <stdio.h>

// 局部变量，用于保存转换计算后的电压值
float ADC_ConvertedValueLocal;
extern unsigned char Tx[20]; //无线发送缓存


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