/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "enc28j60.h"

/* �Ĵ�����ַ���� */
#define ADDR_MASK        0x1F
/* �洢�������� */
#define BANK_MASK        0x60
/* MAC��MII�Ĵ�������*/
#define SPRD_MASK        0x80

/* �ؼ��Ĵ��� */
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

/* SPIָ� ��������ֲ�26ҳ */
/* �����ƼĴ��� */
#define ENC28J60_READ_CTRL_REG       0x00
/* �������� */
#define ENC28J60_READ_BUF_MEM        0x3A
/* д���ƼĴ��� */
#define ENC28J60_WRITE_CTRL_REG      0x40
/* д������ */
#define ENC28J60_WRITE_BUF_MEM       0x7A
/* λ����λ */
#define ENC28J60_BIT_FIELD_SET       0x80
/* λ������ */
#define ENC28J60_BIT_FIELD_CLR       0xA0
/* ϵͳ��λ */
#define ENC28J60_SOFT_RESET          0xFF

/* ���ջ�������ʼ��ַ */
#define RXSTART_INIT          0x0
/* ���ջ�����ֹͣ��ַ */
#define RXSTOP_INIT           (0x1FFF-0x0600-1)
/* ���ͻ�������ʼ��ַ ���ͻ�������СԼ1500�ֽ�*/
#define TXSTART_INIT          (0x1FFF-0x0600)
/* ���ͻ�����ֹͣ��ַ */
#define TXSTOP_INIT           0x1FFF
/* ��̫��������󳤶� */
#define   MAX_FRAMELEN        1500//��󳤶�Ϊ1518

/* �洢�����  ENC28J60 ����Bank0��Bank3 4���洢�� ��Ҫͨ��ECON1�Ĵ���ѡ��*/
static unsigned char enc28j60_bank;
/* ��һ�����ݰ�ָ�룬��������ֲ�P43 ͼ7-3 */
static unsigned int next_pack_ptr;

#define 	ENC28J60_CSL()		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define 	ENC28J60_CSH()		GPIO_SetBits(GPIOA,GPIO_Pin_4)

/*
********************************************************************************
* �� �� ��: BSP_SPI1SendByte
* ����˵��: SPI1�����ֽ�����
* ��    ����uint8_t byte  �����ֽ�
* �� �� ֵ: uint8_t       �����ֽ�
* ʹ��˵��������SPI1ͨ��ԭ�������ֽڱ��з����ֽ�
* ���÷�����BSP_SPI1SendByte(value);
********************************************************************************
*/
uint8_t BSP_SPI1SendByte(uint8_t byte)
{
    /* �ȴ����ͻ���Ĵ���Ϊ�� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* �������� */
    SPI_I2S_SendData(SPI1, byte);		
    
    /* �ȴ����ջ���Ĵ���Ϊ�ǿ� */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    
    return SPI_I2S_ReceiveData(SPI1);
    
}
/*
********************************************************************************
* �� �� ��: enc28j60_readBuffer
* ����˵��: ��������
* ��    ��: unsigned int len        ��ȡ����
*           unsigned char* data     ��ȡָ��
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_readBuffer(unsigned char* pdata,unsigned int len)
{
    /* ʹ��ENC28J60 */
    ENC28J60_CSL();
    /* ͨ��SPI���Ͷ�ȡ����������*/
    BSP_SPI1SendByte(ENC28J60_READ_BUF_MEM);
    
    /* ѭ����ȡ */
    while(len)
    {
        len--;
        /* ��ȡ���� */
        *pdata = (unsigned char)BSP_SPI1SendByte(0);
        /* ��ַָ���ۼ� */
        pdata++;
    }
    
    /* ��ֹENC28J60 */
    ENC28J60_CSH();
}
/*
********************************************************************************
* �� �� ��: enc28j60_writeBuffer
* ����˵��: д������
* ��    ��: unsigned int len        ��ȡ����
*           unsigned char* data     ��ȡָ��
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_writebuf(unsigned char* pdata,unsigned int len )
{
    /* ʹ��ENC28J60 */
    ENC28J60_CSL();
    /* ͨ��SPI����дȡ����������*/
    BSP_SPI1SendByte(ENC28J60_WRITE_BUF_MEM);
    
    /* ѭ������ */
    while(len)
    {
        len--;
        /* �������� */
        BSP_SPI1SendByte(*pdata);
        /* ��ַָ���ۼ� */
        pdata++;
    }
    
    /* ��ֹENC28J60 */
    ENC28J60_CSH();
}

