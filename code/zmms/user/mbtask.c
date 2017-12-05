
#include <rtthread.h>

#include "l_enc28j60.h"
#include "uip.h"
#include "uip_arp.h"
#include "mb.h"
#include "mbutils.h"
#include "tapdev.h"

#define CLOCK_SECOND  1000
/* 定时器 自定义数据结构 */
typedef struct
{
    /* 开始时间 */
    uint16_t start;
    /* 时间间隔 */
    uint16_t interval;
}timer_typedef;

static uint16_t current_clock = 0;

/* 定时器的控制块 */
static rt_timer_t timer1;
/* 定时器1超时函数 */
static void timeout1(void* parameter) {
    /* 时间标志累加 */
    current_clock++;
}

static uint16_t clock_time(void)
{
    return current_clock;
}

static void timer_set(timer_typedef* ptimer,uint16_t interval)
{
    /* 设置时间间隔 */
    ptimer->interval = interval;
    /* 设置启动时间 */
    ptimer->start = clock_time();
}

static void timer_reset(timer_typedef * ptimer)
{ 
    ptimer->start =ptimer->start + ptimer->interval;
}

static int8_t timer_expired(timer_typedef* ptimer)
{
    /* 一定要装换为有符号数，进行数学比较时，多使用有符号数 */
    if((int16_t)(clock_time() - ptimer->start) >= (int16_t)ptimer->interval)
        return 1;
    else
        return 0;
}

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	

static rt_uint8_t network_stack[ 2048 ];
static struct rt_thread network_thread;
static void network_thread_entry(void* parameter) {
     timer_typedef periodic_timer, arp_timer;
    
    uip_ipaddr_t ipaddr;
    
    /* ENC28J60初始化 */
    tapdev_init();                     		 
    /* UIP协议栈初始化 */
    uip_init();
    /* 设置IP地址 */
    {
        uint8_t read_data[4] = {192,168,1,203};
        uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
        uip_sethostaddr(ipaddr);
    }
    /* 设置默认路由器IP地址 */
    {
        uint8_t read_data[4] = {192,168,1,1};
        uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
        uip_setdraddr(ipaddr);
    }
    /* 设置网络掩码 */
    uip_ipaddr(ipaddr, 255,255,255,0);		 
    uip_setnetmask(ipaddr);	
    // MODBUS TCP侦听默认端口 502
    eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);      
    eMBEnable();	
    
    /* 创建定时器1 */
    timer1 = rt_timer_create("timer1", /* 定时器名字是 timer1 */
                            timeout1, /* 超时时回调的处理函数 */
                            RT_NULL, /* 超时函数的入口参数 */
                            1, /* 定时长度，以OS Tick为单位，即10个OS Tick */
                            RT_TIMER_FLAG_PERIODIC); /* 周期性定时器 */
    /* 启动定时器 */
    if (timer1 != RT_NULL) rt_timer_start(timer1);
    
    timer_set(&periodic_timer, CLOCK_SECOND / 20);
    timer_set(&arp_timer, CLOCK_SECOND * 5);
    
    while (1) {
        eMBPoll();
        /* 从网络设备读取一个IP包,返回数据长度 */
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

        if(timer_expired(&periodic_timer)) {
            timer_reset(&periodic_timer);
     
            for(uint8_t i = 0; i < UIP_CONNS; i++) {
                uip_periodic(i);		
                if(uip_len > 0) {
                    uip_arp_out();
                    tapdev_send();
                }
            }   
            #if UIP_UDP
            for(uint8_t i = 0; i < UIP_UDP_CONNS; i++) {
                uip_udp_periodic(i);
                if(uip_len > 0) {
                    uip_arp_out();
                    tapdev_send();
                }
            }
            #endif /* UIP_UDP */
            if (timer_expired(&arp_timer)) {
                timer_reset(&arp_timer);
                uip_arp_timer();
            }
        }
    }
}


int mb_tatsk_init(void) {
    rt_err_t tid;
    
    tid = rt_thread_init(&network_thread,
                            "flash",
                            network_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&network_stack[0],
                            sizeof(network_stack),
                            5,
                            1);
    if (tid == RT_EOK) rt_thread_startup(&network_thread);

    return 0;
}


#define REG_INPUT_START       0x0000                // 输入寄存器起始地址
#define REG_INPUT_NREGS       16                    // 输入寄存器数量

#define REG_HOLDING_START     0x0000                // 保持寄存器起始地址
#define REG_HOLDING_NREGS     16                    // 保持寄存器数量

#define REG_COILS_START       0x0000                // 线圈起始地址
#define REG_COILS_SIZE        16                    // 线圈数量

#define REG_DISCRETE_START    0x0000                // 开关寄存器起始地址
#define REG_DISCRETE_SIZE     16                    // 开关寄存器数量

// 输入寄存器内容
static uint16_t usRegInputBuf[REG_INPUT_NREGS] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
// 寄存器起始地址
static uint16_t usRegInputStart = REG_INPUT_START;
// 保持寄存器内容
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
// 保持寄存器起始地址
static uint16_t usRegHoldingStart = REG_HOLDING_START;
// 线圈状态
static uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0xFF, 0x00};
// 开关状态
static uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0xFF};

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
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
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
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;
            
            case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
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
                *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                                 ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
                iNCoils -= 8;
                usBitOffset += 8;
            }
            break;
            
            case MB_REG_WRITE:
            while( iNCoils > 0 )
            {
                xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                               ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                               *pucRegBuffer++ );
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
            *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                             ( unsigned char)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
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


//extern void enc28j60_isr();
//void EXTI9_5_IRQHandler(void)
//{
//    rt_interrupt_enter();
//    if(EXTI_GetFlagStatus(EXTI_Line5)==SET)
//    {
//        EXTI_ClearITPendingBit(EXTI_Line5);
//        enc28j60_isr();
//    }
//    rt_interrupt_leave();
//}
//
//static rt_uint8_t lwip_stack[ 1024 ];
//static struct rt_thread lwip_thread;
//static void lwip_thread_entry(void* parameter) {
//#if defined(RT_USING_SPI_ENC28J60)
//    /* init enc28j60 driver*/
//    {
//        extern rt_err_t enc28j60_attach(const char * spi_device_name);
//        rt_thread_delay(2);
//        if(enc28j60_attach("enc28j60") != RT_EOK) {
//            rt_kprintf("[error] No such spi enc28j60!\r\n");
//        } else {
//            rt_kprintf("[OK]spi enc28j60!\r\n");
//        }
//    }
//#endif
//    /* initialize lwip stack */
//    /* register ethernetif device */
//    eth_system_device_init();
//    /* initialize lwip system */
//    lwip_system_init();
//    rt_kprintf("TCP/IP initialized!\n");
//    {
//        extern void test_modbus(void);
//        test_modbus();
//    }
//}

