#include "led.h"
/**********************************************************************
*														变量定义
**********************************************************************/

/*********************************************************************
*	函数名：初始化LED配置
* 参数：	无
* 返回值：无
**********************************************************************/
void InitLedCon(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                                   //定义一个IO端口参数结构体
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD , ENABLE);  			   //使能PD端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 ;//初始化PD12.13.14端口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                      //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   					   //推免输出方式
	GPIO_Init(GPIOD, &GPIO_InitStructure);            					   //初始化PD12.13.14
	
	DisableAllLed();
}

/*-------------------------------------------------*/
/*函数名：点亮所有LED                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void EnableAllLed(void)
{
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}
/*-------------------------------------------------*/
/*函数名：熄灭所有LED                              */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DisableAllLed(void)
{
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

/******************************************************
*函数名：点亮某个LED
*参数：	 哪一个
*返回值：无
********************************************************/
void EnableSingleLed(u8 which)
{
	switch(which)
	{
		case 0:
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
			break;
		case 1:
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			break;
		case 2:
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			break;
		default:
			break;
	}
}

/******************************************************
*函数名：熄灭某个LED
*参数：	 哪一个
*返回值：无
********************************************************/
void DisableSingleLed(u8 which)
{
	switch(which)
	{
		case 0:
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
			break;
		case 1:
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
			break;
		case 2:
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
			break;
		default:
			break;
	}
}