/*
********************************************************************************
* �� �� ��: enc28j60_readop
* ����˵��: ���Ĵ�������
* ��    ��: unsigned char op        ������
*           unsigned char address   �Ĵ�����ַ
* �� �� ֵ:                         ��������
* ʹ��˵��: �ú���֧�ֵĲ�����ֻ�����ƼĴ��� ��������
* ���÷���: enc28j60_readop
********************************************************************************
*/
unsigned char enc28j60_readop(unsigned char op, unsigned char address)
{
    unsigned char dat = 0;
    
    /* CS���� ʹ��ENC28J60 */
    ENC28J60_CSL();
    /* ������͵�ַ */
    dat = op | (address & ADDR_MASK);
    /* ͨ��SPIд����*/
    BSP_SPI1SendByte(dat);
    /* ͨ��SPI�������� */
    dat = BSP_SPI1SendByte(0xFF);
    
    /* �����MAC��MII�Ĵ�������һ����ȡ���ֽ���Ч������Ϣ�����ڵ�ַ�����λ*/
    if(address & 0x80)
    {
        /* �ٴ�ͨ��SPI��ȡ���� */
        dat = BSP_SPI1SendByte(0xFF);
    }
    
    /* CS���� ��ֹENC28J60 */
    ENC28J60_CSH();
    
    /* �������� */
    return dat;
}
/*
********************************************************************************
* �� �� ��: enc28j60_writeop
* ����˵��: д�Ĵ�������
* ��    ��: unsigned char op        ������
*           unsigned char address   �Ĵ�����ַ
*           unsigned char data      д������
* �� �� ֵ:                         ��
* ʹ��˵��: �ú���֧�ֵĲ�������: д���ƼĴ��� λ������ λ����1
* ���÷���: enc28j60_writeop
********************************************************************************
*/
void enc28j60_writeop(unsigned char op, unsigned char address, unsigned char data)
{
    unsigned char dat = 0;
    /* ʹ��ENC28J60 */							  	  
    ENC28J60_CSL();	     
    /* ͨ��SPI���� ������ͼĴ�����ַ */                 		
    dat = op | (address & ADDR_MASK);
    /* ͨ��SPI1�������� */
    BSP_SPI1SendByte(dat);
    /* ׼���Ĵ�����ֵ */				  
    dat = data;
    /* ͨ��SPI�������� */
    BSP_SPI1SendByte(dat);
    /* ��ֹENC28J60 */				 
    ENC28J60_CSH();	
}

/*
********************************************************************************
* �� �� ��: enc28j60_setbank
* ����˵��: �趨�Ĵ����洢����
* ��    ��: unsigned char address   �Ĵ�����ַ
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/ 
void enc28j60_setbank(unsigned char address)
{
    /* ���㱾�μĴ�����ַ�ڴ�ȡ�����λ�� */
    if((address & BANK_MASK) != enc28j60_bank)
    {
        /* ���ECON1��BSEL1 BSEL0 ��������ֲ�15ҳ */
        enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        /* ��ע��Ĵ�����ַ�ĺ궨�壬bit6 bit5����Ĵ����洢����λ�� */
        enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        /* ����ȷ����ǰ�Ĵ����洢���� */
        enc28j60_bank = (address & BANK_MASK);
    }
}
/*
********************************************************************************
* �� �� ��: enc28j60_write
* ����˵��: д�Ĵ���
* ��    ��: unsigned char address   �Ĵ�����ַ
*           unsigned char data      �Ĵ�����ֵ
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_write(unsigned char address, unsigned char data)
{
    /* �趨�Ĵ�����ַ���� */
    enc28j60_setbank(address);
    /* д�Ĵ���ֵ ����д�Ĵ�������͵�ַ */
    enc28j60_writeop(ENC28J60_WRITE_CTRL_REG, address, data);
}

