/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*
*	文件名称 : bsp_user_lib.c
*
*	版    本 : V1.2
*
*	说    明 : 
*
*   修改记录 :
*               
*
*********************************************************************************************************
*/
#include "bsp_user_lib.h"
#include "bsp.h"
#include "bsp_sdi.h"

volatile int timer0,timer1,timer2,ZYGD_Timer;

volatile u8 	H_Sen_NO[12]={1,1,1,1,1,1,1,1,1,1,1,1};		//传感器开关

volatile char	H_State[20]={0}; 		//系统状态数组

volatile vu16 Rec_Year,Rec_Month,Rec_Day;
volatile vu8  Rec_Hour,Rec_Min,Rec_Sec;

volatile vu16 Cur_Year,Cur_Month,Cur_Day,Cur_WeekDay, ADCConvertedValue;//,Cur_yDay
volatile vu8  Cur_Hour,Cur_Min,Cur_Sec;

volatile float JZXX_NO[14],JZSX_NO[14],CYBHL_NO[14],CWBHL_NO[14],ZXBHL_NO[14],Alarm_NO[6];

u8 read_gauge_flag = 0x00;

extern u8 Cmd_Type;
extern volatile bool start_bit_flag;

u16  CountNum = 0;
u16 CountValue[IT_COUNTER];

u32 global_retry_timer = 0;
u32 global_elapsed_timer = 0;
u32 global_maximum_timer = 0;
u8  global_break_timer = 0;
u8  global_marking_timer = 0;
u8  global_timeout_timer = 0;

union char_or_float
{
  float f;
  u8 B[4];
} guionData;

typedef struct
{
	uint16_t Year;				/* 日期 */
	uint8_t  Month;
	uint8_t  Day;
	uint8_t  Hour;				/* 时 */
	uint8_t  Minute;			/* 分 */
	uint8_t  Second;			/* 秒 */

} SYSTEM_TIME;

SYSTEM_TIME g_tCurrent,g_tHost;

CJQ_PZ_TYPE cjq_pz,*cjq_pz_ptr;

// CRC 高位字节值表

static const uint8_t s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

// CRC 低位字节值表
const uint8_t s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

