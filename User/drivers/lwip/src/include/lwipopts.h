#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/*****************����̽��궨��*********************/
#define LWIP_TCP_KEEPALIVE           1              //�����û��Զ��屣����ƣ�1���Զ��壩��0��Ĭ�ϣ�
#define TCP_KEEPIDLE_DEFAULT         6000           //����˫���������ݣ���ʱ���ٺ��뿪������̽��
#define TCP_KEEPINTVL_DEFAULT        1000           //���ͱ���̽��ļ��ʱ��(ms)
#define TCP_KEEPCNT_DEFAULT          9              //һ�����ͼ��α���̽�⣬�޷�ӦҲ��Ϊ����

#define SYS_LIGHTWEIGHT_PROT    0  //�ر���

#define NO_SYS                  1  //��ʹ��UCOS����ϵͳ

#define LWIP_NETIF_HOSTNAME     1  //ʹ��hostname

#define CHECKSUM_GEN_ICMP       0  //����ʹ��Ӳ��У�飬�ر����У��

#define MEM_ALIGNMENT           4  //ʹ��4�ֽڶ���ģʽ

#define MEM_SIZE                32000 //�ڴ�Ѵ�С,�����Ӧ�����д������ݷ��͵Ļ����ֵ������ô�һ�� 

#define MEMP_NUM_PBUF           10   //memp�ṹ��pbuf����,���Ӧ�ô�ROM���߾�̬�洢�����ʹ�������ʱ,���ֵӦ�����ô�һ��

#define MEMP_NUM_UDP_PCB        6    //UDPЭ����ƿ�(PCB)����.ÿ�����UDP"����"��Ҫһ��PCB.

#define MEMP_NUM_TCP_PCB        10   //ͬʱ���������TCP����
 
#define MEMP_NUM_TCP_PCB_LISTEN 6    //�ܹ�������TCP��������

#define MEMP_NUM_TCP_SEG        15   //���ͬʱ�ڶ����е�TCP������

#define MEMP_NUM_SYS_TIMEOUT    8    //�ܹ�ͬʱ�����timeout����
 
#define PBUF_POOL_SIZE          20   //pbuf�ڴ�ظ���. 
 
#define PBUF_POOL_BUFSIZE       512  //ÿ��pbuf�ڴ�ش�С. 

#define LWIP_TCP                1    //ʹ��TCP

#define TCP_TTL                 255  //TCP����ʱ��

#define TCP_QUEUE_OOSEQ         0    //��TCP�����ݶγ�������ʱ�Ŀ���λ,���豸���ڴ��С��ʱ�����ӦΪ0

#define TCP_MSS                 (1500 - 40)	  //���TCP�ֶ�

#define TCP_SND_BUF             (4*TCP_MSS)   //TCP���ͻ�������С(bytes)

#define TCP_SND_QUEUELEN        (2* TCP_SND_BUF/TCP_MSS)  //TCP���ͻ�������С(pbuf).���ֵ��СΪ(2 * TCP_SND_BUF/TCP_MSS) 

#define TCP_WND                 (2*TCP_MSS)   //TCP���ʹ���

#define LWIP_ICMP                1   //ʹ��ICMPЭ��

#define LWIP_DHCP                0   //��ʹ�ö�̬IP

#define LWIP_UDP                 1   //ʹ��UDP����
 
#define UDP_TTL                  255 //UDP���ݰ�����ʱ��

#define LWIP_STATS               0   //��ʹ��ͳ������     

#define LWIP_PROVIDE_ERRNO       1   //ʹ��lwip�������


#if  !CHECKSUM_GEN_ICMP   //�жϲ�ʹ�����У�飿
  //CHECKSUM_GEN_IP==0: Ӳ������IP���ݰ���֡У���
  #define CHECKSUM_GEN_IP                 0
  //CHECKSUM_GEN_UDP==0: Ӳ������UDP���ݰ���֡У���
  #define CHECKSUM_GEN_UDP                0
  //CHECKSUM_GEN_TCP==0: Ӳ������TCP���ݰ���֡У���
  #define CHECKSUM_GEN_TCP                0 
  //CHECKSUM_CHECK_IP==0: Ӳ����������IP���ݰ�֡У���
  #define CHECKSUM_CHECK_IP               0
  //CHECKSUM_CHECK_UDP==0: Ӳ����������UDP���ݰ�֡У���
  #define CHECKSUM_CHECK_UDP              0
  //CHECKSUM_CHECK_TCP==0: Ӳ����������TCP���ݰ�֡У���
  #define CHECKSUM_CHECK_TCP              0
#else
  //CHECKSUM_GEN_IP==1: �������IP���ݰ�֡У���
  #define CHECKSUM_GEN_IP                 1
  // CHECKSUM_GEN_UDP==1: �������UDOP���ݰ�֡У���
  #define CHECKSUM_GEN_UDP                1
  //CHECKSUM_GEN_TCP==1: �������TCP���ݰ�֡У���
  #define CHECKSUM_GEN_TCP                1
  // CHECKSUM_CHECK_IP==1: �����������IP���ݰ�֡У���
  #define CHECKSUM_CHECK_IP               1
  // CHECKSUM_CHECK_UDP==1: �����������UDP���ݰ�֡У���
  #define CHECKSUM_CHECK_UDP              1
  //CHECKSUM_CHECK_TCP==1: �����������TCP���ݰ�֡У���
  #define CHECKSUM_CHECK_TCP              1
#endif


#define LWIP_NETCONN                    0    //��ʹ��NETCON����(Ҫ��ʹ��api_lib.c)

#define LWIP_SOCKET                     0    //��ʹ��Socket API(Ҫ��ʹ��sockets.c)

#define LWIP_COMPAT_MUTEX               1    //��ֵ�ź�����ͨ�����ź���

#define LWIP_SO_RCVTIMEO                1    //ʹ��netconn�ṹ����recv_timeout,ʹ��recv_timeout���Ա��������߳�

//#define LWIP_DEBUG                    0    //����DEBUGѡ��
 
#define ICMP_DEBUG                      0    //�ر�ICMPdebug

#endif 

