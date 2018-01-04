#include "stm32f10x.h"
#include <stdio.h>
#include "uip.h"
#include "uip_arp.h"
#include "mb.h"
#include "mbutils.h"
#include "tapdev.h"
#include "enc28j60.h"	
#include "bsp_spi1.h"
#include "bsp_usart.h"
#include "timer.h"
#include "bsp_can.h"
#include "rtc_time.h"
#include "ringbuf.h"
#include "led.h"
#include "wdog.h"
#include "spi_flash.h"

uint8_t rs232_package_merry[35] = {0};
/** -- 环形队列初始化 -- by lhb_steven -- 2017/6/30**/
Stdringbuf RS232 = {
    {
        0
    },
    &RingbufInit,
    &RingbufPut,
    &RingbufGut
};


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

volatile CanTxMsg TxMessage;

extern Stdringbuf RINGBUF;

uint8_t buf[8];

uint8_t plc_coil_online[100];//继电器状态
uint8_t plc_coil_val[100];//继电器状态
uint16_t plc_coil_ele[782];//电量检测

uint8_t plc_set_coil[20];//发送信息

uint16_t plc_time[8];//时间

rtc_t rtc;

uint8_t isset(uint8_t it,uint8_t position);

uint8_t pro_flag = 0;
uint16_t cmd_num_start = 0;//
uint8_t cmd_num = 0;//这个时间下有几个命令

uint8_t led_toggle_num = 0;


#define  FLASH_WriteAddress     0
#define  FLASH_ReadAddress      FLASH_WriteAddress
  
void GPIO_Config(void){  
    GPIO_InitTypeDef GPIO_InitStructure;     
      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
    GPIO_Init(GPIOA, &GPIO_InitStructure);   
	GPIO_WriteBit(GPIOA,GPIO_Pin_1,(BitAction)0);
}  

#define PRO_BU_BAADDR 2097152 //按钮地址
#define PRO_NR_BAADDR PRO_BU_BAADDR+1048576 //编程地址


uint8_t l_h = 0;
uint8_t l_l = 0;
uint8_t l_ok = 0;

void Read_cmd(uint8_t cmd,uint8_t addr) {
	uint8_t val_cmd_S[3] = {0,0};
	uint8_t val_cmd_x[10] = {0,0};
	uint16_t r_cmd_num_start = 0;

	SPI_Flash_Read(val_cmd_S,FLASH_ReadAddress + PRO_BU_BAADDR + ((cmd-1)*3)+ (addr*24),3);
	
	r_cmd_num_start = (uint16_t)val_cmd_S[1];
	r_cmd_num_start |= (uint16_t)(val_cmd_S[2] << 8);

	while( (val_cmd_S[0] > 0) && (r_cmd_num_start != 0xffff)){
		SPI_Flash_Read(val_cmd_x,FLASH_ReadAddress + PRO_NR_BAADDR + (r_cmd_num_start*10),10);
		
		TxMessage.StdId=val_cmd_x[0];
		TxMessage.DLC = 8;
		TxMessage.Data[0]=val_cmd_x[1];
		TxMessage.Data[1]=val_cmd_x[2];
		TxMessage.Data[2]=val_cmd_x[3];
		TxMessage.Data[3]=val_cmd_x[4];
		TxMessage.Data[4]=val_cmd_x[5];
		TxMessage.Data[5]=val_cmd_x[6];
		TxMessage.Data[6]=val_cmd_x[7];
		TxMessage.Data[7]=val_cmd_x[8];
		CAN_tx_data(TxMessage);
		r_cmd_num_start++;//开始地址加
		val_cmd_S[0]--;//，命令减去
	}
}

