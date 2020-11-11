/******************** Copyright (C) 2019-now Henan zhongyuan optical-electric measurement and control technology co.LTD All Rights Reserved*********************
* �ļ���		: sdi.c
* ��  ��	: 
* ��  ��	: 
* ��  ��	: 2019-07-16
* ��  ��	: sdi�ӿں������ú�������apogee��˾SI431�����¶ȴ������¶ȵĶ�ȡ
PA5��adi-12�ӿ���
TIMER5����ʱ���
TIMER7:������������ʹ��
��sdi-12�ӿڷ�������ʱ����Sdi_12_Transmission(const char *cmd,u8 tt);  //cmd�������֣�tt�Ƿ��͸������ִ���
��ȡ�¶�ֵ����SI400_Data(void)������ɹ������¶�ֵ���򷵻��¶�ֵ�����򷵻��¶�ֵΪ-1000��
һ��������ͨѶ�������£�
1.����SDI_PP_Configuration��ʼ��ͨѶ���ż���ض�ʱ����
2.���ȵ���Sdi_12_Transmission("0M!",1)�������������TURE���в���3������ͨѶʧ�ܣ�
3.��ʱ1s��
4.����Sdi_12_Transmission("0D0!",1)�������������TURE���в���5������ͨѶʧ�ܣ�
5.����SI400_Data()���������¶�ֵ��
**************************************************** (C) 2019-���� ������ԭ����ؼ������޹�˾ ��Ȩ���� ****************************************************/

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "bsp_sdi.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

u8  original_data = 0x00;
extern u8  CountNum;
extern u16 CountValue[IT_COUNTER];
bool start_to_receive = FALSE;
bool update_data_flag = FALSE;
u8 receive_data,response_data[RECEIVE_DATA_LEN];
bool first_times_flag = TRUE;
bool global_timeout_flag = FALSE;
bool global_timeout_flag_test = FALSE;		//20180329,�޸Ĳ��������ô�ӡ
bool maximum_overtime_flag = FALSE;

extern u32 global_retry_timer;
extern u32 global_elapsed_timer;
extern u32 global_maximum_timer;
extern u8  global_break_timer;
extern u8  global_marking_timer;
extern u8  global_timeout_timer;
volatile bool start_bit_flag;

void SDI_PP_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;    
//	NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;	
	EXTI_InitTypeDef EXTI_InitStructure;
	
//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource5);

    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
		
}

void Enable_SdiLine_Int(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void Disable_SdiLine_Int(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

		EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;
    EXTI_Init(&EXTI_InitStructure);
}

void Timeout_Timer_Config(u16 p) 
{  
	TIM7->ARR = p; 		     
	
	TIM7->PSC = 71;//9; 	    
	
	TIM7->DIER |= 1 << 0;   //��������ж� 
	
	TIM7->CR1  |= 0X01;     //ʹ�ܶ�ʱ�� 7�������������������Ϊ���ϼ�����  
}

uint8_t insert_parity(uint8_t ch)
{
	uint8_t i,data[8];

	for(i=0;i<7;i++)
	{
		data[i] = (ch >> i) & 0x01;
	}
	
	i = data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6];

	return  i?(ch | 0x80):(ch & 0x7f); 
}

uint8_t check_parity(uint8_t ch)
{
	uint8_t i,data[8];

	for(i=0;i<8;i++)
	{
		data[i] = (ch >> i) & 0x01;
	}
	
	i = data[0] ^ data[1] ^ data[2] ^ data[3] ^ data[4] ^ data[5] ^ data[6] ^ data[7];

	return  i; 
}


void sdi_line_int(u8 en)
{
	EXTI->PR = 1 << 12; 			//���LINE12�ϵ��жϱ�־λ
	
	if(en)
		EXTI->IMR |= 1 << 12;		//������line1�ϵ��ж�
	else
		EXTI->IMR &= ~(1 << 12);  	// ����line1�ϵ��ж�
}

/***************************************************************
* ���� : delay_1us(unsigned int n)
* ���� : us��ʱ����
* ���� : ��
* ��� : ��
* ���� : ��
***************************************************************/
void delay_1us(u32 value)
{
	SysTick->LOAD  = value * 9; 		 //ʱ�����
	SysTick->CTRL |= 0x01;   			 //��ʼ����

	while(!(SysTick->CTRL&(1<<16))); 	 //�ȴ�ʱ�䵽��

	SysTick->CTRL = 0x00000000;  		 //�رռ�����
	SysTick->VAL  = 0x00000000;   		 //��ռ�����	
}

