/*
*********************************************************************************************************
*
*	模块名称 : 定时器模块
*
*	文件名称 : bsp_timer.c
*
*	版    本 : V1.3
*
*	说    明 : 
*
*	修改记录 :
*		
*
*********************************************************************************************************
*/

#include "bsp.h"


static void ConfigTimerNVIC(void)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);		
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	

//		NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;	
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);	
		NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);	
		
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
}

void bsp_InitHardTimer(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	TIM_ICInitTypeDef  TIM_ICInitStructure;  

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); 
	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	
    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;;				  	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	// TIM3 250ms,风速 4Hz
	
	TIM_TimeBaseStructure.TIM_Period =2499;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM3, TIM_FLAG_Update); 
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE); 

	TIM_Cmd(TIM3, ENABLE);
	
	// TIM2 1s
	TIM_TimeBaseStructure.TIM_Period =9999;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 
	TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE); 

	TIM_Cmd(TIM2, ENABLE);
	
	// TIM4 定时10ms
	TIM_TimeBaseStructure.TIM_Period = 19;        //记数值  
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;  //预分频，72000,000/(35999+1)     
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x00;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM4, ENABLE);

	TIM_Cmd(TIM4, ENABLE);	
	
	// TIM4 IT enable 
	TIM_ITConfig(TIM4, TIM_IT_Update,ENABLE);

	//TIM5 CH1 测频

	/* TIM2 configuration: PWM Input mode ------------------------
	The external signal is connected to TIM2 CH2 pin (PA.01), 
	The Rising edge is used as active edge,
	The TIM2 CCR2 is used to compute the frequency value 
	The TIM2 CCR1 is used to compute the duty cycle value
	------------------------------------------------------------ */

	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x00;

	TIM_PWMIConfig(TIM5, &TIM_ICInitStructure);

	/* Select the TIM2 Input Trigger: TI2FP2 */
	TIM_SelectInputTrigger(TIM5, TIM_TS_TI1FP1);

	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM5, TIM_SlaveMode_Reset);

	/* Enable the Master/Slave Mode */
	TIM_SelectMasterSlaveMode(TIM5, TIM_MasterSlaveMode_Enable);

	/* TIM enable counter */
	TIM_Cmd(TIM5, ENABLE);

	/* Enable the CC2 Interrupt Request */
	//TIM_ITConfig(TIM5, TIM_IT_CC1, ENABLE);

	TIM_ITConfig(TIM5, TIM_IT_CC1, DISABLE);
	
//	TIM_TimeBaseStructure.TIM_Period =9;							 //1ms
//	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
//	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
//	/* TIM IT enable */
//	TIM_ITConfig(TIM1,TIM_IT_Update, ENABLE);
//	/* TIM3 enable counter */
//	TIM_Cmd(TIM1, ENABLE);
		TIM_TimeBaseStructure.TIM_Period =9;							 //1ms
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	/* TIM IT enable */
	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);
	/* TIM3 enable counter */
	TIM_Cmd(TIM6, ENABLE);
	
	ConfigTimerNVIC();
}

/*************************************** END OF FILE *********************************/
