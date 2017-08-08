/*发送端程序

通过两个摇杆控制前后和左右转
按键为中断模式。
程序版本号：V1030
*/
#include "stm32f10x.h"
#include "GPIO_Config.h"
#include "NVIC.h"
#include "EXTI.h"
#include "NRF24L01.h"
int i=0;
u8 j;
u8 cfg,fifo_sta;
u16 AD1_val;
u8 TxBuf[32]=
{
0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,
0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,
0x25,0x26,0x27,0x28,0x29,0x30,0x31,0x32,
};
int main(void)
{
	GPIO_Config();		 //IO口初始化
	ADC();
	EXTI_Config();		 //中断配置
	tim3();
	init_NRF24L01();	  //2.4G模块初始化
	j=NRF24L01_Check();
	while(1){
		cfg=SPI_Read(CONFIG);
		fifo_sta=SPI_Read(FIFO_STATUS);
		AD1_val=ADC_GetConversionValue(ADC1);
		if(FORWARD)TxBuf[1]=0x02;
		else
			TxBuf[1]=0x00;
		if(BACK)TxBuf[2]=0x03;
		else
			TxBuf[2]=0x00;
		if(TURN_LEFT)TxBuf[3]=0x04;
		else
			TxBuf[3]=0x00;
		if(TURN_RIGHT)TxBuf[4]=0x05;
		else
			TxBuf[4]=0x00;
		nRF24L01_TxPacket(TxBuf);
		Delay(250);
	}

}
