/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"
#include "dp83848.h"
#include <string.h>

/********************************************************************************************************************
*                                                       宏定义
*********************************************************************************************************************/
#define IFNAME0 'e'
#define IFNAME1 'n'

#define  ETH_DMARxDesc_FrameLengthShift           16         //DMA接收描述符
#define  ETH_ERROR                                ((u32)0)   //出错代码
#define  ETH_SUCCESS                              ((u32)1)   //无措代码

#define ETH_RXBUFNB        5  //接收缓冲器数量
#define ETH_TXBUFNB        5  //发送缓冲器数量

/********************************************************************************************************************
*                                                       变量定义
*********************************************************************************************************************/
extern ETH_DMADESCTypeDef  *DMATxDescToSet;  //当前DMA发送描述符指针，在以太网库文件中定义的
extern ETH_DMADESCTypeDef  *DMARxDescToGet;  //当前DMA接收描述符指针，在以太网库文件中定义的

ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];                    //发送和接收DMA描述符数组
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_MAX_PACKET_SIZE], Tx_Buff[ETH_TXBUFNB][ETH_MAX_PACKET_SIZE];//发送和接收缓冲区

//数据帧结构体，和网卡相关
typedef struct{
u32_t length;                     //帧长度
u32_t buffer;                     //缓冲区
ETH_DMADESCTypeDef *descriptor;   //指向DMA描述符的指针
}FrameTypeDef;

/********************************************************************************************************************
*                                                       函数声明
*********************************************************************************************************************/
FrameTypeDef ETH_RxPkt_ChainMode(void);
u32_t ETH_GetCurrentTxBuffer(void);
u32_t ETH_TxPkt_ChainMode(u16 FrameLength);


/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */
err_t ethernetif_input(struct netif *netif);

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
  u8 *MacAddr = getMacAddr();
  int i;

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */

  netif->hwaddr[0] = MacAddr[0];
  netif->hwaddr[1] = MacAddr[1];
	netif->hwaddr[2] = MacAddr[2];
	netif->hwaddr[3] = MacAddr[3];
	netif->hwaddr[4] = MacAddr[4];
  netif->hwaddr[5] = MacAddr[5];

  /* maximum transfer unit */
  netif->mtu = 1500;

	netif->hostname = getHostName();

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

  ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
  ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

  //开启DMA描述符接收中断
  for(i=0; i<ETH_RXBUFNB; i++)
	{
		ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
	}

  #if  !CHECKSUM_GEN_ICMP     //判断是否开启硬件校验，关闭软件校验
    //开启发送帧校验
    for(i=0; i<ETH_TXBUFNB; i++)
    {
      ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
    }
  #endif
    ETH_Start();          //开启MAC和DMA

}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  int l = 0;      //长度
  u8 *buffer =  (u8 *)ETH_GetCurrentTxBuffer();

  for(q = p; q != NULL; q = q->next) {
    memcpy((u8_t*)&buffer[l], q->payload, q->len);
    l = l + q->len;
  }

  ETH_TxPkt_ChainMode(l);

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  int l = 0;
  FrameTypeDef frame;
  u8 *buffer; 

  p = NULL;
  frame = ETH_RxPkt_ChainMode();
  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = frame.length;//将数据帧长度存放在len内待用

  buffer = (u8 *)frame.buffer; //得到数据区地址
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);//ÄÚ´æ³Ø·ÖÅä¿Õ¼ä

  if (p != NULL)//分配成功
  {
    for (q = p; q != NULL; q = q->next)//利用for循环拷贝数据
    {
      memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
      l = l + q->len;
    }    
  }

  frame.descriptor->Status = ETH_DMARxDesc_OWN; //设置DMA占用描述符
 
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  //通过判断ETH->DMASR寄存器位7，判断接收缓冲区可不可用
  {
    //接收缓冲区不可用，if成立
    ETH->DMASR = ETH_DMASR_RBUS; //清除接收缓冲区不可用标志
    ETH->DMARPDR = 0;//通过写ETH->DMARPDR寄存器，恢复DMA接收
  }

  return p;//返回数据
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
//LWip调用的接收数据函数
//主要就是调用了low_level_input函数
err_t ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  p = low_level_input(netif);//调用low_level_input函数接收数据

  if (p == NULL) return ERR_MEM;//无数据可读，返回错误代码

  err = netif->input(p, netif);//调用LWip源码处理数据
  if (err != ERR_OK) //如果处理失败
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));//调试信息
    pbuf_free(p);//释放掉pbuf空间
    p = NULL;
  }

  return err;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));

  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  low_level_init(netif);

  return ERR_OK;
}

