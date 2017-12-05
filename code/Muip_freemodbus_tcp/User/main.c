#include "stm32f10x.h"
#include <stdio.h>

#include "uip.h"
#include "uip_arp.h"

#include "tapdev.h"
#include "enc28j60.h"	

#include "bsp_spi1.h"
#include "bsp_usart.h"
#include "timer.h"

#include "bsp_can.h"
#include "rtc_time.h"
#include "ringbuf.h"

#include "led.h"
#include "flash.h"

#include "wdog.h"


#include <stdlib.h>

#include "user_mb_app.h"
#include "mb.h"
#include "mb_m.h"
#include "mbutils.h"

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


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])	

CanTxMsg TxMessage;

extern Stdringbuf RINGBUF;

uint8_t buf[8];

uint8_t plc_coil_val[100];//继电器状态
uint16_t plc_coil_ele[782];//电量检测

uint8_t plc_set_coil[20];//发送信息

uint16_t plc_time[8];//时间

rtc_t rtc;


uint8_t isset(uint8_t it,uint8_t position);

uint8_t pro_flag = 0;
uint8_t cmd_num_start = 0;//
uint8_t cmd_num = 0;//这个时间下有几个命令

uint8_t led_toggle_num = 0;

int main(void) {
    StdLed LED = {
        {
            {GPIOB, GPIO_Pin_15},
            {GPIOB, GPIO_Pin_12},
            {GPIOB, GPIO_Pin_13},
            {GPIOB, GPIO_Pin_14},
        },
        &LedInit,
        &LedSetRun,
        &LedSetCAN,
        &LedSetRS232,
        &LedSetRS484,
        &LedRunTiger,
        &LedCANTiger,
        &LedRS232Tiger,
        &LedRS484Tiger,
    };
    Stdflash SFLASH = {
        &FLASH_WriteNWord,
        &FLASH_ReadNWord,
        &FLASH_ReadNBit,
    };
    Stdwdog WDOG = {
        &WdogInit,
        &WdogReload,
    };
    timer_typedef periodic_timer, arp_timer, led_timer;
    uip_ipaddr_t ipaddr;
    
    /* TCP定时器修改为100ms */
    timer_set(&periodic_timer, CLOCK_SECOND / 10);
    timer_set(&arp_timer, CLOCK_SECOND * 5);
    timer_set(&led_timer, CLOCK_SECOND / 5);
    
    /* IO口初始化 主要是为了避免SPI总线上的其他设备 */
    LED.Init(&LED.Led_n);                    
    /* 配置systic作为1ms中断 */
    timer_config(); 
    /* 初始化SPI1 */
    BSP_ConfigSPI1();
    /* ENC28J60初始化 */
    tapdev_init();                     		 
    /* UIP协议栈初始化 */
    uip_init();
    /* 设置IP地址 */
    uip_ipaddr(ipaddr, 192,168,1,15);	
    uip_sethostaddr(ipaddr);
    /* 设置默认路由器IP地址 */
    uip_ipaddr(ipaddr, 192,168,1,1);		 
    uip_setdraddr(ipaddr);
    /* 设置网络掩码 */
    uip_ipaddr(ipaddr, 255,255,255,0);		 
    uip_setnetmask(ipaddr);	
    // MODBUS TCP侦听默认端口 502
//    eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);      
//    eMBEnable();	
    
    eMBMasterTCPInit(MB_MASTER_TCP_PORT_USE_DEFAULT);    
    eMBMasterEnable();
    
    BSP_ConfigUSART1();
    /* 初始化 RTC */
    ltk_rtc_init();
    CAN_init();
    /** -- flash初始化 -- by lhb_steven -- 2017/8/1**/
    FLASH_Unlock();
    ltk_ee_init();
    FLASH_Lock();
    /** -- 看门狗初始化 -- by lhb_steven -- 2017/8/1**/
    WDOG.init();
    while (1) {	
        switch(plc_set_coil[1]) {
            case 1:{
                TxMessage.StdId=plc_set_coil[2];
                TxMessage.RTR=CAN_RTR_DATA;
                TxMessage.IDE=CAN_ID_STD;
                TxMessage.DLC=8;
                TxMessage.Data[0]=plc_set_coil[3];
                TxMessage.Data[1]=plc_set_coil[4];
                TxMessage.Data[2]=plc_set_coil[5];
                TxMessage.Data[3]=plc_set_coil[6];
                TxMessage.Data[4]=plc_set_coil[7];
                TxMessage.Data[5]=plc_set_coil[8];
                TxMessage.Data[6]=plc_set_coil[9];
                TxMessage.Data[7]=plc_set_coil[10];
                CAN_tx_data(TxMessage);
                plc_set_coil[1] = 0;
            }
                break;
            case 2:{
                TxMessage.StdId=plc_set_coil[11];
                TxMessage.RTR=CAN_RTR_DATA;
                TxMessage.IDE=CAN_ID_STD;
                TxMessage.DLC=8;
                TxMessage.Data[0]=plc_set_coil[12];
                TxMessage.Data[1]=plc_set_coil[13];
                TxMessage.Data[2]=plc_set_coil[14];
                TxMessage.Data[3]=plc_set_coil[15];
                TxMessage.Data[4]=plc_set_coil[16];
                TxMessage.Data[5]=plc_set_coil[17];
                TxMessage.Data[6]=plc_set_coil[18];
                TxMessage.Data[7]=plc_set_coil[19];
                CAN_tx_data(TxMessage);
                plc_set_coil[1] = 1;
            }
            break;
        }       
        //读取can数据
        if(RINGBUF.get(&RINGBUF.ringbuf_n,buf,8)) {   
            led_toggle_num++;
            switch(pro_flag) {
                case 0: {
                    switch(buf[1]) {
                        case 0x61:{//输出值
                            plc_coil_val[buf[0]] = buf[7];//桶排序 哈哈
                        }
                        break;
                        case 0x62:{//电量
                            uint8_t address = buf[0]*8;
                            plc_coil_ele[address] = buf[2];
                            plc_coil_ele[address] |= (uint16_t)(buf[3] << 8);
                            
                            plc_coil_ele[address+1] = buf[4];
                            plc_coil_ele[address+1] |= (uint16_t)(buf[5] << 8);
                            
                            plc_coil_ele[address+2] = buf[6];
                            plc_coil_ele[address+2] |= (uint16_t)(buf[7] << 8);
                        }
                        break;
                        case 0x63:{//电量
                            uint8_t address = buf[0]*8;
                            plc_coil_ele[address+3] = buf[2];
                            plc_coil_ele[address+3] |= (uint16_t)(buf[3] << 8);
                            
                            plc_coil_ele[address+4] = buf[4];
                            plc_coil_ele[address+4] |= (uint16_t)(buf[5] << 8);
                            
                            plc_coil_ele[address+5] = buf[6];
                            plc_coil_ele[address+5] |= (uint16_t)(buf[7] << 8);
                        }
                        break;
                        case 0x64:{//电量
                            uint8_t address = buf[0]*8;
                            plc_coil_ele[address+6] = buf[2];
                            plc_coil_ele[address+6] |= (uint16_t)(buf[3] << 8);
                            
                            plc_coil_ele[address+7] = buf[4];
                            plc_coil_ele[address+7] |= (uint16_t)(buf[5] << 8);
                        }
                        break;
                        case 0xff:{//编程
                            if( (buf[1] == 255) && \
                                (buf[2] == 0x66) && \
                                (buf[3] == 0x65) && \
                                (buf[4] == 0x64) && \
                                (buf[5] == 0x63) && \
                                (buf[6] == 0x62) && \
                                (buf[7] == 0x61) ) {
                                    //开锁密室正确 开锁
                                    pro_flag = 1;
                                    cmd_num_start = 0;
                                    FLASH_Unlock();
                                }
                        }
                        break;
                        case 0xb1:{
                            //设置时间
                            rtc.year = (buf[2]+2000);
                            rtc.month = buf[3];
                            rtc.mday = buf[4];
                            rtc.hour = buf[5];
                            rtc.min = buf[6];
                            rtc.sec = buf[7];
                            ltk_rtc_set_time(&rtc);
                        }
                        break;
                        case 0xb2:{
                            //设置IP地址
                        }
                        break;
                        case 0xb3:{
                            //设置默认服务器IP地址
                            
                        }
                        break;
                    }
                }
                    break;
                case 1:{//编程
                    /** -- 去除更新干扰 -- by lhb_steven -- 2017/8/1**/
                    if( (buf[1] == 0x61) || (buf[1] == 0x62) || (buf[1] == 0x63) || (buf[1] == 0x64) ) {
                        break;
                    }
                    if( (buf[0] == 0xf1) && \
                    (buf[1] == 0x87) && \
                    (buf[2] == 0x86) && \
                    (buf[3] == 0x85) && \
                    (buf[4] == 0x84) && \
                    (buf[5] == 0x83) && \
                    (buf[6] == 0x82) && \
                    (buf[7] == 0x81) ) {
                        //加锁密室正确 加锁
                        pro_flag = 0;
                        //清楚
                        {
                            uint16_t big_val_buf[5] = {0x00,0x00,0x00,0x00,0x00};
                            WriteFlashOneWord(cmd_num_start*10,big_val_buf,5);
                        }
                        FLASH_Lock();
                    } else {
                        uint8_t val_buf[10];
                        uint16_t big_val_buf[5];
                        
                        if(buf[7] == 0xa5) {//说明是按钮
                            for(uint8_t cmd = 0;cmd < 8;cmd++) {
                                val_buf[cmd] = buf[cmd];
                            }
                            cmd_num = val_buf[0];
                        } else {//命令
                            val_buf[0] = buf[2];//设置地址
                            for(uint8_t cmd = 0;cmd < 8;cmd++) {
                                val_buf[cmd+1] = buf[cmd];
                            }
                            /** -- 添加结束符号，后面还有命令会覆盖 -- by lhb_steven -- 2017/7/7**/
                            if(cmd_num > 1) {
                                cmd_num--;
                                val_buf[9] = 1;
                            } else {
                                val_buf[9] = 0;
                            }
                        }
                                    
                        for(uint8_t cmd = 0;cmd < 5;cmd++) {
                            big_val_buf[cmd] = (uint16_t)(val_buf[cmd*2] << 8);
                            big_val_buf[cmd] |= (uint16_t)val_buf[(cmd*2)+1];
                        }
                        WriteFlashOneWord(cmd_num_start*10,big_val_buf,5);
                        cmd_num_start++;
                    }
                }
                    break;
            }
        }
//        eMBPoll();
        eMBMasterPoll();
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
        //led定时器
        if(timer_expired(&led_timer)) {
            timer_reset(&led_timer);
            /** -- 喂狗 -- by lhb_steven -- 2017/8/1**/
            WDOG.reload();
             /** -- 系统脉搏 -- by lhb_steven -- 2017/7/26**/
            LED.tagrun(&LED.Led_n);
            /** -- can接收指示灯 -- by lhb_steven -- 2017/8/1**/
            if(led_toggle_num > 0) {
                static uint8_t dr = 0;
                if(dr == 0) {
                    dr = 1;
                    LED.setcan(&LED.Led_n,0);
                } else {
                    dr = 0;
                    led_toggle_num--;
                    LED.setcan(&LED.Led_n,1);
                }
            }
            {
                static uint8_t old_min = 0;
                ltk_rtc_get_time(&rtc);
                plc_time[0] = rtc.year;
                plc_time[1] = rtc.month;
                plc_time[2] = rtc.mday;
                plc_time[3] = rtc.wday;
                plc_time[4] = rtc.hour;
                plc_time[5] = rtc.min;
                plc_time[6] = rtc.sec;
                if(old_min != rtc.min) {
                    uint8_t cmd_count = 0;//第几组数据
                    uint8_t cmd_count2 = 0;//第几组数据
                    old_min = rtc.min;
                    
                    TxMessage.RTR=CAN_RTR_DATA;
                    TxMessage.IDE=CAN_ID_STD;
                    TxMessage.DLC=8;
                    
                    //Flash_Read(200+);
                    
                    while(cmd_count < 120) {
                        uint8_t val_cmd[10];
                        //读取信息
                        //Flash_Read( (200+(cmd_count*10)), val_cmd,10);
                         {
                            uint16_t read_data[5] = {0x00,0x00,0x00,0x00,0x00};
                            ReadFlashNBtye( cmd_count*10,read_data,5);
                            for(uint8_t cmd = 0;cmd < 5;cmd++) {
                                val_cmd[cmd*2] = (uint8_t)(read_data[cmd] >> 8);
                                val_cmd[(cmd*2)+1] = (uint8_t)read_data[cmd];
                            }
                        }
                        if(val_cmd[0] > 0) {//判断是否有命令
                            if( (val_cmd[1] > 11) || (val_cmd[1] == rtc.month) ) { //判断月份
                                if( (val_cmd[2] > 6) || (val_cmd[2] == rtc.wday) ) {//判断星期   
                                    if(val_cmd[3] == rtc.hour) {//判断小时
                                        if(val_cmd[4] == rtc.min) {//判断分钟
                                            //命令 = 外部地址+10 + 上一组数据
                                            cmd_count2 = 0;
                                            
                                            while(cmd_count2 < 120) {
                                                cmd_count++;
                                                //Flash_Read( (200+(cmd_count*10)), val_cmd,10);
                                                {
                                                    uint16_t read_data[5] = {0x00,0x00,0x00,0x00,0x00};
                                                    ReadFlashNBtye( cmd_count*10,read_data,5);
                                                    for(uint8_t cmd = 0;cmd < 5;cmd++) {
                                                        val_cmd[cmd*2] = (uint8_t)(read_data[cmd] >> 8);
                                                        val_cmd[(cmd*2)+1] = (uint8_t)read_data[cmd];
                                                    }
                                                }
                                                LED.tagcan(&LED.Led_n);
                                                TxMessage.StdId=val_cmd[0];
                                                TxMessage.Data[0]=val_cmd[1];
                                                TxMessage.Data[1]=val_cmd[2];
                                                TxMessage.Data[2]=val_cmd[3];
                                                TxMessage.Data[3]=val_cmd[4];
                                                TxMessage.Data[4]=val_cmd[5];
                                                TxMessage.Data[5]=val_cmd[6];
                                                TxMessage.Data[6]=val_cmd[7];
                                                TxMessage.Data[7]=val_cmd[8];
                                                CAN_tx_data(TxMessage);
                                                if( val_cmd[9] > 0) {
                                                    cmd_count++;
                                                } else {
                                                    cmd_count++;
                                                    cmd_count2 = 200;//退出
                                                }
                                            }
                                            
                                        } else {
                                            cmd_count += (val_cmd[0]+1);
                                        }
                                    } else {
                                        cmd_count += (val_cmd[0]+1);
                                    }
                                } else {
                                    cmd_count += (val_cmd[0]+1);
                                }
                            } else {
                                cmd_count += (val_cmd[0]+1);
                            }
                        } else {
                            cmd_count = 200;
                        }
                    }
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
#if UIP_UDP
            /* 轮流处理每个UDP连接, UIP_UDP_CONNS缺省是10个 */
            for(uint8_t i = 0; i < UIP_UDP_CONNS; i++) {
                uip_udp_periodic(i);	/*处理UDP通信事件 */
                /* 如果上面的函数调用导致数据应该被发送出去，全局变量uip_len设定值> 0 */
                if(uip_len > 0) {
                    uip_arp_out();
                    tapdev_send();
                }
            }
#endif /* UIP_UDP */
            /* 定期ARP处理 */
            if (timer_expired(&arp_timer)) {
                timer_reset(&arp_timer);
                uip_arp_timer();
            }
        }
    }
}

//eMBErrorCode
//eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs ) {
//    eMBErrorCode    eStatus = MB_ENOERR;
//    int             iRegIndex;
//    
//    // 查询是否在寄存器范围内
//    // 为了避免警告，修改为有符号整数
//    if( ( (int16_t) usAddress >= REG_INPUT_START ) \
//        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) ) {
//        iRegIndex = ( int )( usAddress - usRegInputStart );
//        while( usNRegs > 0 ) {
//            *pucRegBuffer++ = ( unsigned char )( 0x00 >> 8 );
//            *pucRegBuffer++ = ( unsigned char )( 0x00 & 0xFF );
//            iRegIndex++;
//            usNRegs--;
//        }
//    } else {
//        eStatus = MB_ENOREG;
//    }
//    
//    return eStatus;
//}


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

//
//USHORT m_usAddress;//地址
//USHORT m_usNRegs;//寄存器地址
//eMBRegisterMode m_eMode;
//
//eMBErrorCode
//eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
//                eMBRegisterMode eMode ) {
//    eMBErrorCode    eStatus = MB_ENOERR;
//    int iRegIndex;
//    
//    if( ( (int16_t)usAddress >= REG_HOLDING_START ) \
//        && ( usAddress <= 1608 ) && ( m_usNRegs <= 16 ) ) {
//        iRegIndex = ( int )( usAddress - usRegHoldingStart );
//        m_usAddress = usAddress;//寄存器地址
//        m_usNRegs = usNRegs;//
//        m_eMode = eMode;//模式
//        switch ( eMode ) {
//            case MB_REG_READ://读取   
//            {   
//                while( usNRegs > 0 ) {
//                    uint8_t l_address = 0;
//                    uint8_t w_address = 0;
//                    l_address = iRegIndex/16;
//                    w_address = iRegIndex%16;
//                    if(l_address <= 99) {
//                        if(w_address < 8) {
//                            *pucRegBuffer++ = isset(plc_coil_val[l_address],w_address);
//                            *pucRegBuffer++ = ( uint8_t )( 0x00 );
//                        } else {
//                            uint8_t ele_address = 0;
//                            ele_address = l_address*8 + (w_address-8);
//                            *pucRegBuffer++ = ( uint8_t )( plc_coil_ele[ele_address] >> 8 );
//                            *pucRegBuffer++ = ( uint8_t )( plc_coil_ele[ele_address] & 0xFF );
//                        }
//                    } else {
//                        *pucRegBuffer++ = ( uint8_t )( plc_time[w_address] >> 8 );
//                        *pucRegBuffer++ = ( uint8_t )( plc_time[w_address] & 0xFF );
//                    }
//                    iRegIndex++;
//                    usNRegs--;
//                }
//            }
//            break;
//            
//            case MB_REG_WRITE://写入
//            while( usNRegs > 0 )
//            {
//                uint8_t l_address = 0;
//                uint8_t w_address = 0;
//                uint16_t coil_val = 0;
//                l_address = iRegIndex/16;
//                w_address = iRegIndex%16;
//                if(l_address <= 99) {
//                    coil_val = *pucRegBuffer++ << 8;
//                    coil_val |= *pucRegBuffer++;
//                    
//                    CmdZl(l_address,w_address,coil_val);
//                } else {
//                    plc_time[w_address] =  *pucRegBuffer++ << 8;
//                    plc_time[w_address] |=  *pucRegBuffer++;
//                    
//                    rtc.year = plc_time[0];
//                    rtc.month = plc_time[1];
//                    rtc.mday = plc_time[2];
//                    rtc.wday = plc_time[3];
//                    rtc.hour = plc_time[4];
//                    rtc.min = plc_time[5];
//                    rtc.sec = plc_time[6];
//                    ltk_rtc_set_time(&rtc);
//                }
//                usNRegs--;
//            }
//            break;
//        }
//    } else {
//        eStatus = MB_ENOREG;
//    }
//    return eStatus;
//}

//eMBErrorCode
//eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
//              eMBRegisterMode eMode ) {
//    eMBErrorCode    eStatus = MB_ENOERR;
//    short           iNCoils = ( short )usNCoils;
//    unsigned short  usBitOffset;
//    
//    if( ( (int16_t)usAddress >= REG_COILS_START ) &&
//       ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) ) {
//        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
//        switch ( eMode ) {
//            
//            case MB_REG_READ:
//            while( iNCoils > 0 )
//            {
//                *pucRegBuffer++ = 0x00;//xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
//                iNCoils -= 8;
//                usBitOffset += 8;
//            }
//            break;
//            
//            case MB_REG_WRITE:
//            while( iNCoils > 0 )
//            {
////                xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
////                               ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
////                               *pucRegBuffer++ );
//                *pucRegBuffer++;
//                iNCoils -= 8;
//                usBitOffset += 8;
//            }
//            break;
//        }
//        
//    }
//    else
//    {
//        eStatus = MB_ENOREG;
//    }
//    return eStatus;
//}
//
//eMBErrorCode
//eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
//{
//    eMBErrorCode    eStatus = MB_ENOERR;
//    short           iNDiscrete = ( short )usNDiscrete;
//    unsigned short  usBitOffset;
//    
//    if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
//       ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
//    {
//        usBitOffset = ( unsigned short )( usAddress - REG_DISCRETE_START );
//        
//        while( iNDiscrete > 0 )
//        {
//            *pucRegBuffer++ = 0x00;//xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
////                                             ( unsigned char)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
//            iNDiscrete -= 8;
//            usBitOffset += 8;
//        }
//    }
//    else
//    {
//        eStatus = MB_ENOREG;
//    }
//    return eStatus;
//}




