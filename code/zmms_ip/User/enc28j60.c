
#include  "enc28j60.h"
#include  "uip.h" 
#include  "bsp_spi1.h"

/* �洢�����  ENC28J60 ����Bank0��Bank3 4���洢�� ��Ҫͨ��ECON1�Ĵ���ѡ��*/
static unsigned char enc28j60_bank;
/* ��һ�����ݰ�ָ�룬��������ֲ�P43 ͼ7-3 */
static unsigned int next_pack_ptr;

#define 	ENC28J60_CSL()		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define 	ENC28J60_CSH()		GPIO_SetBits(GPIOA,GPIO_Pin_4)

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

/*
********************************************************************************
* �� �� ��: enc28j60_init
* ����˵��: ��ʼ��enc28j60
* ��    ��: unsigned char* mac_addr  mac��ַ����ָ��
* �� �� ֵ:                         ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_init(unsigned char* mac_addr) {
    GPIO_InitTypeDef GPIO_InitStructure;  
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
*           unsigned char* packet  ���ݰ�ָ��
* �� �� ֵ: ��
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
void enc28j60_packet_send(unsigned char* packet,unsigned int len )
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
*           unsigned char* packet     ���ݰ�ָ��
* �� �� ֵ:                           �������ݰ�����
* ʹ��˵��: 
* ���÷���: 
********************************************************************************
*/
unsigned int enc28j60_packet_receive(unsigned char* packet,unsigned int maxlen )
{
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

