/*********************************************************************************************************
* �ļ�����: Timer3.c
* �ļ�����: �Ժ�һ��
* ��ǰ�汾: 01.00.00
* ����ģ��: Timer
* ��������: 2017��01��20��
* �ļ�ժҪ: 
* ע������: 
* ����˵��: ��ʼ����
**********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "Timer3.h"
#include "Uart.h"
#include "dp83848.h"
#include "etharp.h"
#include "tcpserver.h"
/*********************************************************************************************************
*                                              ��̬��������
*********************************************************************************************************/
static void InitTimer3Mode( void );
static void InitTimer3Nvic( void );

/*********************************************************************************************************
*																								��������
**********************************************************************************************************/

/*********************************************************************************************************
* ��������: InitTimer3Con
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
void InitTimer3Con( void )
{
	InitTimer3Mode();
	InitTimer3Nvic();
}


/*********************************************************************************************************
* ��������: TIM3_IRQHandler
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
void TIM3_IRQHandler( void )
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//Lwip��ѯ
		u32 LwipTime = getLwipTime();
		LwipTime += 10;
		setLwipTime(LwipTime);
		//TCP��ѯ
		#if LWIP_TCP
			addTcpTimeIndex(10);
			if (getTcpTimeIndex() >= 250)
			{
				setTcpTimeIndex(0);
				setTcpPeriodFlag(1);
			}
		#endif
		//Arp��ѯ
		addArpTimeIndex(10);
		if(getArpTimeIndex() >= ARP_TMR_INTERVAL)
		{
			setArpTimeIndex(0);
			setArpPeriodFlag(1);
		}
		//���Ӽ����ѯ
		addCheckConnTimeIndex(10);
		if (getCheckConnTimeIndex() >= CHECK_FOR_CONNECTION_PERIOD)
		{
			setCheckConnTimeIndex(0);
			setCheckConnFlag(1);
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update );
	}
}


/*********************************************************************************************************
* ��������: InitTimer3Mode
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
static void InitTimer3Mode( void )
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE );
	TIM_Cmd(TIM3, ENABLE);
}


/*********************************************************************************************************
* ��������: InitTimer3Nvic
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
static void InitTimer3Nvic( void )
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStructure);
}

