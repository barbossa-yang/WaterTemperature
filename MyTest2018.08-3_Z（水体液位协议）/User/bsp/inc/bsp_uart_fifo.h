/*
*********************************************************************************************************
*
*	模块名称 : 串口中断+FIFO驱动模块
*
*	文件名称 : bsp_uart_fifo.h
*
*	版    本 : V1.0
*
*	说    明 : 头文件
*
*
*	修改时间 ：2018.8.28	
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include "stm32f10x.h"

#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	1
#define	UART3_FIFO_EN	1
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	1

/* RS485芯片发送使能GPIO, PA1 */

#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOB
#define PORT_RS485_TXEN  GPIOB
#define PIN_RS485_TXEN	 GPIO_Pin_8

#define RCC_RS485_TXEN2 	 RCC_APB2Periph_GPIOD
#define PORT_RS485_TXEN2  GPIOD
#define PIN_RS485_TXEN2	 GPIO_Pin_0

#define RS485_RX_EN()	 PORT_RS485_TXEN->BRR = PIN_RS485_TXEN	//低电平
#define RS485_TX_EN()	 PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN

/* RS485 接口电源控制，使能接口1电源*/

#define RCC_RS485_CTRL 	 RCC_APB2Periph_GPIOE
#define PORT_RS485_CTRL  GPIOE

#define RS485_1_CTRL	 GPIO_Pin_10
#define RS485_2_CTRL	 GPIO_Pin_11
#define RS485_3_CTRL	 GPIO_Pin_12
#define RS485_4_CTRL	 GPIO_Pin_13

#define RS485_1_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_1_CTRL	//低电平
#define RS485_1_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_1_CTRL

#define RS485_2_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_2_CTRL	//低电平
#define RS485_2_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_2_CTRL

#define RS485_3_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_3_CTRL	//低电平
#define RS485_3_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_3_CTRL

#define RS485_4_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_4_CTRL	//低电平
#define RS485_4_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_4_CTRL

/* 蓝牙接口控制线，蓝牙模块使用*/

#define RCC_Bluetooth_CTRL 	 RCC_APB2Periph_GPIOF
#define PORT_Bluetooth_CTRL  GPIOF
#define PIN_Bluetooth_CTRL	 GPIO_Pin_12

#define Bluetooth_CTRL_EN()	PORT_Bluetooth_CTRL->BRR  = PORT_Bluetooth_CTRL	//低电平
#define Bluetooth_CTRL_NA()	PORT_Bluetooth_CTRL->BSRR = PORT_Bluetooth_CTRL


/* 定义端口号 */
typedef enum
{
	COM1 = 0,	/* USART1  PA9, PA10 */
	COM2 = 1,	/* USART2, PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
}COM_PORT_E;

#if UART1_FIFO_EN == 1
	#define UART1_BAUD			115200   /*19200*/
	#define UART1_TX_BUF_SIZE	1*512
	#define UART1_RX_BUF_SIZE	1*128
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			9600
	#define UART2_TX_BUF_SIZE	1*32
	#define UART2_RX_BUF_SIZE	1*64
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			115200   //9600
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*128
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			115200
	#define UART4_TX_BUF_SIZE	1*64
	#define UART4_RX_BUF_SIZE	1*64
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			9600
	#define UART5_TX_BUF_SIZE	1*32
	#define UART5_RX_BUF_SIZE	1*8
#endif

/* 串口设备结构体 */
typedef struct
{
	USART_TypeDef *uart;		/* STM32内部串口设备指针 */
	uint8_t *pTxBuf;			/* 发送缓冲区 */
	uint8_t *pRxBuf;			/* 接收缓冲区 */
	uint16_t usTxBufSize;		/* 发送缓冲区大小 */
	uint16_t usRxBufSize;		/* 接收缓冲区大小 */
	__IO uint16_t usTxWrite;			/* 发送缓冲区写指针 */
	__IO uint16_t usTxRead;			/* 发送缓冲区读指针 */
	__IO uint16_t usTxCount;			/* 等待发送的数据个数 */

	__IO uint16_t usRxWrite;			/* 接收缓冲区写指针 */
	__IO uint16_t usRxRead;			/* 接收缓冲区读指针 */
	__IO uint16_t usRxCount;			/* 还未读取的新数据个数 */

	void (*SendBefor)(void); 	/* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
	void (*SendOver)(void); 	/* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
	void (*ReciveNew)(void);	/* 串口收到数据的回调函数指针 */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);

void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(char *_pBuf);

void bsp_Set485Baud(uint32_t _baud);

void bsp_SetUart1Baud(uint32_t _baud);
void bsp_SetUart2Baud(uint32_t _baud);

#endif

/***************************** END OF FILE *********************************/