/***************************************************************
* ���� : delay_ms(int cnt)
* ���� : ms��ʱ����
* ���� : ��
* ��� : ��
* ���� : ��
***************************************************************/
void delay_ms(int cnt)
{
	int i;
	while(cnt--)
	{
	   for(i=0;i<8300;i++);
	   //ι��
//	   IWDG_ReloadCounter();
	}
}

void Set_DQIO_In(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//����GPIO�ٶ�Ϊ50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				  	//����Ϊ����
  	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}

void Set_DQIO_Out(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//����GPIO�ٶ�Ϊ50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				  	//����Ϊ���
  	GPIO_Init(GPIOE, &GPIO_InitStructure);							  	//���������ò���д��
}

void ow_writebyte1(u8 tt,u8 channel)
{
	u8 i,data;

//	Set_DQIO_Out();

	data = ~(insert_parity(tt));
	
	switch(channel)
	{
		case 1:
			SetDQ();
		break;
		case 2:
			SetChannelOne();
		break;
		case 3:
			SetChannelTwo();
		break;
		case 4:
			SetChannelThree();
		break;
		case 5:
			SetChannelFour();
		break;
		case 6:
			SetChannelFive();
		break;
		default:
			break;
	}
	
	delay_1us(833);		// while(!sdi_1200bps_flag);sdi_1200bps_flag = false;

	for(i=0;i<8;i++)
	{
		if(data & 0x01)	
		{
			switch(channel)
			{
				case 1:
					SetDQ();
				break;
				case 2:
					SetChannelOne();
				break;
				case 3:
					SetChannelTwo();
				break;
				case 4:
					SetChannelThree();
				break;
				case 5:
					SetChannelFour();
				break;
				case 6:
					SetChannelFive();
				break;
				default:
					break;
			}
		}
		else
		{
			switch(channel)
			{
				case 1:
					ResetDQ();
				break;
				case 2:
					ResetChannelOne();
				break;
				case 3:
					ResetChannelTwo();
				break;
				case 4:
					ResetChannelThree();
				break;
				case 5:
					ResetChannelFour();
				break;
				case 6:
					ResetChannelFive();
				break;
				default:
					break;
			}
		}
			

		delay_1us(833); // while(!sdi_1200bps_flag);sdi_1200bps_flag = false;
		  
		data >>= 1;        
	}

	switch(channel)
	{
		case 1:
			ResetDQ();
		break;
		case 2:
			ResetChannelOne();
		break;
		case 3:
			ResetChannelTwo();
		break;
		case 4:
			ResetChannelThree();
		break;
		case 5:
			ResetChannelFour();
		break;
		case 6:
			ResetChannelFive();
		break;
		default:
			break;
	}
	delay_1us(833);		// while(!sdi_1200bps_flag);sdi_1200bps_flag = false;

}

void Send_Command(const char *p,u8 channel)
{
	char tt;
	u8 i = 0;

	while( (tt = p[i++])!='\0' ) 
	{
		ow_writebyte1(tt, channel);
//		IWDG_ReloadCounter();
	}
}

// break   = 1;	 at least 12ms
// marking = 0;	 at least 8.33ms

void ow_writebit1(u8 state, u8 channel)
{
	if(state)
	{
		switch(channel)
		{
			case 1:
				SetDQ();
			break;
			case 2:
				SetChannelOne();
			break;
			case 3:
				SetChannelTwo();
			break;
			case 4:
				SetChannelThree();
			break;
			case 5:
				SetChannelFour();
			break;
			case 6:
				SetChannelFive();
			break;
			default:
				break;
		}
	}
	else
	{
		switch(channel)
		{
			case 1:
				ResetDQ();
			break;
			case 2:
				ResetChannelOne();
			break;
			case 3:
				ResetChannelTwo();
			break;
			case 4:
				ResetChannelThree();
			break;
			case 5:
				ResetChannelFour();
			break;
			case 6:
				ResetChannelFive();
			break;
			default:
				break;
		}
	}
}

//--------------------------------------------------------------------------
//��ʼλ���������ж�������ȡ�ֽ�
//ֹͣλ�͵�ƽδ�����ٴο��ж�

u8  ow_readbyte1(void)	 
{
	u8  i,data = 0x00;
	
	for(i=0;i<8;i++)
	{
		delay_1us(833 + 300);

		if(GetDQ())
		{
			data |= 1<<i;
		}
	}
	
	original_data = data;

	data = ~data;
	
	data = data & 0x7f;	 //check_parity(data);
	  
	return data;
}


