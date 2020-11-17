/*
*********************************************************************************************************
*	                                  
*	模块名称 : 主程序模块。
*
*	文件名称 : main.c
*
*	版    本 : V1.0
*
*	说    明 : 
*
*	修改记录 : 版本号     日期          作者        说明
*
*				V1.0    2018-08-29   	nsjun
*    
*         
*********************************************************************************************************
*/

#include "includes.h"		
#include "bsp_sdi.h"

/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void AppTaskCreate (void);

__task void AppTaskStart(void);


/**************************************************************************************
//通信任务 2018.8.31 

//功能：处理与通信口有关的数据通信和数据采集

//通信任务函数、任务栈、任务句柄


**************************************************************************************/

__task void AppTaskComm(void);

static uint64_t AppTaskCommStk[512];   

OS_TID HandleTaskComm = NULL;

void TIM_CallBack1(void);
void TIM_CallBack2(void);
void TIM_CallBack3(void);


/*********************************************************************************/

__task void AppTaskWdg(void);

static uint64_t AppTaskWdgStk[32];   

OS_TID HandleTaskWdg = NULL;

/*************************************************************************************/

__task void AppTaskDAQ(void);

static uint64_t AppTaskDAQStk[64];   	

OS_TID HandleTaskDAQ = NULL;

/***********************************************************************************/
__task void AppTaskSTA(void);

static uint64_t AppTaskSTAStk[64];

OS_TID HandleTaskSTA    = NULL;

/***********************************************************************************/

__task void AppTaskAtmos(void);
static uint64_t AppTaskAtmosStk[256];      
OS_TID HandleTaskAtmos    = NULL;

/***********************************************************************************/

__task void AppTaskHumit(void);
static uint64_t AppTaskHumitStk[256];      
OS_TID HandleTaskHumit    = NULL;

/******************************************************************************/

__task void AppTaskCtl(void);

static uint64_t AppTaskCtlStk[64];

OS_TID HandleTaskCtl    = NULL;

/*******************************************************************************/

__task void AppTaskGauge(void);

static uint64_t AppTaskGaugeStk[256];

OS_TID HandleTaskGauge = NULL;

/*******************************************************************************/

__task void AppTaskWaterTemp(void);
static uint64_t AppTaskWaterTempStk[512];
OS_TID HandleTaskWaterTemp = NULL;


/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/

u32 ulTicks1,ulTicks2;

static uint64_t AppTaskStartStk[2048];   /* 任务栈 */


static uint64_t AppTaskMsgProStk[64];  

__task void AppTaskMsgPro(void);

OS_TID HandleTaskMsgPro = NULL;

extern u8 read_gauge_flag;

volatile u8 Cmd_Type = 0x00;

static u8 first_read_flag = 0x00;

/*
*********************************************************************************************************
*	函 数 名: main主函数
*
*	功能说明: 标准c程序入口。
*
*	形    参: 无
*
*	返 回 值: 无
*********************************************************************************************************
*/

int main (void) 
{	
	bsp_Init();
	//printf("初始化完成\r\n");
 	os_sys_init_user (AppTaskStart,              
	                  5,                         
	                  &AppTaskStartStk,         
	                  sizeof(AppTaskStartStk)); 
	while(1);
}



/*
*********************************************************************************************************
*	函 数 名: AppTaskMsgPro
*	功能说明: 消息处理，这里用作LED闪烁
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 3  


*********************************************************************************************************
*/

extern void Process_Host_Message(void) ;


