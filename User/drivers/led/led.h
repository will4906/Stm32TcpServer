#ifndef _LED_H_
#define _LED_H_

#include "stm32f10x.h"

void InitLedCon(void);               //��ʼ��	
void EnableAllLed(void);              //��������LED
void DisableAllLed(void);             //Ϩ������LED
void EnableSingleLed(u8 which);
void DisableSingleLed(u8 which);
#endif
