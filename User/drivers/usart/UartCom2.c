/*********************************************************************************************************
* �ļ�����: UartCom2.c
* �ļ�����: �Ժ�һ��
* ��ǰ�汾: 01.00.00
* ����ģ��: Uart
* ��������: 2017��01��20��
* �ļ�ժҪ: 
* ע������: 
* ����˵��: ��ʼ����
**********************************************************************************************************/

/*********************************************************************************************************
*                                              ����ͷ�ļ�
*********************************************************************************************************/
#include "UartCom2.h"
#include "Uart.h"
#include "stdio.h"
#include <string.h>
/*********************************************************************************************************
*                                              ��������
*********************************************************************************************************/
static u8 sCom2TxDmaBuf[UART_SEND_BUF];		//����2����DMA������
static u8 sCom2RxBuf[UART_RECV_BUF];		//����2����DMA������
static u8 sCom2SendFlag = 0;		//����2���ͱ�־��1:���ڷ��ͣ�0���������
static u8 sCom2SendLen = 0;		//����2���ͳ���
static u8 sCom2ReadFlag = 0;		//����2���ձ�־��1:���ڽ��գ�0���޽���

/*********************************************************************************************************
*                                              ��̬��������
*********************************************************************************************************/
static void InitCom2Pin( void );
static void InitCom2Nvic( void );
static void InitCom2Dma( void );
static void InitCom2Para( void );

/*********************************************************************************************************
* ��������: InitCom2Con
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
void InitCom2Con( void )
{
	InitCom2Pin();
	InitCom2Nvic();
	InitCom2Dma();
	InitCom2Para();
}

/*********************************************************************************************************
* ��������: printf2
* ��������: ����2��ʽ�����
* ��������: 2017��01��26��
* �������: 
* �������: 
* �� �� ֵ: ��
* ע������: 
**********************************************************************************************************/
void printf2(const char* fmt,...)
{
	char data[200];

	va_list vaList;
	va_start(vaList, fmt);
	vsnprintf(data, 200, fmt, vaList);
	va_end(vaList);

	SendCom2Data((u8*)data, strlen(data));
}
/*********************************************************************************************************
* ��������: SendCom2Data
* ��������: д����
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: ��д����ֽ���
* ע������: 
**********************************************************************************************************/
u8 SendCom2Data( u8 *pBuf, u8 len )
{
	u8 i,tmp,cnt = 0;

	if(1 == sCom2SendFlag)
	{
		DMA_Cmd(DMA1_Channel7, DISABLE );  //�ر�DMAͨ��4
		tmp = DMA_GetCurrDataCounter(DMA1_Channel7);//��ȡ��δ������ɵ��ֽ���
		cnt = sCom2SendLen - tmp;//�Ѿ�������ɵ��ֽ���
		for(i = 0 ; i < tmp; i++)
		{
			sCom2TxDmaBuf[i] = sCom2TxDmaBuf[i+cnt];//���ƻ�δ������ɵ��ֽ�����������ǰ��
		}
		if(len+tmp > UART_SEND_BUF)
		{
			len = UART_SEND_BUF - tmp;
		}
		for(i = 0 ; i < len; i++)
		{
			sCom2TxDmaBuf[i+tmp] = pBuf[i];
		}
	
		cnt = len;
		sCom2SendLen = len + tmp;
		DMA1_Channel7->CNDTR = len + tmp;
		sCom2SendFlag = 1;
		DMA_Cmd(DMA1_Channel7, ENABLE);  
	}
	else
	{
		DMA_Cmd(DMA1_Channel7, DISABLE );
		if(len > UART_SEND_BUF)
		{
			len = UART_SEND_BUF;
		}
		for (i = 0 ; i < len; i ++) 
		{
			sCom2TxDmaBuf[i] = pBuf[i];
		}
	
		cnt = len;
		sCom2SendLen = len;
		DMA1_Channel7->CNDTR = len;
		sCom2SendFlag = 1;
		DMA_Cmd(DMA1_Channel7, ENABLE);
	}
	
	return cnt;
}


