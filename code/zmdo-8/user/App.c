/** -- 主函数 -- by lhb_steven -- 2017/6/15**/
#include "timer.h"
#include "can.h"
#include "sys.h"
#include "tm1650.h"
#include "outsignal.h"
#include "flash.h"
#include "menu.h"
#include "wdog.h"
#include "externalcount.h"

can_packr_obj pacckr[PACKAGE_NUM];
/** -- 程序入口 -- by lhb_steven -- 2017/6/30**/
int main(void) {
    Stdcanbus CANBUS = {
        {
            {GPIOA, GPIO_Pin_11},
            {GPIOA, GPIO_Pin_12},
            {GPIOA, GPIO_Pin_15},
            4,
            0x1800f001,
            B_10K,
            {0,0,0,0,0,0,0,0},
            {0}
        },
        {0},
        &CanInit,
        &CanSend,
        &CanSetID,
        &CanSetBlt,
        &bxcan_get_packget,
    };
    Stdtm1650 TM1650 = {
        {
            {GPIOF,GPIO_Pin_1},
            {GPIOF,GPIO_Pin_0},
             {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D,
            0x07, 0x7F, 0x6F, 0x76, 0x40,0x79, 0x00,0x7c},
            {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            {0x00,0x00,0x00,0x00},
        },
        &Tm1650Init,
        &Tm1650ShowNex,
        &Tm1650ShowLed,
        &TM1650ScanKey,
    };
    StdTimer TIMER = {
        {
            {0,0,0,0,0,0,0,0,0,0},
        },
        &TimerInit,
        &TimerGetClock
    };
    StdSys SYS = {
        &SysInitRcc,
    };
    Stdoutsignal OUTSIGNAL = {
        {
            {GPIOA, GPIO_Pin_7},
            {GPIOA, GPIO_Pin_6},
            {GPIOA, GPIO_Pin_5},
            {GPIOA, GPIO_Pin_4},
            {GPIOA, GPIO_Pin_8},
            {GPIOA, GPIO_Pin_9},
            {GPIOB, GPIO_Pin_6},
            {GPIOB, GPIO_Pin_7},
            
            {GPIOB, GPIO_Pin_3},
            0,
            0,
            0,
        },
        &OutsignedInit,
        &OutsignalSetout,
        &OutsignalReadout,
        &OutsignalEmergencyStop,
    };
    Stdflash SFLASH = {
        &FLASH_WriteNWord,
        &FLASH_ReadNWord,
        &FLASH_ReadNBit,
    };
    StdMenu MENU = {
        {
            0,
            0,
            0,
            0,
        },
        &MenuAdd,
        &MenuSub,
        &MenuGet,
        &MenuSetVar,
        &MenuGetVar,
    };
    Stdwdog WDOG = {
        &WdogInit,
        &WdogReload,
    };
    Stdexternalcount EXTERNACPUNT = {
        {
            {GPIOB, GPIO_Pin_4, 0},
            {GPIOB, GPIO_Pin_5, 0},
            {GPIOB, GPIO_Pin_1, 0},
            {GPIOB, GPIO_Pin_0, 0},
            {GPIOA, GPIO_Pin_3, 0},
            {GPIOA, GPIO_Pin_2, 0},
            {GPIOA, GPIO_Pin_1, 0},
            {GPIOA, GPIO_Pin_0, 0},
        },
        &ExternalcountInit,
        &ExternalcountRead,
    };
    /** -- 系统初始化 -- by lhb_steven -- 2017/6/17**/
    SYS.SysInitRcc();
    /** -- 继电器 电量检测促使花 -- by lhb_steven -- 2017/6/20**/
    OUTSIGNAL.Init(&OUTSIGNAL.outsignal_n);
    /* --定时器初始化-- */
    TIMER.init();
    TIMER.Timer_n.timer[0] = TIMER.getclock() + 200;//系统脉搏
    TIMER.Timer_n.timer[1] = TIMER.getclock() + 1;//按钮
    TIMER.Timer_n.timer[2] = TIMER.getclock() + 2000;//通讯上报
    TIMER.Timer_n.timer[3] = TIMER.getclock() + 8000;//通讯上报-电量
    TIMER.Timer_n.timer[4] = TIMER.getclock() + 2;//急停检测
    /** -- Can初始化 -- by lhb_steven -- 2017/6/15**/
    CANBUS.Init(&CANBUS.can_n);
    /** -- 电量检测初始化 -- by lhb_steven -- 2017/7/4**/
    EXTERNACPUNT.Init(&EXTERNACPUNT.externalcount_n);
    /** -- 显示 按键初始化 -- by lhb_steven -- 2017/6/19**/
    TM1650.init(&TM1650.tm1650_n);
    /** -- 内存判断 -- by lhb_steven -- 2017/6/21**/
    {
        uint16_t flag_data;//数据写入标志位
        SFLASH.read(&flag_data, FLASH_ADDR_FLAG(0), 1);
        if(0x555A != flag_data)
        {
            flag_data = 0x555A;//将标志位置为"已写入"
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(0), 1);
            flag_data = 99;//默认地址
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(1), 1);
            flag_data = 0;//默认继电器状态
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(2), 1);
            flag_data = B_50K;//默认继电器状态
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(3), 1);
        }
    }
    /** -- 设置地址 -- by lhb_steven -- 2017/7/14**/
    CANBUS.setid(&CANBUS.can_n,SFLASH.readbit(FLASH_ADDR_FLAG(1)));//读取ID
    CANBUS.set_btl(&CANBUS.can_n,(btl_e)SFLASH.readbit(FLASH_ADDR_FLAG(3)));//读取ID
    TM1650.show_nex(&TM1650.tm1650_n,0,CANBUS.can_n.id/10);
    TM1650.show_nex(&TM1650.tm1650_n,1,CANBUS.can_n.id%10);
    /** -- 设置IO状态 -- by lhb_steven -- 2017/7/14**/
