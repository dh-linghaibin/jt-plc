/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "modbus.h"
#include "uip.h"
#include "uip_arp.h"
#include "mb.h"
#include "mbutils.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
#define REG_INPUT_START       0					// ����Ĵ�����ʼ��ַ
#define REG_INPUT_NREGS       16                    // ����Ĵ�������

#define REG_HOLDING_START     0                // ���ּĴ�����ʼ��ַ
#define REG_HOLDING_NREGS     16                    // ���ּĴ�������

#define REG_COILS_START       0                // ��Ȧ��ʼ��ַ
#define REG_COILS_SIZE        800              // ��Ȧ����

#define REG_DISCRETE_START    0                // ���ؼĴ�����ʼ��ַ
#define REG_DISCRETE_SIZE     16                    // ���ؼĴ�������

static uint8_t modbus_coil[100];
 uint8_t modbus_coil_r[100];

void modbus_init(struct _modbus_obj* modbus) {
	uint8_t i;     
    /*��ʼ�� enc28j60*/
	modbus->enc28.init(&modbus->enc28);
    for (i = 0; i < 6; i++) {
        modbus->uip_mac.addr[i] = modbus->enc28.mac[i];
    }
    /* �趨mac��ַ*/
    uip_setethaddr(modbus->uip_mac);

	uip_ipaddr_t ipaddr;
	uip_ipaddr(ipaddr, modbus->ipv4_ip[0],modbus->ipv4_ip[1],modbus->ipv4_ip[2],modbus->ipv4_ip[3]);	
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, modbus->gateway_ip[0],modbus->gateway_ip[1],modbus->gateway_ip[2],modbus->gateway_ip[3]);	
	uip_setdraddr(ipaddr);
    uip_ipaddr(ipaddr, modbus->mask_ip[0],modbus->mask_ip[1],modbus->mask_ip[2],modbus->mask_ip[3]);		 
    uip_setnetmask(ipaddr);	
	eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);      
	eMBEnable();
}


void modbus_loop(struct _modbus_obj* modbus) {
	eMBPoll();
	uip_len = modbus->enc28.packet_receive(&modbus->enc28,uip_buf, 1500);
	if(uip_len > 0)	{
		if(BUF->type == htons(UIP_ETHTYPE_IP)) {
			uip_arp_ipin();
			uip_input();
			if (uip_len > 0) {
				uip_arp_out();
				modbus->enc28.packet_send(&modbus->enc28,uip_buf,uip_len);
			}
		} else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {
			uip_arp_arpin();
			if (uip_len > 0) {
				modbus->enc28.packet_send(&modbus->enc28,uip_buf,uip_len);
			}
		}
	}
}
void modbus_heart(struct _modbus_obj* modbus) {
	for(uint8_t i = 0; i < UIP_CONNS; i++) {
		uip_periodic(i);		
		if(uip_len > 0) {
			uip_arp_out();
			modbus->enc28.packet_send(&modbus->enc28,uip_buf,uip_len);
		}
	} 
	#if UIP_UDP
	/* ��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10�� */
	for(uint8_t i = 0; i < UIP_UDP_CONNS; i++) {
		uip_udp_periodic(i);	/*����UDPͨ���¼� */
		/* �������ĺ������õ�������Ӧ�ñ����ͳ�ȥ��ȫ�ֱ���uip_len�趨ֵ> 0 */
		if(uip_len > 0) {
			uip_arp_out();
			modbus->enc28.packet_send(&modbus->enc28,uip_buf,uip_len);
		}
	}
	#endif /* UIP_UDP */  
}

void modbus_set_coil(struct _modbus_obj* modbus,uint16_t num,uint8_t val) {
//	uint16_t r_num = num/8;
//	uint16_t r_bit = num%8;
//	if(val == 0) {
//		modbus_coil[r_num] &= ~(1<<r_bit);
//	} else {	
//		modbus_coil[r_num] |= 1<<r_bit;
//	}
//	modbus_coil_r[r_num] = modbus_coil[r_num];
	modbus_coil[num] = val;
	//modbus_coil_r[num] = val;
}

void modbus_set_input(struct _modbus_obj* modbus,uint16_t num,uint8_t val) {
	modbus_coil[num] = val;
	//modbus_coil_r[num] = val;
}