bool Sdi_12_Transmission(const char *cmd,u8 tt,u8 channel)
{
	volatile u16  i,j,num,k,cnt,data;
	u16 level,temp;
	u8 SendTimes = tt-1;
	u8 outer_loop_ctrl = 3;
	u8 inner_loop_ctrl = 3;	
	
	if(global_elapsed_timer > 87 || first_times_flag)  
	{

retry_label: 					
		global_retry_timer = 0x00; 
		Set_DQIO_Out();		
		global_break_timer = 0x00; 
		ow_writebit1(0x01,channel);	
		while(global_break_timer < 12);
		inner_loop_ctrl = 3; 
	}

Inner_Ctrl:
	Set_DQIO_Out();
	global_timeout_timer = 0x00;
	global_marking_timer = 0x00; 
	ow_writebit1(0x00,channel);	
	while(global_marking_timer < 9);
	Send_Command(cmd,channel);	//transmit the address,the command,& !	
	if(SendTimes != 0)
	{
		delay_ms(66);
		SendTimes = 0;
		goto Inner_Ctrl; 
	}
	Set_DQIO_In();
	global_maximum_timer = 0;
 	start_bit_flag 		  = FALSE;
	start_to_receive 	  = FALSE;
	global_timeout_flag   = FALSE;
	maximum_overtime_flag = FALSE;
							   
	CountNum = 0x00;
	receive_data = 0x00;	
	Timeout_Timer_Config(65530);
	delay_1us(500);		
	Enable_SdiLine_Int(); 

	while(!start_bit_flag)
	{
		if(global_timeout_timer > 65)
		{
			global_timeout_flag = TRUE;
			break; 
		}

//		IWDG_ReloadCounter();
	}

	delay_ms(200);

	Disable_SdiLine_Int();

	if(start_bit_flag)
	{
		u8 temp_value[10] = {0};
		start_to_receive = TRUE;
		start_bit_flag = FALSE;
		num = 1; i = 0; j = 0; cnt = 0;
		do
		{
			if(CountNum > num + 1)	 
			{
				level = CountValue[num++];	
				temp  = round(CountValue[num++]/833.0);
				for(j=0;j<temp;j++)
				{
					if(level == 1)
					{
						temp_value[i++] = 0x01; 
					}
					else
					{
						temp_value[i++] = 0x00; 
					}
					if(i == 10)
					{
					  data = 0x00;
						for(k=8;k>=1;k--)
						{
							data <<= 1;														
							if(temp_value[k])
							{
								data |= 0x01;
							}
							else
							{
								data &= 0xfe;
							}
						}						
						original_data = data;					
						data = ~data;						
						data = data & 0x7f;	
						receive_data = data;
						response_data[cnt++] = receive_data;
						if(data == 0x0D) 
						{	
								Disable_SdiLine_Int();
								i = 0;
								break;	
						}			
						i = 0;			
					}
				}
			}
			if(global_maximum_timer > 395) //���ʱ��
			{
				maximum_overtime_flag = TRUE;
				
//				printf("CountNum = %d ", CountNum);
//				for(int p = 0; p < CountNum; p++)
//				{
//					printf("%d ", CountValue[p]);
//				}
//				printf("%s ", response_data);
//				memset(CountValue, 0 ,IT_COUNTER);
//				printf("global_maximum_timer = %d %d %d\r\n",global_maximum_timer,num, i);
				break;							
			}
			if(original_data == 0x0ff)
			{
//				printf("original_data\r\n");
			  break;
			}
//			IWDG_ReloadCounter();
		} while(receive_data != 0x0d); 
		
		memset(CountValue, 0 ,IT_COUNTER);
		
		if(maximum_overtime_flag)	
		   goto exit_label;
		else
		if(!global_timeout_flag)
		{
		 	//check data,valid response?			
			global_elapsed_timer = 0x00;  //restart 87ms timer
			return TRUE;
		}
	}
	
	if(global_retry_timer < 113)		
	{
		SendTimes = tt;		//20180316�޸ģ�ÿ��ѭ�������δ�����
		goto Inner_Ctrl;		
//		goto exit_label;	//20180316�޸ģ����������������
	}

	if(--inner_loop_ctrl)
	{
		SendTimes = tt;		//20180316�޸ģ�ÿ��ѭ�������δ�����
		goto Inner_Ctrl;		
//		goto exit_label;	//20180316�޸ģ����������������
	}

   	if(--outer_loop_ctrl)
		goto retry_label;
//		goto exit_label;   //20180316�޸ģ����������������

exit_label:	
	global_elapsed_timer = 0x00;
	return FALSE;
}


//void InquireDeviceInfo(u8 channel)
//{
//	char channel_addr[3];
//	sprintf(channel_addr, "%dI!", channel);
//	if(Sdi_12_Transmission(channel_addr,1))
//	{
//		printf("%s", response_data);
//	}
//}