//    if(OUTSIGNAL.readstop(&OUTSIGNAL.outsignal_n) == 1) {//只有急停没有按下才可以
//        uint8_t coil_val = 0;
//        OUTSIGNAL.outsignal_n.coil_val = SFLASH.readbit(FLASH_ADDR_FLAG(2));
//        coil_val = OUTSIGNAL.outsignal_n.coil_val;
//        for(uint8_t i = 0;i < 8;i++) {
//            uint8_t val = 1;
//            if ((coil_val & 0x80) == 0) {
//                val = 0;
//            }
//            coil_val <<= 1;
//            /** -- 关闭继电器和指示灯 -- by lhb_steven -- 2017/7/7**/
//            OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,7-i,val);
//            TM1650.show_led(&TM1650.tm1650_n,7-i,val);
//        }
//    }
    /** -- 看门狗初始化 -- by lhb_steven -- 2017/6/26**/
    WDOG.init();
    do{
        /** -- 系统运行灯 -- by lhb_steven -- 2017/6/21**/
        if(TIMER.Timer_n.timer[0] <= TIMER.getclock()) {
            static uint8_t dr = 0;
            TIMER.Timer_n.timer[0] = TIMER.getclock() + 200;
            dr = dr==0?1:0;
            TM1650.show_led(&TM1650.tm1650_n,9,dr);
            /** -- 通讯指示，当接收到1个包闪烁一次 -- by lhb_steven -- 2017/8/1**/
            if(MENU.menu_n.message_packet_num > 0) {
                static uint8_t message_dr = 0; 
                message_dr = message_dr==0?1:0;
                if(message_dr == 0) {
                    MENU.menu_n.message_packet_num--;
                }
                TM1650.show_led(&TM1650.tm1650_n,10,message_dr);
            }
            /** -- 数码管闪烁,提示正在设置地址 -- by lhb_steven -- 2017/6/26**/
            if(MENU.getmenu(&MENU.menu_n) > 0) {
                static uint8_t dr = 0;
                if(dr == 0) {
                    dr = 1;
                    TM1650.show_nex(&TM1650.tm1650_n,0,13);
                    TM1650.show_nex(&TM1650.tm1650_n,1,13);
                } else {
                    dr = 0;
                    if(MENU.getmenu(&MENU.menu_n) == 1) {
                        TM1650.show_nex(&TM1650.tm1650_n,0,MENU.getvar(&MENU.menu_n)/10);
                        TM1650.show_nex(&TM1650.tm1650_n,1,MENU.getvar(&MENU.menu_n)%10);
                    } else {
                        TM1650.show_nex(&TM1650.tm1650_n,0,14);
                        TM1650.show_nex(&TM1650.tm1650_n,1,MENU.getvar(&MENU.menu_n));
                    }
                }
                /** -- 数码管超时不操作，退出 -- by lhb_steven -- 2017/7/31**/
                if(MENU.menu_n.lcd_out_num < 30) {
                    MENU.menu_n.lcd_out_num++;
                } else {
                    MENU.submenu(&MENU.menu_n);
                    TM1650.show_nex(&TM1650.tm1650_n,0,CANBUS.can_n.id/10);
                    TM1650.show_nex(&TM1650.tm1650_n,1,CANBUS.can_n.id%10);
                }
            }
            /** -- 喂狗 -- by lhb_steven -- 2017/6/26**/
            WDOG.reload();            
        }
        /** -- 数据报文 -- by lhb_steven -- 2017/7/14**/
        if(TIMER.Timer_n.timer[2] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[2] = TIMER.getclock() + 3000;
            /** -- 数据上报 -- by lhb_steven -- 2017/7/5**/
//            CANBUS.can_n.TxMessage.StdId = 254;//主机地址
//            CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//本机地址
//            CANBUS.can_n.TxMessage.Data[1] = 0x61;//输出值
//            CANBUS.can_n.TxMessage.Data[7] = OUTSIGNAL.outsignal_n.coil_val;//寄存器值
//            CANBUS.Send(&CANBUS.can_n);
        }
        /** -- 报文-电量 -- by lhb_steven -- 2017/7/14**/
        if(TIMER.Timer_n.timer[3] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[3] = TIMER.getclock() + 8000;
            /** -- 数据上报 -- by lhb_steven -- 2017/7/5**/
        }
        /** -- 急停检测 -- by lhb_steven -- 2017/7/14**/
        if(TIMER.Timer_n.timer[4] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[4] = TIMER.getclock() + 2;
            /** -- 检测急停 -- by lhb_steven -- 2017/7/14**/
            if(OUTSIGNAL.readstop(&OUTSIGNAL.outsignal_n) == 0) {
                OUTSIGNAL.outsignal_n.stop_flag = 1;
            } else {
                if(OUTSIGNAL.outsignal_n.stop_flag == 1) {
                    OUTSIGNAL.outsignal_n.stop_flag = 0;
                    //恢复状态
//                    {
//                        uint8_t coil_val = 0;
//                        OUTSIGNAL.outsignal_n.coil_val = SFLASH.readbit(FLASH_ADDR_FLAG(2));
//                        coil_val = OUTSIGNAL.outsignal_n.coil_val;
//                        for(uint8_t i = 0;i < 8;i++) {
//                            uint8_t val = 1;
//                            if ((coil_val & 0x80) == 0) {
//                                val = 0;
//                            }
//                            coil_val <<= 1;
//                            /** -- 关闭继电器和指示灯 -- by lhb_steven -- 2017/7/7**/
//                            OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,7-i,val);
//                            TM1650.show_led(&TM1650.tm1650_n,7-i,val);
//                        }
//                    }
                }
            }
        }
        if(OUTSIGNAL.outsignal_n.stop_flag == 0) {
            /** -- 按键检测 -- by lhb_steven -- 2017/6/21**/
            if(TIMER.Timer_n.timer[1] <= TIMER.getclock()) {
                TIMER.Timer_n.timer[1] = TIMER.getclock() + 1;
                /** -- 检测按钮 -- by lhb_steven -- 2017/7/14**/
                switch(TM1650.readkey(&TM1650.tm1650_n)) {
                    case 0xe6:
                    TM1650.tm1650_n.key_down_num = 0;
                    break;
                    case 0xfa:
                    TM1650.tm1650_n.key_down_num = 1;
                    break;
                    case 0xea:
                    TM1650.tm1650_n.key_down_num = 2;
                    break;
                    case 0xf6:
                    TM1650.tm1650_n.key_down_num = 3;
                    break;
                    case 0xe2:
                    TM1650.tm1650_n.key_down_num = 4;
                    break;
                    case 0xf2:
                    TM1650.tm1650_n.key_down_num = 5;
                    break;
                    case 0xee:
                    TM1650.tm1650_n.key_down_num = 6;
                    break;
                    case 0x6e:
                    TM1650.tm1650_n.key_down_num = 7;
                    break;
                    case 0x66:
                    TM1650.tm1650_n.key_down_num = 9;
                    break;
                    case 0x7a:
                    TM1650.tm1650_n.key_down_num = 10;
                    break;
                    case 0x6a:
                    TM1650.tm1650_n.key_down_num = 8;
                    break;
                    default:
                    /** -- 判断设置按钮放开 -- by lhb_steven -- 2017/6/26**/
                    if(TM1650.tm1650_n.key_down_num == 9) {
                        if( (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] >= 1) &&
                           (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 34) ){
                               switch(MENU.getmenu(&MENU.menu_n)) {
                                   case 0:
                                   
                                   break;
                                   case 1:
                                   if(MENU.getvar(&MENU.menu_n) < 99) {
                                       MENU.setvar(&MENU.menu_n,MENU.getvar(&MENU.menu_n)+1);
                                   } else {
                                       MENU.setvar(&MENU.menu_n,0);
                                   }
                                   break;
                                   case 2:
                                   if(MENU.getvar(&MENU.menu_n) < 5) {
                                       MENU.setvar(&MENU.menu_n,MENU.getvar(&MENU.menu_n)+1);
                                   } else {
                                       MENU.setvar(&MENU.menu_n,0);
                                   }
                                   break;
                               }
                           }
                    } else if(TM1650.tm1650_n.key_down_num == 10) {
                        if( (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] >= 1) &&
                           (TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 34) ){
                               switch(MENU.getmenu(&MENU.menu_n)) {
                                   case 0:
                                   
                                   break;
                                   case 1:
                                   if(MENU.getvar(&MENU.menu_n) > 0) {
                                       MENU.setvar(&MENU.menu_n,MENU.getvar(&MENU.menu_n)-1);
                                   } else {
                                       MENU.setvar(&MENU.menu_n,99);
                                   }
                                   break;
                                   case 2:
                                   if(MENU.getvar(&MENU.menu_n) > 0) {
                                       MENU.setvar(&MENU.menu_n,MENU.getvar(&MENU.menu_n)-1);
                                   } else {
                                       MENU.setvar(&MENU.menu_n,5);
                                   }
                                   break;
                               }
                           }
                    }
                    TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] = 0;
                    TM1650.tm1650_n.key_down_num = 20;
                    break;
                }
                /** -- 继电器控制按钮检测 -- by lhb_steven -- 2017/6/26**/
                if(TM1650.tm1650_n.key_down_num <= 8) { 
                    if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 10)
                        TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
                    if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 1) {
                        uint8_t var = 0;
                        var = (OUTSIGNAL.readout(&OUTSIGNAL.outsignal_n,TM1650.tm1650_n.key_down_num) == 0)?1:0;
                        OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,TM1650.tm1650_n.key_down_num,var);
                        TM1650.show_led(&TM1650.tm1650_n,TM1650.tm1650_n.key_down_num,var); 
                        /** -- 保存IO状态 -- by lhb_steven -- 2017/7/14**/
                        {
                            uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
                            //SFLASH.write(&val, FLASH_ADDR_FLAG(2), 1);
                            
                            CANBUS.send_msg.send_id = 0xff;	  
                            CANBUS.send_msg.id = CANBUS.can_n.id; 
                            CANBUS.send_msg.device_id = 0xd0;	 
                            CANBUS.send_msg.cmd = 0x01;		 
                            CANBUS.send_msg.len = 1;			
                            CANBUS.send_msg.arr[0]  = OUTSIGNAL.outsignal_n.coil_val;
                            CANBUS.Send(&CANBUS);
                        }
                    }
                    /** -- 地址设置+ 按钮 -- by lhb_steven -- 2017/6/26**/
                } else if(TM1650.tm1650_n.key_down_num == 9) {
                    if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 35)
                        TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
                    if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 34) {
                        switch(MENU.getmenu(&MENU.menu_n)) {
                            case 0:
                            MENU.setvar(&MENU.menu_n,CANBUS.can_n.id);
                            MENU.addmenu(&MENU.menu_n);
                            break;
                            case 1:
                            CANBUS.setid(&CANBUS.can_n,MENU.getvar(&MENU.menu_n));
                            MENU.submenu(&MENU.menu_n);
                            TM1650.show_nex(&TM1650.tm1650_n,0,CANBUS.can_n.id/10);
                            TM1650.show_nex(&TM1650.tm1650_n,1,CANBUS.can_n.id%10);
                            CANBUS.setid(&CANBUS.can_n,CANBUS.can_n.id);
                            SFLASH.write(&CANBUS.can_n.id, FLASH_ADDR_FLAG(1), 1);
                            break;
                        }
                    }
                    /** -- 地址减按钮检测 -- by lhb_steven -- 2017/6/26**/
                } else if(TM1650.tm1650_n.key_down_num == 10) {
                    if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] < 35)
                        TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] ++;
                    if(TM1650.tm1650_n.key_count[TM1650.tm1650_n.key_down_num] == 34) {
                        switch(MENU.getmenu(&MENU.menu_n)) {
                            case 0:
                            MENU.setvar(&MENU.menu_n,CANBUS.can_n.btl);
                            MENU.menu_n.menu_page = 2;
                            break;
                            case 2:
                            CANBUS.set_btl(&CANBUS.can_n,(btl_e)MENU.getvar(&MENU.menu_n));
                            MENU.menu_n.menu_page = 0;
                            TM1650.show_nex(&TM1650.tm1650_n,0,CANBUS.can_n.id/10);
                            TM1650.show_nex(&TM1650.tm1650_n,1,CANBUS.can_n.id%10);
                            uint16_t blt = CANBUS.can_n.btl;
                            SFLASH.write(&blt, FLASH_ADDR_FLAG(3), 1);
                            break;
                        }
                    }
                }
            }
            /** -- can监控 -- by lhb_steven -- 2017/6/21**/
            can_package_obj *pack = CANBUS.get_packget(&CANBUS.can_n);
            for(int i = 0;i < PACKAGE_NUM;i++) {
                uint8_t can_rx_flag = 0;
                if(pack->package[i].flag == F_USE) { /* ??è?êy?Y */
                    for(int j = 0;j < PACKAGE_NUM;j++) {
                        if(pacckr[j].flag == F_USE) { /* ?D??ê?·?ê1ó? */
                            if(pacckr[j].id == pack->package[i].dat[0]) { /* ?D??IDê?·??àí? */
                                for(int k = 0;k < 7;k++) { /* ′ò°ü */
                                    pacckr[j].arr[pacckr[j].pack_bum + k] = pack->package[i].dat[1+k];
                                }
                                pacckr[j].pack_bum += 7;
                                if(pacckr[j].pack_bum >= pacckr[j].len) { /* ?D??′ò°üê?·?íê3é */
                                    pacckr[j].flag = F_PACK_OK; /* ′ò°üíê3é */
                                }
                                can_rx_flag = 1;
                            }
                            break;
                        }
                    }
                    if(can_rx_flag == 0) {
                        if(pack->package[i].dat[1] == 0x3a) { 
                            for(int j = 0;j < PACKAGE_NUM;j++) {
                                if(pacckr[j].flag == F_NO_USE) { 
                                    pacckr[j].id = pack->package[i].dat[0]; 
                                    pacckr[j].device_id = pack->package[i].dat[2];
                                    pacckr[j].len = pack->package[i].dat[3];
                                    pacckr[j].cmd = pack->package[i].dat[4];
                                    for(int k = 0;k < 3;k++) { 
                                        pacckr[j].arr[k] = pack->package[i].dat[5+k];
                                    }
                                    if(pacckr[j].len <= 3) { 
                                        pacckr[j].flag = F_PACK_OK; 
                                    } else {
                                        pacckr[j].pack_bum = 3;
                                        pacckr[j].flag = F_USE; 
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    pack->package[i].flag = F_NO_USE;
                }
            }
       
            for(int i = 0;i < PACKAGE_NUM;i++) { 
                if(pacckr[i].flag == F_PACK_OK) {
                    switch(pacckr[i].device_id) {
                        case 0xf0: { 
                            switch(pacckr[i].cmd) {
                                case 0:
                                
                                break;
                                case 1: {
                                    uint8_t dat = pacckr[i].arr[0];
                                    for(int i = 0;i < 8;i++) {
                                        OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,i,(dat&0x01)<<7); 
                                        TM1650.show_led(&TM1650.tm1650_n,i,(dat&0x01)<<7);
                                         dat>>=1;
                                    }
                                }
                                    break;	
                                case 2:

                                break;
                                case 3:

                                break;
                            }
                        }
                            break;
                    }
                    {
                        uint16_t val = OUTSIGNAL.outsignal_n.coil_val;
                        //SFLASH.write(&val, FLASH_ADDR_FLAG(2), 1);
                    }
                    CANBUS.send_msg.send_id = 0xff;	  
                    CANBUS.send_msg.id = CANBUS.can_n.id; 
                    CANBUS.send_msg.device_id = 0xd0;	 
                    CANBUS.send_msg.cmd = 0x01;		 
                    CANBUS.send_msg.len = 1;			
                    CANBUS.send_msg.arr[0]  = OUTSIGNAL.outsignal_n.coil_val;
                    CANBUS.Send(&CANBUS);

                    pacckr[i].flag = F_NO_USE;
                }
            }
        } else {
            /** -- 关闭继电器和指示灯 -- by lhb_steven -- 2017/7/7**/
            OUTSIGNAL.setout(&OUTSIGNAL.outsignal_n,8,0);
            TM1650.show_led(&TM1650.tm1650_n,8,0);
            
            /** -- can监控 去除包 -- by lhb_steven -- 2017/6/21**/
//            if( CANBUS.readpack(&CANBUS.can_n) ) {
//                
//            }
        }
    }while(1);
}