modbus_coil_obj modbus_up_coil(struct _modbus_obj* modbus,uint8_t adr) {
	modbus_coil_obj coil;
	coil.id = 0xffff;
	if(modbus_coil[adr] != modbus_coil_r[adr]) {
		coil.id = adr;
		coil.val = modbus_coil_r[adr];
		return coil;
	}
	return coil;
}

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    if( ( (int16_t) usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) ) {
        iRegIndex = ( int )( usAddress);
        while( usNRegs > 0 ) {
            *pucRegBuffer++ = ( unsigned char )( 0xff );
            *pucRegBuffer++ = ( unsigned char )( 0xff );
            iRegIndex++;
            usNRegs--;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
        && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) ) {
        iRegIndex = ( int )( usAddress);
        switch ( eMode ) {
        case MB_REG_READ:            
            while( usNRegs > 0 ) {
                *pucRegBuffer++ = ( unsigned char )( 0x02 );
                *pucRegBuffer++ = ( unsigned char )( 0x00 );
                iRegIndex++;
                usNRegs--;
            }
            break;
        case MB_REG_WRITE:
            while( usNRegs > 0 ) {
//                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
//                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    
    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNCoils = ( short )usNCoils;
    unsigned short  usBitOffset;
    if( ( (int16_t)usAddress >= REG_COILS_START ) &&
       ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) ) {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode ) {
        case MB_REG_READ:
            while( iNCoils > 0 ) {
                *pucRegBuffer++ = modbus_coil[usBitOffset];
                iNCoils -= 8;
                usBitOffset += 1;
            }
            break;
        case MB_REG_WRITE:
            while( iNCoils > 0 ) {
				uint16_t r_num = usBitOffset/8;
				uint16_t r_bit = usBitOffset%8;
				if((*pucRegBuffer++) == 0) {
					modbus_coil_r[r_num] &= ~(1<<r_bit);
				} else {	
					modbus_coil_r[r_num] |= 1<<r_bit;
				}
                iNCoils -= 8;
                usBitOffset += 8;
            }
            break;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNDiscrete = ( short )usNDiscrete;
    unsigned short  usBitOffset;
    if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
       ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) ) {
        usBitOffset = ( unsigned short )( usAddress - REG_DISCRETE_START );
        
        while( iNDiscrete > 0 ) {
            *pucRegBuffer++ = 0x04;
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}


#include <string.h>
#include <stdio.h>

#define uip_log(...)  printf(__VA_ARGS__)

//ͨ�ų���״̬��(�û������Լ�����)  
enum
{
	STATE_CMD		= 0,	//�������״̬ 
	STATE_TX_TEST	= 1,	//�����������ݰ�״̬(�ٶȲ���)  
	STATE_RX_TEST	= 2		//�����������ݰ�״̬(�ٶȲ���)  
};	 
//���� uip_tcp_appstate_t �������ͣ��û��������Ӧ�ó�����Ҫ�õ�
//��Ա��������Ҫ���Ľṹ�����͵����֣���Ϊ����������ᱻuip���á�
//uip.h �ж���� 	struct uip_conn  �ṹ���������� uip_tcp_appstate_t		  
struct tcp_demo_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 

//����Ӧ�ó���ص����� 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_demo_appcall //����ص�����Ϊ tcp_demo_appcall 
#endif
/////////////////////////////////////TCP SERVER/////////////////////////////////////
extern u8 tcp_server_databuf[];   		//�������ݻ���	 
extern u8 tcp_server_sta;				//�����״̬   
//tcp server ����
void tcp_server_aborted(void);
void tcp_server_timedout(void);
void tcp_server_closed(void);
void tcp_server_connected(void);
void tcp_server_newdata(void);
void tcp_server_acked(void);
void tcp_server_senddata(void);
			    
u8 tcp_server_databuf[200];   	//�������ݻ���	  
u8 tcp_server_sta;				//�����״̬
//[7]:0,������;1,�Ѿ�����;
//[6]:0,������;1,�յ��ͻ�������
//[5]:0,������;1,��������Ҫ����

 	   
//����һ��TCP ������Ӧ�ûص�������
//�ú���ͨ��UIP_APPCALL(tcp_demo_appcall)����,ʵ��Web Server�Ĺ���.
//��uip�¼�����ʱ��UIP_APPCALL�����ᱻ����,���������˿�(1200),ȷ���Ƿ�ִ�иú�����
//���� : ��һ��TCP���ӱ�����ʱ�����µ����ݵ�������Ѿ���Ӧ��������Ҫ�ط����¼�
void tcp_server_demo_appcall(void)
{
 	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	if(uip_aborted())tcp_server_aborted();		//������ֹ
 	if(uip_timedout())tcp_server_timedout();	//���ӳ�ʱ   
	if(uip_closed())tcp_server_closed();		//���ӹر�	   
 	if(uip_connected())tcp_server_connected();	//���ӳɹ�	    
	if(uip_acked())tcp_server_acked();			//���͵����ݳɹ��ʹ� 
	//���յ�һ���µ�TCP���ݰ� 
	if (uip_newdata())//�յ��ͻ��˷�����������
	{
		if((tcp_server_sta&(1<<6))==0)//��δ�յ�����
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
	    	strcpy((char*)tcp_server_databuf,uip_appdata);				   	  		  
			tcp_server_sta|=1<<6;//��ʾ�յ��ͻ�������
		}
	}else if(tcp_server_sta&(1<<5))//��������Ҫ����
	{
		s->textptr=tcp_server_databuf;
		s->textlen=strlen((const char*)tcp_server_databuf);
		tcp_server_sta&=~(1<<5);//������
	}   
	//����Ҫ�ط��������ݵ�����ݰ��ʹ���ӽ���ʱ��֪ͨuip�������� 
	if(uip_rexmit()||uip_newdata()||uip_acked()||uip_connected()||uip_poll())
	{
		tcp_server_senddata();
	}
}	  
//��ֹ����				    
void tcp_server_aborted(void)
{
	tcp_server_sta&=~(1<<7);	//��־û������
	uip_log("tcp_server aborted!\r\n");//��ӡlog
}
//���ӳ�ʱ
void tcp_server_timedout(void)
{
	tcp_server_sta&=~(1<<7);	//��־û������
	uip_log("tcp_server timeout!\r\n");//��ӡlog
}
//���ӹر�
void tcp_server_closed(void)
{
	tcp_server_sta&=~(1<<7);	//��־û������
	uip_log("tcp_server closed!\r\n");//��ӡlog
}
//���ӽ���
void tcp_server_connected(void)
{								  
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//uip_conn�ṹ����һ��"appstate"�ֶ�ָ��Ӧ�ó����Զ���Ľṹ�塣
	//����һ��sָ�룬��Ϊ�˱���ʹ�á�
 	//����Ҫ�ٵ���Ϊÿ��uip_conn�����ڴ棬����Ѿ���uip�з�����ˡ�
	//��uip.c �� ����ش������£�
	//		struct uip_conn *uip_conn;
	//		struct uip_conn uip_conns[UIP_CONNS]; //UIP_CONNSȱʡ=10
	//������1�����ӵ����飬֧��ͬʱ�����������ӡ�
	//uip_conn��һ��ȫ�ֵ�ָ�룬ָ��ǰ��tcp��udp���ӡ�
	tcp_server_sta|=1<<7;		//��־���ӳɹ�
  	uip_log("tcp_server connected!\r\n");//��ӡlog
	s->state=STATE_CMD; 		//ָ��״̬
	s->textlen=0;
	s->textptr="Connect to ALIENTEK STM32 Board Successfully!\r\n";
	s->textlen=strlen((char *)s->textptr);
} 
//���͵����ݳɹ��ʹ�
void tcp_server_acked(void)
{						    	 
	struct tcp_demo_appstate *s=(struct tcp_demo_appstate *)&uip_conn->appstate;
	s->textlen=0;//��������
	uip_log("tcp_server acked!\r\n");//��ʾ�ɹ�����		 
}
//�������ݸ��ͻ���
void tcp_server_senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//s->textptr : ���͵����ݰ�������ָ��
	//s->textlen �����ݰ��Ĵ�С����λ�ֽڣ�		   
	if(s->textlen>0)uip_send(s->textptr, s->textlen);//����TCP���ݰ�	 
}




