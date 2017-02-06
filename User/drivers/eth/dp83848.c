#include <string.h>
#include "dp83848.h"
#include "Uart.h"
#include "mem.h"
#include "netif.h"
#include "memp.h"
#include "init.h"
#include "etharp.h"
#include "tcp_impl.h"
#include "tcpserver.h"
#include "led.h"
#include "UartCom2.h"

/********************************************************************************************************
*																								�궨��
**********************************************************************************************************/
#define MAC_LEN 6
#define IP_LEN  4
/*********************************************************************************************************
*																								��������
**********************************************************************************************************/
static u32 LwipTime = 0;                 //LWip����ʱ��

struct netif  DP83848_netif;    //����һ��ȫ�ֵ�����ӿڣ�ע����������Ҫ�õ�

u32 getLwipTime(void)
{
	return LwipTime;
}
void setLwipTime(u32 lt)
{
	LwipTime = lt;
}

static u8 MacAddr[MAC_LEN]={0,0,0,0,0,1};  //MAC��ַ������Ψһ��

u8* getMacAddr(void)
{
	return MacAddr;
}

void setMacAddr(u8 ma[])
{
	int i;
	for(i = 0; i < MAC_LEN; i ++)
	{
		MacAddr[i] = ma[i];
	}
}

static char hostName[100] = "LwipTestBoard";

char* getHostName(void)
{
	return hostName;
}

void setHostName(char *hn)
{
	strcpy(hostName, hn);
}

/*---------��̬��IP��ַ---------*/
static unsigned char ipAddr[IP_LEN] = {192, 168, 0, 200};

unsigned char* getIpAddr(void)
{
	return ipAddr;
}
void setIpAddr(unsigned char *ia)
{
	int i;
	for(i = 0; i < IP_LEN; i++)
	{
		ipAddr[i] = ia[i];
	}
}
/*----------��������------------*/
static unsigned char subnetMask[IP_LEN] = {192, 168, 0, 1};

unsigned char* getSubnetMask(void)
{
	return subnetMask;
}
void setSubnetMask(unsigned char* sm)
{
	int i;
	for (i = 0; i < IP_LEN; i++)
	{
		subnetMask[i] = sm[i];
	}
}
/*------------����-------------*/
static unsigned char gateway[IP_LEN] = {255, 255, 255, 0};

unsigned char* getGateway(void)
{
	return gateway;
}
void setGateway(unsigned char* gw)
{
	int i;
	for (i = 0; i < IP_LEN; i++)
	{
		gateway[i] = gw[i];
	}
}
/************���ض˿�***********/
static u16 tcpLocalPort = 5000;

u16 getTcpLocalPort(void)
{
	return tcpLocalPort;
}
void setTcpLocalPort(u16 tlp)
{
	tcpLocalPort = tlp;
}

/************������ip��ַ********/
static u8 serverIpAddr[IP_LEN] = {192,168,0,60};

u8* getServerIpAddr(void)
{
	return serverIpAddr;
}
void setServerIpAddr(u8 *sia)
{
	int i;
	for (i = 0; i < IP_LEN; i++)
	{
		serverIpAddr[i] = sia[i];
	}
}

/*********�������˿ں�**********/
static u16 serverPort = 8080; 		//��Χ��0-9999

u16 getServerPort(void)
{
	return serverPort;
}
void setServerPort(u16 sp)
{
	serverPort = sp;
}

static u32 ArpTimeIndex = 0;						 //ARP��ѯ��ʱ��

u32 getArpTimeIndex(void)
{
	return ArpTimeIndex;
}
void setArpTimeIndex(u32 ati)
{
	ArpTimeIndex = ati;
}
void addArpTimeIndex(u32 addIndex)
{
	ArpTimeIndex += addIndex;
}

static u32 TcpTimeIndex = 0;						 //Tcp��ѯ��ʱ��

u32 getTcpTimeIndex(void)
{
	return TcpTimeIndex;
}
void setTcpTimeIndex(u32 tti)
{
	TcpTimeIndex = tti;
}
void addTcpTimeIndex(u32 addIndex)
{
	TcpTimeIndex += addIndex;
}

static u32 CheckConnTimeIndex = 0;						//���Ӽ���ʱ

