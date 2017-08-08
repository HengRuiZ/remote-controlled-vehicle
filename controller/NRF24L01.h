#ifndef __NRF24L01_H
#define __NRF24L01_H  
 
#include "stm32f10x.h"



extern u8 TxBuf[32];

#define	  RX_DR			 ((sta>>6)&0x1)
#define   TX_DS			 ((sta>>5)&0x1)
#define   MAX_RT		 ((sta>>4)&0x1)

//NRF24L01寄存器操作命令
#define NRF_READ_REG        0x00  //读配置寄存器,低5位为寄存器地址
#define NRF_WRITE_REG       0x20  //写配置寄存器,低5位为寄存器地址
#define NRF_RD_RX_PLOAD     0x61  //读RX有效数据,1~32字节
#define NRF_WR_TX_PLOAD     0xA0  //写TX有效数据,1~32字节
#define NRF_FLUSH_TX        0xE1  //清除TX FIFO寄存器.发射模式下用
#define NRF_FLUSH_RX        0xE2  //清除RX FIFO寄存器.接收模式下用
#define NRF_REUSE_TX_PL     0xE3  //重新使用上一包数据,CE为高,数据包被不断发送.
#define NRF_NOP             0xFF  //空操作,可以用来读状态寄存器
//SPI(NRF24L01)寄存器地址
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define STATUS          0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道0接收数据长度
#define RX_PW_P2        0x13  // 接收频道0接收数据长度
#define RX_PW_P3        0x14  // 接收频道0接收数据长度
#define RX_PW_P4        0x15  // 接收频道0接收数据长度
#define RX_PW_P5        0x16  // 接收频道0接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
//24L01发送接收数据宽度定义
#define TX_ADR_WIDTH    5   //5字节的地址宽度
#define RX_ADR_WIDTH    5   //5字节的地址宽度
#define TX_PLOAD_WIDTH  32  //20字节的用户数据宽度
#define RX_PLOAD_WIDTH  32  //20字节的用户数据宽度



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
