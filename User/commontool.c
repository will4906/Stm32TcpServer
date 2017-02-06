#include "commontool.h"

void copyArr(unsigned char *target, unsigned char *src, unsigned char len)
{
	unsigned char i;
	
	for(i = 0; i < len; i ++)
	{
		target[i] = src[i];
	}
}

