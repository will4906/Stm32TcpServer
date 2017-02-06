#ifndef _DP83848_H_
#define _DP83848_H_

#include "stm32f10x.h"
#include "stm32_eth.h"

#define PHY_ADDRESS             0x01  //DP83848地址，根据核心板硬件连接决定
#define LWIP_MAX_DHCP_TRIES		4     //DHCP服务器最大重试次数

void InitEthernet(void);
void InitLwipCon(void);
void HandleLwipPeriodicEvent(void);

u32 getLwipTime(void);
void setLwipTime(u32 lt);

u8* getMacAddr(void);
void setMacAddr(u8 ma[]);

char* getHostName(void);
void setHostName(char *hn);

unsigned char* getIpAddr(void);
void setIpAddr(unsigned char *ia);

unsigned char* getSubnetMask(void);
void setSubnetMask(unsigned char* sm);

unsigned char* getGateway(void);
void setGateway(unsigned char* gw);

u16 getServerPort(void);
void setServerPort(u16 sp);

u16 getTcpLocalPort(void);
void setTcpLocalPort(u16 tlp);

u8* getServerIpAddr(void);
void setServerIpAddr(u8 *sia);

u32 getArpTimeIndex(void);
void setArpTimeIndex(u32 ati);
void addArpTimeIndex(u32 addIndex);

u32 getTcpTimeIndex(void);
void setTcpTimeIndex(u32 tti);
void addTcpTimeIndex(u32 addIndex);

u32 getCheckConnTimeIndex(void);
void setCheckConnTimeIndex(u32 cctt);
void addCheckConnTimeIndex(u32 addIndex);

u8 getArpPeriodFlag(void);
void setArpPeriodFlag(u8 apf);

u8 getTcpPeriodFlag(void);
void setTcpPeriodFlag(u8 tpf);

u8 getCheckConnFlag(void);
void setCheckConnFlag(u8 ccf);

#endif