__task void AppTaskMsgPro(void)
{
    while(1)
    {
			Process_Host_Message();		
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 5  
*********************************************************************************************************
*/
__task void AppTaskStart(void)
{

	AppTaskCreate();
	
    while(1)
    {
		os_dly_wait(10);
		
		os_evt_set(IWDG_BIT_5, HandleTaskWdg);
    }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskComm
*
*	功能说明: 通信任务
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 6  
*********************************************************************************************************
*/

// 2018.8.31 

extern u32 timer1_1s;

extern void SendTcp(u8 cmd);

__task void AppTaskComm(void)
{
    while(1)
    {
		os_dly_wait(500);
		
		/* 发送事件标志，表示任务正常运行 */
		os_evt_set(IWDG_BIT_6, HandleTaskWdg);
    }	
}


/*
*********************************************************************************************************
*	函 数 名: TIM_CallBack1
*	功能说明: 定时器中断的回调函数。		  			  
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM_CallBack1(void)
{
	
	if(first_read_flag)
		Cmd_Type = 0x06;
	else
	{
		Cmd_Type = 0x02;
		first_read_flag = 0x0ff;
	}		
	
	isr_evt_set (GAUG_BIT_0, HandleTaskGauge);	
}

void TIM_CallBack2(void)
{
	isr_evt_set (GAUG_BIT_1, HandleTaskGauge);
}

void Fun_CallBack1(void)
{
	os_evt_set (GAUG_BIT_0, HandleTaskGauge);
}

void TIM_CallBack3(void)
{
	isr_evt_set (ATOS_BIT_0, HandleTaskAtmos);
}

void TIM_CallBack4(void)
{
	isr_evt_set (GAUG_BIT_2, HandleTaskGauge);
}


void USART_CallBack2(void)
{
	isr_evt_set (HUMI_BIT_1, HandleTaskHumit);
}

void USART_CallBack21(void)
{
	isr_evt_set (GAUG_BIT_1, HandleTaskGauge);
}

void USART_CallBack5(void)
{
	isr_evt_set (STATE_BIT_0, HandleTaskSTA);
}

void TASK1_CallBack1(u8 delay)
{
	os_dly_wait(delay);
}


void TASK1_CallBack2(void)
{
//	bsp_Led1Toggle();
//	bsp_Led4Toggle();
	
	os_evt_set(IWDG_BIT_1, HandleTaskWdg);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskSTA
*
*	功能说明: 采集电源控制器状态。
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 2  
*********************************************************************************************************
*/

__task void AppTaskSTA(void)
{
	const uint16_t usMaxBlockTime = 500; 	 
	
	OS_RESULT xResult;	
	
    while(1)
    {
		xResult = os_evt_wait_or (STATE_BIT_0, usMaxBlockTime);
		
		switch (xResult)
		{
			case OS_R_EVT:
				
				break;	

			case OS_R_TMO:
				break;
			default:                     
				break;
		}	
        os_dly_wait(500);
		bsp_Led3Toggle();
		
		os_evt_set(IWDG_BIT_0, HandleTaskWdg);
		
    }	
}



/*
*********************************************************************************************************
*	函 数 名: AppTaskAtmos
*
*	功能说明: 模拟电压采集及计算任务。
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 8  
*********************************************************************************************************
*/

__task void AppTaskAtmos(void)
{
	const uint16_t usMaxBlockTime = 400; 	 
	
	OS_RESULT xResult;	
	
    while(1)
    {
		xResult = os_evt_wait_or (ATOS_BIT_0, usMaxBlockTime);
		
		switch (xResult)
		{
			case OS_R_EVT:
				
			    #if DUSE_DEBUG_OUTPUT == 1
					printf("接收到采集气压事件\r\n");
				#endif
				break;	

			case OS_R_TMO:
				
			    #if DUSE_DEBUG_OUTPUT == 1
					printf("采集气压事件超时\r\n");
				#endif	
				
				break;
			
			default:                     
				break;
		}			
		

		//计算1分钟均值、海平面气压

		os_evt_set(IWDG_BIT_4, HandleTaskWdg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskWaterTemp
*
*	功能说明: 采集温湿度任务。
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 8  
*********************************************************************************************************
*/
extern u8 response_data[RECEIVE_DATA_LEN];
__task void AppTaskWaterTemp(void)
{
	while(1)
	{
		RTC_ReadClock();
		
		if((g_tRTC.Sec == 0)&&(g_tRTC.Min%10 == 0))
		{
			Sdi_12_Transmission("1R0!",1, 1);
			printf("%s", response_data);
			memset(response_data, 0, RECEIVE_DATA_LEN);

			Sdi_12_Transmission("2R0!",1, 1);
			printf("%s", response_data);
			memset(response_data, 0, RECEIVE_DATA_LEN);

			Sdi_12_Transmission("3R0!",1, 1);
			printf("%s", response_data);
			memset(response_data, 0, RECEIVE_DATA_LEN);

			Sdi_12_Transmission("4R0!",1, 1);
			printf("%s", response_data);
			memset(response_data, 0, RECEIVE_DATA_LEN);

			Sdi_12_Transmission("5R0!",1, 1);
			printf("%s", response_data);
			memset(response_data, 0, RECEIVE_DATA_LEN);

			Sdi_12_Transmission("6R0!",1, 1);
			printf("%s", response_data);
			memset(response_data, 0, RECEIVE_DATA_LEN);
		}
		
		os_evt_set(IWDG_BIT_9, HandleTaskWdg);
		os_dly_wait(100);
	}
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskHumit
*
*	功能说明: 采集温湿度任务。
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 8  
*********************************************************************************************************
*/

extern void Analyze_Humit(void);

u8 r_485_port = 0x00;



u8 R_485_Ctrl(u8 tt)
{
	u8 ch = tt;
	
//	GPIO_SetBits(PORT_RS485_CTRL,RS485_1_CTRL);
//	GPIO_SetBits(PORT_RS485_CTRL,RS485_2_CTRL);
	
	return ch;	
}
	
__task void AppTaskHumit(void)
{
	const uint16_t usMaxBlockTime = 400; 	 
	
	OS_RESULT xResult;	
	
	u16 ret_flags = 0x0000;
	
    while(1)
    {
		xResult = os_evt_wait_or (HUMI_BIT_ALL, usMaxBlockTime);
		
		switch (xResult)
		{
			case OS_R_EVT:
				
			    #if DUSE_DEBUG_OUTPUT == 1
					printf("接收到温湿度事件\r\n");
				#endif
			
				ret_flags = os_evt_get ();
			
				if(ret_flags == HUMI_BIT_0)
				{
					if(Cmd_Type == 0xC1)
					{
						r_485_port = R_485_Ctrl(0x02);
						Read_From_WSD();
					}
				}
				
				if(ret_flags == HUMI_BIT_1)
				{
					if(Cmd_Type == 0x41)
					{
						Analyze_Humit();
					}
					
				}
			
				//IWDG_ReloadCounter();
				break;	

			case OS_R_TMO:
				
			    r_485_port = R_485_Ctrl(0x00);

			
				#if DUSE_DEBUG_OUTPUT == 1
					printf("采集温湿度事件超时\r\n");
				#endif	
				
				break;
			
			default:                     
				break;
		}			
		

		//计算1分钟均值

		os_evt_set(IWDG_BIT_3, HandleTaskWdg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGauge
*	功能说明: 信号处理		
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 1  
*********************************************************************************************************
*/

extern vu32 freq[Filter_Times];

extern vu32 Timer4_Counter;

extern bool  measure_success_flag,gauge_timeout_flag,freq_en;

u32 wait_time_gauge = 0;

__task void AppTaskGauge(void)
{
	OS_RESULT xResult;	
	
	vu8  i,ret_flags = 0,ret;
	
	const uint16_t usMaxBlockTime = 400; 
	
    while(1)
    {
		xResult = os_evt_wait_or (GAUG_BIT_ALL, usMaxBlockTime);

		//上电后第一次发送读取液位传感器序列号的命令，收到06语句后解析。TIM2每3秒进行一次任务回调，发送一次读取数据命令，然后收到86命令，然后解析出长度。
		switch (xResult)
		{
			case OS_R_EVT:
				
				ret_flags = os_evt_get();
				bsp_Led1Toggle();
				if(ret_flags == GAUG_BIT_0)
				{
					if(Cmd_Type == 0x02)	//Cmd_Type == 0x02时是第一次读数，当第一次会先读传感器的序列号
					{
						printf("1\r\n");
						for(i=0;i<32;i++)
						{
							os_evt_set(IWDG_BIT_8, HandleTaskWdg);
							os_dly_wait(500);  // 雷达液位需要初始化，期间不能发命令读数，否则会导致雷达液位出BUG。所以第一次读数延迟10秒
						}
						i=0;		
						printf("2\r\n");						
						wait_time_gauge =0;													
						r_485_port = R_485_Ctrl(0x02);						
						Read_From_Gauge(0x01);						
					}
					else
					if(Cmd_Type == 0x06)
					{
						Analyze_Gauge(0x06);    
						wait_time_gauge =0;
						r_485_port = R_485_Ctrl(0x02);
						Read_From_Gauge(0x02);						
					}					
					
				}
				
				if(ret_flags == GAUG_BIT_1)
				{

					if(Cmd_Type == 0x06)
					{
					
						Analyze_Gauge(0x06);
						printf("4\r\n");
						r_485_port = R_485_Ctrl(0x02);
						Read_From_Gauge(0x02);						
					}
					
					if(Cmd_Type == 0x86)
					{

						Analyze_Gauge(0x86);
						printf("5\r\n");
						r_485_port = R_485_Ctrl(0x00);
						
						Cmd_Type = 0xC1;
						os_evt_set (HUMI_BIT_0, HandleTaskHumit);
					}					
					
				}
			
//				if(stop_wdg_flag == 0)
//					IWDG_ReloadCounter();
				
				break;	

			case OS_R_TMO:
				
			    r_485_port = R_485_Ctrl(0x00);
				Cmd_Type = 0xC1;
				os_evt_set (HUMI_BIT_0, HandleTaskHumit);
		
				break;
			
			default:                     
				break;
		}
		

		//计算1分钟均值
		
		os_evt_set(IWDG_BIT_8, HandleTaskWdg);
		
	
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskScan
*	功能说明: 数据采集。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/

__task void AppTaskDAQ(void)
{
    while(1)
    {
        os_dly_wait(10);
		
		os_evt_set(IWDG_BIT_2, HandleTaskWdg);
    }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskCtl
*
*	功能说明: 控制LED闪烁。
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 2  
*********************************************************************************************************
*/

extern u8 sensor_state[12];

extern float gauge_temp[];


extern float gauge_high;

extern u8 gauge_read_sucess;

__task void AppTaskCtl(void)
{
    while(1)
    {
		os_dly_wait(91);
    
		os_evt_set(IWDG_BIT_7, HandleTaskWdg);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	

	//采集电源状态任务
	HandleTaskSTA    = os_tsk_create_user(	AppTaskSTA,              	
											1,                       	
											&AppTaskSTAStk,          	
											sizeof(AppTaskSTAStk));  	
	
	HandleTaskMsgPro = os_tsk_create_user(	AppTaskMsgPro,            	
											2,                      	
											&AppTaskMsgProStk,       	
											sizeof(AppTaskMsgProStk)); 	
	//数据采集任务：风速、风向、降雨量
	HandleTaskDAQ    = os_tsk_create_user(	AppTaskDAQ,             	
											3,                         
											&AppTaskDAQStk,         	
											sizeof(AppTaskDAQStk)); 	
	
	//温湿度采集任务
	HandleTaskHumit  = os_tsk_create_user(	AppTaskHumit,                
											4,                         
											&AppTaskHumitStk,            
											sizeof(AppTaskHumitStk));  
	//气压采集任务										
	HandleTaskAtmos  = os_tsk_create_user(	AppTaskAtmos,                
											5,                         
											&AppTaskAtmosStk,            
											sizeof(AppTaskAtmosStk));  
		//温度链采集任务									
	HandleTaskWaterTemp  = os_tsk_create_user(	AppTaskWaterTemp,                
											6,                         
											&AppTaskWaterTempStk,            
											sizeof(AppTaskWaterTempStk));
	
	//外部通信任务
	HandleTaskComm   = os_tsk_create_user(	AppTaskComm,               
											7,                         
											&AppTaskCommStk,           
											sizeof(AppTaskCommStk));   
	
	//外部控制任务										
	HandleTaskCtl    = os_tsk_create_user(	AppTaskCtl,                
											8,                         
											&AppTaskCtlStk,            
											sizeof(AppTaskCtlStk));
	//潮高采集任务
	HandleTaskGauge  = os_tsk_create_user(	AppTaskGauge,             	
											9,                         
											&AppTaskGaugeStk,         	
											sizeof(AppTaskGaugeStk)); 
	//看门狗任务
	HandleTaskWdg    = os_tsk_create_user(	AppTaskWdg,                
											10,                         
											&AppTaskWdgStk,            
											sizeof(AppTaskWdgStk));
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskWdg
*
*	功能说明: 看门狗任务。
*
*	形    参: 无
*
*	返 回 值: 无
*
*   优 先 级: 7  
*********************************************************************************************************
*/

__task void AppTaskWdg(void)
{

	OS_RESULT xResult;	
	
	const uint16_t usMaxBlockTime = 1000; 
	
    while(1)
    {
		xResult = os_evt_wait_and (IWDG_BIT_ALL, usMaxBlockTime);
		
		switch (xResult)
		{
			case OS_R_EVT:
				
				if(stop_wdg_flag == 0)
					IWDG_ReloadCounter();
			
				break;	
			
			default:                     
				break;
		}	
    }	
}

/*******************************************************************************
* Function Name  : RTC_IRQHandler
* Description    : This function handles RTC global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

extern bool alarm_lamp_state;
extern int flash_call_led;

void RTC_IRQHandler(void)
{
	vu8 i;
	
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);
		
		
		timer0++;
		
		timer1++;
		
		timer2++;
		

		if(ZYGD_Timer++ > 9)
			ZYGD_Timer %= 10;
		
	}
}

/*********************************** END OF FILE *********************************/

