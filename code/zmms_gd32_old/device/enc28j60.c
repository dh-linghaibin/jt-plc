/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "enc28j60.h"

/* 寄存器地址掩码 */
#define ADDR_MASK        0x1F
/* 存储区域掩码 */
#define BANK_MASK        0x60
/* MAC和MII寄存器掩码*/
#define SPRD_MASK        0x80

/* 关键寄存器 */
#define EIE              0x1B
#define EIR              0x1C
#define ESTAT            0x1D
#define ECON2            0x1E
#define ECON1            0x1F
// Bank 0 registers
#define ERDPTL           (0x00|0x00)
#define ERDPTH           (0x01|0x00)
#define EWRPTL           (0x02|0x00)
#define EWRPTH           (0x03|0x00)
#define ETXSTL           (0x04|0x00)
#define ETXSTH           (0x05|0x00)
#define ETXNDL           (0x06|0x00)
#define ETXNDH           (0x07|0x00)
#define ERXSTL           (0x08|0x00)
#define ERXSTH           (0x09|0x00)
#define ERXNDL           (0x0A|0x00)
#define ERXNDH           (0x0B|0x00)

#define ERXRDPTL         (0x0C|0x00)
#define ERXRDPTH         (0x0D|0x00)
#define ERXWRPTL         (0x0E|0x00)
#define ERXWRPTH         (0x0F|0x00)
#define EDMASTL          (0x10|0x00)
#define EDMASTH          (0x11|0x00)
#define EDMANDL          (0x12|0x00)
#define EDMANDH          (0x13|0x00)
#define EDMADSTL         (0x14|0x00)
#define EDMADSTH         (0x15|0x00)
#define EDMACSL          (0x16|0x00)
#define EDMACSH          (0x17|0x00)
// Bank 1 registers
#define EHT0             (0x00|0x20)
#define EHT1             (0x01|0x20)
#define EHT2             (0x02|0x20)
#define EHT3             (0x03|0x20)
#define EHT4             (0x04|0x20)
#define EHT5             (0x05|0x20)
#define EHT6             (0x06|0x20)
#define EHT7             (0x07|0x20)
#define EPMM0            (0x08|0x20)
#define EPMM1            (0x09|0x20)
#define EPMM2            (0x0A|0x20)
#define EPMM3            (0x0B|0x20)
#define EPMM4            (0x0C|0x20)
#define EPMM5            (0x0D|0x20)
#define EPMM6            (0x0E|0x20)
#define EPMM7            (0x0F|0x20)
#define EPMCSL           (0x10|0x20)
#define EPMCSH           (0x11|0x20)
#define EPMOL            (0x14|0x20)
#define EPMOH            (0x15|0x20)
#define EWOLIE           (0x16|0x20)
#define EWOLIR           (0x17|0x20)
#define ERXFCON          (0x18|0x20)
#define EPKTCNT          (0x19|0x20)
// Bank 2 registers
#define MACON1           (0x00|0x40|0x80)
#define MACON2           (0x01|0x40|0x80)
#define MACON3           (0x02|0x40|0x80)
#define MACON4           (0x03|0x40|0x80)
#define MABBIPG          (0x04|0x40|0x80)
#define MAIPGL           (0x06|0x40|0x80)
#define MAIPGH           (0x07|0x40|0x80)
#define MACLCON1         (0x08|0x40|0x80)
#define MACLCON2         (0x09|0x40|0x80)
#define MAMXFLL          (0x0A|0x40|0x80)
#define MAMXFLH          (0x0B|0x40|0x80)
#define MAPHSUP          (0x0D|0x40|0x80)
#define MICON            (0x11|0x40|0x80)
#define MICMD            (0x12|0x40|0x80)
#define MIREGADR         (0x14|0x40|0x80)
#define MIWRL            (0x16|0x40|0x80)
#define MIWRH            (0x17|0x40|0x80)
#define MIRDL            (0x18|0x40|0x80)
#define MIRDH            (0x19|0x40|0x80)
// Bank 3 registers
#define MAADR1           (0x00|0x60|0x80)
#define MAADR0           (0x01|0x60|0x80)
#define MAADR3           (0x02|0x60|0x80)
#define MAADR2           (0x03|0x60|0x80)
#define MAADR5           (0x04|0x60|0x80)
#define MAADR4           (0x05|0x60|0x80)
#define EBSTSD           (0x06|0x60)
#define EBSTCON          (0x07|0x60)
#define EBSTCSL          (0x08|0x60)
#define EBSTCSH          (0x09|0x60)
#define MISTAT           (0x0A|0x60|0x80)
#define EREVID           (0x12|0x60)
#define ECOCON           (0x15|0x60)
#define EFLOCON          (0x17|0x60)
#define EPAUSL           (0x18|0x60)
#define EPAUSH           (0x19|0x60)
// PHY registers
#define PHCON1           0x00
#define PHSTAT1          0x01
#define PHHID1           0x02
#define PHHID2           0x03
#define PHCON2           0x10
#define PHSTAT2          0x11
#define PHIE             0x12
#define PHIR             0x13
#define PHLCON           0x14

