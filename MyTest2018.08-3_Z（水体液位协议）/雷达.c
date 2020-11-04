/*****************************************************************************************************/


u8 s_cmd1[] = {0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x02,0x80,0x00,0x00,0x82}; 
u8 s_cmd2[] = {0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x82,0x62,0xC7,0x13,0x97,0xFC,0x03,0x00,0x5C};

u16 read_radar_count = 0;

void Read_From_Gauge(u8 tt)
{
	read_radar_count++;
	
	if( tt != 0x00 )
	{
		TxPointer2 = 0x00;
		
		if(tt == 0x01)
		{
			TxCounter2 = sizeof(s_cmd1);
			memcpy((char *)TxBuffer2,(char *)s_cmd1,TxCounter2);		
		}
		else
		if( tt == 0x02)
		{
			TxCounter2 = sizeof(s_cmd2);
			memcpy((char *)TxBuffer2,(char *)s_cmd2,TxCounter2);			
		}
		
		RS485_TX_EN();
		
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		
		TR_FLAG2 = 0x80;
	}	
}

extern void Fun_CallBack1(void);

float gauge_temp[]= {0.0,0.0,0.0,0.0};


float gauge_high =0;

u8 gauge_read_sucess =0x00;

void Analyze_Gauge( u8 tt )
{
	uint8_t  i;

	uint8_t tmpLength,uintCheck,uintSum;	
	
	Com_Type2 = RxBuffer2[0]; 

	uintCheck = 0;
	
	tmpLength = Rx_Fra_Len2;

	for(i = 0;i < tmpLength;i++)
	{
		uintCheck ^= RxBuffer2[i];
	}
	
	uintSum  = RxBuffer2[tmpLength];
	
	if(uintCheck == 0x00)	
	{
		if(Com_Type2 == 0x06)
		{
			i=8;			
			
			s_cmd2[i++] = RxBuffer2[0x07];
			s_cmd2[i++] = RxBuffer2[0x08];
			s_cmd2[i++] = RxBuffer2[0x0f];
			s_cmd2[i++] = RxBuffer2[0x10];
			s_cmd2[i++] = RxBuffer2[0x11];
			
			uintCheck = 0;
			
			for(i = 7;i < sizeof(s_cmd2)-1;i++)
			{
				uintCheck ^= s_cmd2[i];
			}
			
			s_cmd2[i] = uintCheck;
			
			TR_FLAG2 = 0x80;  
			
			read_gauge_flag = 0x0ff;
			
			read_gauge_flag  =0x03;
		}
		else
		{
			i = 0x0a;
			
			guionData.B[3] = RxBuffer2[i++];
			guionData.B[2] = RxBuffer2[i++];
			guionData.B[1] = RxBuffer2[i++];
			guionData.B[0] = RxBuffer2[i++];
			
			gauge_temp[0] = guionData.f;
			
			i++;
			
			guionData.B[3] = RxBuffer2[i++];
			guionData.B[2] = RxBuffer2[i++];
			guionData.B[1] = RxBuffer2[i++];
			guionData.B[0] = RxBuffer2[i++];
			
			gauge_temp[1] = guionData.f;	
			
			i++;
			
			guionData.B[3] = RxBuffer2[i++];
			guionData.B[2] = RxBuffer2[i++];
			guionData.B[1] = RxBuffer2[i++];
			guionData.B[0] = RxBuffer2[i++];
			
			gauge_temp[2] = guionData.f;

			gauge_high = (float)SETUP_HIGH - gauge_temp[1]*100;
			
			gauge_read_sucess =0x0ff;
			
			read_radar_count =0x00;
			
			i++;

			guionData.B[3] = RxBuffer2[i++];
			guionData.B[2] = RxBuffer2[i++];
			guionData.B[1] = RxBuffer2[i++];
			guionData.B[0] = RxBuffer2[i++];
			
			gauge_temp[3] = guionData.f;
			
			read_gauge_flag = 0x00;
			
			TR_FLAG2 = 0x80;  

			read_gauge_flag  =0x00;
		}
	}
	
}


/*******************************************************************************
* Function Name  : UART2_IRQHandler
* Description    : This function handles UART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

extern void USART_CallBack2(void);
extern void USART_CallBack21(void);

static u8 temp_ptr = 0x00;	

//u8 temp[64],ch_ptr=0;

void USART2_IRQHandler(void)
{
	uint8_t ch;
	
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		ch = USART_ReceiveData(USART2);
		
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);

		if(r_485_port == 0x01)
		{
			if( TR_FLAG2 == 0x80 )
			{
				if( ch == 0xaa )
					TR_FLAG2 = 0x81;
			}
			else if( TR_FLAG2 == 0x81 )
			{
				if( ch < 0x80 )
				{	
					TR_FLAG2    = 0x83;
					Rx_Fra_Len2 = ch;
					RxCounter2  = Rx_Fra_Len2 + 3;
					RxPointer2  = 0; 
					R_interval2 = 0;
				}
				else
				{
					TR_FLAG2 = 0x80;
				} 
			}
			else if( TR_FLAG2 == 0x83 )  
			{
				RxBuffer2[RxPointer2] = ch;	
				RxPointer2++; 
				
				R_interval2 = 0;
				
				if( RxPointer2 == RxCounter2 )
				{
					TR_FLAG2 = 0x87; 

					Cmd_Type = 0x41;
					
					USART_CallBack2();
				}
			}
		}
		else
		if(r_485_port == 0x02)
		{
			if( TR_FLAG2 == 0x80 )
			{	
				if( (ch == 0x06)  || (ch == 0x86)) 
				{
					RxPointer2 = 0;
					
					temp_ptr = ch;
					
					RxBuffer2[RxPointer2++] = ch;
					
					(ch == 0x06)? (R_interval2 = 29):(R_interval2 = 35);
					
					TR_FLAG2 = 0x81;
				}
			}
			else
			if( TR_FLAG2 == 0x81 )
			{
				RxBuffer2[RxPointer2++] = ch;	
				
				if(RxPointer2 >= R_interval2)
				{
					TR_FLAG2 = 0x87;

					Rx_Fra_Len2 = R_interval2;
					
					if(temp_ptr == 0x06)
						Cmd_Type = 0x06;
					else
						Cmd_Type = 0x86;
					
					USART_CallBack21();					
				}
			}
		}
	}
	
	/* 处理发送缓冲区空中断 */
	
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		USART_SendData(USART2,TxBuffer2[TxPointer2++]);
		
		if(TxPointer2 >= TxCounter2)
		{
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);

			USART_ITConfig(USART2, USART_IT_TC, ENABLE);
		}
	}
	else if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		if (TxPointer2 >= TxCounter2)
		{
			USART_ITConfig(USART2, USART_IT_TC, DISABLE);

			RS485_RX_EN();
		}
		else
		{
			USART_SendData(USART2,TxBuffer2[TxPointer2++]);
		}
	}
}