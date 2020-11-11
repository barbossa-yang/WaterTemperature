/******************** Copyright (C) 2019-now Henan zhongyuan optical-electric measurement and control technology co.LTD All Rights Reserved*********************
* 文件名    ：sdi.h
* 描述      ：sdi-12接口初始化
* 平台      : STM32F103VE +
* 硬件配置：------------------------
*          |   PA5  -  sdi-12接口	|
*          |   TIMER7				|
*           ------------------------
* 修改记录:
* 日期         	 作者        记录
* 2019-07-16  	 杨闯      版本V1.0

**************************************************** (C) 2019-现在 河南中原光电测控技术有限公司 版权所有 ****************************************************/

#ifndef __BSP_SDI_H__
#define __BSP_SDI_H__

#define RECEIVE_DATA_LEN 30
#define IT_COUNTER       300

#define ResetDQ() GPIO_ResetBits(GPIOE,GPIO_Pin_5)  //输出低电平
#define SetDQ()   GPIO_SetBits(GPIOE,GPIO_Pin_5)   //输出高电平
#define GetDQ()   GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) //读取DQ的电平
#define ResetChannelOne()  GPIO_ResetBits(GPIOC,GPIO_Pin_10)
#define SetChannelOne()  GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define ResetChannelTwo()  GPIO_ResetBits(GPIOC,GPIO_Pin_11)
#define SetChannelTwo()  GPIO_SetBits(GPIOC,GPIO_Pin_11)
#define ResetChannelThree()  GPIO_ResetBits(GPIOC,GPIO_Pin_12)
#define SetChannelThree()  GPIO_SetBits(GPIOC,GPIO_Pin_12)
#define ResetChannelFour()  GPIO_ResetBits(GPIOD,GPIO_Pin_2)
#define SetChannelFour()  GPIO_SetBits(GPIOD,GPIO_Pin_2)
#define ResetChannelFive()  GPIO_ResetBits(GPIOB,GPIO_Pin_3)
#define SetChannelFive()  GPIO_SetBits(GPIOB,GPIO_Pin_3)

//#include "main.h"
#include "stm32f10x.h"
#include "bsp.h"

void SDI_PP_Configuration(void);
void Enable_SdiLine_Int(void);
void Disable_SdiLine_Int(void);
void Timeout_Timer_Config(u16 p);
uint8_t insert_parity(uint8_t ch);
uint8_t check_parity(uint8_t ch);
void sdi_line_int(u8 en);
void Set_DQIO_In(void);
void delay_1us(u32 value);
void delay_ms(int cnt);
void Set_DQIO_Out(void);
void ow_writebyte1(u8 tt,u8 chanel);
void Send_Command(const char *p,u8 channel);
void ow_writebit1(u8 state,u8 channel);
u8  ow_readbyte1(void);
bool Sdi_12_Transmission(const char *cmd,u8 tt,u8 channel);
void TIM5_IRQHandler(void);
void InquireDeviceInfo(u8 channel);

#endif

