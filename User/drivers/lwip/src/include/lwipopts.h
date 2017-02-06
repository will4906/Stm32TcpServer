#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/*****************保活探测宏定义*********************/
#define LWIP_TCP_KEEPALIVE           1              //开启用户自定义保活机制，1（自定义），0（默认）
#define TCP_KEEPIDLE_DEFAULT         6000           //连接双方都无数据，延时多少毫秒开启保活探测
#define TCP_KEEPINTVL_DEFAULT        1000           //发送保活探测的间隔时间(ms)
#define TCP_KEEPCNT_DEFAULT          9              //一共发送几次保活探测，无反应也认为断连

#define SYS_LIGHTWEIGHT_PROT    0  //关保护

#define NO_SYS                  1  //不使用UCOS操作系统

#define LWIP_NETIF_HOSTNAME     1  //使用hostname

#define CHECKSUM_GEN_ICMP       0  //我们使用硬件校验，关闭软件校验

#define MEM_ALIGNMENT           4  //使用4字节对齐模式

#define MEM_SIZE                32000 //内存堆大小,如果在应用中有大量数据发送的话这个值最好设置大一点 

#define MEMP_NUM_PBUF           10   //memp结构的pbuf数量,如果应用从ROM或者静态存储区发送大量数据时,这个值应该设置大一点

#define MEMP_NUM_UDP_PCB        6    //UDP协议控制块(PCB)数量.每个活动的UDP"连接"需要一个PCB.

#define MEMP_NUM_TCP_PCB        10   //同时建立激活的TCP数量
 
#define MEMP_NUM_TCP_PCB_LISTEN 6    //能够监听的TCP连接数量

#define MEMP_NUM_TCP_SEG        15   //最多同时在队列中的TCP段数量

#define MEMP_NUM_SYS_TIMEOUT    8    //能够同时激活的timeout个数
 
#define PBUF_POOL_SIZE          20   //pbuf内存池个数. 
 
#define PBUF_POOL_BUFSIZE       512  //每个pbuf内存池大小. 

#define LWIP_TCP                1    //使用TCP

#define TCP_TTL                 255  //TCP生存时间

#define TCP_QUEUE_OOSEQ         0    //当TCP的数据段超出队列时的控制位,当设备的内存过小的时候此项应为0

#define TCP_MSS                 (1500 - 40)	  //最大TCP分段

#define TCP_SND_BUF             (4*TCP_MSS)   //TCP发送缓冲区大小(bytes)

#define TCP_SND_QUEUELEN        (2* TCP_SND_BUF/TCP_MSS)  //TCP发送缓冲区大小(pbuf).这个值最小为(2 * TCP_SND_BUF/TCP_MSS) 

#define TCP_WND                 (2*TCP_MSS)   //TCP发送窗口

#define LWIP_ICMP                1   //使用ICMP协议

#define LWIP_DHCP                0   //不使用动态IP

#define LWIP_UDP                 1   //使用UDP服务
 
#define UDP_TTL                  255 //UDP数据包生存时间

#define LWIP_STATS               0   //不使用统计数据     

#define LWIP_PROVIDE_ERRNO       1   //使用lwip错误编码


#if  !CHECKSUM_GEN_ICMP   //判断不使用软件校验？
  //CHECKSUM_GEN_IP==0: 硬件生成IP数据包的帧校验和
  #define CHECKSUM_GEN_IP                 0
  //CHECKSUM_GEN_UDP==0: 硬件生成UDP数据包的帧校验和
  #define CHECKSUM_GEN_UDP                0
  //CHECKSUM_GEN_TCP==0: 硬件生成TCP数据包的帧校验和
  #define CHECKSUM_GEN_TCP                0 
  //CHECKSUM_CHECK_IP==0: 硬件检查输入的IP数据包帧校验和
  #define CHECKSUM_CHECK_IP               0
  //CHECKSUM_CHECK_UDP==0: 硬件检查输入的UDP数据包帧校验和
  #define CHECKSUM_CHECK_UDP              0
  //CHECKSUM_CHECK_TCP==0: 硬件检查输入的TCP数据包帧校验和
  #define CHECKSUM_CHECK_TCP              0
#else
  //CHECKSUM_GEN_IP==1: 软件生成IP数据包帧校验和
  #define CHECKSUM_GEN_IP                 1
  // CHECKSUM_GEN_UDP==1: 软件生成UDOP数据包帧校验和
  #define CHECKSUM_GEN_UDP                1
  //CHECKSUM_GEN_TCP==1: 软件生成TCP数据包帧校验和
  #define CHECKSUM_GEN_TCP                1
  // CHECKSUM_CHECK_IP==1: 软件检查输入的IP数据包帧校验和
  #define CHECKSUM_CHECK_IP               1
  // CHECKSUM_CHECK_UDP==1: 软件检查输入的UDP数据包帧校验和
  #define CHECKSUM_CHECK_UDP              1
  //CHECKSUM_CHECK_TCP==1: 软件检查输入的TCP数据包帧校验和
  #define CHECKSUM_CHECK_TCP              1
#endif


#define LWIP_NETCONN                    0    //不使能NETCON函数(要求使用api_lib.c)

#define LWIP_SOCKET                     0    //不使能Socket API(要求使用sockets.c)

#define LWIP_COMPAT_MUTEX               1    //二值信号量带通互斥信号量

#define LWIP_SO_RCVTIMEO                1    //使能netconn结构体中recv_timeout,使用recv_timeout可以避免阻塞线程

//#define LWIP_DEBUG                    0    //开启DEBUG选项
 
#define ICMP_DEBUG                      0    //关闭ICMPdebug

#endif 

