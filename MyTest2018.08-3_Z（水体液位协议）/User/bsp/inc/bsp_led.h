/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	
*********************************************************************************************************
*/

#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "stm32f10x.h"

/* 管脚对应的RCC时钟 */

#define RCC_ALL_LED 	(RCC_APB2Periph_GPIOC)

#define GPIO_PORT_LED1  GPIOC		
#define GPIO_PIN_LED1	GPIO_Pin_2

#define GPIO_PORT_LED2  GPIOC		
#define GPIO_PIN_LED2	GPIO_Pin_1

#define GPIO_PORT_LED3  GPIOC		
#define GPIO_PIN_LED3	GPIO_Pin_1

#define GPIO_PORT_LED4  GPIOC
#define GPIO_PIN_LED4	GPIO_Pin_0

/* 供外部调用的函数声明 */

void bsp_InitLed(void);

void bsp_Led1Toggle(void);

void bsp_Led3Toggle(void);
void bsp_Led4Toggle(void);

void bsp_Led1On(void);

void bsp_Led3On(void);
void bsp_Led4On(void);	

void bsp_Led1Off(void);

void bsp_Led3Off(void);
void bsp_Led4Off(void);

uint8_t bsp_IsLedOn(uint8_t _no);


#endif

/***************************** END OF FILE *********************************/
