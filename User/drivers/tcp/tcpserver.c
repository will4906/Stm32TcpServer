#include "tcpserver.h"
#include "dp83848.h"
#include <string.h>
#include "Uart.h"
#include "UartCom2.h"

static err_t  OnTcpServerAccept(void *arg,struct tcp_pcb *pcb,err_t err);
static err_t OnTcpServerRecv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err);

static void OnTcpReceiveData(void *recvData, unsigned short len);

struct tcp_pcb *tcp_server_pcb;
struct tcp_pcb *tcp_client_pcb;
static u8 connectFlag = 0;
/*-------------------------------------------------*/
/*函数名：TCP服务器的初始化                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void InitTcpServer(void)
{                 
	tcp_server_pcb = tcp_new(); 						    //新建一个TCP控制块
	tcp_bind(tcp_server_pcb,IP_ADDR_ANY,getTcpLocalPort());  //绑定IP和端口号
	tcp_server_pcb = tcp_listen(tcp_server_pcb); 					    //服务器进入监听
	tcp_accept(tcp_server_pcb,OnTcpServerAccept); 		    //设置连接接入回调函数
}

/*-------------------------------------------------*/
/*函数名：连接接入回调函数                         */
/*参  数：arg：传入参数                            */
/*参  数：pcb：TCP控制块                           */
/*参  数：err：错误值                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
static err_t  OnTcpServerAccept(void *arg,struct tcp_pcb *pcb,err_t err)
{
	connectFlag = 1;
	tcp_client_pcb = pcb;
	tcp_setprio(pcb, TCP_PRIO_MIN); 		//设置回调函数优先级，当存在几个连接时特别重要,此函数必须调用*/
	tcp_recv(pcb,OnTcpServerRecv); 		    //设置接收函数回调函数
	
	return ERR_OK;
}

/*-------------------------------------------------*/
/*函数名：接收回调函数                             */
/*参  数：arg：传入参数                            */
/*参  数：pcb：TCP控制块                           */
/*参  数：p：数据缓冲区指针                        */
/*参  数：err：错误值                              */
/*返回值：无                                       */
/*-------------------------------------------------*/
static err_t OnTcpServerRecv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
	if(p != NULL)    //p是数据缓冲区指针，不为空表示有数据到
	{
		tcp_recved(pcb, p->tot_len);	              //接收数据			      
		OnTcpReceiveData(p->payload, p->tot_len);
		
		pbuf_free(p); 								  //释放缓冲区
	}
	return ERR_OK;
}

/**********************************************************
* 函数功能：服务器发送消息给客户端的函数
* 参数：		数据，数据的长度
*	返回值：	是否发送成功
***********************************************************/
err_t SendTcpDataToClient(u8 *buff, u16 length)					
{
 	err_t err;              
	
	if(connectFlag == 1)                               
	{
		err = tcp_write(tcp_client_pcb,buff,length,1);	
	}
	else
	{
		return ERR_CONN;
	}
	return err;   			
}

/**********************************************************
* 函数功能：当服务器接收到消息时的回调函数
* 参数：		接收的消息，消息的长度
*	返回值：	无
***********************************************************/
static void OnTcpReceiveData(void *recvData, unsigned short len)
{
	//Add code here
	//在此处添加接收到数据的处理代码
	SendTcpDataToClient(recvData, len);
	SendUartData(UART_PORT_COM2, recvData, len);
}

/******************************************************************
*函数：  检查连接状态，若断开则重连
*参数：  无
*返回值：无
*注：    此函数需以一定的时间调用，否则无法发挥作用
*********************************************************************/
void CheckForConnection(void)
{
	struct tcp_pcb *cpcb;   //TCP控制块
	connectFlag = 0;
	for(cpcb = tcp_active_pcbs; cpcb != NULL; cpcb = cpcb->next)                    //轮询TCP活动列表
	{
		if(cpcb->local_port == getTcpLocalPort() && cpcb->remote_port == getServerPort())  //如果TCP_LOCAL_PORT端口指定的连接没有断开
		{
			connectFlag = 1;  						//连接标志
			break;							   	
		}
	}
	if (!connectFlag)
	{
		CloseTcp(tcp_client_pcb);                                     //关闭连接
	}
}

/*-------------------------------------------------*/
/*函数名：关闭连接函数                             */
/*参  数：pcb：TCP控制块                           */
/*返回值：无                                       */
/*-------------------------------------------------*/
void CloseTcp(struct tcp_pcb *pcb)
{
	tcp_abort(pcb);        //关闭连接
	tcp_arg(pcb,NULL);     //移除回调函数 
	tcp_recv(pcb,NULL);    //移除回调函数 
	tcp_sent(pcb,NULL);    //移除回调函数 
	tcp_err(pcb,NULL);     //移除回调函数 
	tcp_poll(pcb,NULL,0);  //移除回调函数  
}




