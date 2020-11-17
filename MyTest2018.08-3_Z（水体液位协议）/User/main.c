/*
*********************************************************************************************************
*	                                  
*	ģ������ : ������ģ�顣
*
*	�ļ����� : main.c
*
*	��    �� : V1.0
*
*	˵    �� : 
*
*	�޸ļ�¼ : �汾��     ����          ����        ˵��
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
											��������
**********************************************************************************************************
*/
static void AppTaskCreate (void);

__task void AppTaskStart(void);


/**************************************************************************************
//ͨ������ 2018.8.31 

//���ܣ�������ͨ�ſ��йص�����ͨ�ź����ݲɼ�

//ͨ��������������ջ��������


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
											 ����
**********************************************************************************************************
*/

u32 ulTicks1,ulTicks2;

static uint64_t AppTaskStartStk[2048];   /* ����ջ */


static uint64_t AppTaskMsgProStk[64];  

__task void AppTaskMsgPro(void);

OS_TID HandleTaskMsgPro = NULL;

extern u8 read_gauge_flag;

volatile u8 Cmd_Type = 0x00;

static u8 first_read_flag = 0x00;

/*
*********************************************************************************************************
*	�� �� ��: main������
*
*	����˵��: ��׼c������ڡ�
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*********************************************************************************************************
*/

int main (void) 
{	
	bsp_Init();
	//printf("��ʼ�����\r\n");
 	os_sys_init_user (AppTaskStart,              
	                  5,                         
	                  &AppTaskStartStk,         
	                  sizeof(AppTaskStartStk)); 
	while(1);
}



/*
*********************************************************************************************************
*	�� �� ��: AppTaskMsgPro
*	����˵��: ��Ϣ������������LED��˸
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 3  


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
*	�� �� ��: AppTaskStart
*	����˵��: ��������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 5  
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
*	�� �� ��: AppTaskComm
*
*	����˵��: ͨ������
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 6  
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
		
		/* �����¼���־����ʾ������������ */
		os_evt_set(IWDG_BIT_6, HandleTaskWdg);
    }	
}


/*
*********************************************************************************************************
*	�� �� ��: TIM_CallBack1
*	����˵��: ��ʱ���жϵĻص�������		  			  
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: AppTaskSTA
*
*	����˵��: �ɼ���Դ������״̬��
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 2  
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
*	�� �� ��: AppTaskAtmos
*
*	����˵��: ģ���ѹ�ɼ�����������
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 8  
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
					printf("���յ��ɼ���ѹ�¼�\r\n");
				#endif
				break;	

			case OS_R_TMO:
				
			    #if DUSE_DEBUG_OUTPUT == 1
					printf("�ɼ���ѹ�¼���ʱ\r\n");
				#endif	
				
				break;
			
			default:                     
				break;
		}			
		

		//����1���Ӿ�ֵ����ƽ����ѹ

		os_evt_set(IWDG_BIT_4, HandleTaskWdg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskWaterTemp
*
*	����˵��: �ɼ���ʪ������
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 8  
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
*	�� �� ��: AppTaskHumit
*
*	����˵��: �ɼ���ʪ������
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 8  
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
					printf("���յ���ʪ���¼�\r\n");
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
					printf("�ɼ���ʪ���¼���ʱ\r\n");
				#endif	
				
				break;
			
			default:                     
				break;
		}			
		

		//����1���Ӿ�ֵ

		os_evt_set(IWDG_BIT_3, HandleTaskWdg);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskGauge
*	����˵��: �źŴ���		
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 1  
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

		//�ϵ���һ�η��Ͷ�ȡҺλ���������кŵ�����յ�06���������TIM2ÿ3�����һ������ص�������һ�ζ�ȡ�������Ȼ���յ�86���Ȼ����������ȡ�
		switch (xResult)
		{
			case OS_R_EVT:
				
				ret_flags = os_evt_get();
				bsp_Led1Toggle();
				if(ret_flags == GAUG_BIT_0)
				{
					if(Cmd_Type == 0x02)	//Cmd_Type == 0x02ʱ�ǵ�һ�ζ���������һ�λ��ȶ������������к�
					{
						printf("1\r\n");
						for(i=0;i<32;i++)
						{
							os_evt_set(IWDG_BIT_8, HandleTaskWdg);
							os_dly_wait(500);  // �״�Һλ��Ҫ��ʼ�����ڼ䲻�ܷ��������������ᵼ���״�Һλ��BUG�����Ե�һ�ζ����ӳ�10��
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
		

		//����1���Ӿ�ֵ
		
		os_evt_set(IWDG_BIT_8, HandleTaskWdg);
		
	
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskScan
*	����˵��: ���ݲɼ���
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: 4  
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
*	�� �� ��: AppTaskCtl
*
*	����˵��: ����LED��˸��
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 2  
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
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	

	//�ɼ���Դ״̬����
	HandleTaskSTA    = os_tsk_create_user(	AppTaskSTA,              	
											1,                       	
											&AppTaskSTAStk,          	
											sizeof(AppTaskSTAStk));  	
	
	HandleTaskMsgPro = os_tsk_create_user(	AppTaskMsgPro,            	
											2,                      	
											&AppTaskMsgProStk,       	
											sizeof(AppTaskMsgProStk)); 	
	//���ݲɼ����񣺷��١����򡢽�����
	HandleTaskDAQ    = os_tsk_create_user(	AppTaskDAQ,             	
											3,                         
											&AppTaskDAQStk,         	
											sizeof(AppTaskDAQStk)); 	
	
	//��ʪ�Ȳɼ�����
	HandleTaskHumit  = os_tsk_create_user(	AppTaskHumit,                
											4,                         
											&AppTaskHumitStk,            
											sizeof(AppTaskHumitStk));  
	//��ѹ�ɼ�����										
	HandleTaskAtmos  = os_tsk_create_user(	AppTaskAtmos,                
											5,                         
											&AppTaskAtmosStk,            
											sizeof(AppTaskAtmosStk));  
		//�¶����ɼ�����									
	HandleTaskWaterTemp  = os_tsk_create_user(	AppTaskWaterTemp,                
											6,                         
											&AppTaskWaterTempStk,            
											sizeof(AppTaskWaterTempStk));
	
	//�ⲿͨ������
	HandleTaskComm   = os_tsk_create_user(	AppTaskComm,               
											7,                         
											&AppTaskCommStk,           
											sizeof(AppTaskCommStk));   
	
	//�ⲿ��������										
	HandleTaskCtl    = os_tsk_create_user(	AppTaskCtl,                
											8,                         
											&AppTaskCtlStk,            
											sizeof(AppTaskCtlStk));
	//���߲ɼ�����
	HandleTaskGauge  = os_tsk_create_user(	AppTaskGauge,             	
											9,                         
											&AppTaskGaugeStk,         	
											sizeof(AppTaskGaugeStk)); 
	//���Ź�����
	HandleTaskWdg    = os_tsk_create_user(	AppTaskWdg,                
											10,                         
											&AppTaskWdgStk,            
											sizeof(AppTaskWdgStk));
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskWdg
*
*	����˵��: ���Ź�����
*
*	��    ��: ��
*
*	�� �� ֵ: ��
*
*   �� �� ��: 7  
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

