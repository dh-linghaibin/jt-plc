/* ����ͷ�ļ� *****************************************************************/
#include "l_enc28j60.h"
/* ˽���������� ***************************************************************/
/* ˽�г����� *****************************************************************/
/* ˽�ж����� *****************************************************************/
/* ˽�б��� *******************************************************************/
/* ȫ�ֱ��� */
/* ˽�к������� ***************************************************************/
/* ˽�к���  ******************************************************************/
/* �洢�����  ENC28J60 ����Bank0��Bank3 4���洢�� ��Ҫͨ��ECON1�Ĵ���ѡ��*/
static uint8_t enc28j60_bank;
/* ��һ�����ݰ�ָ�룬��������ֲ�P43 ͼ7-3 */
static uint16_t next_pack_ptr;

#define ENC28J60_CSL()  GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define ENC28J60_CSH()  GPIO_SetBits(GPIOA,GPIO_Pin_4)

uint8_t spi_send_data(uint8_t byte) {
	/* �ȴ����ͻ���Ĵ���Ϊ�� */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	/* �������� */
	SPI_I2S_SendData(SPI1, byte);
	/* �ȴ����ջ���Ĵ���Ϊ�ǿ� */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

uint8_t spi_receive_data(void) {
	/* �ȴ����ͻ���Ĵ���Ϊ�� */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	/* ��������,ͨ������0xFF,��÷������� */
	SPI_I2S_SendData(SPI1, 0xFF);
	/* �ȴ����ջ���Ĵ���Ϊ�ǿ� */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	/* ���ش�SPIͨ���н��յ������� */
	return SPI_I2S_ReceiveData(SPI1);
}

/*
********************************************************************************
* �� �� ��: enc28j60_readop
* ����˵��: ���Ĵ�������
* ��    ��: uint8_t op        ������
*           uint8_t address   �Ĵ�����ַ
* �� �� ֵ:                         ��������
* ʹ��˵��: �ú���֧�ֵĲ�����ֻ�����ƼĴ��� ��������
* ���÷���: enc28j60_readop
********************************************************************************
*/

uint8_t enc28j60_readop(uint8_t op, uint8_t address) {
    uint8_t dat = 0;
    /* CS���� ʹ��ENC28J60 */
    ENC28J60_CSL();
    /* ������͵�ַ */
    dat = op | (address & ADDR_MASK);
    /* ͨ��SPIд����*/
    spi_send_data(dat);
    /* ͨ��SPI�������� */
    dat = spi_send_data(0xFF);
    
    /* �����MAC��MII�Ĵ�������һ����ȡ���ֽ���Ч������Ϣ�����ڵ�ַ�����λ*/
    if(address & 0x80) {
        /* �ٴ�ͨ��SPI��ȡ���� */
        dat = spi_send_data(0xFF);
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
* ��    ��: uint8_t op        ������
*           uint8_t address   �Ĵ�����ַ
*           uint8_t data      д������
* �� �� ֵ:                         ��
* ʹ��˵��: �ú���֧�ֵĲ�������: д���ƼĴ��� λ������ λ����1
* ���÷���: enc28j60_writeop
********************************************************************************
*/
void enc28j60_writeop(uint8_t op, uint8_t address, uint8_t data) {
    uint8_t dat = 0;
    /* ʹ��ENC28J60 */							  	  
    ENC28J60_CSL();	     
    /* ͨ��SPI���� ������ͼĴ�����ַ */                 		
    dat = op | (address & ADDR_MASK);
    /* ͨ��SPI1�������� */
    spi_send_data(dat);
    /* ׼���Ĵ�����ֵ */				  
    dat = data;
    /* ͨ��SPI�������� */
    spi_send_data(dat);
    /* ��ֹENC28J60 */				 
    ENC28J60_CSH();	
}

/*
********************************************************************************
* �� �� ��: enc28j60_readBuffer
* ����˵��: ��������
* ��    ��: unsigned int len        ��ȡ����
*           uint8_t* data     ��ȡָ��
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_readBuffer(uint8_t* pdata,unsigned int len) {
    /* ʹ��ENC28J60 */
    ENC28J60_CSL();
    /* ͨ��SPI���Ͷ�ȡ����������*/
    spi_send_data(ENC28J60_READ_BUF_MEM);
    
    /* ѭ����ȡ */
    while(len) {
        len--;
        /* ��ȡ���� */
        *pdata = (uint8_t)spi_send_data(0);
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
*           uint8_t* data     ��ȡָ��
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_writebuf(uint8_t* pdata,unsigned int len ) {
    /* ʹ��ENC28J60 */
    ENC28J60_CSL();
    /* ͨ��SPI����дȡ����������*/
    spi_send_data(ENC28J60_WRITE_BUF_MEM);
    
    /* ѭ������ */
    while(len) {
        len--;
        /* �������� */
        spi_send_data(*pdata);
        /* ��ַָ���ۼ� */
        pdata++;
    }
    
    /* ��ֹENC28J60 */
    ENC28J60_CSH();
}

/*
********************************************************************************
* �� �� ��: enc28j60_setbank
* ����˵��: �趨�Ĵ����洢����
* ��    ��: uint8_t address   �Ĵ�����ַ
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/ 
void enc28j60_setbank(uint8_t address) {
    /* ���㱾�μĴ�����ַ�ڴ�ȡ�����λ�� */
    if((address & BANK_MASK) != enc28j60_bank) {
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
* �� �� ��: enc28j60_read
* ����˵��: ��ȡ�Ĵ���ֵ
* ��    ��: uint8_t address   �Ĵ�����ַ
* �� �� ֵ:                         �Ĵ���ֵ
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
uint8_t enc28j60_read(uint8_t address) {
    /* �趨�Ĵ�����ַ���� */
    enc28j60_setbank(address);
    /* ��ȡ�Ĵ���ֵ ���Ͷ��Ĵ�������͵�ַ */
    return enc28j60_readop(ENC28J60_READ_CTRL_REG, address);
}

/*
********************************************************************************
* �� �� ��: enc28j60_write
* ����˵��: д�Ĵ���
* ��    ��: uint8_t address   �Ĵ�����ַ
*           uint8_t data      �Ĵ�����ֵ
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_write(uint8_t address, uint8_t data) {
    /* �趨�Ĵ�����ַ���� */
    enc28j60_setbank(address);
    /* д�Ĵ���ֵ ����д�Ĵ�������͵�ַ */
    enc28j60_writeop(ENC28J60_WRITE_CTRL_REG, address, data);
}

/*
********************************************************************************
* �� �� ��: enc28j60_writephy
* ����˵��: д����Ĵ���
* ��    ��: uint8_t address   ����Ĵ�����ַ
*           unsigned int data       ����Ĵ�����ֵ ����Ĵ�����Ϊ16λ��
* �� �� ֵ:                         ��
* ʹ��˵��: PHY�Ĵ�������ͨ��SPI����ֱ�ӷ��ʣ�����ͨ��һ������ļĴ���������
*           ��������ֲ�19ҳ
* ���÷���: 
********************************************************************************
*/
void enc28j60_writephy(uint8_t address, unsigned int data) {
    /* ��MIREGADRд���ַ ��������ֲ�19ҳ*/
    enc28j60_write(MIREGADR, address);
    /* д���8λ���� */
    enc28j60_write (MIWRL, data);
    /* д���8λ���� */
    enc28j60_write(MIWRH, data>>8);
    /* �ȴ�PHY�Ĵ���д����� */
    while( enc28j60_read(MISTAT) & MISTAT_BUSY );
}

/*
********************************************************************************
* �� �� ��: enc28j60_init
* ����˵��: ��ʼ��enc28j60
* ��    ��: uint8_t* mac_addr  mac��ַ����ָ��
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_init(uint8_t* mac_addr) {
    GPIO_InitTypeDef GPIO_InitStructure;  
    SPI_InitTypeDef SPI_InitStructure; 
    
    /* ��GPIOAʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |\
        RCC_APB2Periph_GPIOA ,ENABLE );
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
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
    /* ���ݴ����λ��ǰ */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    /* ��ʼ��SPI1 */
    SPI_Init(SPI1, &SPI_InitStructure);
    
	/* ��ʹ��SPI�ڵ�SS������� GPIOA.4 */
	SPI_SSOutputCmd(SPI1,ENABLE);
    /* ʹ��SPI1 */
    SPI_Cmd(SPI1, ENABLE); 
    
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
    enc28j60_write(MAADR5, mac_addr[0]);	
    enc28j60_write(MAADR4, mac_addr[1]);
    enc28j60_write(MAADR3, mac_addr[2]);
    enc28j60_write(MAADR2, mac_addr[3]);
    enc28j60_write(MAADR1, mac_addr[4]);
    enc28j60_write(MAADR0, mac_addr[5]);
    
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

/*
********************************************************************************
* �� �� ��: enc28j60_packet_send
* ����˵��: ͨ��enc28j60�������ݰ�
* ��    ��: unsigned int len       ���ݰ�����
*           uint8_t* packet  ���ݰ�ָ��
* �� �� ֵ: ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_packet_send(uint8_t* packet,uint16_t len )
{
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

/*
********************************************************************************
* �� �� ��: enc28j60PacketReceive
* ����˵��: ͨ��enc28j60�������ݰ�
* ��    ��: unsigned int maxlen       �������ݰ���󳤶�ֵ
*           uint8_t* packet     ���ݰ�ָ��
* �� �� ֵ:                           �������ݰ�����
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
uint16_t enc28j60_packet_receive(uint8_t* packet,uint16_t maxlen )
{
    uint16_t rxstat;
    uint16_t len;
    /* �Ƿ��յ���̫�����ݰ� */
    if( enc28j60_read(EPKTCNT) == 0 ) {
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
    if (len > maxlen-1) {
        len = maxlen-1;
    }
    /* ���CRC�ͷ��Ŵ��� */
    /* ERXFCON.CRCEN��Ĭ�����á�ͨ�����ǲ���Ҫ��� */
    if ((rxstat & 0x80)==0) {
        len=0;
    } else {
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

