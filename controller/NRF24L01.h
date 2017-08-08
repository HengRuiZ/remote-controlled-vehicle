#ifndef __NRF24L01_H
#define __NRF24L01_H  
 
#include "stm32f10x.h"



extern u8 TxBuf[32];

#define	  RX_DR			 ((sta>>6)&0x1)
#define   TX_DS			 ((sta>>5)&0x1)
#define   MAX_RT		 ((sta>>4)&0x1)

//NRF24L01�Ĵ�����������
#define NRF_READ_REG        0x00  //�����üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_WRITE_REG       0x20  //д���üĴ���,��5λΪ�Ĵ�����ַ
#define NRF_RD_RX_PLOAD     0x61  //��RX��Ч����,1~32�ֽ�
#define NRF_WR_TX_PLOAD     0xA0  //дTX��Ч����,1~32�ֽ�
#define NRF_FLUSH_TX        0xE1  //���TX FIFO�Ĵ���.����ģʽ����
#define NRF_FLUSH_RX        0xE2  //���RX FIFO�Ĵ���.����ģʽ����
#define NRF_REUSE_TX_PL     0xE3  //����ʹ����һ������,CEΪ��,���ݰ������Ϸ���.
#define NRF_NOP             0xFF  //�ղ���,����������״̬�Ĵ���
//SPI(NRF24L01)�Ĵ�����ַ
#define CONFIG          0x00  // �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
#define EN_AA           0x01  // �Զ�Ӧ��������
#define EN_RXADDR       0x02  // �����ŵ�����
#define SETUP_AW        0x03  // �շ���ַ�������
#define SETUP_RETR      0x04  // �Զ��ط���������
#define RF_CH           0x05  // ����Ƶ������
#define RF_SETUP        0x06  // �������ʡ����Ĺ�������
#define STATUS          0x07  // ״̬�Ĵ���
#define OBSERVE_TX      0x08  // ���ͼ�⹦��
#define CD              0x09  // ��ַ���           
#define RX_ADDR_P0      0x0A  // Ƶ��0�������ݵ�ַ
#define RX_ADDR_P1      0x0B  // Ƶ��1�������ݵ�ַ
#define RX_ADDR_P2      0x0C  // Ƶ��2�������ݵ�ַ
#define RX_ADDR_P3      0x0D  // Ƶ��3�������ݵ�ַ
#define RX_ADDR_P4      0x0E  // Ƶ��4�������ݵ�ַ
#define RX_ADDR_P5      0x0F  // Ƶ��5�������ݵ�ַ
#define TX_ADDR         0x10  // ���͵�ַ�Ĵ���
#define RX_PW_P0        0x11  // ����Ƶ��0�������ݳ���
#define RX_PW_P1        0x12  // ����Ƶ��0�������ݳ���
#define RX_PW_P2        0x13  // ����Ƶ��0�������ݳ���
#define RX_PW_P3        0x14  // ����Ƶ��0�������ݳ���
#define RX_PW_P4        0x15  // ����Ƶ��0�������ݳ���
#define RX_PW_P5        0x16  // ����Ƶ��0�������ݳ���
#define FIFO_STATUS     0x17  // FIFOջ��ջ��״̬�Ĵ�������
//24L01���ͽ������ݿ�ȶ���
#define TX_ADR_WIDTH    5   //5�ֽڵĵ�ַ���
#define RX_ADR_WIDTH    5   //5�ֽڵĵ�ַ���
#define TX_PLOAD_WIDTH  32  //20�ֽڵ��û����ݿ��
#define RX_PLOAD_WIDTH  32  //20�ֽڵ��û����ݿ��



void init_NRF24L01(void);
u8 SPI_RW(u8 uchar);
u8 SPI_Read(u8 reg);
u8 SPI_RW_Reg(u8 reg, u8 value);
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 uchars);
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 uchars);
void SetRX_Mode(void);
u8 nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char * tx_buf);
u8 NRF24L01_Check(void);



#endif
