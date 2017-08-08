#ifndef __GPIO_Config_H
#define	__GPIO_Config_H

#include "stm32f10x.h"

#define   LED1_ON        GPIO_ResetBits(GPIOB, GPIO_Pin_8)
#define   LED1_OFF       GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define   NRF_CE_1		 GPIO_SetBits(GPIOA, GPIO_Pin_3)
#define   NRF_CE_0		 GPIO_ResetBits(GPIOA, GPIO_Pin_3)
#define   NRF_CSN_1		 GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define   NRF_CSN_0		 GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define   NRF_SCK_1		 GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define   NRF_SCK_0		 GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define   NRF_MOSI_1	 GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define   NRF_MOSI_0 	 GPIO_ResetBits(GPIOA, GPIO_Pin_7)

#define   NRF_MISO		 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
#define	  NRF_IRQ 		 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)

#define   FORWARD		   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)
#define	  BACK 		     GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)
#define   TURN_LEFT		 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)
#define	  TURN_RIGHT 	 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)

void GPIO_Config(void);
#endif