/*
********************************************************************************
* �� �� ��: enc28j60_read
* ����˵��: ��ȡ�Ĵ���ֵ
* ��    ��: unsigned char address   �Ĵ�����ַ
* �� �� ֵ:                         �Ĵ���ֵ
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
unsigned char enc28j60_read(unsigned char address)
{
    /* �趨�Ĵ�����ַ���� */
    enc28j60_setbank(address);
    /* ��ȡ�Ĵ���ֵ ���Ͷ��Ĵ�������͵�ַ */
    return enc28j60_readop(ENC28J60_READ_CTRL_REG, address);
}
/*
********************************************************************************
* �� �� ��: enc28j60_writephy
* ����˵��: д����Ĵ���
* ��    ��: unsigned char address   ����Ĵ�����ַ
*           unsigned int data       ����Ĵ�����ֵ ����Ĵ�����Ϊ16λ��
* �� �� ֵ:                         ��
* ʹ��˵��: PHY�Ĵ�������ͨ��SPI����ֱ�ӷ��ʣ�����ͨ��һ������ļĴ���������
*           ��������ֲ�19ҳ
* ���÷���: 
********************************************************************************
*/
void enc28j60_writephy(unsigned char address, unsigned int data)
{
    /* ��MIREGADRд���ַ ��������ֲ�19ҳ*/
    enc28j60_write(MIREGADR, address);
    /* д���8λ���� */
    enc28j60_write (MIWRL, data);
    /* д���8λ���� */
    enc28j60_write(MIWRH, data>>8);
    /* �ȴ�PHY�Ĵ���д����� */
    while( enc28j60_read(MISTAT) & MISTAT_BUSY );
}

