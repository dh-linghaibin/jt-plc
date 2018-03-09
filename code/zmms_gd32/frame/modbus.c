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
#include "rtc.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	
#define REG_INPUT_START       0					// 输入寄存器起始地址
#define REG_INPUT_NREGS       800                    // 输入寄存器数量

#define REG_HOLDING_START     0                // 保持寄存器起始地址
#define REG_HOLDING_NREGS     800                    // 保持寄存器数量

#define REG_COILS_START       0                // 线圈起始地址
#define REG_COILS_SIZE        800              // 线圈数量

#define REG_DISCRETE_START    0                // 开关寄存器起始地址
#define REG_DISCRETE_SIZE     800                    // 开关寄存器数量

static uint8_t modbus_coil[100];
uint8_t modbus_coil_r[100];
uint8_t modbus_input[20];

int modbus_Holding[120];

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
	modbus_input[num] = val;
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

extern rtc_obj rtc;

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
                *pucRegBuffer++ = ( unsigned char )( modbus_Holding[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( modbus_Holding[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;
        case MB_REG_WRITE:
            while( usNRegs > 0 ) {
				modbus_Holding[iRegIndex] = *pucRegBuffer++ << 8;
				modbus_Holding[iRegIndex] |= *pucRegBuffer++;
				iRegIndex++;
				usNRegs--;
				if(iRegIndex >= 100) {
					rtc_t t_rtc;
					t_rtc.year = modbus_Holding[100];
                    t_rtc.month = modbus_Holding[101];
                    t_rtc.mday = modbus_Holding[102];
                    t_rtc.wday = modbus_Holding[103];
                    t_rtc.hour = modbus_Holding[104];
                    t_rtc.min = modbus_Holding[105];
                    t_rtc.sec = modbus_Holding[106];
					rtc.set(&rtc,t_rtc);
				}
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
		uint16_t adr_num = usBitOffset/8;
        switch ( eMode ) {
        case MB_REG_READ:
            while( iNCoils > 0 ) {
                *pucRegBuffer++ = modbus_coil[adr_num];
                iNCoils -= 8;
                adr_num += 1;
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
        uint16_t adr_num = usBitOffset/8;
        while( iNDiscrete > 0 ) {
            *pucRegBuffer++ = modbus_input[adr_num];
            iNDiscrete -= 8;
            adr_num += 1;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

