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
#define REG_INPUT_START       0					// 输入寄存器起始地址
#define REG_INPUT_NREGS       16                    // 输入寄存器数量

#define REG_HOLDING_START     0                // 保持寄存器起始地址
#define REG_HOLDING_NREGS     16                    // 保持寄存器数量

#define REG_COILS_START       0                // 线圈起始地址
#define REG_COILS_SIZE        800              // 线圈数量

#define REG_DISCRETE_START    0                // 开关寄存器起始地址
#define REG_DISCRETE_SIZE     16                    // 开关寄存器数量

static uint8_t modbus_coil[100];
 uint8_t modbus_coil_r[100];

void modbus_init(struct _modbus_obj* modbus) {
	uint8_t i;     
    /*初始化 enc28j60*/
	modbus->enc28.init(&modbus->enc28);
    for (i = 0; i < 6; i++) {
        modbus->uip_mac.addr[i] = modbus->enc28.mac[i];
    }
    /* 设定mac地址*/
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
	/* 轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个 */
	for(uint8_t i = 0; i < UIP_UDP_CONNS; i++) {
		uip_udp_periodic(i);	/*处理UDP通信事件 */
		/* 如果上面的函数调用导致数据应该被发送出去，全局变量uip_len设定值> 0 */
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

//通信程序状态字(用户可以自己定义)  
enum
{
	STATE_CMD		= 0,	//命令接收状态 
	STATE_TX_TEST	= 1,	//连续发送数据包状态(速度测试)  
	STATE_RX_TEST	= 2		//连续接收数据包状态(速度测试)  
};	 
//定义 uip_tcp_appstate_t 数据类型，用户可以添加应用程序需要用到
//成员变量。不要更改结构体类型的名字，因为这个类型名会被uip引用。
//uip.h 中定义的 	struct uip_conn  结构体中引用了 uip_tcp_appstate_t		  
struct tcp_demo_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 

//定义应用程序回调函数 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_demo_appcall //定义回调函数为 tcp_demo_appcall 
#endif
/////////////////////////////////////TCP SERVER/////////////////////////////////////
extern u8 tcp_server_databuf[];   		//发送数据缓存	 
extern u8 tcp_server_sta;				//服务端状态   
//tcp server 函数
void tcp_server_aborted(void);
void tcp_server_timedout(void);
void tcp_server_closed(void);
void tcp_server_connected(void);
void tcp_server_newdata(void);
void tcp_server_acked(void);
void tcp_server_senddata(void);
			    
u8 tcp_server_databuf[200];   	//发送数据缓存	  
u8 tcp_server_sta;				//服务端状态
//[7]:0,无连接;1,已经连接;
//[6]:0,无数据;1,收到客户端数据
//[5]:0,无数据;1,有数据需要发送

 	   
//这是一个TCP 服务器应用回调函数。
//该函数通过UIP_APPCALL(tcp_demo_appcall)调用,实现Web Server的功能.
//当uip事件发生时，UIP_APPCALL函数会被调用,根据所属端口(1200),确定是否执行该函数。
//例如 : 当一个TCP连接被创建时、有新的数据到达、数据已经被应答、数据需要重发等事件
void tcp_server_demo_appcall(void)
{
 	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	if(uip_aborted())tcp_server_aborted();		//连接终止
 	if(uip_timedout())tcp_server_timedout();	//连接超时   
	if(uip_closed())tcp_server_closed();		//连接关闭	   
 	if(uip_connected())tcp_server_connected();	//连接成功	    
	if(uip_acked())tcp_server_acked();			//发送的数据成功送达 
	//接收到一个新的TCP数据包 
	if (uip_newdata())//收到客户端发过来的数据
	{
		if((tcp_server_sta&(1<<6))==0)//还未收到数据
		{
			if(uip_len>199)
			{		   
				((u8*)uip_appdata)[199]=0;
			}		    
	    	strcpy((char*)tcp_server_databuf,uip_appdata);				   	  		  
			tcp_server_sta|=1<<6;//表示收到客户端数据
		}
	}else if(tcp_server_sta&(1<<5))//有数据需要发送
	{
		s->textptr=tcp_server_databuf;
		s->textlen=strlen((const char*)tcp_server_databuf);
		tcp_server_sta&=~(1<<5);//清除标记
	}   
	//当需要重发、新数据到达、数据包送达、连接建立时，通知uip发送数据 
	if(uip_rexmit()||uip_newdata()||uip_acked()||uip_connected()||uip_poll())
	{
		tcp_server_senddata();
	}
}	  
//终止连接				    
void tcp_server_aborted(void)
{
	tcp_server_sta&=~(1<<7);	//标志没有连接
	uip_log("tcp_server aborted!\r\n");//打印log
}
//连接超时
void tcp_server_timedout(void)
{
	tcp_server_sta&=~(1<<7);	//标志没有连接
	uip_log("tcp_server timeout!\r\n");//打印log
}
//连接关闭
void tcp_server_closed(void)
{
	tcp_server_sta&=~(1<<7);	//标志没有连接
	uip_log("tcp_server closed!\r\n");//打印log
}
//连接建立
void tcp_server_connected(void)
{								  
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//uip_conn结构体有一个"appstate"字段指向应用程序自定义的结构体。
	//声明一个s指针，是为了便于使用。
 	//不需要再单独为每个uip_conn分配内存，这个已经在uip中分配好了。
	//在uip.c 中 的相关代码如下：
	//		struct uip_conn *uip_conn;
	//		struct uip_conn uip_conns[UIP_CONNS]; //UIP_CONNS缺省=10
	//定义了1个连接的数组，支持同时创建几个连接。
	//uip_conn是一个全局的指针，指向当前的tcp或udp连接。
	tcp_server_sta|=1<<7;		//标志连接成功
  	uip_log("tcp_server connected!\r\n");//打印log
	s->state=STATE_CMD; 		//指令状态
	s->textlen=0;
	s->textptr="Connect to ALIENTEK STM32 Board Successfully!\r\n";
	s->textlen=strlen((char *)s->textptr);
} 
//发送的数据成功送达
void tcp_server_acked(void)
{						    	 
	struct tcp_demo_appstate *s=(struct tcp_demo_appstate *)&uip_conn->appstate;
	s->textlen=0;//发送清零
	uip_log("tcp_server acked!\r\n");//表示成功发送		 
}
//发送数据给客户端
void tcp_server_senddata(void)
{
	struct tcp_demo_appstate *s = (struct tcp_demo_appstate *)&uip_conn->appstate;
	//s->textptr : 发送的数据包缓冲区指针
	//s->textlen ：数据包的大小（单位字节）		   
	if(s->textlen>0)uip_send(s->textptr, s->textlen);//发送TCP数据包	 
}




