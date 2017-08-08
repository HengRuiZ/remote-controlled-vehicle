#include "GPIO_Config.h"
#include "NRF24L01.h"
#include "EXTI.h"

u8  TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���ص�ַ
u8  RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01};	//���յ�ַ

u8 sta;

 /*NRF24L01��ʼ��*/
void init_NRF24L01(void)	   
{
    Delay_ms(200);
 	NRF_CE_0;    // chip enable
 	NRF_CSN_1;   // Spi disable 
 	NRF_SCK_0;   // Spi clock line init high
	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // д���ص�ַ	
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); // д���ն˵�ַ
	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x00);      //  Ƶ��0�Զ�	ACKӦ������	
	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x00);  //  ������յ�ַֻ��Ƶ��0�������Ҫ��Ƶ�����Բο�Page21  
	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 0);        //   �����ŵ�����Ϊ2.4GHZ���շ�����һ��
	//SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ32�ֽ�
	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x01);   		//���÷�������Ϊ1MHZ�����书��Ϊ���ֵ0dB	
	//SPI_RW_Reg(NRF_WRITE_REG + SETUP_RETR,0x00);   		 // �Զ��ط������500΢�룬�ط�15��
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG,0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������

}

/*NRF24L01��SPIдʱ��*/	
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

/*NRF24L01��SPIʱ��*/
u8 SPI_Read(u8 reg)
{
	u8 reg_val;
	
	NRF_CSN_0;                // CSN low, initialize SPI communication...
	SPI_RW(reg);            // Select register to read from..
	reg_val = SPI_RW(0);    // ..then read registervalue
	NRF_CSN_1;                // CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
} 

/*NRF24L01��д�Ĵ�������*/
u8 SPI_RW_Reg(u8 reg, u8 value)
{
	u8 status;
	
	NRF_CSN_0;                   // CSN low, init SPI transaction
	status = SPI_RW(reg);      // select register
	SPI_RW(value);             // ..and write value to it..
	NRF_CSN_1;                   // CSN high again
	
	return(status);            // return nRF24L01 status uchar
}

/*���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���*/
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

/*����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���*/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars)
{
	u8 status,uchar_ctr;
	
	NRF_CSN_0;            //SPIʹ��       
	status = SPI_RW(reg);   
	for(uchar_ctr=0; uchar_ctr<uchars; uchar_ctr++) //
		SPI_RW(*pBuf++);
	NRF_CSN_1;           //�ر�SPI
	return(status);    // 
}

/*���ݽ�������*/
void SetRX_Mode(void)
{
	NRF_CE_0;
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	NRF_CE_1; 
	Delay(60);   //��ʱ����̫��
}

/*���ݶ�ȡ�����rx_buf���ջ�������*/
u8 nRF24L01_RxPacket(u8* rx_buf)
{
    u8 revale=0;
	sta=SPI_Read(STATUS);	// ��ȡ״̬�Ĵ������ж����ݽ���״��
	if(RX_DR)				// �ж��Ƿ���յ�����
	{
	    NRF_CE_0; 			//SPIʹ��
		SPI_Read_Buf(NRF_RD_RX_PLOAD,rx_buf,TX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;			//��ȡ������ɱ�־
	}
	SPI_RW_Reg(NRF_WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	return revale;
}

/*���� tx_buf������*/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	NRF_CE_0;			//StandBy Iģʽ	
	sta=SPI_Read(STATUS);	// ��ȡ״̬�Ĵ������ж����ݷ���״��
	//Delay(2250);
	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_Write_Buf(NRF_WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // װ������	
	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0e);   		 // IRQ�շ�����ж���Ӧ��16λCRC��������
	NRF_CE_1;		 //�ø�CE���������ݷ���
	Delay(300);
}


u8 NRF24L01_Check(void)
{
	u8 buf2[5];
	u8 buf1[5]={0x34,0x43,0x10,0x10,0x01};
	u8 i;  	 
	//SPI_Write_Buf(NRF_WRITE_REG + 0x10,buf1,4);//д��5���ֽڵĵ�ַ.	
	SPI_Read_Buf(0x10,buf2,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)if(buf1[i]!=buf2[i])break;	 							   
	if(i!=5)return 1;//���24L01����	
	return 0;		 //��⵽24L01
}	 	 

