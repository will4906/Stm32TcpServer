#include "stm32f10x.h"
#include "stm32_eth.h"
#include "delay.h"
#include "TotalTimer.h"
#include "led.h"
#include "Uart.h"
#include "UartCom2.h"
#include "dp83848.h"
#include "tcpserver.h"
#include <string.h>

u32 w_buff[512] = {1,2,3,4,5,6,7,8,9};   
u32 r_buff[512];   

int main()
{
	u8 data[500];
	delay_init();
	//定时器时间设置为10ms
	InitTimerCon();
	InitUartCon();
	InitLedCon();
	InitEthernet();
	InitLwipCon();
	InitTcpServer();
	
	while(1)
	{
		//EnableAllLed();
		if(ReadUartData(UART_PORT_COM2,data))
		{
			printf2("the err is %d\n", SendTcpDataToClient(data, strlen((char*)data)));
		}
		//DisableAllLed();
		HandleLwipPeriodicEvent();
	}
}