// ENC28J60 ERXFCON Register Bit Definitions
#define ERXFCON_UCEN     0x80
#define ERXFCON_ANDOR    0x40
#define ERXFCON_CRCEN    0x20
#define ERXFCON_PMEN     0x10
#define ERXFCON_MPEN     0x08
#define ERXFCON_HTEN     0x04
#define ERXFCON_MCEN     0x02
#define ERXFCON_BCEN     0x01
// ENC28J60 EIE Register Bit Definitions
#define EIE_INTIE        0x80
#define EIE_PKTIE        0x40
#define EIE_DMAIE        0x20
#define EIE_LINKIE       0x10
#define EIE_TXIE         0x08
#define EIE_WOLIE        0x04
#define EIE_TXERIE       0x02
#define EIE_RXERIE       0x01
// ENC28J60 EIR Register Bit Definitions
#define EIR_PKTIF        0x40
#define EIR_DMAIF        0x20
#define EIR_LINKIF       0x10
#define EIR_TXIF         0x08
#define EIR_WOLIF        0x04
#define EIR_TXERIF       0x02
#define EIR_RXERIF       0x01
// ENC28J60 ESTAT Register Bit Definitions
#define ESTAT_INT        0x80
#define ESTAT_LATECOL    0x10
#define ESTAT_RXBUSY     0x04
#define ESTAT_TXABRT     0x02
#define ESTAT_CLKRDY     0x01
// ENC28J60 ECON2 Register Bit Definitions
#define ECON2_AUTOINC    0x80
#define ECON2_PKTDEC     0x40
#define ECON2_PWRSV      0x20
#define ECON2_VRPS       0x08
// ENC28J60 ECON1 Register Bit Definitions
#define ECON1_TXRST      0x80
#define ECON1_RXRST      0x40
#define ECON1_DMAST      0x20
#define ECON1_CSUMEN     0x10
#define ECON1_TXRTS      0x08
#define ECON1_RXEN       0x04
#define ECON1_BSEL1      0x02
#define ECON1_BSEL0      0x01
// ENC28J60 MACON1 Register Bit Definitions
#define MACON1_LOOPBK    0x10
#define MACON1_TXPAUS    0x08
#define MACON1_RXPAUS    0x04
#define MACON1_PASSALL   0x02
#define MACON1_MARXEN    0x01
// ENC28J60 MACON2 Register Bit Definitions
#define MACON2_MARST     0x80
#define MACON2_RNDRST    0x40
#define MACON2_MARXRST   0x08
#define MACON2_RFUNRST   0x04
#define MACON2_MATXRST   0x02
#define MACON2_TFUNRST   0x01
// ENC28J60 MACON3 Register Bit Definitions
#define MACON3_PADCFG2   0x80
#define MACON3_PADCFG1   0x40
#define MACON3_PADCFG0   0x20
#define MACON3_TXCRCEN   0x10
#define MACON3_PHDRLEN   0x08
#define MACON3_HFRMLEN   0x04
#define MACON3_FRMLNEN   0x02
#define MACON3_FULDPX    0x01
// ENC28J60 MICMD Register Bit Definitions
#define MICMD_MIISCAN    0x02
#define MICMD_MIIRD      0x01
// ENC28J60 MISTAT Register Bit Definitions
#define MISTAT_NVALID    0x04
#define MISTAT_SCAN      0x02
#define MISTAT_BUSY      0x01
// ENC28J60 PHY PHCON1 Register Bit Definitions
#define PHCON1_PRST      0x8000
#define PHCON1_PLOOPBK   0x4000
#define PHCON1_PPWRSV    0x0800
#define PHCON1_PDPXMD    0x0100
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
#define PHSTAT1_PFDPX    0x1000
#define PHSTAT1_PHDPX    0x0800
#define PHSTAT1_LLSTAT   0x0004
#define PHSTAT1_JBSTAT   0x0002
// ENC28J60 PHY PHCON2 Register Bit Definitions
#define PHCON2_FRCLINK   0x4000
#define PHCON2_TXDIS     0x2000
#define PHCON2_JABBER    0x0400
#define PHCON2_HDLDIS    0x0100

