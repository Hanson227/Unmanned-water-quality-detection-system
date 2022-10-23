#include <stm32f4xx.h>
#include <stdio.h>

// �ֲ����������ڱ���ת�������ĵ�ѹֵ
float ADC_ConvertedValueLocal;
extern unsigned char Tx[20]; //���߷��ͻ���


void PH_Value_Conversion()
{
	float PH_Value = 0;
	ADC_ConvertedValueLocal = (float)Get_Adc_Average(20)*(3.3/4096); // ��ȡת����ADֵ
	PH_Value = -5.7541f * ADC_ConvertedValueLocal + 15.654f;

	if (PH_Value <= 0.0f)
	{
		PH_Value = 0.0f;
	}
	if (PH_Value >= 14.0f)
	{
		PH_Value = 14.0f;
	}

	/*��ʾ��ѹ*/
	Tx[0] = (int)(PH_Value * 100) / 1000 + '0';
	Tx[1] = (int)(PH_Value * 100) % 1000 / 100 + '0';
	Tx[2] = '.';
	Tx[3] = (int)(PH_Value * 100) % 100 / 10 + '0';
	Tx[4] = (int)(PH_Value * 100) % 10 + '0';
}