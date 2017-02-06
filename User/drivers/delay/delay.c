#include "delay.h"

/*-------------------------------------------------*/
/*����������ʼ���ӳٺ���                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void delay_init(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	//SysTick���δ�ʱ�ӣ���Ƶ������Ƶ�ʵ�1/8
	//ͨ����Ƶ��Ϊ����72M����ôSysTick��Ƶ����9M
	//��ôSysTick������19����������(1/9)us
}

/*-------------------------------------------------*/
/*���������ӳ�΢�뺯��                             */
/*��  ����us����ʱ����΢��                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void delay_us(u32 us)
{		
	u32 temp;	    	 
	SysTick->LOAD=us*9;                       //SysTick����ֵ��delay_init����������֪���� SysTick������һ������1/9΢�룬���Գ���9
	SysTick->VAL=0x00;                        //��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //��ʼ���� 
	do
	{
		temp=SysTick->CTRL;                        //��ȡSysTick״̬�Ĵ���
	}
	while((temp&0x01)&&!(temp&(1<<16)));           //�ȴ�ʱ�䵽���־λ��λ   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
	SysTick->VAL =0X00;                            //��ռ�����	 
}

/*-------------------------------------------------*/
/*���������ӳٺ��뺯��                             */
/*��  ����ms����ʱ���ٺ���                         */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void delay_ms(u32 ms)
{
	//��������ע��һ������SysTickʱ�Ӽ�������24λ�ģ�9MƵ���£��ܹ�����ʱ1864.135ms
	//����������1800Ϊ���ޣ�С��1800����ʱһ�μ������У�����1800�Ķ�μ���	
	u32 i;
	u32 temp;
	
	if(ms<1800)   //С��1800ms
	{
		SysTick->LOAD=(u32)ms*9*1000;             //ʱ�����,SysTickһ����ʹ1/9΢�룬�����ms�Ļ�������9�ٳ���1000
		SysTick->VAL =0x00;                       //��ռ�����
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //��ʼ����  
		do
		{
			temp=SysTick->CTRL;                        //��ȡSysTick״̬�Ĵ���
		}
		while((temp&0x01)&&!(temp&(1<<16)));           //�ȴ�ʱ�䵽���־λ��λ   
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
		SysTick->VAL =0X00;                            //��ռ�����	 
	}
	else   //����1800ms
	{
		for(i=0;i<(ms/1800);i++)    //����1800�������֣�forѭ������ʱ
		{
			SysTick->LOAD=1800*9*1000;                //ʱ�����,1800ms��ʱ
			SysTick->VAL =0x00;                       //��ռ�����
			SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //��ʼ����  
			do
			{
				temp=SysTick->CTRL;                        //��ȡSysTick״̬�Ĵ���
			}
			while((temp&0x01)&&!(temp&(1<<16)));           //�ȴ�ʱ�䵽���־λ��λ   
			SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
			SysTick->VAL =0X00;                            //��ռ�����	 
		}
		//�������������ֵ���ʱ
		SysTick->LOAD=(u32)(ms%1800)*9*1000;     //ʱ�����,��������
		SysTick->VAL =0x00;                       //��ռ�����
		SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;  //��ʼ����  
		do
		{
		temp=SysTick->CTRL;                       //��ȡSysTick״̬�Ĵ���
		}
		while((temp&0x01)&&!(temp&(1<<16)));           //�ȴ�ʱ�䵽���־λ��λ   
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;       //�رռ�����
		SysTick->VAL =0X00;                            //��ռ�����	
	}
} 