// ENC28J60 Packet Control Byte Bit Definitions
#define PKTCTRL_PHUGEEN  0x08
#define PKTCTRL_PPADEN   0x04
#define PKTCTRL_PCRCEN   0x02
#define PKTCTRL_POVERRIDE 0x01

/* SPI指令集 详见数据手册26页 */
/* 读控制寄存器 */
#define ENC28J60_READ_CTRL_REG       0x00
/* 读缓冲区 */
#define ENC28J60_READ_BUF_MEM        0x3A
/* 写控制寄存器 */
#define ENC28J60_WRITE_CTRL_REG      0x40
/* 写缓冲区 */
#define ENC28J60_WRITE_BUF_MEM       0x7A
/* 位域置位 */
#define ENC28J60_BIT_FIELD_SET       0x80
/* 位域清零 */
#define ENC28J60_BIT_FIELD_CLR       0xA0
/* 系统复位 */
#define ENC28J60_SOFT_RESET          0xFF

/* 接收缓冲区起始地址 */
#define RXSTART_INIT          0x0
/* 接收缓冲区停止地址 */
#define RXSTOP_INIT           (0x1FFF-0x0600-1)
/* 发送缓冲区起始地址 发送缓冲区大小约1500字节*/
#define TXSTART_INIT          (0x1FFF-0x0600)
/* 发送缓冲区停止地址 */
#define TXSTOP_INIT           0x1FFF
/* 以太网报文最大长度 */
#define   MAX_FRAMELEN        1500//最大长度为1518

/* 存储区编号  ENC28J60 具有Bank0到Bank3 4个存储区 需要通过ECON1寄存器选择*/
static unsigned char enc28j60_bank;
/* 下一个数据包指针，详见数据手册P43 图7-3 */
static unsigned int next_pack_ptr;

#define 	ENC28J60_CSL()		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define 	ENC28J60_CSH()		GPIO_SetBits(GPIOA,GPIO_Pin_4)