/*********************************************************************************************************
* ��������: ReadCom2Data
* ��������: ������
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: �������ֽ���
* ע������: 
**********************************************************************************************************/
u8 ReadCom2Data( u8 *pBuf )
{
	u8 i,cnt = 0;
	
	if (sCom2ReadFlag == 1)
	{
		cnt = UART_RECV_BUF - DMA_GetCurrDataCounter(DMA1_Channel6);
		if(cnt > 0)
		{
			DMA_Cmd(DMA1_Channel6, DISABLE );
	
			for(i = 0 ; i < cnt;i++)
			{
				pBuf[i] = sCom2RxBuf[i];
			}
	
			sCom2ReadFlag = 0;
			DMA1_Channel6->CNDTR = UART_RECV_BUF;//���ô������ݳ���
			DMA_Cmd(DMA1_Channel6, ENABLE );
		}
	}
	
	return cnt;
}


/*********************************************************************************************************
* ��������: USART2_IRQHandler
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
void USART2_IRQHandler( void )
{
	u8 tmp = 0;

	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		tmp = USART2->SR;
		tmp = USART2->DR; //��USART_IT_IDLE��־
		sCom2ReadFlag = 1;
	}
}


/*********************************************************************************************************
* ��������: DMA1_Channel7_IRQHandler
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
void DMA1_Channel7_IRQHandler( void )
{

	if(DMA_GetFlagStatus(DMA1_FLAG_TC7) == SET)
	{
		sCom2SendFlag = 0;
		DMA_ClearFlag(DMA1_FLAG_TC7);
		DMA_Cmd(DMA1_Channel7, DISABLE );
	}
}


/*********************************************************************************************************
* ��������: DMA1_Channel6_IRQHandler
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
void DMA1_Channel6_IRQHandler( void )
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC6) == SET)
	{
		sCom2ReadFlag = 1;
		DMA_ClearFlag(DMA1_FLAG_TC6);
	}

}


/*********************************************************************************************************
* ��������: InitCom2Pin
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
static void InitCom2Pin( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE );			//����GPIOD��ʱ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_AFIO, ENABLE );			//����AFIO��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);		//����USART2��ʱ��
	GPIO_PinRemapConfig( GPIO_Remap_USART2, ENABLE );			//������ӳ��ʹ��

	//����USART2��GPIO��USART2 Tx (PD.05)/USART2 Rx (PD.06)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOD, &GPIO_InitStructure );                            //����GPIOD5
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOD, &GPIO_InitStructure );                            //����GPIOD6
}


/*********************************************************************************************************
* ��������: InitCom2Nvic
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
static void InitCom2Nvic( void )
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel= DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel= DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=3;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*********************************************************************************************************
* ��������: InitCom2Dma
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
static void InitCom2Dma( void )
{
	DMA_InitTypeDef    DMA_Initstructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);
	DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)sCom2RxBuf;
	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_Initstructure.DMA_BufferSize = UART_RECV_BUF;
	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;
	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
	DMA_Initstructure.DMA_Priority = DMA_Priority_High;
	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6,&DMA_Initstructure);

	DMA_Cmd(DMA1_Channel6,ENABLE);
	DMA_ITConfig(DMA1_Channel6,DMA_IT_TC,ENABLE);

	DMA_DeInit(DMA1_Channel7); 
	DMA_Initstructure.DMA_PeripheralBaseAddr =  (u32)(&USART2->DR);
	DMA_Initstructure.DMA_MemoryBaseAddr     = (u32)sCom2TxDmaBuf;
	DMA_Initstructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_Initstructure.DMA_BufferSize = UART_SEND_BUF;
	DMA_Initstructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Initstructure.DMA_MemoryInc =DMA_MemoryInc_Enable;
	DMA_Initstructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_Initstructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_Initstructure.DMA_Mode = DMA_Mode_Normal;
	DMA_Initstructure.DMA_Priority = DMA_Priority_High;
	DMA_Initstructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel7,&DMA_Initstructure);

	DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel7,ENABLE);
}


/*********************************************************************************************************
* ��������: InitCom2Para
* ��������: 
* ��������: 2017��01��20��
* �������: 
* �������: 
* �� �� ֵ: 
* ע������: 
**********************************************************************************************************/
static void InitCom2Para( void )
{
	USART_InitTypeDef USART_InitStructure;

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_Init(USART2,&USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	USART_Cmd( USART2, ENABLE );
}

