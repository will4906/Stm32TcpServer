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
/*��������TCP�������ĳ�ʼ��                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void InitTcpServer(void)
{                 
	tcp_server_pcb = tcp_new(); 						    //�½�һ��TCP���ƿ�
	tcp_bind(tcp_server_pcb,IP_ADDR_ANY,getTcpLocalPort());  //��IP�Ͷ˿ں�
	tcp_server_pcb = tcp_listen(tcp_server_pcb); 					    //�������������
	tcp_accept(tcp_server_pcb,OnTcpServerAccept); 		    //�������ӽ���ص�����
}

/*-------------------------------------------------*/
/*�����������ӽ���ص�����                         */
/*��  ����arg���������                            */
/*��  ����pcb��TCP���ƿ�                           */
/*��  ����err������ֵ                              */
/*����ֵ����                                       */
/*-------------------------------------------------*/
static err_t  OnTcpServerAccept(void *arg,struct tcp_pcb *pcb,err_t err)
{
	connectFlag = 1;
	tcp_client_pcb = pcb;
	tcp_setprio(pcb, TCP_PRIO_MIN); 		//���ûص��������ȼ��������ڼ�������ʱ�ر���Ҫ,�˺����������*/
	tcp_recv(pcb,OnTcpServerRecv); 		    //���ý��պ����ص�����
	
	printf2("connect accept\r\n");
	return ERR_OK;
}

/*-------------------------------------------------*/
/*�����������ջص�����                             */
/*��  ����arg���������                            */
/*��  ����pcb��TCP���ƿ�                           */
/*��  ����p�����ݻ�����ָ��                        */
/*��  ����err������ֵ                              */
/*����ֵ����                                       */
/*-------------------------------------------------*/
static err_t OnTcpServerRecv(void *arg, struct tcp_pcb *pcb,struct pbuf *p,err_t err)
{
	if(p != NULL)    //p�����ݻ�����ָ�룬��Ϊ�ձ�ʾ�����ݵ�
	{
		tcp_recved(pcb, p->tot_len);	              //��������			      
		OnTcpReceiveData(p->payload, p->tot_len);
		
		pbuf_free(p); 								  //�ͷŻ�����
	}
	return ERR_OK;
}

/**********************************************************
* �������ܣ�������������Ϣ���ͻ��˵ĺ���
* ������		���ݣ����ݵĳ���
*	����ֵ��	�Ƿ��ͳɹ�
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
* �������ܣ������������յ���Ϣʱ�Ļص�����
* ������		���յ���Ϣ����Ϣ�ĳ���
*	����ֵ��	��
***********************************************************/
static void OnTcpReceiveData(void *recvData, unsigned short len)
{
	//tcp_write(tcp_client_pcb,recvData, len,1);       //�����յ�����ԭ������
	SendTcpDataToClient(recvData, len);
	//printf2("recv: %s\r\n",p->payload);
	SendUartData(UART_PORT_COM2, recvData, len);
}

/******************************************************************
*������  �������״̬�����Ͽ�������
*������  ��
*����ֵ����
*ע��    �˺�������һ����ʱ����ã������޷���������
*********************************************************************/
void CheckForConnection(void)
{
	struct tcp_pcb *cpcb;   //TCP���ƿ�
	connectFlag = 0;
	for(cpcb = tcp_active_pcbs; cpcb != NULL; cpcb = cpcb->next)                    //��ѯTCP��б�
	{
		if(cpcb->local_port == getTcpLocalPort() && cpcb->remote_port == getServerPort())  //���TCP_LOCAL_PORT�˿�ָ��������û�жϿ�
		{
			connectFlag = 1;  						//���ӱ�־
			break;							   	
		}
	}
	if (!connectFlag)
	{
		CloseTcp(tcp_client_pcb);                                     //�ر�����
		//InitTcpClient(getTcpLocalPort(),getServerIpAddr(),getServerPort());         //��������
	}
}

/*-------------------------------------------------*/
/*���������ر����Ӻ���                             */
/*��  ����pcb��TCP���ƿ�                           */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void CloseTcp(struct tcp_pcb *pcb)
{
	tcp_abort(pcb);        //�ر�����
	tcp_arg(pcb,NULL);     //�Ƴ��ص����� 
	tcp_recv(pcb,NULL);    //�Ƴ��ص����� 
	tcp_sent(pcb,NULL);    //�Ƴ��ص����� 
	tcp_err(pcb,NULL);     //�Ƴ��ص����� 
	tcp_poll(pcb,NULL,0);  //�Ƴ��ص�����  
}




