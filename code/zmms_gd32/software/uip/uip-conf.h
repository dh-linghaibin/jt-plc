
#ifndef __UIP_CONF_H
#define __UIP_CONF_H

#include <inttypes.h>

typedef uint8_t                   u8_t;
typedef uint16_t                  u16_t;	
typedef unsigned short            uip_stats_t;

/* ���TCP������ */
#define UIP_CONF_MAX_CONNECTIONS  10
/* ���˿ڼ����� */
#define UIP_CONF_MAX_LISTENPORTS  10	  
/* uIP �����С*/
#define UIP_CONF_BUFFER_SIZE      1500
/* CPU�ֽ�˳�� */ 
#define UIP_CONF_BYTE_ORDER       UIP_LITTLE_ENDIAN
/* ��־����	 */			
#define UIP_CONF_LOGGING          0
/* UDP֧�ֿ���*/
#define UIP_CONF_UDP              0
/* UDPУ��Ϳ��� */
#define UIP_CONF_UDP_CHECKSUMS    1
/* uIPͳ�ƿ��� */
#define UIP_CONF_STATISTICS       1

#include "uip_modbus.h"
#endif
