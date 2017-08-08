#ifndef __EXTI_H
#define __EXTI_H
#include "stm32f10x.h"



void EXTI_Config(void);	
void EXTI15_10_IRQHandler(void);
void Delay(u32 t);
void Delay_ms(u32 n);
#endif
