#include "tapdev.h"
#include "uip.h"
#include "uip_arp.h"

#include "enc28j60.h"

struct uip_eth_addr uip_mac;
static unsigned char ethernet_mac[6] = {0x04,0x02,0x35,0x00,0x00,0x01};  //MAC��ַ       
/*---------------------------------------------------------------------------*/
/* ��������Ӳ��������IP��ַ */
void tapdev_init(void)
{ 
    unsigned char i;     
    
    /*��ʼ�� enc28j60*/
    enc28j60_init(ethernet_mac);        
    
    for (i = 0; i < 6; i++)
    {
        uip_mac.addr[i] = ethernet_mac[i];
    }
    
    /* �趨mac��ַ*/
    uip_setethaddr(uip_mac);
    
}

/****************************************************************************
* ��    �ƣ�uint16_t tapdev_read(void)
* ��    �ܣ�                                                                         
* ��ڲ�������ȡһ������
* ���ڲ���: ���һ�����ݰ��յ��������ݰ����ȣ����ֽ�Ϊ��λ������Ϊ�㡣
* ˵    ����
* ���÷�����
****************************************************************************/ 
uint16_t tapdev_read(void)
{   
    return  enc28j60_packet_receive(uip_buf, 1500);
}
/****************************************************************************
* ��    �ƣ�void tapdev_send(void)
* ��    �ܣ�                                                                         
* ��ڲ���������һ������
* ���ڲ���: 
* ˵    ����
* ���÷�����
****************************************************************************/ 
void tapdev_send(void)
{
    enc28j60_packet_send(uip_buf,uip_len);
}