void en28j60_init(struct _en28j60_obj* en28) {
   /* GPIO�ṹ�� */
    GPIO_InitTypeDef  GPIO_InitStructure; 
    /* SPI�ṹ�� */
    SPI_InitTypeDef SPI_InitStructure; 
    
    /* ʹ��APB2�����ʱ�� */
    /* ʹ��SPIʱ�ӣ�ʹ��GPIOAʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |\
        RCC_APB2Periph_GPIOA ,ENABLE );
    
    /* SPI1 SCK@GPIOA.5 SPI1 MOSI@GPIOA.7 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  |  GPIO_Pin_7; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /* ����������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    /* SPI1 MISO@GPIOA.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /* �������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 
    
    
    /* ˫��˫��ȫ˫�� */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	/* ����ģʽ */
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
    /* 8λ֡�ṹ */
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
    /* ʱ�ӿ���ʱΪ�� */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;        
    /* ��1�������ز������� */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;      
    /* MSS �˿�������� */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;         
    /* SPIʱ�� 72Mhz / 8 = 9M */ 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
    /* ���ݴ����λ��ǰ */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    /* ��ʼ��SPI1 */
    SPI_Init(SPI1, &SPI_InitStructure);
    
	/* ��ʹ��SPI�ڵ�SS������� GPIOA.4 */
	SPI_SSOutputCmd(SPI1,ENABLE);
    /* ʹ��SPI1 */
    SPI_Cmd(SPI1, ENABLE); 

	
    /* ��GPIOAʱ�� */
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
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);//����Ч
    
    // ENC28J60��������ж����ţ�����δʹ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	         	 	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);	
    
    /* ��ֹENC28J60 */
    ENC28J60_CSH();
    /* ENC28J60�����λ �ú������ԸĽ� */
    enc28j60_writeop(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET); 
    /* ��ѯESTAT.CLKRDYλ */
    while(!(enc28j60_read(ESTAT) & ESTAT_CLKRDY));
    
    /* ���ý��ջ�������ʼ��ַ �ñ�������ÿ�ζ�ȡ������ʱ������һ�������׵�ַ */
    next_pack_ptr = RXSTART_INIT;
    
    /* ���ý��ջ����� ��ʼָ�� */
    enc28j60_write(ERXSTL, RXSTART_INIT & 0xFF);
    enc28j60_write(ERXSTH, RXSTART_INIT >> 8);
    
    /* ���ý��ջ����� ��ָ�� */ 
    enc28j60_write(ERXRDPTL, RXSTART_INIT&0xFF);
    enc28j60_write(ERXRDPTH, RXSTART_INIT>>8);
    
    /* ���ý��ջ����� ����ָ�� */
    enc28j60_write(ERXNDL, RXSTOP_INIT&0xFF);
    enc28j60_write(ERXNDH, RXSTOP_INIT>>8);
    
    /* ���÷��ͻ����� ��ʼָ�� */
    enc28j60_write(ETXSTL, TXSTART_INIT&0xFF);
    enc28j60_write(ETXSTH, TXSTART_INIT>>8);
    /* ���÷��ͻ����� ����ָ�� */
    enc28j60_write(ETXNDL, TXSTOP_INIT&0xFF);
    enc28j60_write(ETXNDH, TXSTOP_INIT>>8);
    
    /* ʹ�ܵ������� ʹ��CRCУ�� ʹ�� ��ʽƥ���Զ�����*/
    enc28j60_write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
    enc28j60_write(EPMM0, 0x3f);
    enc28j60_write(EPMM1, 0x30);
    enc28j60_write(EPMCSL, 0xf9);
    enc28j60_write(EPMCSH, 0xf7);
    
    /* ʹ��MAC���� ����MAC������ͣ����֡ �����յ���ͣ����֡ʱֹͣ����*/
    /* �����ֲ�34ҳ */
    enc28j60_write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    
    /* �˳���λ״̬ */
    enc28j60_write(MACON2, 0x00);
    
    /* ��0������ж�֡��60�ֽڳ� ��׷��һ��CRC ����CRCʹ�� ֡����У��ʹ�� MACȫ˫��ʹ��*/
    /* ��ʾ ����ENC28J60��֧��802.3���Զ�Э�̻��ƣ� ���ԶԶ˵����翨��Ҫǿ������Ϊȫ˫�� */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX);
    
    
    /* ����Ĭ��ֵ */
    enc28j60_write(MAIPGL, 0x12);
    /* ����Ĭ��ֵ */
    enc28j60_write(MAIPGH, 0x0C);
    /* ����Ĭ��ֵ */
    enc28j60_write(MABBIPG, 0x15);
    
    /* ���֡���� */
    enc28j60_write(MAMXFLL, MAX_FRAMELEN & 0xFF);	
    enc28j60_write(MAMXFLH, MAX_FRAMELEN >> 8);
    
    /* д��MAC��ַ */
    enc28j60_write(MAADR5, en28->mac[0]);	
    enc28j60_write(MAADR4, en28->mac[1]);
    enc28j60_write(MAADR3, en28->mac[2]);
    enc28j60_write(MAADR2, en28->mac[3]);
    enc28j60_write(MAADR1, en28->mac[4]);
    enc28j60_write(MAADR0, en28->mac[5]);
    
    /* ����PHYΪȫ˫��  LEDBΪ������ */
    enc28j60_writephy(PHCON1, PHCON1_PDPXMD);
    
    /* LED״̬ */
    enc28j60_writephy(PHLCON,0x0476);	
    
    /* ��˫���ػ���ֹ */
    enc28j60_writephy(PHCON2, PHCON2_HDLDIS);
    
    /* ����BANK0 */	
    enc28j60_setbank(ECON1);
    
    /* ʹ���ж� ȫ���ж� �����ж� ���մ����ж� */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE|EIE_RXERIE);
    
    /* ����ʹ��λ */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

