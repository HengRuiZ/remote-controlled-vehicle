#include "EXTI.h"
#include "NVIC.h"
#include "GPIO_Config.h"
#include "NRF24L01.h"


void Delay(u32 t);



void EXTI_Config(void)
{	
	/*����һ��EXTI_InitTypeDef ���͵Ľṹ�壬���ֽ�EXTI_InitStructure*/
	EXTI_InitTypeDef EXTI_InitStructure;

	/*��ʼ���ж���������*/
	NVIC_Config();	

	/*�ж���*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line10|EXTI_Line11|EXTI_Line12|EXTI_Line13; 
	/*����ģʽ*/
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

	/*�����ź�*/
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//�����ش���

	/*ʹ���ж���*/
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;

	/*���ÿ⺯������ʼ��EXTI*/
	EXTI_Init(&EXTI_InitStructure);

}

void EXTI0_IRQHandler(void)
{   
		if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0);
		TxBuf[4]=0xcf;
		nRF24L01_TxPacket(TxBuf);
		LED1_ON;
//		Delay(2250);
	//	SPI_RW_Reg(NRF_WRITE_REG+STATUS,0XFF);
		Delay(2250);							 									 						
	}
		if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1);
		TxBuf[4]=0xfc;
		nRF24L01_TxPacket(TxBuf);
		LED1_OFF;
		Delay(2250);						 									 						
	}
		
}
void Delay_ms(u32 n)
{
	while(n--)
		Delay(1000);
}
void Delay(u32 n)
{
	u8 i;
	while(n--)
	{
		i=4;
		while(i--);
	}
}
