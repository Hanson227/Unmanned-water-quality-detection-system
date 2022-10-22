#ifndef __USART1_H
#define __USART1_H

#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>

extern int fputc(int ch, FILE *f);
extern void usart1_init(uint32_t baud);
extern void USART1_IRQHandler(void);

#endif