uint16_t en28j60_packet_receive(struct _en28j60_obj* en28,uint8_t * packet,uint16_t maxlen) {
	unsigned int rxstat;
    unsigned int len;
    
    /* �Ƿ��յ���̫�����ݰ� */
    if( enc28j60_read(EPKTCNT) == 0 )
    {
        return(0);
    }
    
    /* ���ý��ջ�������ָ�� */
    enc28j60_write(ERDPTL, (next_pack_ptr));
    enc28j60_write(ERDPTH, (next_pack_ptr)>>8);
    
    /* �������ݰ��ṹʾ�� �����ֲ�43ҳ */
    
    /* ����һ������ָ�� */
    next_pack_ptr  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
    next_pack_ptr |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0)<<8;
    
    /* �����ĳ��� */
    len  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
    len |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0)<<8;
    
    /* ɾ��CRC���� */
    len-= 4; 	
    
    /* ��ȡ����״̬ */
    rxstat  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
    rxstat |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0) << 8;
    
    /* ���Ƽ����ĳ���	*/  
    if (len > maxlen-1)
    {
        len = maxlen-1;
    }
    /* ���CRC�ͷ��Ŵ��� */
    /* ERXFCON.CRCEN��Ĭ�����á�ͨ�����ǲ���Ҫ��� */
    if ((rxstat & 0x80)==0)
    {
        len=0;
    }
    else
    {
        /* �ӽ��ջ������и������ݰ� */
        enc28j60_readBuffer( packet,len );
    }
    
    /* �ƶ����ջ����� ��ָ��*/
    enc28j60_write(ERXRDPTL, (next_pack_ptr));
    enc28j60_write(ERXRDPTH, (next_pack_ptr)>>8);
    
    /* ���ݰ��ݼ� */
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    
    /* ���س��� */
    return(len);
}

void en28j60_packet_send(struct _en28j60_obj* en28,uint8_t * packet,uint16_t len) {
  /* ��ѯ�����߼���λλ */
    while((enc28j60_read(ECON1) & ECON1_TXRTS)!= 0);
    
    /* ���÷��ͻ�������ʼ��ַ */    
    enc28j60_write(EWRPTL, TXSTART_INIT & 0xFF);
    enc28j60_write(EWRPTH, TXSTART_INIT >> 8);
    
    /* ���÷��ͻ�����������ַ ��ֵ��Ӧ�������ݰ�����*/   
    enc28j60_write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
    enc28j60_write(ETXNDH, (TXSTART_INIT + len) >>8);
    
    /* ����֮ǰ���Ϳ��ư���ʽ�� ???????*/
    enc28j60_writeop(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    
    /* ͨ��ENC28J60�������ݰ� */
    enc28j60_writebuf( packet,len );
    
    /* ��ʼ����*/
    enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
    
    /* ��λ�����߼������⡣�μ� Rev. B4 Silicon Errata point 12. */
    if( (enc28j60_read(EIR) & EIR_TXERIF) )
    {
        enc28j60_setbank(ECON1);
        enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }
}

void en28j60_set_mac(struct _en28j60_obj* en28) {
	/* д��MAC��ַ */
    enc28j60_write(MAADR5, en28->mac[0]);	
    enc28j60_write(MAADR4, en28->mac[1]);
    enc28j60_write(MAADR3, en28->mac[2]);
    enc28j60_write(MAADR2, en28->mac[3]);
    enc28j60_write(MAADR1, en28->mac[4]);
    enc28j60_write(MAADR0, en28->mac[5]);
}
