#include "stm32f10x.h"

#ifndef _DELAY_H_
#define _DELAY_H_


void delay_init(void);         //延时初始化函数
void delay_ms(u32 ms);         //延时毫秒函数
void delay_us(u32 us);         //延时微秒函数

#endif
