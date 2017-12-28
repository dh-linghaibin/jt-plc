/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#ifndef _TCP_FILE_H_
#define _TCP_FILE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "uip.h"
#include <string.h>
#include <stdio.h>

//comminication status
enum {
	STATE_CMD		= 0,	//receiving command
	STATE_TX_TEST	= 1,	//continious Tx packet
	STATE_RX_TEST	= 2		//continious Rx packet  
};	 

//define uip_tcp_appstate_t variable type
//user cannot change the structure's name
//uip.h call structure below	  
struct tcp_demo_appstate {
	uint8_t state;
	uint8_t *textptr;
	int textlen;
};	

void tcp_server_aborted(void);
void tcp_server_timedout(void);
void tcp_server_closed(void);
void tcp_server_connected(void);
void tcp_server_newdata(void);
void tcp_server_acked(void);
void tcp_server_senddata(void);
void tcp_server_file(void);

#ifdef __cplusplus
}
#endif

#endif
