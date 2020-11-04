/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*
*	�ļ����� : bsp_uart_fifo.h
*
*	��    �� : V1.0
*
*	˵    �� : ͷ�ļ�
*
*
*	�޸�ʱ�� ��2018.8.28	
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

/* RS485оƬ����ʹ��GPIO, PA1 */

#define RCC_RS485_TXEN 	 RCC_APB2Periph_GPIOB
#define PORT_RS485_TXEN  GPIOB
#define PIN_RS485_TXEN	 GPIO_Pin_8

#define RCC_RS485_TXEN2 	 RCC_APB2Periph_GPIOD
#define PORT_RS485_TXEN2  GPIOD
#define PIN_RS485_TXEN2	 GPIO_Pin_0

#define RS485_RX_EN()	 PORT_RS485_TXEN->BRR = PIN_RS485_TXEN	//�͵�ƽ
#define RS485_TX_EN()	 PORT_RS485_TXEN->BSRR = PIN_RS485_TXEN

/* RS485 �ӿڵ�Դ���ƣ�ʹ�ܽӿ�1��Դ*/

#define RCC_RS485_CTRL 	 RCC_APB2Periph_GPIOE
#define PORT_RS485_CTRL  GPIOE

#define RS485_1_CTRL	 GPIO_Pin_10
#define RS485_2_CTRL	 GPIO_Pin_11
#define RS485_3_CTRL	 GPIO_Pin_12
#define RS485_4_CTRL	 GPIO_Pin_13

#define RS485_1_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_1_CTRL	//�͵�ƽ
#define RS485_1_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_1_CTRL

#define RS485_2_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_2_CTRL	//�͵�ƽ
#define RS485_2_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_2_CTRL

#define RS485_3_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_3_CTRL	//�͵�ƽ
#define RS485_3_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_3_CTRL

#define RS485_4_CTRL_NA()	PORT_RS485_TXEN->BRR  = RS485_4_CTRL	//�͵�ƽ
#define RS485_4_CTRL_EN()	PORT_RS485_TXEN->BSRR = RS485_4_CTRL

/* �����ӿڿ����ߣ�����ģ��ʹ��*/

#define RCC_Bluetooth_CTRL 	 RCC_APB2Periph_GPIOF
#define PORT_Bluetooth_CTRL  GPIOF
#define PIN_Bluetooth_CTRL	 GPIO_Pin_12

#define Bluetooth_CTRL_EN()	PORT_Bluetooth_CTRL->BRR  = PORT_Bluetooth_CTRL	//�͵�ƽ
#define Bluetooth_CTRL_NA()	PORT_Bluetooth_CTRL->BSRR = PORT_Bluetooth_CTRL


/* ����˿ں� */
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

/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;			/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;			/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;			/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;			/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;			/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;			/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(void);	/* �����յ����ݵĻص�����ָ�� */
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