/*
********************************************************************************
* 函 数 名: BSP_SPI1SendByte
* 功能说明: SPI1发送字节数据
* 参    数：uint8_t byte  发送字节
* 返 回 值: uint8_t       返回字节
* 使用说明：根据SPI1通信原理，发送字节必有返回字节
* 调用方法：BSP_SPI1SendByte(value);
********************************************************************************
*/
uint8_t BSP_SPI1SendByte(uint8_t byte)
{
    /* 等待发送缓冲寄存器为空 */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* 发送数据 */
    SPI_I2S_SendData(SPI1, byte);		
    
    /* 等待接收缓冲寄存器为非空 */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    
    return SPI_I2S_ReceiveData(SPI1);
    
}
/*
********************************************************************************
* 函 数 名: enc28j60_readBuffer
* 功能说明: 读缓冲区
* 参    数: unsigned int len        读取长度
*           unsigned char* data     读取指针
* 返 回 值:                         无
* 使用说明: 
* 调用方法: 
********************************************************************************
*/
void enc28j60_readBuffer(unsigned char* pdata,unsigned int len)
{
    /* 使能ENC28J60 */
    ENC28J60_CSL();
    /* 通过SPI发送读取缓冲区命令*/
    BSP_SPI1SendByte(ENC28J60_READ_BUF_MEM);
    
    /* 循环读取 */
    while(len)
    {
        len--;
        /* 读取数据 */
        *pdata = (unsigned char)BSP_SPI1SendByte(0);
        /* 地址指针累加 */
        pdata++;
    }
    
    /* 禁止ENC28J60 */
    ENC28J60_CSH();
}
/*
********************************************************************************
* 函 数 名: enc28j60_writeBuffer
* 功能说明: 写缓冲区
* 参    数: unsigned int len        读取长度
*           unsigned char* data     读取指针
* 返 回 值:                         无
* 使用说明: 
* 调用方法: 
********************************************************************************
*/
void enc28j60_writebuf(unsigned char* pdata,unsigned int len )
{
    /* 使能ENC28J60 */
    ENC28J60_CSL();
    /* 通过SPI发送写取缓冲区命令*/
    BSP_SPI1SendByte(ENC28J60_WRITE_BUF_MEM);
    
    /* 循环发送 */
    while(len)
    {
        len--;
        /* 发送数据 */
        BSP_SPI1SendByte(*pdata);
        /* 地址指针累加 */
        pdata++;
    }
    
    /* 禁止ENC28J60 */
    ENC28J60_CSH();
}

/*
********************************************************************************
* 函 数 名: enc28j60_readop
* 功能说明: 读寄存器命令
* 参    数: unsigned char op        操作码
*           unsigned char address   寄存器地址
* 返 回 值:                         返回数据
* 使用说明: 该函数支持的操作码只读控制寄存器 读缓冲器
* 调用方法: enc28j60_readop
********************************************************************************
*/
unsigned char enc28j60_readop(unsigned char op, unsigned char address)
{
    unsigned char dat = 0;
    
    /* CS拉低 使能ENC28J60 */
    ENC28J60_CSL();
    /* 操作码和地址 */
    dat = op | (address & ADDR_MASK);
    /* 通过SPI写数据*/
    BSP_SPI1SendByte(dat);
    /* 通过SPI读出数据 */
    dat = BSP_SPI1SendByte(0xFF);
    
    /* 如果是MAC和MII寄存器，第一个读取的字节无效，该信息包含在地址的最高位*/
    if(address & 0x80)
    {
        /* 再次通过SPI读取数据 */
        dat = BSP_SPI1SendByte(0xFF);
    }
    
    /* CS拉高 禁止ENC28J60 */
    ENC28J60_CSH();
    
    /* 返回数据 */
    return dat;
}
/*
********************************************************************************
* 函 数 名: enc28j60_writeop
* 功能说明: 写寄存器命令
* 参    数: unsigned char op        操作码
*           unsigned char address   寄存器地址
*           unsigned char data      写入数据
* 返 回 值:                         无
* 使用说明: 该函数支持的操作码有: 写控制寄存器 位域清零 位域置1
* 调用方法: enc28j60_writeop
********************************************************************************
*/
void enc28j60_writeop(unsigned char op, unsigned char address, unsigned char data)
{
    unsigned char dat = 0;
    /* 使能ENC28J60 */							  	  
    ENC28J60_CSL();	     
    /* 通过SPI发送 操作码和寄存器地址 */                 		
    dat = op | (address & ADDR_MASK);
    /* 通过SPI1发送数据 */
    BSP_SPI1SendByte(dat);
    /* 准备寄存器数值 */				  
    dat = data;
    /* 通过SPI发送数据 */
    BSP_SPI1SendByte(dat);
    /* 禁止ENC28J60 */				 
    ENC28J60_CSH();	
}