int main(void) {
    StdLed LED = {
        {
            {GPIOB, GPIO_Pin_11},
            {GPIOB, GPIO_Pin_0},
            {GPIOB, GPIO_Pin_10},
            {GPIOB, GPIO_Pin_1},
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
    Stdwdog WDOG = {
        &WdogInit,
        &WdogReload,
    };
    timer_typedef periodic_timer, arp_timer, led_timer;
    uip_ipaddr_t ipaddr;
	
	for(uint16_t i = 0;i < 4535;i++) {
		for(uint16_t k = 0;k < 400;k++);
	}
	
	RS232.init(&RS232.ringbuf_n, rs232_package_merry, 35);

    /* TCP定时器修改为100ms */
    timer_set(&periodic_timer, CLOCK_SECOND / 10);
    timer_set(&arp_timer, CLOCK_SECOND * 5);
    timer_set(&led_timer, CLOCK_SECOND / 5);
	
	SPI_Flash_Init();
	
//	{
//		uint8_t xxx[1] = {0};
//		SPI_Flash_Read(xxx ,0,1);	
//		if(xxx[0] != 0xa5) {
//			SPI_Flash_Erase_Chip();
//			xxx[0] = 0xa5;
//			SPI_Flash_Write(xxx ,0,1);
//		} else {
//			//SPI_Flash_Erase_Chip();
//		}
//	}
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
	{
		uint8_t read_data[4] = {0x00,0x00,0x00,0x00};
		SPI_Flash_Read(read_data,10,4);
		uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
		uip_sethostaddr(ipaddr);
	}
    /* 设置默认路由器IP地址 */
	{
		uint8_t read_data[4] = {0x00,0x00,0x00,0x00};
		//at45db161_read();
		SPI_Flash_Read(read_data,14,4);
		uip_ipaddr(ipaddr, read_data[0],read_data[1],read_data[2],read_data[3]);	
		uip_setdraddr(ipaddr);
	}
    /* 设置网络掩码 */
    uip_ipaddr(ipaddr, 255,255,255,0);		 
    uip_setnetmask(ipaddr);	
    // MODBUS TCP侦听默认端口 502
    eMBTCPInit(MB_TCP_PORT_USE_DEFAULT);      
    eMBEnable();	
    BSP_ConfigUSART2();
    GPIO_Config(); 
	
    /* 初始化 RTC */
    ltk_rtc_init();
    CAN_init();
    /** -- 看门狗初始化 -- by lhb_steven -- 2017/8/1**/
    WDOG.init();
    while (1) {	
		if(l_ok == 1) {
			l_ok = 0;
			Read_cmd(l_l,l_h);
		}
		
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
							plc_coil_val[buf[0]] = buf[7];
							plc_coil_online[buf[0]] = 0x01;//设备在线
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
                                    //FLASH_Unlock();
                                }
                        }
                        break;
						case 0x67:{//编程 厂家485面板----------------------需要具体修改
							 if((buf[1] == 0x67) && \
                                (buf[2] == 0x66) && \
                                (buf[3] == 0x65) && \
                                (buf[4] == 0x64) && \
                                (buf[5] == 0x63) && \
                                (buf[6] == 0x62) && \
                                (buf[7] == 0x61) ) {
                                    //开锁密室正确 开锁
                                    pro_flag = 2;
                                    cmd_num_start = 0;
                                    //FLASH_Unlock();
                                }
						}
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
							uint8_t val_buf[4];
							val_buf[0] = buf[4];
							val_buf[1] = buf[5];
							val_buf[2] = buf[6];
							val_buf[3] = buf[7];
							//at45db161_write();
							SPI_Flash_Write(val_buf,10,4);
                        }
                        break;
                        case 0xb3:{
                            //设置默认服务器IP地址
							uint8_t val_buf[4];
							val_buf[0] = buf[4];
							val_buf[1] = buf[5];
							val_buf[2] = buf[6];
							val_buf[3] = buf[7];
							//at45db161_write();
							SPI_Flash_Write(val_buf,14,4);
                        }
                        break;
						case 0xb4:{//按钮按下了
							Read_cmd(buf[3]+1,buf[2]);
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
                            uint8_t big_val_buf[10] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
							//at45db161_write();
							
							SPI_Flash_Write(big_val_buf,20+(cmd_num_start+1)*10,10);
                        }
                        //FLASH_Lock();
                    } else {
                        uint8_t val_buf[10];
                        
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
						//at45db161_write();
						SPI_Flash_Write(val_buf,20+cmd_num_start*10,10);
						
                        cmd_num_start++;
                    }
                }
                    break;
				case 2:{//厂家485 面板控制
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
                    (buf[7] == 0x81) ) {//0x81  
                        //加锁密室正确 加锁
                        pro_flag = 0;
                    } else {
						//开始写入
						uint8_t val_buf_x[10];
                        
						if( (buf[7] == 0xa5) && (buf[6] == 0) && 
                        (buf[5] == 0) && (buf[4] == 0) ) {	
							val_buf_x[0] = buf[1]; //<< 8;
							val_buf_x[1] = (uint8_t)cmd_num_start;	
							val_buf_x[2] = (uint8_t)(cmd_num_start >> 8);	
							//at45db161_write( FLASH_WriteAddress + PRO_BU_BAADDR + ((buf[0]-1)*3)+ (buf[2]*24),3,val_buf_x);
							
							SPI_Flash_Write(val_buf_x,FLASH_WriteAddress + PRO_BU_BAADDR + ((buf[0]-1)*3)+ (buf[2]*24),3);
							
							cmd_num = buf[1];//有多少条命令
						} else {
							val_buf_x[0] = buf[2];//设置地址
                            for(uint8_t cmd = 0;cmd < 8;cmd++) {
                                val_buf_x[cmd+1] = buf[cmd];
                            }
                            /** -- 添加结束符号，后面还有命令会覆盖 -- by lhb_steven -- 2017/7/7**/
                            if(cmd_num > 1) {
                                cmd_num--;
                                val_buf_x[9] = 1;
                            } else {
                                val_buf_x[9] = 0;
                            }
							//at45db161_write(FLASH_WriteAddress + PRO_NR_BAADDR + cmd_num_start*10,10,val_buf_x);
							SPI_Flash_Write(val_buf_x,FLASH_WriteAddress + PRO_NR_BAADDR + cmd_num_start*10,10);
							
							//SPI_Flash_Read(val_buf_x,FLASH_WriteAddress + PRO_NR_BAADDR + cmd_num_start*10,10);
							
							cmd_num_start++;
						}
					}
				}
					break;
            }
        }
        //读取485数据
		if(RS232.get(&RS232.ringbuf_n,buf,8)) {
			switch(buf[0]) {//头
				case 0x77:{
					switch(buf[1]) {//机型
						case 0x00:{
							if(buf[3] == 3) {//功能号码
								LED.tagrs485(&LED.Led_n);
								switch(buf[4]) {//按钮号码
									case 0x00:{
										Read_cmd(1,buf[5]);
									}
										break;
									case 0x10:{
										Read_cmd(2,buf[5]);
									}
										break;
									case 0x20:{
										Read_cmd(3,buf[5]);
									}
										break;
									case 0x30:{
										Read_cmd(4,buf[5]);
									}
										break;
									case 0x40:{
										Read_cmd(5,buf[5]);
									}
										break;
									case 0x50:{
										Read_cmd(6,buf[5]);
									}
										break;
									case 0x60:{
										Read_cmd(7,buf[5]);
									}
										break;
									case 0x70:{
										Read_cmd(8,buf[5]);
									}
										break;
								}		
							}
						}	
							break;
					}
				}	
					break;
			}
		}
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
			static uint8_t old_min = 0;
			ltk_rtc_get_time(&rtc);
			plc_time[0] = rtc.year;
			plc_time[1] = rtc.month;
			plc_time[2] = rtc.mday;
			plc_time[3] = rtc.wday;
			plc_time[4] = rtc.hour;
			plc_time[5] = rtc.min;
			plc_time[6] = rtc.sec;
