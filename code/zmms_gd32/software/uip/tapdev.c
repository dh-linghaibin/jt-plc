#include "tapdev.h"
#include "uip.h"
#include "uip_arp.h"
#include "enc28j60.h"

static en28j60_obj enc28 = {
	{0x05,0x01,0x35,0x01,0x00,0x01},//MAC��ַ    
	&en28j60_init,
	&en28j60_packet_receive,
	&en28j60_packet_send,
	&en28j60_set_mac,
};

struct uip_eth_addr uip_mac;   
/*---------------------------------------------------------------------------*/
/* ��������Ӳ��������IP��ַ */
void tapdev_init(void) { 
    unsigned char i;     
    /*��ʼ�� enc28j60*/
	enc28.init(&enc28);
    
    for (i = 0; i < 6; i++) {
        uip_mac.addr[i] = enc28.mac[i];
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
uint16_t tapdev_read(void) {   
    return  enc28.packet_receive(&enc28,uip_buf, 1500);
}
/****************************************************************************
* ��    �ƣ�void tapdev_send(void)
* ��    �ܣ�                                                                         
* ��ڲ���������һ������
* ���ڲ���: 
* ˵    ����
* ���÷�����
****************************************************************************/ 
void tapdev_send(void) {
    enc28.packet_send(&enc28,uip_buf,uip_len);
}