/*
********************************************************************************
* 函 数 名: enc28j60_setbank
* 功能说明: 设定寄存器存储区域
* 参    数: unsigned char address   寄存器地址
* 返 回 值:                         无
* 使用说明: 
* 调用方法: 
********************************************************************************
*/ 
void enc28j60_setbank(unsigned char address)
{
    /* 计算本次寄存器地址在存取区域的位置 */
    if((address & BANK_MASK) != enc28j60_bank)
    {
        /* 清除ECON1的BSEL1 BSEL0 详见数据手册15页 */
        enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        /* 请注意寄存器地址的宏定义，bit6 bit5代码寄存器存储区域位置 */
        enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        /* 重新确定当前寄存器存储区域 */
        enc28j60_bank = (address & BANK_MASK);
    }
}
/*
********************************************************************************
* 函 数 名: enc28j60_write
* 功能说明: 写寄存器
* 参    数: unsigned char address   寄存器地址
*           unsigned char data      寄存器数值
* 返 回 值:                         无
* 使用说明: 
* 调用方法: 
********************************************************************************
*/
void enc28j60_write(unsigned char address, unsigned char data)
{
    /* 设定寄存器地址区域 */
    enc28j60_setbank(address);
    /* 写寄存器值 发送写寄存器命令和地址 */
    enc28j60_writeop(ENC28J60_WRITE_CTRL_REG, address, data);
}

/*
********************************************************************************
* 函 数 名: enc28j60_read
* 功能说明: 读取寄存器值
* 参    数: unsigned char address   寄存器地址
* 返 回 值:                         寄存器值
* 使用说明: 
* 调用方法: 
********************************************************************************
*/
unsigned char enc28j60_read(unsigned char address)
{
    /* 设定寄存器地址区域 */
    enc28j60_setbank(address);
    /* 读取寄存器值 发送读寄存器命令和地址 */
    return enc28j60_readop(ENC28J60_READ_CTRL_REG, address);
}
/*
********************************************************************************
* 函 数 名: enc28j60_writephy
* 功能说明: 写物理寄存器
* 参    数: unsigned char address   物理寄存器地址
*           unsigned int data       物理寄存器数值 物理寄存器均为16位宽
* 返 回 值:                         无
* 使用说明: PHY寄存器不能通过SPI命令直接访问，而是通过一组特殊的寄存器来访问
*           详见数据手册19页
* 调用方法: 
********************************************************************************
*/
void enc28j60_writephy(unsigned char address, unsigned int data)
{
    /* 向MIREGADR写入地址 详见数据手册19页*/
    enc28j60_write(MIREGADR, address);
    /* 写入低8位数据 */
    enc28j60_write (MIWRL, data);
    /* 写入高8位数据 */
    enc28j60_write(MIWRH, data>>8);
    /* 等待PHY寄存器写入完成 */
    while( enc28j60_read(MISTAT) & MISTAT_BUSY );
}

