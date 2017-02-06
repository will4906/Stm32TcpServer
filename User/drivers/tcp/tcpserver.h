#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "stm32f10x.h"
#include "stm32_eth.h"
#include "tcp_impl.h"

#define CHECK_FOR_CONNECTION_PERIOD		20000					//

void InitTcpServer(void);
err_t SendTcpDataToClient(u8 *buff, u16 length);
void CheckForConnection(void);
void CloseTcp(struct tcp_pcb *pcb);

#endif

