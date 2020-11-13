/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*
*	文件名称 : bsp_user_lib.h
*
*	版    本 : V1.2
*
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_USER_LIB_H
#define __BSP_USER_LIB_H

#include "stm32f10x.h"


#define AnsiString uint8_t

#define Max_Data_Length 	1280

int  str_len(char *_str);
void str_cpy(char *_tar, char *_src);
int  str_cmp(char * s1, char * s2);
void mem_set(char *_tar, char _data, int _len);

void int_to_str(int _iNumber, char *_pBuf, unsigned char _len);
int  str_to_int(char *_pStr);

uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);

uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen) ;
int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

char  BcdToChar(uint8_t _bcd);
void  HexToAscll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes);
uint32_t AsciiToUint32(char *pAscii);

#define RCC_GRC_DATA 	 RCC_APB2Periph_GPIOD
#define PORT_GRC_DATA    GPIOD

#define GPIO_PIN_GRC1	 GPIO_Pin_8
#define GPIO_PIN_GRC2	 GPIO_Pin_9
#define GPIO_PIN_GRC3	 GPIO_Pin_10
#define GPIO_PIN_GRC4	 GPIO_Pin_11
#define GPIO_PIN_GRC5	 GPIO_Pin_12
#define GPIO_PIN_GRC6	 GPIO_Pin_13
#define GPIO_PIN_GRC7	 GPIO_Pin_14
#define GPIO_PIN_GRC8	 GPIO_Pin_15


#define GPIO_PIN_AD_V    GPIO_Pin_5
#define GPIO_PIN_RAIN    GPIO_Pin_6
#define GPIO_PIN_WS      GPIO_Pin_7

//控制

#define RCC_Led_Ctrl 	 RCC_APB2Periph_GPIOG
#define PORT_Led_Ctrl	 GPIOG

#define PIN_Blue_Led	 GPIO_Pin_0

#define PIN_Yellow_Led	 GPIO_Pin_1

#define PIN_Orange_Led	 GPIO_Pin_2

#define PIN_Red_Led		 GPIO_Pin_3

#define PIN_Call_Led	 GPIO_Pin_4

#define PIN_Call_Alert	 GPIO_Pin_5

void bsp_InitOthers(void);

void Read_From_WSD(void);
void Analyze_Humit(void);

void Read_From_Gauge(u8 tt);
void Analyze_Gauge(u8 tt);

u8 Read_Gauge(u8 timeout);


extern volatile int timer0,timer1,timer2,ZYGD_Timer;



#endif

/***************************** END OF FILE *********************************/