void en28j60_init(struct _en28j60_obj* en28) {
   /* GPIO结构体 */
    GPIO_InitTypeDef  GPIO_InitStructure; 
    /* SPI结构体 */
    SPI_InitTypeDef SPI_InitStructure; 
    
    /* 使能APB2上相关时钟 */
    /* 使能SPI时钟，使能GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |\
        RCC_APB2Periph_GPIOA ,ENABLE );
    
    /* SPI1 SCK@GPIOA.5 SPI1 MOSI@GPIOA.7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  |  GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /* 复用推挽输出 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    /* SPI1 MISO@GPIOA.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /* 浮动输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    
    /* 双线双向全双工 */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	/* 主机模式 */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
    /* 8位帧结构 */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
    /* 时钟空闲时为低 */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;        
    /* 第1个上升沿捕获数据 */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;      
    /* MSS 端口软件控制 */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;         
    /* SPI时钟 72Mhz / 8 = 9M */ 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
    /* 数据传输高位在前 */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    /* 初始化SPI1 */
    SPI_Init(SPI1, &SPI_InitStructure);
    
	/* 把使能SPI口的SS输出功能 GPIOA.4 */
	SPI_SSOutputCmd(SPI1,ENABLE);
    /* 使能SPI1 */
    SPI_Cmd(SPI1, ENABLE); 

	
    /* 打开GPIOA时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC , ENABLE);
    
    /* enc28j60 CS @GPIOA.4 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    //net RST cs
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4;				     
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);	
    GPIO_SetBits(GPIOA,GPIO_Pin_0);
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);//低有效
    
    // ENC28J60接收完成中断引脚，本例未使用
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         	 	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    
    /* 禁止ENC28J60 */
    ENC28J60_CSH();
    /* ENC28J60软件复位 该函数可以改进 */
    enc28j60_writeop(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET); 
    /* 查询ESTAT.CLKRDY位 */
    while(!(enc28j60_read(ESTAT) & ESTAT_CLKRDY));
    
    /* 设置接收缓冲区起始地址 该变量用于每次读取缓冲区时保留下一个包的首地址 */
    next_pack_ptr = RXSTART_INIT;
    
    /* 设置接收缓冲区 起始指针 */
    enc28j60_write(ERXSTL, RXSTART_INIT & 0xFF);
    enc28j60_write(ERXSTH, RXSTART_INIT >> 8);
    
    /* 设置接收缓冲区 读指针 */ 
    enc28j60_write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60_write(ERXRDPTH, RXSTART_INIT>>8);
    
    /* 设置接收缓冲区 结束指针 */
    enc28j60_write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60_write(ERXNDH, RXSTOP_INIT>>8);
    
    /* 设置发送缓冲区 起始指针 */
    enc28j60_write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60_write(ETXSTH, TXSTART_INIT>>8);
    /* 设置发送缓冲区 结束指针 */
    enc28j60_write(ETXNDL, TXSTOP_INIT&0xFF);
    enc28j60_write(ETXNDH, TXSTOP_INIT>>8);
    
    /* 使能单播过滤 使能CRC校验 使能 格式匹配自动过滤*/
    enc28j60_write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
    enc28j60_write(EPMM0, 0x3f);
    enc28j60_write(EPMM1, 0x30);
    enc28j60_write(EPMCSL, 0xf9);
    enc28j60_write(EPMCSH, 0xf7);
    
    /* 使能MAC接收 允许MAC发送暂停控制帧 当接收到暂停控制帧时停止发送*/
    /* 数据手册34页 */
    enc28j60_write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    
    /* 退出复位状态 */
    enc28j60_write(MACON2, 0x00);
    
    /* 用0填充所有短帧至60字节长 并追加一个CRC 发送CRC使能 帧长度校验使能 MAC全双工使能*/
    /* 提示 由于ENC28J60不支持802.3的自动协商机制， 所以对端的网络卡需要强制设置为全双工 */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
    
    
    /* 填入默认值 */
    enc28j60_write(MAIPGL, 0x12);
    /* 填入默认值 */
    enc28j60_write(MAIPGH, 0x0C);
    /* 填入默认值 */
    enc28j60_write(MABBIPG, 0x15);
    
    /* 最大帧长度 */
    enc28j60_write(MAMXFLL, MAX_FRAMELEN & 0xFF);	
    enc28j60_write(MAMXFLH, MAX_FRAMELEN >> 8);
    
    /* 写入MAC地址 */
    enc28j60_write(MAADR5, en28->mac[0]);	
    enc28j60_write(MAADR4, en28->mac[1]);
    enc28j60_write(MAADR3, en28->mac[2]);
    enc28j60_write(MAADR2, en28->mac[3]);
    enc28j60_write(MAADR1, en28->mac[4]);
    enc28j60_write(MAADR0, en28->mac[5]);
    
    /* 配置PHY为全双工  LEDB为拉电流 */
    enc28j60_writephy(PHCON1, PHCON1_PDPXMD);
    
    /* LED状态 */
    enc28j60_writephy(PHLCON,0x0476);	
    
    /* 半双工回环禁止 */
    enc28j60_writephy(PHCON2, PHCON2_HDLDIS);
    
    /* 返回BANK0 */	
    enc28j60_setbank(ECON1);
    
    /* 使能中断 全局中断 接收中断 接收错误中断 */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE|EIE_RXERIE);
    
    /* 接收使能位 */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

