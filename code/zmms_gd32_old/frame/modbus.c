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

extern rtc_obj rtc;
uint16_t plc_time[8];
extern uint8_t plc_coil_val[100];
uint8_t plc_set_coil[20];
uint8_t l_h = 0;
uint8_t l_l = 0;
uint8_t l_ok = 0;

uint8_t isset(uint8_t it,uint8_t position) {
    uint8_t oc=1;
    oc<<=(position);
    return (oc&it)!=0?1:0;
}


void CmdZl(uint8_t l_ad,uint8_t w_ad,uint16_t coil_val) {
    if(w_ad < 8) {
        switch(plc_set_coil[1]) {
            case 0:{
                plc_set_coil[7] = 4;
                plc_set_coil[8] = 4;
                plc_set_coil[9] = 4;
                plc_set_coil[10] = 4;
                
                plc_set_coil[2] = l_ad;//设备地址
                plc_set_coil[3] = 0x01;//设置继电器
                
                if(w_ad < 4) {
                    plc_set_coil[4] = 0x01;//第一组
                    if(coil_val == 0) {
                        plc_set_coil[7+w_ad] = 0x00;
                    } else {
                        plc_set_coil[7+w_ad] = 0x01;
                    }
                } else {
                    plc_set_coil[4] = 0x02;//第2组
                    if(coil_val == 0) {
                        plc_set_coil[3+w_ad] = 0x00;
                    } else {
                        plc_set_coil[3+w_ad] = 0x01;
                    }
                }
                
                plc_set_coil[1] = 1;//需要发送的组
            }
            break;
            case 1:{
                if( (plc_set_coil[2] == l_ad) && (plc_set_coil[4] == 0x01) ) {
                    //已经有发送组
                    if(w_ad < 4) {
                        if(coil_val == 0) {
                            plc_set_coil[7+w_ad] = 0x00;
                        } else {
                            plc_set_coil[7+w_ad] = 0x01;
                        }
                    } else {
                        if(coil_val == 0) {
                            plc_set_coil[3+w_ad] = 0x00;
                        } else {
                            plc_set_coil[3+w_ad] = 0x01;
                        }
                    }
                } else {
                    plc_set_coil[16] = 2;
                    plc_set_coil[17] = 2;
                    plc_set_coil[18] = 2;
                    plc_set_coil[19] = 2;
                    
                    plc_set_coil[11] = l_ad;//设备地址
                    plc_set_coil[12] = 0x01;//设置继电器
                    
                    if(w_ad < 4) {
                        plc_set_coil[13] = 0x01;//第一组
                        if(coil_val == 0) {
                            plc_set_coil[16+w_ad] = 0x00;
                        } else {
                            plc_set_coil[16+w_ad] = 0x01;
                        }
                    } else {
                        plc_set_coil[13] = 0x02;//第2组
                        if(coil_val == 0) {
                            plc_set_coil[14+w_ad] = 0x00;
                        } else {
                            plc_set_coil[14+w_ad] = 0x01;
                        }
                    }
                    
                    plc_set_coil[1] = 2;//需要发送的组
                }
            }
            break;
            case 2:{
                if( (plc_set_coil[2] == l_ad) && (plc_set_coil[4] == 0x01) ) {
                    //已经有发送组
                    if(w_ad < 4) {
                        if(coil_val == 0) {
                            plc_set_coil[7+w_ad] = 0x00;
                        } else {
                            plc_set_coil[7+w_ad] = 0x01;
                        }
                    } else {
                        if(coil_val == 0) {
                            plc_set_coil[3+w_ad] = 0x00;
                        } else {
                            plc_set_coil[3+w_ad] = 0x01;
                        }
                    }
                } if( (plc_set_coil[11] == l_ad) && (plc_set_coil[13] == 0x01) ) {
                    if(w_ad < 4) {
                        if(coil_val == 0) {
                            plc_set_coil[16+w_ad] = 0x00;
                        } else {
                            plc_set_coil[16+w_ad] = 0x01;
                        }
                    } else {
                        if(coil_val == 0) {
                            plc_set_coil[14+w_ad] = 0x00;
                        } else {
                            plc_set_coil[14+w_ad] = 0x01;
                        }
                    }
                }
            }
            break;
        }
    }
}

uint16_t usRegHoldingStart = REG_HOLDING_START;
USHORT m_usAddress;//地址
USHORT m_usNRegs;//寄存器地址
eMBRegisterMode m_eMode;

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
     if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
        && ( usAddress <= 1618 ) && ( m_usNRegs <= 16 ) ) {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        m_usAddress = usAddress;//寄存器地址
        m_usNRegs = usNRegs;//
        m_eMode = eMode;//模式
 switch ( eMode ) {
            case MB_REG_READ://读取   
            {   
                while( usNRegs > 0 ) {
                    uint8_t l_address = 0;
                    uint8_t w_address = 0;
                    l_address = iRegIndex/16;
                    w_address = iRegIndex%16;
                    if(l_address <= 99) {
                        if(w_address < 8) {
							*pucRegBuffer++ = ( uint8_t )( 0x00 );
                            *pucRegBuffer++ = isset(plc_coil_val[l_address],w_address);
                        } else {
                            uint8_t ele_address = 0;
                            ele_address = l_address*8 + (w_address-8);
                            *pucRegBuffer++ = ( uint8_t )( 0x00 );
                            *pucRegBuffer++ = ( uint8_t )( 0x00 );
                        }
                    } else if(l_address <= 100){
                        *pucRegBuffer++ = ( uint8_t )( plc_time[w_address] >> 8 );
                        *pucRegBuffer++ = ( uint8_t )( plc_time[w_address] & 0xFF );
                    } else {
						*pucRegBuffer++ = ( uint8_t )( l_h );
                        *pucRegBuffer++ = ( uint8_t )( l_l );
					}
                    iRegIndex++;
                    usNRegs--;
                }
            }
            break;
            
            case MB_REG_WRITE://写入
            while( usNRegs > 0 )
            {
                uint8_t l_address = 0;
                uint8_t w_address = 0;
                uint16_t coil_val = 0;
                l_address = iRegIndex/16;
                w_address = iRegIndex%16;
				
                if(l_address <= 99) {
                    coil_val = *pucRegBuffer++ << 8;
                    coil_val |= *pucRegBuffer++;
                    
                    CmdZl(l_address,w_address,coil_val);
                } else if(l_address <= 100){
                    plc_time[w_address] =  *pucRegBuffer++ << 8;
                    plc_time[w_address] |=  *pucRegBuffer++;
                    
					rtc_t t_rtc;
                    t_rtc.year = plc_time[0];
                    t_rtc.month = plc_time[1];
                    t_rtc.mday = plc_time[2];
                    t_rtc.wday = plc_time[3];
                    t_rtc.hour = plc_time[4];
                    t_rtc.min = plc_time[5];
                    t_rtc.sec = plc_time[6];
                    rtc.set(&rtc,t_rtc);
                } else if(l_address <= 101){
					if(w_address == 0) {
						l_h =  *pucRegBuffer++;
						l_l =  *pucRegBuffer++;
					}
					l_ok = 1;
				} else {
					
				}
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

