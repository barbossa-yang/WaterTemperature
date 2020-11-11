/******************** Copyright (C) 2019-now Henan zhongyuan optical-electric measurement and control technology co.LTD All Rights Reserved*********************
* 文件名		: sdi.c
* 作  者	: 
* 版  本	: 
* 日  期	: 2019-07-16
* 描  述	: sdi接口函数，该函数用于apogee公司SI431红外温度传感器温度的读取
PA5：adi-12接口线
TIMER5：超时检测
TIMER7:解析接收数据使用
向sdi-12接口发送命令时调用Sdi_12_Transmission(const char *cmd,u8 tt);  //cmd是命令字，tt是发送该命令字次数
获取温度值调用SI400_Data(void)，如果成功读出温度值，则返回温度值，否则返回温度值为-1000；
一个完整的通讯流程如下：
1.调用SDI_PP_Configuration初始化通讯引脚及相关定时器；
2.首先调用Sdi_12_Transmission("0M!",1)函数，如果返回TURE进行步骤3，否则通讯失败；
3.延时1s；
4.调用Sdi_12_Transmission("0D0!",1)函数，如果返回TURE进行步骤5，否则通讯失败；
5.调用SI400_Data()函数返回温度值。
**************************************************** (C) 2019-现在 河南中原光电测控技术有限公司 版权所有 ****************************************************/

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
bool global_timeout_flag_test = FALSE;		//20180329,修改测试问题用打印
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
	
	TIM7->DIER |= 1 << 0;   //允许更新中断 
	
	TIM7->CR1  |= 0X01;     //使能定时器 7（这里面包括计数方向为向上计数）  
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
	EXTI->PR = 1 << 12; 			//清除LINE12上的中断标志位
	
	if(en)
		EXTI->IMR |= 1 << 12;		//不屏蔽line1上的中断
	else
		EXTI->IMR &= ~(1 << 12);  	// 屏蔽line1上的中断
}

/***************************************************************
* 函数 : delay_1us(unsigned int n)
* 描述 : us延时程序
* 输入 : 无
* 输出 : 无
* 返回 : 无
***************************************************************/
void delay_1us(u32 value)
{
	SysTick->LOAD  = value * 9; 		 //时间加载
	SysTick->CTRL |= 0x01;   			 //开始倒数

	while(!(SysTick->CTRL&(1<<16))); 	 //等待时间到达

	SysTick->CTRL = 0x00000000;  		 //关闭计数器
	SysTick->VAL  = 0x00000000;   		 //清空计数器	
}

/***************************************************************
* 函数 : delay_ms(int cnt)
* 描述 : ms延时程序
* 输入 : 无
* 输出 : 无
* 返回 : 无
***************************************************************/
void delay_ms(int cnt)
{
	int i;
	while(cnt--)
	{
	   for(i=0;i<8300;i++);
	   //喂狗
//	   IWDG_ReloadCounter();
	}
}

void Set_DQIO_In(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//设置GPIO速度为50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;				  	//设置为输入
  	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}

void Set_DQIO_Out(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				  	//设置GPIO速度为50MHZ
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				  	//设置为输出
  	GPIO_Init(GPIOE, &GPIO_InitStructure);							  	//将以上设置参数写入
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
//起始位高脉冲沿中断启动读取字节
//停止位低电平未读，再次开中断

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
			if(global_maximum_timer > 395) //最大时间
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
		SendTimes = tt;		//20180316修改，每次循环问两次传感器
		goto Inner_Ctrl;		
//		goto exit_label;	//20180316修改，发两次命令后跳出
	}

	if(--inner_loop_ctrl)
	{
		SendTimes = tt;		//20180316修改，每次循环问两次传感器
		goto Inner_Ctrl;		
//		goto exit_label;	//20180316修改，发两次命令后跳出
	}

   	if(--outer_loop_ctrl)
		goto retry_label;
//		goto exit_label;   //20180316修改，发两次命令后跳出

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