//网卡接收数据函数
FrameTypeDef ETH_RxPkt_ChainMode(void)
{
  u32 framelength = 0;             //变量待用
  FrameTypeDef frame = {0,0};      //帧结构待用

  if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (u32)RESET)//如果DMA占用描述符成立
  {
	frame.length = ETH_ERROR;   //存放错误代码

    if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  //如果发送缓存不可用，if成立
    {
		ETH->DMASR = ETH_DMASR_RBUS; //清除接收缓冲区不可用标志
		ETH->DMARPDR = 0;//通过写ETH->DMARPDR寄存器，恢复DMA接收
    }

    return frame; //返回帧结构
  }
  //如果上步if不成立，标志描述符由CPU占用
  //又要进行3个判断
  //ETH_DMARxDesc_ES判断接收中是否出错，成立表示没有错误发生
  //ETH_DMARxDesc_LS判断是否到了最后一个缓冲区
  //ETH_DMARxDesc_FS判断是否包含了帧的第一个缓冲区
  if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (u32)RESET) &&
     ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (u32)RESET) &&
     ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (u32)RESET))
  {
     //都成立的话，得到帧长度值，
	 //DMA接收描述符RDES0软件寄存器位16-位29存放帧长度值
	 //右移16位，然后还要减去4个自己的CRC校验
    framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARxDesc_FrameLengthShift) - 4;

	frame.buffer = DMARxDescToGet->Buffer1Addr;	//得到接收描述符下Buffer1Addr地址，它指向了数据缓冲区
  }
  else//如果上步if不成立
  {
    framelength = ETH_ERROR;//记录错误代码
  }

  frame.length = framelength; //将帧长度值，记录在frame结构体中的length成员


  frame.descriptor = DMARxDescToGet;//frame结构体中的descriptor成员指向当前的DMA接收描述符

  DMARxDescToGet = (ETH_DMADESCTypeDef*) (DMARxDescToGet->Buffer2NextDescAddr);//将当前接收DMA描述符指针，指向下一个接收DMA链表中的DMA描述符

  return (frame);  //返回帧结构
}

//网卡发送数据
u32_t ETH_TxPkt_ChainMode(u16 FrameLength)
{
  if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)//如果DMA占用描述符成立
  {
    return ETH_ERROR;//返回错误代码
  }

  //如果if不成立，表明CPU占用描述符
  DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);//设置发送帧长度

  DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;//ETH_DMATxDesc_LS和ETH_DMATxDesc_FS置1，表示帧中存放了，第一个和最后一个分块

  DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;//把描述符给DMA用

  if ((ETH->DMASR & ETH_DMASR_TBUS) != (u32)RESET)//如果发送缓存不成立，if成立
  {
    ETH->DMASR = ETH_DMASR_TBUS;//清除发送缓存不可用标志
    ETH->DMATPDR = 0;//写ETH->DMATPDR寄存器，以求回复发送流程
  }

  DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);//将当前发送DMA描述符指针，指向下一个发送DMA链表中的DMA描述符

  return ETH_SUCCESS;   //返回成功代码
}

//获取发送DMA描述符下的缓冲区
u32_t ETH_GetCurrentTxBuffer(void)
{
  return (DMATxDescToSet->Buffer1Addr);   //得到DMA描述符内Buffer1Addr的地址
}