u32 getCheckConnTimeIndex(void)
{
	return CheckConnTimeIndex;
}
void setCheckConnTimeIndex(u32 cctt)
{
	CheckConnTimeIndex = cctt;
}
void addCheckConnTimeIndex(u32 addIndex)
{
	CheckConnTimeIndex += addIndex;
}
static u8 ArpPeriodFlag = 0;							//ARP���ڱ�־λ

u8 getArpPeriodFlag(void)
{
	return ArpPeriodFlag;
}
void setArpPeriodFlag(u8 apf)
{
	ArpPeriodFlag = apf;
}

static u8 TcpPeriodFlag = 0;							//TCP���ڱ�־λ

u8 getTcpPeriodFlag(void)
{
	return TcpPeriodFlag;
}
void setTcpPeriodFlag(u8 tpf)
{
	TcpPeriodFlag = tpf;
}

static u8 CheckConnFlag = 0;							//���Ӽ�����ڱ�־λ

u8 getCheckConnFlag(void)
{
	return CheckConnFlag;
}
void setCheckConnFlag(u8 ccf)
{
	CheckConnFlag = ccf;
}

/*********************************************************************************************************
*                                              ��̬��������
*********************************************************************************************************/
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);
static void Ethernet_Configuration(void);

err_t ethernetif_init(struct netif *netif); //�����ļ��ں�������
err_t ethernetif_input(struct netif *netif);//�����ļ��ں�������
/*********************************************************************************************************
*����������ʼ����̫��
*������  ��
*����ֵ����
**********************************************************************************************************/
void InitEthernet(void)
{
	GPIO_Configuration();
	NVIC_Configuration();
	Ethernet_Configuration();
}

/*-------------------------------------------------*/
/*����������ʼ��lwip����ǰ������                   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void InitLwipCon(void)
{
	struct ip_addr ipaddr;     //����IP��ַ�ṹ��
	struct ip_addr netmask;    //������������ṹ��
	struct ip_addr gw;         //�������ؽṹ��

	mem_init();                //��ʼ����̬�ڴ��
	memp_init();               //��ʼ���ڴ��
	lwip_init();               //��ʼ��LWIP�ں�
	
	IP4_ADDR(&ipaddr, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);            //����IP��ַ
	IP4_ADDR(&netmask, subnetMask[0], subnetMask[1], subnetMask[2], subnetMask[3]);   //������������
	IP4_ADDR(&gw, gateway[0], gateway[1], gateway[2], gateway[3]);                //��������

	printf2("�����徲̬IP��ַ��  %d.%d.%d.%d\r\n",ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	printf2("���������������ַ��%d.%d.%d.%d\r\n", subnetMask[0], subnetMask[1], subnetMask[2], subnetMask[3]);
	printf2("���������ص�ַ��    %d.%d.%d.%d\r\n", gateway[0], gateway[1], gateway[2], gateway[3]);
	
	ETH_MACAddressConfig(ETH_MAC_Address0, MacAddr);  //����MAC��ַ

	//ע����������   ethernetif_init��������Ҫ�Լ�ʵ�֣���������netif����ֶΣ��Լ���ʼ���ײ�Ӳ����
	netif_add(&DP83848_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

	//��ע����������ע�����������ΪĬ������
	netif_set_default(&DP83848_netif);

	//������
	netif_set_up(&DP83848_netif);
}

/*-------------------------------------------------*/
/*�������������������ݺ���                           */
/*��  ������                                        */
/*����ֵ����                                        */
/*-------------------------------------------------*/
void LwIP_Pkt_Handle(void)
{ 
	ethernetif_input(&DP83848_netif);   //�����绺�����ж�ȡ���յ������ݰ������䷢�͸�LWIP���� 
}
 
/*-------------------------------------------------*/
/*��������LWIP������������                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HandleLwipPeriodicEvent(void)
{
	if (getTcpPeriodFlag() == 1)
	{
		setTcpPeriodFlag(0);
		tcp_tmr();
	}
	if (getArpPeriodFlag() == 1)
	{
		setArpPeriodFlag(0);
		etharp_tmr();
	}
	if(getCheckConnFlag() == 1)
	{
		setCheckConnFlag(0);
		CheckForConnection();
	}
}

/*-------------------------------------------------*/
/*����������̫���ӿ�����                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //���ļ����ڳ�ʼ��GPIO�õ��Ľṹ��

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	//����PA2  �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//����PC1  �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//����PB11 PB12 PB13 �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//�ı�RX_DV RXD0��RXD1��ӳ�� 
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

	//����PA0 PA1 ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//����PB10 ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//����PD8 PD9 PD10 ��������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	//����PA8 �����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*-------------------------------------------------*/
