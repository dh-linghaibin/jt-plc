/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "w25qxx.h"
#include "ff.h"

void test(void) {
	FATFS fs; /* FatFs文件系统对象 */
	FIL fnew; /* 文件对象 */
	FRESULT res_sd; /* 文件操作结果 */
	UINT fnum; /* 文件成功读写数量 */
	BYTE ReadBuffer[1024]= {0}; /* 读缓冲区 */
	BYTE WriteBuffer[] = "linghaibin very best ha on the world \r\n";//
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd = f_mount(&fs,"0:",0);
	/*----------------------- 格式化测试 ---------------------------*/
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if (res_sd == FR_NO_FILESYSTEM) {
		//printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
		/* 格式化 */
		res_sd=f_mkfs("0:",FM_FAT,0,work, sizeof work );
		if (res_sd == FR_OK) {
			//printf("》SD卡已成功格式化文件系统。\r\n");
			/* 格式化后，先取消挂载 */
			res_sd = f_mount(0,"0:",0);
			/* 重新挂载 */
			res_sd = f_mount(&fs,"0:",0);
		} else {
			//printf("《《格式化失败。》》\r\n");
			while (1);
		}
	} else if (res_sd!=FR_OK) {
		//printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
		//printf("！！可能原因：SD卡初始化不成功。\r\n");
		while (1);
	} else {
		//printf("》文件系统挂载成功，可以进行读写测试\r\n");
	}
	/*--------------------- 文件系统测试：写测试 -----------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	//printf("\r\n****** 即将进行文件写入测试... ******\r\n");
	res_sd=f_open(&fnew,"0:ee.txt",FA_CREATE_ALWAYS|FA_WRITE);
	if ( res_sd == FR_OK ) {
		//printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
		/* 将指定存储区内容写入到文件内 */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
		if (res_sd==FR_OK) {
			//printf("》文件写入成功，写入字节数据：%d\n",fnum);
			//printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
		} else {
			//printf("！！文件写入失败：(%d)\n",res_sd);
		}
		/* 不再读写，关闭文件 */
		f_close(&fnew);
	} else {
		//printf("！！打开/创建文件失败。\r\n");

	}
	/*------------------ 文件系统测试：读测试 --------------------------*/
	//printf("****** 即将进行文件读取测试... ******\r\n");
	res_sd=f_open(&fnew,"0:ee.txt",FA_OPEN_EXISTING|FA_READ);
	if (res_sd == FR_OK) {
		//printf("》打开文件成功。\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
		if (res_sd==FR_OK) {
			//printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
			//printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
		} else {
			//printf("！！文件读取失败：(%d)\n",res_sd);
		}
	} else {
		//printf("！！打开文件失败。\r\n");
	}

	/* 不再读写，关闭文件 */
	f_close(&fnew);
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(0,"0:",0);
	/* 操作完成，停机 */
//	while (1) {

//	}
}

#include "uip.h"
#include "uip_arp.h"
#include "mb.h"
#include "mbutils.h"
#include "timer.h"
#include "tapdev.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	

int main(void) {
	test();
	timer_typedef periodic_timer, arp_timer;
    uip_ipaddr_t ipaddr;
	
	timer_config();
    tapdev_init();                     		 
    uip_init();
	{
		uint8_t read_data[4] = {192,168,1,220};
		uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
		uip_sethostaddr(ipaddr);
	}
	{
		uint8_t read_data[4] = {192,168,1,1};
		uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
		uip_setdraddr(ipaddr);
	}
    uip_ipaddr(ipaddr, 255,255,255,0);		 
    uip_setnetmask(ipaddr);	
	eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);      
	eMBEnable();	

	timer_set(&periodic_timer, CLOCK_SECOND / 20);
    timer_set(&arp_timer, CLOCK_SECOND * 5);
	while(1) {
		eMBPoll();
		uip_len = tapdev_read();
		/* 收到数据	*/
		if(uip_len > 0)	{
			/* 处理IP数据包 */
			if(BUF->type == htons(UIP_ETHTYPE_IP)) {
				uip_arp_ipin();
				uip_input();
				if (uip_len > 0) {
					uip_arp_out();
					tapdev_send();
				}
			} else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {
				/* 处理ARP报文 */
				uip_arp_arpin();
				if (uip_len > 0) {
					tapdev_send();
				}
			}
		}
		/* 0.5秒定时器超时 */
		if(timer_expired(&periodic_timer)) {
			timer_reset(&periodic_timer);
			/* 处理TCP连接, UIP_CONNS缺省是10个 */
			for(uint8_t i = 0; i < UIP_CONNS; i++) {
				/* 处理TCP通信事件 */
				uip_periodic(i);		
				if(uip_len > 0) {
					uip_arp_out();
					tapdev_send();
				}
			}   
			/* 定期ARP处理 */
			if (timer_expired(&arp_timer)) {
				timer_reset(&arp_timer);
				uip_arp_timer();
			}
		}
	}
}


#define REG_INPUT_START       0x0000                // 输入寄存器起始地址
#define REG_INPUT_NREGS       16                    // 输入寄存器数量

#define REG_HOLDING_START     0x0000                // 保持寄存器起始地址
#define REG_HOLDING_NREGS     16                    // 保持寄存器数量

#define REG_COILS_START       0x0000                // 线圈起始地址
#define REG_COILS_SIZE        16                    // 线圈数量

#define REG_DISCRETE_START    0x0000                // 开关寄存器起始地址
#define REG_DISCRETE_SIZE     16                    // 开关寄存器数量

uint16_t usRegInputStart = REG_INPUT_START;

uint16_t usRegHoldingStart = REG_HOLDING_START;

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    // 查询是否在寄存器范围内
    // 为了避免警告，修改为有符号整数
    if( ( (int16_t) usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( 0x55 );
            *pucRegBuffer++ = ( unsigned char )( 0x55 );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    
    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
        && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
        case MB_REG_READ:            
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( 0x55 );
                *pucRegBuffer++ = ( unsigned char )( 0x55 );
                iRegIndex++;
                usNRegs--;
            }
            break;
            
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
//                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
//                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    
    return eStatus;
}


eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNCoils = ( short )usNCoils;
    unsigned short  usBitOffset;
    
    if( ( (int16_t)usAddress >= REG_COILS_START ) &&
       ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
            
        case MB_REG_READ:
            while( iNCoils > 0 )
            {
//                *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
//                                                 ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
                iNCoils -= 8;
                usBitOffset += 8;
            }
            break;
            
        case MB_REG_WRITE:
            while( iNCoils > 0 )
            {
//                xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
//                               ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
//                               *pucRegBuffer++ );
                iNCoils -= 8;
                usBitOffset += 8;
            }
            break;
        }
        
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNDiscrete = ( short )usNDiscrete;
    unsigned short  usBitOffset;
    
    if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
       ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_DISCRETE_START );
        
        while( iNDiscrete > 0 )
        {
//            *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
//                                             ( unsigned char)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
