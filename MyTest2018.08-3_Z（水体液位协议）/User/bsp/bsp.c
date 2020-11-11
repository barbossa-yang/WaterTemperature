/*
*********************************************************************************************************
*
*	模块名称 : BSP模块(For STM32F1XX)
*
*	文件名称 : bsp.c
*
*	版    本 : V1.0
*
*	修改时间 ：2018.8.24
*	
*	修改记录 :
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "bsp_sdi.h"
#include "bsp_user_lib.h"

char gstrStationID[]  = "A0001";
char gstrStationDNO[] = "002";	

extern CJQ_PZ_TYPE cjq_pz,*cjq_pz_ptr;

void Initialize_Cjq_Pz(void)
{
	cjq_pz_ptr = &cjq_pz;

	//Read_Para_Command();
	
	if(cjq_pz_ptr->init_flag != 0x0AA55)
	{
		strcpy(cjq_pz_ptr->Manufaxturer, "Henan Zhong Yuan Opto Electronics Measurement & Control Technology Co.,Ltd");
		strcpy(cjq_pz_ptr->SerialNumber, "201809#0001");
	
		strcpy(cjq_pz_ptr->Model_Ident,	"ZYC500");
		strcpy(cjq_pz_ptr->SoftwareVersion, "V01.00");
		strcpy(cjq_pz_ptr->HardwareVersion, "V01.00");
	
		strcpy(cjq_pz_ptr->Station_ID, "B0002");
		strcpy(cjq_pz_ptr->Station_DNO, "09");
	
		strcpy(cjq_pz_ptr->Station_LAT, "43.23.09");
		strcpy(cjq_pz_ptr->Station_LONG, "112.45.87");
		strcpy(cjq_pz_ptr->Station_ALT,	"8848.5"	);		

		cjq_pz_ptr->DTMD_Interval   = 1;
		cjq_pz_ptr->DHTD_Interval   = 1;				
		cjq_pz_ptr->STATRD_Interval = 60;			
		cjq_pz_ptr->SD_CF_SET       = 1;           
		cjq_pz_ptr->GALE_Value      = 17;         
		cjq_pz_ptr->TMAX_Value      = 40;          
		cjq_pz_ptr->TMIN_Value      = 0;          
		cjq_pz_ptr->RMAX_Value      = 50;          
		cjq_pz_ptr->DTLT_Value      = 35;          
		cjq_pz_ptr->DTLV_Value      = 20;          			
	
	
		cjq_pz_ptr->init_flag = 0x0AA55;					
	
		//Save_Para_Command();
	}
	
	if(cjq_pz_ptr->Station_ID[5] == '\0')
		  strcpy(gstrStationID,cjq_pz_ptr->Station_ID);
	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_Init
*	功能说明: 初始化硬件设备。只需要调用一次。该函数配置CPU寄存器和外设的寄存器并初始化一些全局变量。
*			 全局变量。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	bsp_InitUart(); 
	
	bsp_InitLed(); 	
	
	bsp_InitSPIBus();
	
	bsp_InitOthers();
	
	Initialize_Cjq_Pz();

	bsp_InitIwdg(0xC35);

	bsp_InitHardTimer();	

	bsp_InitRTC();
	
	RTC_ReadClock();
	
	SDI_PP_Configuration();
	
	/* 打印系统开机状态，方便查看系统是否复位 */
	printf("%04d-%02d-%02d %02d:%02d:%02d System Reset!\r\n",g_tRTC.Year,g_tRTC.Mon,g_tRTC.Day,g_tRTC.Hour,g_tRTC.Min,g_tRTC.Sec);
	
}

/***************************** END OF FILE *********************************/