/*
*********************************************************************************************************
*	函 数 名: str_len
*	功能说明: 计算字符串长度.0是结束符
*	形    参: _str : 缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
int str_len(char *_str)
{
	int len = 0;

	while (*_str++) len++;
	return len;
}

/*
*********************************************************************************************************
*	函 数 名: str_cpy
*	功能说明: 复制字符串
*	形    参: tar : 目标缓冲区
*			 src : 源缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void str_cpy(char *_tar, char *_src)
{
	do
	{
		*_tar++ = *_src;
	}
	while (*_src++);
}

/*
*********************************************************************************************************
*	函 数 名: str_cmp
*	功能说明: 字符串比较
*	形    参: s1 : 字符串1
*			  s2 : 字符串2
*	返 回 值: 0 表示相等 非0表示不等
*********************************************************************************************************
*/
int str_cmp(char * s1, char * s2)
{
	while ((*s1!=0) && (*s2!=0) && (*s1==*s2))
	{
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

/*
*********************************************************************************************************
*	函 数 名: str_copy
*	功能说明: 复制字符串
*	形    参: tar : 目标缓冲区
*			 src : 源缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void mem_set(char *_tar, char _data, int _len)
{
	while (_len--)
	{
		*_tar++ = _data;
	}
}

/*
*********************************************************************************************************
*	函 数 名: int_to_ascii
*	功能说明: 将整数转换为ASCII数组。支持负数。
*	形    参: _Number : 整数
*			 _pBuf : 目标缓冲区, 存放转换后的结果。以0结束的字符串。
*			 _len : ASCII字符个数, 字符串长度
*	返 回 值: 无
*********************************************************************************************************
*/
void int_to_str(int _iNumber, char *_pBuf, unsigned char _len)
{
	unsigned char i;
	int iTemp;

	if (_iNumber < 0)	/* 负数 */
	{
		iTemp = -_iNumber;	/* 转为正数 */
	}
	else
	{
		iTemp = _iNumber;
	}

	mem_set(_pBuf, ' ',_len);

	/* 将整数转换为ASCII字符串 */
	for (i = 0; i < _len; i++)
	{
		_pBuf[_len - 1 - i] = (iTemp % 10) + '0';
		iTemp = iTemp / 10;
		if (iTemp == 0)
		{
			break;
		}
	}
	_pBuf[_len] = 0;

	if (_iNumber < 0)	/* 负数 */
	{
		for (i = 0; i < _len; i++)
		{
			if ((_pBuf[i] == ' ') && (_pBuf[i + 1] != ' '))
			{
				_pBuf[i] = '-';
				break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: str_to_int
*	功能说明: 将ASCII码字符串转换成整数。 遇到小数点自动越过。
*	形    参: _pStr :待转换的ASCII码串. 可以以逗号，#或0结束。 2014-06-20 修改为非0-9的字符。
*	返 回 值: 二进制整数值
*********************************************************************************************************
*/
int str_to_int(char *_pStr)
{
	unsigned char flag;
	char *p;
	int ulInt;
	unsigned char  i;
	unsigned char  ucTemp;

	p = _pStr;
	if (*p == '-')
	{
		flag = 1;	/* 负数 */
		p++;
	}
	else
	{
		flag = 0;
	}

	ulInt = 0;
	for (i = 0; i < 15; i++)
	{
		ucTemp = *p;
		if (ucTemp == '.')	/* 遇到小数点，自动跳过1个字节 */
		{
			p++;
			ucTemp = *p;
		}
		if ((ucTemp >= '0') && (ucTemp <= '9'))
		{
			ulInt = ulInt * 10 + (ucTemp - '0');
			p++;
		}
		else
		{
			break;
		}
	}

	if (flag == 1)
	{
		return -ulInt;
	}
	return ulInt;
}

/*
*********************************************************************************************************
*	函 数 名: BEBufToUint16
*	功能说明: 将2字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint16
*	功能说明: 将2字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}


/*
*********************************************************************************************************
*	函 数 名: BEBufToUint32
*	功能说明: 将4字节数组(大端Big Endian次序，高字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*
*   大端(Big Endian)与小端(Little Endian)
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint32
*	功能说明: 将4字节数组(小端Little Endian，低字节在前)转换为16位整数
*	形    参: _pBuf : 数组
*	返 回 值: 16位整数值
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}

/*
*********************************************************************************************************
*	函 数 名: CRC16_Modbus
*	功能说明: 计算CRC。 用于Modbus协议。
*	形    参: _pBuf : 参与校验的数据
*			  _usLen : 数据长度
*	返 回 值: 16位整数值。 对于Modbus ，此结果高字节先传送，低字节后传送。
*
*   所有可能的CRC值都被预装在两个数组当中，当计算报文内容时可以简单的索引即可；
*   一个数组包含有16位CRC域的所有256个可能的高位字节，另一个数组含有低位字节的值；
*   这种索引访问CRC的方式提供了比对报文缓冲区的每一个新字符都计算新的CRC更快的方法；
*
*  注意：此程序内部执行高/低CRC字节的交换。此函数返回的是已经经过交换的CRC值；也就是说，该函数的返回值可以直接放置
*        于报文用于发送；
*********************************************************************************************************
*/
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen)
{
	uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
	uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
	uint16_t usIndex; 		/* CRC循环中的索引 */

    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}


/*
*********************************************************************************************************
*	函 数 名: CaculTwoPoint
*	功能说明: 根据2点直线方程，计算Y值
*	形    参:  2个点的坐标和x输入量
*	返 回 值: x对应的y值
*********************************************************************************************************
*/
int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
	return y1 + ((int64_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

/*
*********************************************************************************************************
*	函 数 名: BcdToChar
*	功能说明: 将BCD码转为ASCII字符。 比如 0x0A ==> 'A'
*	形    参: 	_bcd   ：输入的二进制数。必须小于16
*	返 回 值: 转换结果
*********************************************************************************************************
*/
char BcdToChar(uint8_t _bcd)
{
	if (_bcd < 10)
	{
		return _bcd + '0';
	}
	else if (_bcd < 16)
	{
		return _bcd + 'A';
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: HexToAscll
*	功能说明: 将二进制数组转换为16进制格式的ASCII字符串。每个2个ASCII字符后保留1个空格。
*			  0x12 0x34 转化为 0x31 0x32 0x20 0x33 0x34 0x00  即 "1234"
*	形    参: 	_pBin   ：输入的数据，二进制数组
*				_pAscii ：存放转换结果, ASCII字符串，0结束。1个二进制对应2个ASCII字符.
*	返 回 值: 转换得到的整数
*********************************************************************************************************
*/
void HexToAscll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes)
{
	uint16_t i;
	
	for (i = 0; i < _BinBytes; i++)
	{
		_pAscii[3 * i] = BcdToChar(_pHex[i] >> 4);
		_pAscii[3 * i + 1] = BcdToChar(_pHex[i] & 0x0F);
		_pAscii[3 * i + 2] = ' ';
	}
	_pAscii[3 * i + 2] = 0;
}

/*
*********************************************************************************************************
*	函 数 名: AsciiToUint32
*	功能说明: 变长的 ASCII 字符转换为32位整数  ASCII 字符以空格或者0结束 。 支持16进制和10进制输入
*	形    参: *pAscii ：要转换的ASCII码
*	返 回 值: 转换得到的整数
*********************************************************************************************************
*/
uint32_t AsciiToUint32(char *pAscii)
{
	char i;
	char bTemp;
	char bIsHex;
	char bLen;
	char bZeroLen;
	uint32_t lResult;
	uint32_t lBitValue;

	/* 判断是否是16进制数 */
	bIsHex = 0;
	if ((pAscii[0] == '0') && ((pAscii[1] == 'x') || (pAscii[1] == 'X')))
	{
		bIsHex=1;
	}

	lResult=0;
	// 最大数值为 4294967295, 10位+2字符"0x" //
	if (bIsHex == 0)
	{ // 十进制 //
		// 求长度 //
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i];
			if (bTemp != '0')
				break;
		}
		bZeroLen = i;

		for (i = 0; i < 10; i++)
		{
			if ((pAscii[i] < '0') || (pAscii[i] > '9'))
				break;
			lBitValue = lBitValue * 10;
		}
		bLen = i;
		lBitValue = lBitValue / 10;
		if (lBitValue == 0)
			lBitValue=1;
		for (i = bZeroLen; i < bLen; i++)
		{
			lResult += (pAscii[i] - '0') * lBitValue;
			lBitValue /= 10;
		}
	}
	else
	{	/* 16进制 */
		/* 求长度 */
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i + 2];
			if(bTemp!='0')
				break;
		}
		bZeroLen = i;
		for (; i < 8; i++)
		{
			bTemp=pAscii[i+2];
			if (((bTemp >= 'A') && (bTemp <= 'F')) ||
				((bTemp>='a')&&(bTemp<='f')) ||
				((bTemp>='0')&&(bTemp<='9')) )
			{
				lBitValue=lBitValue * 16;
			}
			else
			{
				break;
			}
		}
		lBitValue = lBitValue / 16;
		if (lBitValue == 0)
			lBitValue = 1;
		bLen = i;
		for (i = bZeroLen; i < bLen; i++)
		{
			bTemp = pAscii[i + 2];
			if ((bTemp >= 'A') && (bTemp <= 'F'))
			{
				bTemp -= 0x37;
			}
			else if ((bTemp >= 'a') && (bTemp <= 'f'))
			{
				bTemp -= 0x57;
			}
			else if ((bTemp >= '0') && (bTemp <= '9'))
			{
				bTemp -= '0';
			}
			lResult += bTemp*lBitValue;
			lBitValue /= 16;
		}
	}
	return lResult;
}

/*******************************************************************************
* Function Name  : EXTI3_IRQHandler
* Description    : This function handles External interrupt Line 3 request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void EXTI9_5_IRQHandler(void)
{
	u16 IC7ReadValue1;
	u8 CaptureLevel = 0;
	
	if (EXTI_GetITStatus(EXTI_Line6) != RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
	
	if (EXTI_GetITStatus(EXTI_Line7) != RESET)
	{		
		EXTI_ClearITPendingBit(EXTI_Line7);
	}	
  
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		IC7ReadValue1 = TIM_GetCounter(TIM7);

		TIM_SetCounter(TIM7,0);

		EXTI_ClearITPendingBit(EXTI_Line5);

		CountValue[CountNum++] = IC7ReadValue1;

		CaptureLevel = GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5) ;	

		CountValue[CountNum++] = CaptureLevel;

		if(CaptureLevel == 1)    
	  {	
			start_bit_flag = TRUE;
		}
	}
	
}

/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/


u8  timer_1s = 0;
u32 timer1_1s= 0;

u32 timer1_1s_1= 0;

extern void TIM_CallBack1(void);

extern void TIM_CallBack3(void);

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{	
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);		
		
		timer_1s++;
		timer1_1s++;
		
		timer1_1s_1++;
		
		if(timer_1s%3 == 0)	//3s定时采集信号
		{
			TIM_CallBack1();
			
			TIM_CallBack3();
		}			
		
		if(H_Sen_NO[2]>0 || H_Sen_NO[3]>0)
		{
		}
	}	
}
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

//定时器3用作计风速

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{	
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);	
	}
}

/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
//	{
//		global_retry_timer++;		
// 		global_elapsed_timer++;		
//		global_break_timer++;
//		global_marking_timer++;		
//		global_maximum_timer++;
//		global_timeout_timer++; 
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
	{
		global_retry_timer++;		
 		global_elapsed_timer++;		
		global_break_timer++;
		global_marking_timer++;		
		global_maximum_timer++;
		global_timeout_timer++; 
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}
}

/*****************************************************************************/

int comp(const void*a,const void*b)
{
	return *(int*)a-*(int*)b;
}

extern u32 freq[Filter_Times];

u32 tide_min,tide_sec[3],tide_3s[20];

u32  tide_temp[20];

u32  time_sec_counter=0;

u32  freq_3sec,freq_1min;

int  diff[3];

float cur_min_tide;


/*******************************************************************************/

volatile u8 RxBuffer5[32];

volatile u8 TxCounter5,TxPointer5,RxCounter5,RxPointer5;

volatile u8 TR_FLAG5,Tx_Fra_Len5,Rx_Fra_Len5;
volatile u8 R_interval5,Com_Type5;

static u8 TxBuffer5[]={0};


extern void USART_CallBack5(void);

void UART5_IRQHandler(void)
{
//	uint8_t ch;	
	
	/* 处理接收中断 */	
	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		//ch = USART_ReceiveData(UART5);
		USART_ReceiveData(UART5);
		
		USART_ClearITPendingBit(UART5,USART_IT_RXNE);
	}
	
	if (USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{
		USART_SendData(UART5,TxBuffer5[TxPointer5++]);
		
		if(TxPointer5 >= TxCounter5)
		{
			USART_ITConfig(UART5, USART_IT_TXE, DISABLE);

			USART_ITConfig(UART5, USART_IT_TC, ENABLE);
		}
	}
	else if (USART_GetITStatus(UART5, USART_IT_TC) != RESET)
	{
		if (TxPointer5 >= TxCounter5)
		{
			USART_ITConfig(UART5, USART_IT_TC, DISABLE);
		}
		else
		{
			USART_SendData(UART5,TxBuffer5[TxPointer5++]);
		}
	}
}


/*******************************************************************************/
//RS485操作函数

extern u8 r_485_port;



/*********************************************************************************************/
//温湿度数据采集函数

/****************************************************************
* 	函数：Read_From_WSD(void)
*
* 	描述：串口2发送采集命令
*
****************************************************************/


volatile u8 RxBuffer2[64],TxBuffer2[32];

volatile u8 TxCounter2,TxPointer2,RxCounter2,RxPointer2;

volatile u8 TR_FLAG2,Tx_Fra_Len2,Rx_Fra_Len2;
volatile u8 R_interval2,Com_Type2;

u8	 S_Addr,D_Addr = 1;

float Temp_fVal,Hum_fVal;

void Read_From_WSD(void)
{
	
	u8 i,j,temp;
	u16 checksum;
	
	Tx_Fra_Len2  = 0x03;
	Com_Type2    = 0xC1;
	
	TxBuffer2[0x00] = 0xff;
	TxBuffer2[0x01] = 0xff;
	TxBuffer2[0x02] = 0xff;
	
	TxBuffer2[0x03] = 0xaa; 
	
	TxBuffer2[0x04] = Tx_Fra_Len2;
	TxBuffer2[0x05] = Com_Type2;
	
	TxBuffer2[0x06] = S_Addr;
	TxBuffer2[0x07] = D_Addr;
	
	checksum  = 0x00;
	
	j = 5 + Tx_Fra_Len2;
	
	for( i = 5;i < j;i++ )
	{
		checksum = checksum + TxBuffer2[i];
	}
	
	temp = checksum & 0xff;    
	
	TxBuffer2[i++] = temp;
	
	temp = checksum >> 8;  
	
	TxBuffer2[i++] = temp;
	
	TxBuffer2[i++] = 0xdd;
	TxBuffer2[i++] = 0xff;
	TxBuffer2[i++] = 0xff;
	
	TxPointer2 = 0x00;
	
	TxCounter2 = i;
	
//	RS485_TX_EN();
	
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	
	TR_FLAG2 = 0x80;	
}


void Analyze_Humit( void )
{
	uint8_t  i;

	uint16_t tmpLength,uintCheck,uintSum;	
	
	Com_Type2 = RxBuffer2[0]; 

	uintCheck = 0;
	
	tmpLength = Rx_Fra_Len2;

	for(i = 0;i < tmpLength;i++)
	{
		uintCheck += RxBuffer2[i];
	}
	
	uintSum  = RxBuffer2[tmpLength];
	uintSum +=(RxBuffer2[tmpLength + 1 ] << 8 );
	
	if(uintCheck == uintSum)	
	{
		if(Com_Type2 == 0x41)
		{
			i = 3;
			
			guionData.B[0] = RxBuffer2[i++];
			guionData.B[1] = RxBuffer2[i++];
			guionData.B[2] = RxBuffer2[i++];
			guionData.B[3] = RxBuffer2[i++];
			
			Temp_fVal = guionData.f;
			
			guionData.B[0] = RxBuffer2[i++];
			guionData.B[1] = RxBuffer2[i++];
			guionData.B[2] = RxBuffer2[i++];
			guionData.B[3] = RxBuffer2[i++];
			
			Hum_fVal = guionData.f;	
			
			TR_FLAG2 = 0x80;  

		}
		
	}
	
}

/*****************************************************************************************************/


u8 s_cmd1[] = {0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x02,0x80,0x00,0x00,0x82}; 
//u8 s_cmd2[] = {0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x82,0x62,0xC7,0x13,0x97,0xFE,0x03,0x00,0x5E};
u8 s_cmd2[] = {0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x0FF,0x82,0x62,0xC7,0x10,0x41,0xC8,0x03,0x00,0xBD};


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

	uint8_t tmpLength,uintCheck;	
	
	//uint8_t uintSum;
	
	Com_Type2 = RxBuffer2[0]; 

	uintCheck = 0;
	
	tmpLength = Rx_Fra_Len2;

	for(i = 0;i < tmpLength;i++)
	{
		uintCheck ^= RxBuffer2[i];
	}
	
//	uintSum  = RxBuffer2[tmpLength];
	RxBuffer2[tmpLength];
	
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
			printf("gauge_temp = %.2fcm\r\n",gauge_temp[1]*100);
			
//			gauge_high = (float)SETUP_HIGH - gauge_temp[1]*100;
//			printf("gauge_high = %f \r\n",gauge_high);
			
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
					
					//USART_CallBack21();					
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

/*
*********************************************************************************************************
*	函 数 名: bsp_InitOthers
*	功能说明: 初始化. 该函数被 bsp_Init() 调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_InitOthers(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	EXTI_InitTypeDef EXTI_InitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_GRC_DATA, ENABLE);
	
	//用作系统电压检测
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 , ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//格雷码
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_GRC1 | GPIO_PIN_GRC2 |  GPIO_PIN_GRC3 |  GPIO_PIN_GRC4 | GPIO_PIN_GRC5 | GPIO_PIN_GRC6 | GPIO_PIN_GRC7 | GPIO_PIN_GRC8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PORT_GRC_DATA, &GPIO_InitStructure);	

	//雨量和风速
	
	GPIO_InitStructure.GPIO_Pin =GPIO_PIN_RAIN | GPIO_PIN_WS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PORT_GRC_DATA, &GPIO_InitStructure);

	//电压检测
	
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_AD_V;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(PORT_GRC_DATA, &GPIO_InitStructure);
	
	{	//雨量中断	PD6
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6);

		EXTI_InitStructure.EXTI_Line = EXTI_Line6  ;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		 
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);

		EXTI_InitStructure.EXTI_Line = EXTI_Line7  ;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;		 
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);		
		
		
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);		
	}	
	
	//控制管脚初始化	
	{
		RCC_APB2PeriphClockCmd(RCC_Led_Ctrl, ENABLE);
		GPIO_InitStructure.GPIO_Pin   = PIN_Blue_Led | PIN_Yellow_Led | PIN_Orange_Led | PIN_Red_Led | PIN_Call_Led | PIN_Call_Alert;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
		GPIO_Init(PORT_Led_Ctrl , &GPIO_InitStructure);	

		//关闭
		
		GPIO_ResetBits(PORT_Led_Ctrl ,PIN_Blue_Led);
		GPIO_ResetBits(PORT_Led_Ctrl ,PIN_Yellow_Led);
		GPIO_ResetBits(PORT_Led_Ctrl ,PIN_Orange_Led);
		GPIO_ResetBits(PORT_Led_Ctrl ,PIN_Red_Led);
		GPIO_ResetBits(PORT_Led_Ctrl ,PIN_Call_Led);
		GPIO_ResetBits(PORT_Led_Ctrl ,PIN_Call_Alert);
	}
	
	//气温
	JZXX_NO[0] = -50;		//极值下限
	JZSX_NO[0] = 50;		//极值上限
	CYBHL_NO[0]= 5;		    //存疑变化率
	CWBHL_NO[0]= 10;		//错误变化率
	ZXBHL_NO[0]= 0;		    //最小变化率

	//湿度
	JZXX_NO[1] = 0;			
	JZSX_NO[1] = 100;
	CYBHL_NO[1]= 5;
	CWBHL_NO[1]= 10;
	ZXBHL_NO[1]= 0;

	//风向
	JZXX_NO[2] = 0;			
	JZSX_NO[2] = 360;
	CYBHL_NO[2]= 10;
	CWBHL_NO[2]= 20;
	ZXBHL_NO[2]= 0;

	//风速
	JZXX_NO[3] = 0;			
	JZSX_NO[3] = 60;
	CYBHL_NO[3]= 5;
	CWBHL_NO[3]= 10;
	ZXBHL_NO[3]= 0;

	//2分钟风速
	JZXX_NO[4] = 0;			
	JZSX_NO[4] = 60;
	CYBHL_NO[4]= 5;
	CWBHL_NO[4]= 10;
	ZXBHL_NO[4]= 0;

	//10分钟风速
	JZXX_NO[5] = 0;			
	JZSX_NO[5] = 60;
	CYBHL_NO[5]= 5;
	CWBHL_NO[5]= 10;
	ZXBHL_NO[5]= 0;

	//雨量
	JZXX_NO[6] = 0;			
	JZSX_NO[6] = 4;
	CYBHL_NO[6]= 3;
	CWBHL_NO[6]= 4;
	ZXBHL_NO[6]= 0;
}

extern uint8_t com1GetChar(uint8_t *_pByte);

extern void TASK1_CallBack1(u8 delay);
extern void TASK1_CallBack2(void);

extern char gstrStationID[],gstrStationDNO[];	

void Process_ALT_Command(uint8_t *_ucaBuf, uint16_t _usLen);

void Process_DATETIME_Command(uint8_t *_ucaBuf, uint16_t _usLen);

void Process_DNO_Command(uint8_t *_ucaBuf, uint16_t _usLen);

void Process_ID_Command(uint8_t *_ucaBuf, uint16_t _usLen);

void Process_SENST_Command(uint8_t *_ucaBuf, uint16_t _usLen);

void Process_Snapshot_Command(void);

void Process_Reset_Command(void);

static char strIn[40];

static char *cmdItem[20];

static char strOut[Max_Data_Length];

const  char Enter[]			= "\r\n";

const  char Error_Message[] = "BAD COMMAND.\r\n";

const  char Set_Sucess[]    = "<T>\r\n";

const  char Set_Faild[]     = "<F>\r\n";

static char ID[]       = "ID--设置或读取气象观测站的区站号 \r\n命令符：ID\r\n参数：台站区站号（5位数字或字母） \r\n";
static char ALT[]      = "ALT--设置或读取观测场拔海高度 \r\n命令符：ALT \r\n";


void SendTcp_Com1(u8 cmd);

void SendHeart_Com1(void);

extern u8 device_id;

u8 radar_state     = 0x00;      //雷达状态

u8 snap_shot_flag = 0x00;
u8 stop_wdg_flag  = 0x00;

void Process_Host_Message(void)  
{	
	uint8_t ucData,error;

	uint8_t ucGprsFrame = 0,Frame_End_Pos;	
	uint8_t usGprsPos   = 0;	
	uint8_t rcFlag      = 1;	

	unsigned int sentCount;
	
	unsigned char now_value,pre_value=0x0ff;

	uint8_t gblnComm_Success = 0x00;
	//uint16_t Send_Timer = 0x0000,i;

	static uint8_t ucaGprsBuf[254];
	
	while(1)
	{
		if (com1GetChar(&ucData))
		{
			ucaGprsBuf[usGprsPos++] = ucData;

			while(rcFlag && com1GetChar(&ucData))
			{
				if (usGprsPos < sizeof(ucaGprsBuf))
				{
					ucaGprsBuf[usGprsPos++] = ucData;

					if ((ucData == '\r') || (ucData == '\n') || (ucData == '/'))
					{
						ucGprsFrame = 0x0ff;
					
						Frame_End_Pos = usGprsPos-1;
						ucaGprsBuf[Frame_End_Pos] = ' ';
					
						if(ucData == '\n')
						{
								rcFlag = 0x00;
						}
					}
				}

				TASK1_CallBack2();				
			}
			
			if(rcFlag) 
			{
				TASK1_CallBack1(10);
					
				while(rcFlag && com1GetChar(&ucData))
				{
					if (usGprsPos < sizeof(ucaGprsBuf))
					{
						ucaGprsBuf[usGprsPos++] = ucData;

						if ((ucData == '\r') || (ucData == '\n') || (ucData == '/'))
						{
							ucGprsFrame = 0x0ff;
										
							Frame_End_Pos = usGprsPos-1;
							ucaGprsBuf[Frame_End_Pos] = ' ';
										
							if(ucData == '\n')
							{
								rcFlag = 0x00;
							}
						}
					}

					TASK1_CallBack2();
				}					
			}
			
			if(ucGprsFrame == 0)
			{
				memcpy(strOut, Error_Message, strlen(Error_Message));
				gblnComm_Success = 0x00;
			}
			else
			{
				ucaGprsBuf[usGprsPos] = 0x00;
				gblnComm_Success      = 0x0ff;
			}
			
			rcFlag      = 1;
			ucGprsFrame = 0;	
			usGprsPos   = 0;
		}
				
		TASK1_CallBack2();
		
		
		
		if(gblnComm_Success)
		{
			gblnComm_Success = 0x00;
		
			error  = 0x00;

			strcpy(strOut, "");
			strcpy(strIn, "");
		
			ucData = ucaGprsBuf[0];
			
			switch(ucData) 		
			{	
				case 'A':
					if (memcmp(ucaGprsBuf, "ALT ", 4) == 0)
					{
						Process_ALT_Command(ucaGprsBuf,Frame_End_Pos);
					}
					else
						error++;

					break;

				case 'D':
					if (memcmp(ucaGprsBuf, "DATETIME ", 9) == 0)
					{
						Process_DATETIME_Command(ucaGprsBuf,Frame_End_Pos);
					}	
					else if (memcmp(ucaGprsBuf, "DNO ", 4) == 0)
					{
						Process_DNO_Command(ucaGprsBuf,Frame_End_Pos);
					}												
					else
						error++;

					break;

				case 'I':
					if (memcmp(ucaGprsBuf, "ID ", 3) == 0)
					{
						Process_ID_Command(ucaGprsBuf,Frame_End_Pos);
					}		
					else
						error++;

					break;

				case 'R':
					if (memcmp(ucaGprsBuf, "RESET ", 6) == 0)
					{
						Process_Reset_Command();
					}							
					else
						error++;

					break;

				case 'S':
					if (memcmp(ucaGprsBuf, "SENST ", 6) == 0)
					{
						Process_SENST_Command(ucaGprsBuf,Frame_End_Pos);
					}	
					else if (memcmp(ucaGprsBuf, "SNAPSHOT ", 9) == 0)
					{
						Process_Snapshot_Command();
					}						
					else
						error++;

					break;

				default :
					error++;

					break;							

			}	

			sentCount = strlen(strOut);
			
			if(sentCount >0)
			{		
				comSendBuf(COM1,(uint8_t *)strOut,sentCount);
				strcpy(strOut, "");
			}				
		}
		
		RTC_ReadClock();
		
		now_value = g_tRTC.Min;
		
		if((g_tRTC.Sec >=10)&&(pre_value != now_value))
		{
			pre_value = now_value;		
		
#if SEA_TIDE == 1	
			if((g_tRTC.Min%1)  == 0)//timer1_1s >= 60)
			{	

			
#else		
				if((g_tRTC.Min%1) == 0)//timer1_1s >= 600)
				{
#endif			 
					timer1_1s =0; 
					SendTcp_Com1(0);
				}
			}
			
		if(timer1_1s_1>=180)
		{
			timer1_1s_1 =0; 
			SendHeart_Com1();
		}

		if(snap_shot_flag)
		{
			snap_shot_flag = 0x00;
			
			SendTcp_Com1(1);	
		}
					
		TASK1_CallBack1(10);
		
		if(read_radar_count >= 20)
			radar_state = 0x05;
		else
		if(read_radar_count >= 10)
			radar_state = 0x02;
		else
			radar_state = 0x00;
	}
}

void Process_DATETIME_Command(uint8_t *_ucaBuf, uint16_t _usLen)
{
	//uint8_t temp8,len=0,strCount = 0;		
	uint8_t strCount = 0;
	uint8_t i,result;
	
	char *p;

	p = (char *)_ucaBuf;
	p[_usLen] = 0;
	
	if( strstr(p,"?") != NULL)
	{
		return;
	}
	else
	{
		for(i=0;i<20;i++)
		{			
			if(i == 0)
					cmdItem[0] = strtok(p, " "); 
			else
					cmdItem[i] = strtok(NULL, " ");

			if(cmdItem[i] == NULL)
				break;
			
			strCount++; 
		}
		
		if(strCount >= 3)   
		{
			strcat(strIn,cmdItem[1]);
			strcat(strIn," ");
			strcat(strIn,cmdItem[2]);

			g_tHost.Year   =   atoi(strIn     ); 
			g_tHost.Month  =   atoi(strIn + 5 ); 
			g_tHost.Day    =   atoi(strIn + 8 );         
			g_tHost.Hour   =   atoi(strIn + 11); 
			g_tHost.Minute =   atoi(strIn + 14); 
			g_tHost.Second =   atoi(strIn + 17); 
		
			if((g_tHost.Year>1999) && (g_tHost.Month>0 && g_tHost.Month<13 ) &&(g_tHost.Day>0 && g_tHost.Day<=31)&&
			(g_tHost.Hour<24)&&(g_tHost.Minute<60)&&(g_tHost.Second<60))
			{
				result = RTC_WriteClock(g_tHost.Year,g_tHost.Month,g_tHost.Day,g_tHost.Hour,g_tHost.Minute,g_tHost.Second);//RTC_DateTime_Set(&g_tHost);
			
				if( result )
				{
					sprintf(strOut,"<17,B0002,%02d,%s,T,N>\r\n",device_id,cmdItem[0]);
				}
				else
				{ 
					sprintf(strOut,"<17,B0002,%02d,%s,F,N>\r\n",device_id,cmdItem[0]);
				}
			}
			else
			{
				sprintf(strOut,"<17,B0002,%02d,%s,BAD COMMAND.,N>\r\n",device_id,cmdItem[0]);
			}
		}
		else   
		{
			RTC_ReadClock();	
		
			sprintf(strOut,"<17,B0002,%02d,%s,%04d-%02d-%02d %02d:%02d:%02d,N>\r\n",device_id,cmdItem[0],g_tRTC.Year,g_tRTC.Mon,g_tRTC.Day,g_tRTC.Hour,g_tRTC.Min,g_tRTC.Sec);
		}
	}
}

void Process_ID_Command(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint8_t len,strCount = 0;		
	uint8_t i;

	char chTemp;

	uint8_t para_validate = 0x00;
	
	char *p;

	p = (char *)_ucaBuf;
	p[_usLen] = 0;
	
	if( strstr(p,"?") != NULL)
	{
		strcpy(strOut,ID);
	}
	else
	{
		for(i=0;i<20;i++)
		{			
			if(i == 0)
				cmdItem[0] = strtok(p, " "); 
			else
				cmdItem[i] = strtok(NULL, " ");

			if(cmdItem[i] == NULL)
				break;
					
			strCount++; 
		}
			
		if(strCount > 1)   
		{
			if(strCount == 2)
			{
				strcpy(strIn,cmdItem[1]);
				len = strlen(strIn);
							
				if(len == 5)
				{
					for(i=0;i< len;i++)
					{
						chTemp = strIn[i];
											
						if((chTemp>=0x30 && chTemp<=0x39)||(chTemp>=0x41 && chTemp<=0x5A)||(chTemp>=0x61 && chTemp<=0x7A))
							para_validate = 0x0ff;
						else
						{  
							para_validate = 0x00;
							break;
						}
					}
									
					if( para_validate )
					{
						strcpy(cjq_pz_ptr->Station_ID,strIn);
						
						cjq_pz_ptr->init_flag = 0x0AA55;					

						strcpy(gstrStationID,cjq_pz_ptr->Station_ID);											

						sprintf(strOut,"<17,%s,%02d,%s,T,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
					}											
					else
					{
						sprintf(strOut,"<17,%s,%02d,%s,F,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
						return;
					}	
				}
			}
			else
			{
				sprintf(strOut,"<17,%s,%02d,%s,F,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
				return;
			}
		}
		else
		{
			sprintf(strOut,"<17,%s,%02d,%s,%s,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0],cjq_pz_ptr->Station_ID);
		}
	}
}

static char DNO[]      = "DNO--设置或读取设备序号\r\n命令符：DNO\r\n";

void Process_DNO_Command(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint8_t len,strCount = 0;		
	uint8_t i;

   char chTemp;

	uint8_t para_validate = 0x00;
	
	char *p;

	p = (char *)_ucaBuf;
	p[_usLen] = 0;
	
	if( strstr(p,"?") != NULL)
	{
			strcpy(strOut,DNO);
	}
	else
	{
		for(i=0;i<20;i++)
		{			
			if(i == 0)
					cmdItem[0] = strtok(p, " "); 
			else
					cmdItem[i] = strtok(NULL, " ");

			if(cmdItem[i] == NULL)
				break;
			
			strCount++; 
		}
			
		if(strCount > 1)   
		{
			if(strCount == 2)
			{
				strcpy(strIn,cmdItem[1]);
				len = strlen(strIn);
						
				if(len == 2)  //2018.9.26 改为2byte
				{
					for(i=0;i< len;i++)
					{
						chTemp = strIn[i];
						
						if(chTemp >= 0x30 && chTemp <= 0x39)
							para_validate = 0x0ff;
						else
						{  
							para_validate = 0x00;
							break;
						}
					}
								
					if( para_validate )
					{
						strcpy(cjq_pz_ptr->Station_DNO,strIn);
						
						device_id = atoi(strIn);
						
						cjq_pz_ptr->init_flag = 0x0AA55;					

						strcpy(gstrStationDNO,cjq_pz_ptr->Station_DNO);											

						sprintf(strOut,"<17,%s,%02d,%s,T,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
					}											
					else
					{
						sprintf(strOut,"<17,%s,%02d,%s,F,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
						return;
					}	
				}
			}
			else
			{
				sprintf(strOut,"<17,%s,%02d,%s,F,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
				return;
			}
		}
		else
		{
			sprintf(strOut,"<17,%s,%02d,%s,%s,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0],cjq_pz_ptr->Station_DNO);
		}
	}
}


void Process_Reset_Command(void)
{
		stop_wdg_flag = 0x01;
}

void Process_Snapshot_Command(void)
{
	snap_shot_flag = 0x01;
}

float alt_value;

void Process_ALT_Command(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint8_t strCount = 0, i;		

	//uint8_t len;
	
	char *p;
	
	float ftemp;
	
	p = (char *)_ucaBuf;
	p[_usLen] = 0;

	if( strstr(p,"?") != NULL)
	{
		strcpy(strOut,ALT);
	}	
	else
	{
		for(i=0;i<20;i++)
		{			
			if(i == 0)
				cmdItem[0] = strtok(p, " "); 
			else
				cmdItem[i] = strtok(NULL, " ");

			if(cmdItem[i] == NULL)
				break;
			
			strCount++; 
		}		

		if(strCount == 2)
		{
			strcpy(strIn,cmdItem[1]);
			//len = ;		
			strlen(strIn);
			ftemp = atof(strIn);
			
			alt_value = ftemp;
			
			sprintf(strOut,"<17,%s,%02d,%s,T,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);

		}
		else 
		if(strCount == 1)			
		{
			sprintf(strOut,"<17,%s,%02d,%s,%4.1f,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0],alt_value);
		}		
		else   
		{
			sprintf(strOut,"<17,%s,%02d,%s,F,N>\r\n",cjq_pz_ptr->Station_ID,device_id,cmdItem[0]);
		}
	}	
}

static char SENST[]    = "SENST--设置或读取采传感器状态 \r\n命令符：SENST XXX \r\n";

int32_t StringToInt(char *_pStr)
{
 	uint8_t flag;
 	char *p;
 	uint32_t ulInt;
 	uint8_t i;
 	uint8_t ucTemp;

 	p = _pStr;
 	if (*p == '-')
 	{
 		flag = 1;	/* 负数 */
 		p++;
 	}
 	else
 	{
 		flag = 0;
 	}

 	ulInt = 0;
 	for (i = 0; i < 15; i++)
 	{
 		ucTemp = *p;
 		if (ucTemp == '.')	/* 遇到小数点，自动跳过1个字节 */
 		{
 			p++;
 			ucTemp = *p;
 		}
 		if ((ucTemp >= '0') && (ucTemp <= '9'))
 		{
 			ulInt = ulInt * 10 + (ucTemp - '0');
 			p++;
 		}
 		else
 		{
 			break;
 		}
 	}

 	if (flag == 1)
 	{
 		return -ulInt;
 	}
 	return ulInt;
}

u8 sensor_state[12] ={1,1,1,1,1,1,1,1,1,1,1,1};

void Process_SENST_Command(uint8_t *_ucaBuf, uint16_t _usLen)
{
	uint8_t len,strCount = 0;		
	uint8_t i,ch;
	
	char *p;

	p = (char *)_ucaBuf;
	p[_usLen] = 0;
	
	if( strstr(p,"?") != NULL)
	{
		strcpy(strOut,SENST);
	}
	else
	{
		for(i=0;i<20;i++)
		{			
			if(i == 0)
				cmdItem[0] = strtok(p, " "); 
			else
				cmdItem[i] = strtok(NULL, " ");

			if(cmdItem[i] == NULL)
				break;
			
			strCount++; 
		}
	
		if(strCount > 1)  
		{
			if(strCount == 3)
			{
				strcpy(strIn,cmdItem[1]);
				len = strlen(strIn);

				if(!strncmp(strIn,"100",len))
				{
					strcpy(strIn,cmdItem[2]);
					ch = StringToInt(strIn);
					
					if(ch < 0x0a) sensor_state[0] = ch; 
				}
				else
				if(!strncmp(strIn,"101",len))
				{
					strcpy(strIn,cmdItem[2]);

					ch = StringToInt(strIn);
					
					if(ch < 0x03) sensor_state[1] = ch; 					
				}
				else
				if(!strncmp(strIn,"102",len))
				{
					strcpy(strIn,cmdItem[2]);

					ch = StringToInt(strIn);
					
					if(ch < 0x03) sensor_state[2] = ch; 					
				}
				else
				if(!strncmp(strIn,"103",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[3] = StringToInt(strIn);
				}
				else
				if(!strncmp(strIn,"104",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[4] = StringToInt(strIn);
				}
				else
				if(!strncmp(strIn,"105",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[5] = StringToInt(strIn);
				}
				else
				if(!strncmp(strIn,"106",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[6] = StringToInt(strIn);
				}
				else
				if(!strncmp(strIn,"107",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[7] = StringToInt(strIn);
				}
				else			
				if(!strncmp(strIn,"108",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[8] = StringToInt(strIn);
				}
				else
				if(!strncmp(strIn,"109",len))
				{
					strcpy(strIn,cmdItem[2]);
					sensor_state[9] = StringToInt(strIn);
				}
				else
				{
					strcpy(strOut,Set_Faild); 
					return;
				}

				strcpy(strOut,Set_Sucess);
				return;
			}
			else            
			if(strCount == 2)
			{
				strcpy(strIn,cmdItem[1]);
				len = strlen(strIn);
				
				if(!strncmp(strIn,"100",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[0]);
				}
				else
				if(!strncmp(strIn,"101",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[1]);
				}
				else
				if(!strncmp(strIn,"102",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[2]);
				}
				else
				if(!strncmp(strIn,"103",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[3]);
				}
				else
				if(!strncmp(strIn,"104",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[4]);
				}
				else
				if(!strncmp(strIn,"105",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[5]);
				}
				else
				if(!strncmp(strIn,"106",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[6]);
				}
				else
				if(!strncmp(strIn,"107",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[7]);
				}
				else
				if(!strncmp(strIn,"108",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[8]);
				}
				else
				if(!strncmp(strIn,"109",len))
				{
					sprintf(strOut,"<%01d>\r\n",sensor_state[9]);
				}
				else
				{
				   strcpy(strOut,Error_Message); 
					return;
				}
			}
			else
			{
				strcpy(strOut,Error_Message); 
			}
		}	
	}

}


extern float gauge_temp[];

extern float gauge_high;

u8 device_id = 0x51;  			//设备id

u8 element_counter = 0x01;		//
	
u8 state_counter   = 0x01;		//

unsigned int sent_checksum;		//


void SendTcp_Com1(u8 cmd)
{
	u8  temp8 = 0,ch;
	u16 j,len = 0;
	
	u8  sent_type = cmd;		//发送类型：00 自动 01 手动
   
	temp8=sprintf(strOut,"BG,B0002,17,%02d,%02d",sent_type,device_id);
	len=len+temp8;
	
	temp8=sprintf(&strOut[len],",%04d%02d%02d%02d%02d%02d",g_tRTC.Year,g_tRTC.Mon,g_tRTC.Day,g_tRTC.Hour,g_tRTC.Min,g_tRTC.Sec);
	len=len+temp8;	
	
	temp8=sprintf(&strOut[len],",%03d,%02d",element_counter,state_counter);
	len=len+temp8;	
	
	//海潮
	temp8=sprintf(&strOut[len],",YS01,%5.1f",gauge_temp[1]*1000);
	len=len+temp8;

	temp8=sprintf(&strOut[len],",ST20,%d",radar_state);
	len=len+temp8;
	
	sent_checksum = 0;
		
	for(j=0;j<len;j++)
	{
		ch = strOut[j];
		sent_checksum += ch;
	}

	temp8=sprintf(&strOut[len],",%04d,ED\r\n",sent_checksum%10000);
	len=len+temp8;	
	comSendBuf(COM3,(uint8_t *)strOut,len);
	comSendBuf(COM1,(uint8_t *)strOut,len);
}


//<17,B0001,09>\r\n
//<业务号,台站号,设备号>\r\n

void SendHeart_Com1(void)
{
	u8  temp8 = 0;
	u16 len = 0;
	
	temp8=sprintf(strOut,"<17,B0002,%02d>\r\n",device_id);
	len=len+temp8;
	
	comSendBuf(COM1,(uint8_t *)strOut,len);
}



/************************************** END OF FILE ************************************/
