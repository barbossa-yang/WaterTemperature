/*
*********************************************************************************************************
*
*	模块名称 : BSP模块
*
*	文件名称 : bsp.h
*
*	说    明 : 这是底层驱动模块所有的h文件的汇总文件。 
*			   应用程序只需 #include bsp.h 即可，
*			   不需要#include 每个模块的 h 文件
*
*	修改时间 ：2018.8.24
*
*********************************************************************************************************
*/
#pragma once
#ifndef _BSP_H_
#define _BSP_H

#define STM32_V4


/* 检查是否定义了开发板型号 */
#if !defined (STM32_V4) && !defined (STM32_X2)
	#error "Please define the board model : STM32_X2 or STM32_V4"
#endif

/* 定义 BSP 版本号 */
#define __STM32F1_BSP_VERSION		"1.1"

/* CPU空闲时执行的函数 */
//#define CPU_IDLE()		bsp_Idle()

#define SEA_TIDE  0


#define  USE_RTX    1

#define  USE_USER_DEFIN		1

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
#define BSP_Printf		printf
//#define BSP_Printf(...)

#include "stm32f10x.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ctype.h>
#include <time.h>
	
#define MaxTimes 	 16

#define Filter_Times 48

//typedef enum {FALSE = 0, TRUE = !FALSE} bool;

//#define true   		TRUE
//#define false  		FALSE
#define FALSE       0
#define TRUE        1
	
#define IWDG_BIT_0	 (1 << 0)
#define IWDG_BIT_1	 (1 << 1)
#define IWDG_BIT_2	 (1 << 2)
#define IWDG_BIT_3	 (1 << 3)

#define IWDG_BIT_4	 (1 << 4)
#define IWDG_BIT_5	 (1 << 5)
#define IWDG_BIT_6	 (1 << 6)
#define IWDG_BIT_7	 (1 << 7)

#define IWDG_BIT_8	 (1 << 8)
#define IWDG_BIT_9	 (1 << 9)

#if SEA_TIDE == 1
#define IWDG_BIT_ALL (IWDG_BIT_0 | IWDG_BIT_1 | IWDG_BIT_2 | IWDG_BIT_3 | IWDG_BIT_4 | IWDG_BIT_5 | IWDG_BIT_6 | IWDG_BIT_7 | IWDG_BIT_8 | IWDG_BIT_9)
#else
#define IWDG_BIT_ALL (IWDG_BIT_1 | IWDG_BIT_8 | IWDG_BIT_9)
#endif

#define UART_BIT_0	 (1 << 0)
#define UART_BIT_4	 (1 << 4)

#define UART_BIT_1	 (1 << 1)
#define UART_BIT_2	 (1 << 2)
#define UART_BIT_3	 (1 << 3)
#define UART_BIT_4	 (1 << 4)
#define UART_BIT_5	 (1 << 5)

#define UART_BIT_ALL (UART_BIT_1 | UART_BIT_3 )

#define STATE_BIT_0	 (1 << 0)

#define ATOS_BIT_0	 (1 << 0)

#define GAUG_BIT_0	 (1 << 0)
#define GAUG_BIT_1	 (1 << 1)
#define GAUG_BIT_2	 (1 << 2)

#define GAUG_BIT_ALL (GAUG_BIT_0 | GAUG_BIT_1| GAUG_BIT_2)

#define HUMI_BIT_0   (1 << 0)
#define HUMI_BIT_1   (1 << 1)
#define HUMI_BIT_ALL (HUMI_BIT_0 | HUMI_BIT_1)

#define EXTI9_5_ISR_MOVE_OUT

/* 打印调试数据到串口1 */

#define USE_DEBUG_OUTPUT	1 

#include "bsp_uart_fifo.h"

#include "bsp_led.h"

#include "bsp_timer.h"

#include "bsp_cpu_rtc.h"


#include "bsp_spi_flash.h"

#include "bsp_spi_bus.h"

#include "bsp_user_lib.h"

#include "bsp_tim_pwm.h"

#include "bsp_iwdg.h"

#define SETUP_HIGH  390

typedef __packed struct 
{ 	
		char Manufaxturer[80];		//="Henan Zhong Yuan Opto Electronics Measurement & Control Technology Co.,Ltd";
		char SerialNumber[16];		//="201401#0001"
		char Model_Ident[8]; 		//="ZYC500"
		char SoftwareVersion[8];	//="V01.00"
		char HardwareVersion[8];  	//="V01.00"
	
		char Station_ID[8];			//="A0003"
		char Station_DNO[8];		//="002"	
	
		char Station_LAT[16]; 		//="43.23.09"
		char Station_LONG[16];		//="112.45.87"
		char Station_ALT[8];  		//="8848.5"	
	
		u32  DTMD_Interval;			//1
		u32  DHTD_Interval;			//1
		u32  STATRD_Interval;		//60
		u32  SD_CF_SET;           	//1
		u32  GALE_Value;          	//17
		u32  TMAX_Value;          	//40
		u32  TMIN_Value;          	//0
		u32  RMAX_Value;          	//50
		u32  DTLT_Value;          	//35
		u32  DTLV_Value;          	//20
		
		u32  init_flag;           	//0xAA55;

}  CJQ_PZ_TYPE;

//CJQ_PZ_TYPE cjq_pz,*cjq_pz_ptr;

//extern CJQ_PZ_TYPE cjq_pz,*cjq_pz_ptr;

extern u8 stop_wdg_flag;

/* 提供给其他C文件调用的函数 */

void bsp_Init(void);
void bsp_Idle(void);
void BSP_Tick_Init (void);



#endif

/***************************** END OF FILE *********************************/
