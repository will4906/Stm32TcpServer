#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

void InitLedCon(void);               //初始化	
void EnableAllLed(void);              //点亮所有LED
void DisableAllLed(void);             //熄灭所有LED
void EnableSingleLed(u8 which);
void DisableSingleLed(u8 which);
#endif