//            {
//                static uint8_t old_min = 0;
//                ltk_rtc_get_time(&rtc);
//                plc_time[0] = rtc.year;
//                plc_time[1] = rtc.month;
//                plc_time[2] = rtc.mday;
//                plc_time[3] = rtc.wday;
//                plc_time[4] = rtc.hour;
//                plc_time[5] = rtc.min;
//                plc_time[6] = rtc.sec;
//                
//				if(old_min != rtc.min) {
//                    uint8_t cmd_count = 0;//第几组数据
//                    uint8_t cmd_count2 = 0;//第几组数据
//                    old_min = rtc.min;
//                    
//                    TxMessage.RTR=CAN_RTR_DATA;
//                    TxMessage.IDE=CAN_ID_STD;
//                    TxMessage.DLC=8;
//                    
//                    while(cmd_count < 120) {
//                        uint8_t val_cmd[10];
//                        //读取信息
//                        {
//							SPI_Flash_Read(val_cmd,20 + cmd_count*10,10);
//                        }
//                        if( (val_cmd[0] > 0) && (val_cmd[0] != 0xff) ) {//判断是否有命令
//                            if( (val_cmd[1] > 11) || (val_cmd[1] == rtc.month) ) { //判断月份
//                                if( (val_cmd[2] > 6) || (val_cmd[2] == rtc.wday) ) {//判断星期   
//                                    if(val_cmd[3] == rtc.hour) {//判断小时
//                                        if(val_cmd[4] == rtc.min) {//判断分钟
//                                            //命令 = 外部地址+10 + 上一组数据
//                                            cmd_count2 = 0;
//                                            while(cmd_count2 < 120) {
//                                                cmd_count++;
//                                                SPI_Flash_Read(val_cmd,20 + cmd_count*10,10);
//                                                LED.tagcan(&LED.Led_n);
//                                                TxMessage.StdId=val_cmd[0];
//                                                TxMessage.Data[0]=val_cmd[1];
//                                                TxMessage.Data[1]=val_cmd[2];
//                                                TxMessage.Data[2]=val_cmd[3];
//                                                TxMessage.Data[3]=val_cmd[4];
//                                                TxMessage.Data[4]=val_cmd[5];
//                                                TxMessage.Data[5]=val_cmd[6];
//                                                TxMessage.Data[6]=val_cmd[7];
//                                                TxMessage.Data[7]=val_cmd[8];
//                                                CAN_tx_data(TxMessage);
//                                                if( val_cmd[9] > 0) {
//                                                } else {
//                                                    cmd_count2 = 200;//退出
//                                                }
//                                            }
//                                            
//                                        } else {
//                                            cmd_count += (val_cmd[0]+1);
//                                        }
//                                    } else {
//                                        cmd_count += (val_cmd[0]+1);
//                                    }
//                                } else {
//                                    cmd_count += (val_cmd[0]+1);
//                                }
//                            } else {
//                                cmd_count += (val_cmd[0]+1);
//                            }
//                        } else {
//                            cmd_count = 200;
//                        }
//                    }
//                }
//            }
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

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    
    // 查询是否在寄存器范围内
    // 为了避免警告，修改为有符号整数
    if( ( (int16_t) usAddress >= REG_INPUT_START ) \
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) ) {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 ) {
            *pucRegBuffer++ = ( unsigned char )( 0x00 >> 8 );
            *pucRegBuffer++ = ( unsigned char )( 0x00 & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    
    return eStatus;
}


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


USHORT m_usAddress;//地址
USHORT m_usNRegs;//寄存器地址
eMBRegisterMode m_eMode;

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                eMBRegisterMode eMode ) {
    eMBErrorCode    eStatus = MB_ENOERR;
    int iRegIndex;
    
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
                            *pucRegBuffer++ = ( uint8_t )( plc_coil_ele[ele_address] >> 8 );
                            *pucRegBuffer++ = ( uint8_t )( plc_coil_ele[ele_address] & 0xFF );
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
                    
                    rtc.year = plc_time[0];
                    rtc.month = plc_time[1];
                    rtc.mday = plc_time[2];
                    rtc.wday = plc_time[3];
                    rtc.hour = plc_time[4];
                    rtc.min = plc_time[5];
                    rtc.sec = plc_time[6];
                    ltk_rtc_set_time(&rtc);
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
            while( iNCoils > 0 )
            {
                *pucRegBuffer++ = 0x00;//xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,( unsigned char )( iNCoils > 8 ? 8 : iNCoils ) );
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
                *pucRegBuffer++;
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
            *pucRegBuffer++ = 0x00;//xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
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


void USART2_IRQHandler(void) {
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		RS232.put(&RS232.ringbuf_n,USART_ReceiveData(USART2));
	}
}
