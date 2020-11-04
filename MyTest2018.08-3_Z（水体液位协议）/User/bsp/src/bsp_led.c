/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*
*	文件名称 : bsp_led.c
*
*	版    本 : V1.0
*
*	说    明 : 驱动LED指示灯
*
*	修改时间 ：2018.8.24
*	
*	修改记录 :
*
*********************************************************************************************************
*/

#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_InitLed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 打开GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_ALL_LED, ENABLE);

	//熄灯
	
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
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参: 无
*	返 回 值: 无
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
*	函 数 名: bsp_IsLedOn
*	功能说明: 判断LED指示灯是否已经点亮。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 1表示已经点亮，0表示未点亮
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
