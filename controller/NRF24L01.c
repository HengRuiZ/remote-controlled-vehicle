#include "GPIO_Config.h"
#include "NRF24L01.h"
#include "EXTI.h"

u8  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//本地地址
u8  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//接收地址

u8 sta;

 /*NRF24L01初始化*/
void init_NRF24L01(void)	   
{
    Delay_ms(200);
 	NRF_CE_0;    // chip enable
 	NRF_CSN_1;   // Spi disable 
 	NRF_SCK_0;   // Spi clock line init high
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // 写本地地址	
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // 写接收端地址
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x00);      //  频道0自动	ACK应答允许	
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x00);  //  允许接收地址只有频道0，如果需要多频道可以参考Page21  
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 0);        //   设置信道工作为2.4GHZ，收发必须一致
	//SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //设置接收数据长度，本次设置为32字节
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x01);   		//设置发射速率为1MHZ，发射功率为最大值0dB	
	//SPI_RW_Reg(NRF_WRITE_REG + SETUP_RETR,0x00);   		 // 自动重发，间隔500微秒，重发15次
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG,0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送

}

/*NRF24L01的SPI写时序*/	
u8 SPI_RW(u8 uchar)
{
	u8 bit_ctr;
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++) // output 8-bit
   	{
		if((uchar & 0x80)==0x80)
		{NRF_MOSI_1;}
		else
		{NRF_MOSI_0;}                // output 'uchar', MSB to MOSI
		uchar = (uchar << 1);           // shift next bit into MSB..
		NRF_SCK_1;                      // Set SCK high..
		Delay(10);
		uchar |= NRF_MISO;       		  // capture current MISO bit
		NRF_SCK_0;            		  // ..then set SCK low again
   	}
    return(uchar);           		  // return read uchar
}

/*NRF24L01的SPI时序*/
u8 SPI_Read(u8 reg)
{
	u8 reg_val;
	
	NRF_CSN_0;                // CSN low, initialize SPI communication...
	SPI_RW(reg);            // Select register to read from..
	reg_val = SPI_RW(0);    // ..then read registervalue
	NRF_CSN_1;                // CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
} 

/*NRF24L01读写寄存器函数*/
u8 SPI_RW_Reg(u8 reg, u8 value)
{
	u8 status;
	
	NRF_CSN_0;                   // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	NRF_CSN_1;                   // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}

/*用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数*/
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	NRF_CSN_0;                    		// Set CSN low, init SPI tranaction
	status = SPI_RW(reg);       		// Select register to write to and read status uchar
	
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0);    // 
	
	NRF_CSN_1;                           
	
	return(status);                    // return nRF24L01 status uchar
}

/*用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数*/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	NRF_CSN_0;            //SPI使能       
	status = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
		SPI_RW(*pBuf++);
	NRF_CSN_1;           //关闭SPI
	return(status);    // 
}

/*数据接收配置*/
void SetRX_Mode(void)
{
	NRF_CE_0;
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		// IRQ收发完成中断响应，16位CRC	，主接收
	NRF_CE_1; 
	Delay(60);   //延时不能太短
}

/*数据读取后放如rx_buf接收缓冲区中*/
u8 nRF24L01_RxPacket(u8* rx_buf)
{
    u8 revale=0;
	sta=SPI_Read(STATUS);	// 读取状态寄存其来判断数据接收状况
	if(RX_DR)				// 判断是否接收到数据
	{
	    NRF_CE_0; 			//SPI使能
		SPI_Read_Buf(NRF_RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;			//读取数据完成标志
	}
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,sta);   //接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清楚中断标志
	return revale;
}

/*发送 tx_buf中数据*/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	NRF_CE_0;			//StandBy I模式	
	sta=SPI_Read(STATUS);	// 读取状态寄存其来判断数据发送状况
	//Delay(2250);
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // 装载接收端地址
	SPI_Write_Buf(NRF_WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // 装载数据	
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ收发完成中断响应，16位CRC，主发送
	NRF_CE_1;		 //置高CE，激发数据发送
	Delay(300);
}


u8 NRF24L01_Check(void)
{
	u8 buf2[5];
	u8 buf1[5]={0x34,0x43,0x10,0x10,0x01};
	u8 i;  	 
	//SPI_Write_Buf(NRF_WRITE_REG + 0x10,buf1,4);//写入5个字节的地址.	
	SPI_Read_Buf(0x10,buf2,5); //读出写入的地址  
	for(i=0;i<5;i++)if(buf1[i]!=buf2[i])break;	 							   
	if(i!=5)return 1;//检测24L01错误	
	return 0;		 //检测到24L01
}	 	 

