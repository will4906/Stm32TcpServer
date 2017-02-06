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
*																								宏定义
**********************************************************************************************************/
#define MAC_LEN 6
#define IP_LEN  4
/*********************************************************************************************************
*																								变量定义
**********************************************************************************************************/
static u32 LwipTime = 0;                 //LWip周期时钟

struct netif  DP83848_netif;    //定义一个全局的网络接口，注册网卡函数要用到

u32 getLwipTime(void)
{
	return LwipTime;
}
void setLwipTime(u32 lt)
{
	LwipTime = lt;
}

static u8 MacAddr[MAC_LEN]={0,0,0,0,0,1};  //MAC地址，具有唯一性

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

/*---------静态的IP地址---------*/
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
/*----------子网掩码------------*/
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
/*------------网关-------------*/
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
/************本地端口***********/
static u16 tcpLocalPort = 5000;

u16 getTcpLocalPort(void)
{
	return tcpLocalPort;
}
void setTcpLocalPort(u16 tlp)
{
	tcpLocalPort = tlp;
}

/************服务器ip地址********/
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

/*********服务器端口号**********/
static u16 serverPort = 8080; 		//范围：0-9999

u16 getServerPort(void)
{
	return serverPort;
}
void setServerPort(u16 sp)
{
	serverPort = sp;
}

static u32 ArpTimeIndex = 0;						 //ARP查询计时器

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

static u32 TcpTimeIndex = 0;						 //Tcp查询计时器

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

static u32 CheckConnTimeIndex = 0;						//连接检查计时

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
static u8 ArpPeriodFlag = 0;							//ARP周期标志位

u8 getArpPeriodFlag(void)
{
	return ArpPeriodFlag;
}
void setArpPeriodFlag(u8 apf)
{
	ArpPeriodFlag = apf;
}

static u8 TcpPeriodFlag = 0;							//TCP周期标志位

u8 getTcpPeriodFlag(void)
{
	return TcpPeriodFlag;
}
void setTcpPeriodFlag(u8 tpf)
{
	TcpPeriodFlag = tpf;
}

static u8 CheckConnFlag = 0;							//连接检查周期标志位

u8 getCheckConnFlag(void)
{
	return CheckConnFlag;
}
void setCheckConnFlag(u8 ccf)
{
	CheckConnFlag = ccf;
}

/*********************************************************************************************************
*                                              静态函数定义
*********************************************************************************************************/
static void GPIO_Configuration(void);
static void NVIC_Configuration(void);
static void Ethernet_Configuration(void);

err_t ethernetif_init(struct netif *netif); //其他文件内函数声明
err_t ethernetif_input(struct netif *netif);//其他文件内函数声明
/*********************************************************************************************************
*函数名：初始化以太网
*参数：  无
*返回值：无
**********************************************************************************************************/
void InitEthernet(void)
{
	GPIO_Configuration();
	NVIC_Configuration();
	Ethernet_Configuration();
}

/*-------------------------------------------------*/
/*函数名：初始化lwip启动前的事宜                   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void InitLwipCon(void)
{
	struct ip_addr ipaddr;     //定义IP地址结构体
	struct ip_addr netmask;    //定义子网掩码结构体
	struct ip_addr gw;         //定义网关结构体

	mem_init();                //初始化动态内存堆
	memp_init();               //初始化内存池
	lwip_init();               //初始化LWIP内核
	
	IP4_ADDR(&ipaddr, ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);            //设置IP地址
	IP4_ADDR(&netmask, subnetMask[0], subnetMask[1], subnetMask[2], subnetMask[3]);   //设置子网掩码
	IP4_ADDR(&gw, gateway[0], gateway[1], gateway[2], gateway[3]);                //设置网关

	printf2("开发板静态IP地址：  %d.%d.%d.%d\r\n",ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	printf2("开发板子网掩码地址：%d.%d.%d.%d\r\n", subnetMask[0], subnetMask[1], subnetMask[2], subnetMask[3]);
	printf2("开发板网关地址：    %d.%d.%d.%d\r\n", gateway[0], gateway[1], gateway[2], gateway[3]);
	
	ETH_MACAddressConfig(ETH_MAC_Address0, MacAddr);  //配置MAC地址

	//注册网卡函数   ethernetif_init函数，需要自己实现，用于配置netif相关字段，以及初始化底层硬件。
	netif_add(&DP83848_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);

	//将注册网卡函数注册的网卡设置为默认网卡
	netif_set_default(&DP83848_netif);

	//打开网卡
	netif_set_up(&DP83848_netif);
}

/*-------------------------------------------------*/
/*函数名：网卡接收数据函数                           */
/*参  数：无                                        */
/*返回值：无                                        */
/*-------------------------------------------------*/
void LwIP_Pkt_Handle(void)
{ 
	ethernetif_input(&DP83848_netif);   //从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
}
 
