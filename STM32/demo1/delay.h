#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"
#include "sys.h"
#include <stdio.h>

void delay_ms(uint32_t n);
void delay_us(uint32_t n);
void _sys_exit(int return_code);

#endif