uint16_t en28j60_packet_receive(struct _en28j60_obj* en28,uint8_t * packet,uint16_t maxlen) {
	unsigned int rxstat;
    unsigned int len;
    
    /* 是否收到以太网数据包 */
    if( enc28j60_read(EPKTCNT) == 0 )
    {
        return(0);
    }
    
    /* 设置接收缓冲器读指针 */
    enc28j60_write(ERDPTL, (next_pack_ptr));
    enc28j60_write(ERDPTH, (next_pack_ptr)>>8);
    
    /* 接收数据包结构示例 数据手册43页 */
    
    /* 读下一个包的指针 */
    next_pack_ptr  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
    next_pack_ptr |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0)<<8;
    
    /* 读包的长度 */
    len  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0)<<8;
    
    /* 删除CRC计数 */
    len-= 4; 	
    
    /* 读取接收状态 */
    rxstat  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0) << 8;
    
    /* 限制检索的长度	*/  
    if (len > maxlen-1)
    {
        len = maxlen-1;
    }
    /* 检查CRC和符号错误 */
    /* ERXFCON.CRCEN是默认设置。通常我们不需要检查 */
    if ((rxstat & 0x80)==0)
    {
        len=0;
    }
    else
    {
        /* 从接收缓冲器中复制数据包 */
        enc28j60_readBuffer( packet,len );
    }
    
    /* 移动接收缓冲区 读指针*/
    enc28j60_write(ERXRDPTL, (next_pack_ptr));
    enc28j60_write(ERXRDPTH, (next_pack_ptr)>>8);
    
    /* 数据包递减 */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    
    /* 返回长度 */
    return(len);
}

void en28j60_packet_send(struct _en28j60_obj* en28,uint8_t * packet,uint16_t len) {
  /* 查询发送逻辑复位位 */
    while((enc28j60_read(ECON1) & ECON1_TXRTS)!= 0);
    
    /* 设置发送缓冲区起始地址 */    
    enc28j60_write(EWRPTL, TXSTART_INIT & 0xFF);
    enc28j60_write(EWRPTH, TXSTART_INIT >> 8);
    
    /* 设置发送缓冲区结束地址 该值对应发送数据包长度*/   
    enc28j60_write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
    enc28j60_write(ETXNDH, (TXSTART_INIT + len) >>8);
    
    /* 发送之前发送控制包格式字 ???????*/
    enc28j60_writeop(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    
    /* 通过ENC28J60发送数据包 */
    enc28j60_writebuf( packet,len );
    
    /* 开始发送*/
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
    
    /* 复位发送逻辑的问题。参见 Rev. B4 Silicon Errata point 12. */
    if( (enc28j60_read(EIR) & EIR_TXERIF) )
    {
        enc28j60_setbank(ECON1);
        enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }
}

void en28j60_set_mac(struct _en28j60_obj* en28) {
	/* 写入MAC地址 */
    enc28j60_write(MAADR5, en28->mac[0]);	
    enc28j60_write(MAADR4, en28->mac[1]);
    enc28j60_write(MAADR3, en28->mac[2]);
    enc28j60_write(MAADR2, en28->mac[3]);
    enc28j60_write(MAADR1, en28->mac[4]);
    enc28j60_write(MAADR0, en28->mac[5]);
}