/*-------------------------------------------------*/
/*函数名：LWIP周期性任务函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
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
/*函数名：以太网接口配置                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
static void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; //库文件，内初始化GPIO用到的结构体

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO , ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	
	//配置PA2  复用推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置PC1  复用推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//配置PB11 PB12 PB13 复用推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//改变RX_DV RXD0和RXD1的映射 
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);

	//配置PA0 PA1 浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//配置PB10 浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//配置PD8 PD9 PD10 浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

	//配置PA8 复用推免输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*-------------------------------------------------*/
/*函数名：以太网中断配置                           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;            //库文件内，NVIC结构体

	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);     //库函数，设置向量表的位置和偏移，将向量表设置在FLASH区，无偏移。

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);   //库函数，设置优先级分组：先占优先级和从优先级
													//NVIC_PriorityGroup_2：先占优先级2位 从优先级2位

	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;           //设置中断通道=以太网中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//设置先占优先级=2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;       //设置从优先级=0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          //使能中断
	NVIC_Init(&NVIC_InitStructure);                          //库函数，根据NVIC_InitStructure结构体内的参数初始化NVIC寄存器
}

/*-------------------------------------------------*/
/*函数名：以太网配置                               */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
static void Ethernet_Configuration(void)
{ 
	ETH_InitTypeDef ETH_InitStructure;  //库文件，初始化以太网用到的结构体

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);	 
		
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);//该函数设置以太网接口为RMII

	//程序思路利用PLL3时钟为MCO提供RMII需要的50M时钟
	//系统初始化STM32主时钟为72M的时候，PLL3被设置为了外部25M晶振5分频 25/5=5M
	RCC_PLL3Config(RCC_PLL3Mul_10);//PLL3倍频10倍 5*10=50M

	RCC_PLL3Cmd(ENABLE);//使能PLL3时钟

	while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)//等待PLL3使能成功
	{}
	  
	RCC_MCOConfig(RCC_MCO_PLL3CLK);//选择MCO的时钟源为PLL3

	ETH_DeInit();       //复位以太网外设寄存器到默认值，先使能，在禁止，已恢复默认值

	ETH_SoftwareReset();//软件复位，通过设置ETH->DMABMR寄存器的位0实现

	while (ETH_GetSoftwareResetStatus() == SET);//等待软复位成功

	ETH_StructInit(&ETH_InitStructure);//初始化结构体ETH_InitStructure各个成员为默认值

	//ETH_InitStructure结构体内成员较多，填充外默认值后，修改一些关键成员，以适应当前的需要
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;   			//开启网络自适应功能
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;					//关闭反馈
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable; 		//关闭重传功能
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; 	//关闭自动去除PDA/CRC功能 
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;						//关闭接收所有的帧
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;//允许接收所有广播帧
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;			//关闭混合模式的地址过滤  
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;//对于组播地址使用完美地址过滤   
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;	//对单播地址使用完美地址过滤 
    
	#if  !CHECKSUM_GEN_ICMP    //判断是否开启硬件校验,关闭软件校验
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable; 			//开启ipv4和TCP/UDP/ICMP的帧校验和卸载   
	#endif
	
	//当我们使用帧校验和卸载功能的时候，一定要使能存储转发模式,存储转发模式中要保证整个帧存储在FIFO中,
	//这样MAC能插入/识别出帧校验值,当真校验正确的时候DMA就可以处理帧,否则就丢弃掉该帧
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable; //开启丢弃TCP/IP错误帧
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;     //开启接收数据的存储转发模式    
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;   //开启发送数据的存储转发模式  

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;     	//禁止转发错误帧  
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;	//不转发过小的好帧 
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;  		//打开处理第二帧功能
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;  	//开启DMA传输的地址对齐功能
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;            			//开启固定突发功能    
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;     		//DMA发送的最大突发长度为32个节拍   
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;			//DMA接收的最大突发长度为32个节拍
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
	ETH_Init(&ETH_InitStructure,PHY_ADDRESS);		        //配置ETH

	ETH_DMAITConfig(ETH_DMA_IT_NIS|ETH_DMA_IT_R,ENABLE);  	//使能以太网接收中断	
}

void ETH_IRQHandler(void)
{
  //检测是否收到数据包
  while(ETH_GetRxPktSize() != 0) 
  {		
    LwIP_Pkt_Handle();//接收数据函数
  }

  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);     //清除DMA中断标志位
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);   //清除DMA接收中断标志位
}