/*����������̫���ж�����                           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;            //���ļ��ڣ�NVIC�ṹ��

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);     //�⺯���������������λ�ú�ƫ�ƣ���������������FLASH������ƫ�ơ�

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   //�⺯�����������ȼ����飺��ռ���ȼ��ʹ����ȼ�
													//NVIC_PriorityGroup_2����ռ���ȼ�2λ �����ȼ�2λ

	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;           //�����ж�ͨ��=��̫���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//������ռ���ȼ�=2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //���ô����ȼ�=0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);                          //�⺯��������NVIC_InitStructure�ṹ���ڵĲ�����ʼ��NVIC�Ĵ���
}

/*-------------------------------------------------*/
/*����������̫������                               */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
static void Ethernet_Configuration(void)
{ 
	ETH_InitTypeDef ETH_InitStructure;  //���ļ�����ʼ����̫���õ��Ľṹ��

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);	 
		
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);//�ú���������̫���ӿ�ΪRMII

	//����˼·����PLL3ʱ��ΪMCO�ṩRMII��Ҫ��50Mʱ��
	//ϵͳ��ʼ��STM32��ʱ��Ϊ72M��ʱ��PLL3������Ϊ���ⲿ25M����5��Ƶ 25/5=5M
	RCC_PLL3Config(RCC_PLL3Mul_10);//PLL3��Ƶ10�� 5*10=50M

	RCC_PLL3Cmd(ENABLE);//ʹ��PLL3ʱ��

	while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)//�ȴ�PLL3ʹ�ܳɹ�
	{}
	  
	RCC_MCOConfig(RCC_MCO_PLL3CLK);//ѡ��MCO��ʱ��ԴΪPLL3

	ETH_DeInit();       //��λ��̫������Ĵ�����Ĭ��ֵ����ʹ�ܣ��ڽ�ֹ���ѻָ�Ĭ��ֵ

	ETH_SoftwareReset();//�����λ��ͨ������ETH->DMABMR�Ĵ�����λ0ʵ��

	while (ETH_GetSoftwareResetStatus() == SET);//�ȴ���λ�ɹ�

	ETH_StructInit(&ETH_InitStructure);//��ʼ���ṹ��ETH_InitStructure������ԱΪĬ��ֵ

	//ETH_InitStructure�ṹ���ڳ�Ա�϶࣬�����Ĭ��ֵ���޸�һЩ�ؼ���Ա������Ӧ��ǰ����Ҫ
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//������������Ӧ����
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//�رշ���
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//�ر��ش�����
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//�ر��Զ�ȥ��PDA/CRC���� 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//�رս������е�֡
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//����������й㲥֡
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//�رջ��ģʽ�ĵ�ַ����  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//�����鲥��ַʹ��������ַ����   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//�Ե�����ַʹ��������ַ���� 
    
	#if  !CHECKSUM_GEN_ICMP    //�ж��Ƿ���Ӳ��У��,�ر����У��
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//����ipv4��TCP/UDP/ICMP��֡У���ж��   
	#endif
	
	//������ʹ��֡У���ж�ع��ܵ�ʱ��һ��Ҫʹ�ܴ洢ת��ģʽ,�洢ת��ģʽ��Ҫ��֤����֡�洢��FIFO��,
	//����MAC�ܲ���/ʶ���֡У��ֵ,����У����ȷ��ʱ��DMA�Ϳ��Դ���֡,����Ͷ�������֡
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //��������TCP/IP����֡
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //�����������ݵĴ洢ת��ģʽ    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //�����������ݵĴ洢ת��ģʽ  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//��ֹת������֡  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//��ת����С�ĺ�֡ 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//�򿪴���ڶ�֡����
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//����DMA����ĵ�ַ���빦��
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//�����̶�ͻ������    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA���͵����ͻ������Ϊ32������   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA���յ����ͻ������Ϊ32������
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	ETH_Init(&ETH_InitStructure,PHY_ADDRESS);		        //����ETH

	ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  	//ʹ����̫�������ж�	
}

void ETH_IRQHandler(void)
{
  //����Ƿ��յ����ݰ�
  while(ETH_GetRxPktSize() != 0) 
  {		
    LwIP_Pkt_Handle();//�������ݺ���
  }

  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);     //���DMA�жϱ�־λ
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);   //���DMA�����жϱ�־λ
}







