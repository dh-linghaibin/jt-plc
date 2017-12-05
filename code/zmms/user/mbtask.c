
#include <rtthread.h>

#include "l_enc28j60.h"
#include "uip.h"
#include "uip_arp.h"
#include "mb.h"
#include "mbutils.h"
#include "tapdev.h"

#define CLOCK_SECOND  1000
/* ��ʱ�� �Զ������ݽṹ */
typedef struct
{
    /* ��ʼʱ�� */
    uint16_t start;
    /* ʱ���� */
    uint16_t interval;
}timer_typedef;

static uint16_t current_clock = 0;

/* ��ʱ���Ŀ��ƿ� */
static rt_timer_t timer1;
/* ��ʱ��1��ʱ���� */
static void timeout1(void* parameter) {
    /* ʱ���־�ۼ� */
    current_clock++;
}

static uint16_t clock_time(void)
{
    return current_clock;
}

static void timer_set(timer_typedef* ptimer,uint16_t interval)
{
    /* ����ʱ���� */
    ptimer->interval = interval;
    /* ��������ʱ�� */
    ptimer->start = clock_time();
}

static void timer_reset(timer_typedef * ptimer)
{ 
    ptimer->start =ptimer->start + ptimer->interval;
}

static int8_t timer_expired(timer_typedef* ptimer)
{
    /* һ��Ҫװ��Ϊ�з�������������ѧ�Ƚ�ʱ����ʹ���з����� */
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
    
    /* ENC28J60��ʼ�� */
    tapdev_init();                     		 
    /* UIPЭ��ջ��ʼ�� */
    uip_init();
    /* ����IP��ַ */
    {
        uint8_t read_data[4] = {192,168,1,203};
        uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
        uip_sethostaddr(ipaddr);
    }
    /* ����Ĭ��·����IP��ַ */
    {
        uint8_t read_data[4] = {192,168,1,1};
        uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
        uip_setdraddr(ipaddr);
    }
    /* ������������ */
    uip_ipaddr(ipaddr, 255,255,255,0);		 
    uip_setnetmask(ipaddr);	
    // MODBUS TCP����Ĭ�϶˿� 502
    eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);      
    eMBEnable();	
    
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1", /* ��ʱ�������� timer1 */
                            timeout1, /* ��ʱʱ�ص��Ĵ����� */
                            RT_NULL, /* ��ʱ��������ڲ��� */
                            1, /* ��ʱ���ȣ���OS TickΪ��λ����10��OS Tick */
                            RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
    /* ������ʱ�� */
    if (timer1 != RT_NULL) rt_timer_start(timer1);
    
    timer_set(&periodic_timer, CLOCK_SECOND / 20);
    timer_set(&arp_timer, CLOCK_SECOND * 5);
    
    while (1) {
        eMBPoll();
        /* �������豸��ȡһ��IP��,�������ݳ��� */
        uip_len = tapdev_read();
        /* �յ�����	*/
        if(uip_len > 0)	{
            /* ����IP���ݰ� */
            if(BUF->type == htons(UIP_ETHTYPE_IP)) {
                uip_arp_ipin();
                uip_input();
                if (uip_len > 0) {
                    uip_arp_out();
                    tapdev_send();
                }
            } else if (BUF->type == htons(UIP_ETHTYPE_ARP)) {
                /* ����ARP���� */
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


#define REG_INPUT_START       0x0000                // ����Ĵ�����ʼ��ַ
#define REG_INPUT_NREGS       16                    // ����Ĵ�������

#define REG_HOLDING_START     0x0000                // ���ּĴ�����ʼ��ַ
#define REG_HOLDING_NREGS     16                    // ���ּĴ�������

#define REG_COILS_START       0x0000                // ��Ȧ��ʼ��ַ
#define REG_COILS_SIZE        16                    // ��Ȧ����

#define REG_DISCRETE_START    0x0000                // ���ؼĴ�����ʼ��ַ
#define REG_DISCRETE_SIZE     16                    // ���ؼĴ�������

// ����Ĵ�������
static uint16_t usRegInputBuf[REG_INPUT_NREGS] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
// �Ĵ�����ʼ��ַ
static uint16_t usRegInputStart = REG_INPUT_START;
// ���ּĴ�������
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
// ���ּĴ�����ʼ��ַ
static uint16_t usRegHoldingStart = REG_HOLDING_START;
// ��Ȧ״̬
static uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0xFF, 0x00};
// ����״̬
static uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0xFF};

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    // ��ѯ�Ƿ��ڼĴ�����Χ��
    // Ϊ�˱��⾯�棬�޸�Ϊ�з�������
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

