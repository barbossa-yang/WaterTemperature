/*
*********************************************************************************************************
*
*	ģ������ : LEDָʾ������ģ��
*
*	�ļ����� : bsp_led.c
*
*	��    �� : V1.0
*
*	˵    �� : ����LEDָʾ��
*
*	�޸�ʱ�� ��2018.8.24
*	
*	�޸ļ�¼ :
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitLed
*	����˵��: ����LEDָʾ����ص�GPIO,  �ú����� bsp_Init() ���á�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_ALL_LED, ENABLE);

	//Ϩ��
	
	GPIO_PORT_LED1->BSRR = GPIO_PIN_LED1;
	GPIO_PORT_LED2->BSRR = GPIO_PIN_LED2;
	GPIO_PORT_LED3->BSRR = GPIO_PIN_LED3;
	GPIO_PORT_LED4->BSRR = GPIO_PIN_LED4;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1;
	GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2;
	GPIO_Init(GPIO_PORT_LED2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED3;
	GPIO_Init(GPIO_PORT_LED3, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED4;
	GPIO_Init(GPIO_PORT_LED4, &GPIO_InitStructure);
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedToggle
*	����˵��: ��תָ����LEDָʾ�ơ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Led1Toggle(void)
{
	GPIO_PORT_LED1->ODR ^= GPIO_PIN_LED1;
}

//void bsp_Led2Toggle(void)
//{
//	GPIO_PORT_LED2->ODR ^= GPIO_PIN_LED2;
//}

void bsp_Led3Toggle(void)
{
	GPIO_PORT_LED3->ODR ^= GPIO_PIN_LED3;
}

void bsp_Led4Toggle(void)
{
	GPIO_PORT_LED4->ODR ^= GPIO_PIN_LED4;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOn
*	����˵��: ����ָ����LEDָʾ�ơ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_Led1On(void)
{
	GPIO_PORT_LED1->BRR = GPIO_PIN_LED1;
}

//void bsp_Led2On(void)
//{
//	GPIO_PORT_LED2->BRR = GPIO_PIN_LED2;
//}

void bsp_Led3On(void)
{
	GPIO_PORT_LED3->BRR = GPIO_PIN_LED3;
}

void bsp_Led4On(void)
{
	GPIO_PORT_LED4->BRR = GPIO_PIN_LED4;
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_LedOff
*	����˵��: Ϩ��ָ����LEDָʾ�ơ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/

void bsp_Led1Off(void)
{
	GPIO_PORT_LED1->BSRR = GPIO_PIN_LED1;
}

//void bsp_Led2Off(void)
//{
//	GPIO_PORT_LED2->BSRR = GPIO_PIN_LED2;
//}

void bsp_Led3Off(void)
{
	GPIO_PORT_LED3->BSRR = GPIO_PIN_LED3;
}

void bsp_Led4Off(void)
{
	GPIO_PORT_LED4->BSRR = GPIO_PIN_LED4;
}

void bsp_LedOff(uint8_t _no)
{
	_no--;

	if (_no == 0)
	{
		GPIO_PORT_LED1->BSRR = GPIO_PIN_LED1;
	}
	else if (_no == 1)
	{
		GPIO_PORT_LED2->BSRR = GPIO_PIN_LED2;
	}
	else if (_no == 2)
	{
		GPIO_PORT_LED3->BSRR = GPIO_PIN_LED3;
	}
	else if (_no == 3)
	{
		GPIO_PORT_LED4->BSRR = GPIO_PIN_LED4;
	}
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_IsLedOn
*	����˵��: �ж�LEDָʾ���Ƿ��Ѿ�������
*	��    ��:  _no : ָʾ����ţ���Χ 1 - 4
*	�� �� ֵ: 1��ʾ�Ѿ�������0��ʾδ����
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
	if (_no == 1)
	{
		if ((GPIO_PORT_LED1->ODR & GPIO_PIN_LED1) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 2)
	{
		if ((GPIO_PORT_LED2->ODR & GPIO_PIN_LED2) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 3)
	{
		if ((GPIO_PORT_LED3->ODR & GPIO_PIN_LED3) == 0)
		{
			return 1;
		}
		return 0;
	}
	else if (_no == 4)
	{
		if ((GPIO_PORT_LED4->ODR & GPIO_PIN_LED4) == 0)
		{
			return 1;
		}
		return 0;
	}

	return 0;
}

/***************************** END OF FILE *********************************/
