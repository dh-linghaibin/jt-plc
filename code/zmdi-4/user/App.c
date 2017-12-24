/** -- 主函数 -- by lhb_steven -- 2017/6/15**/
#include "timer.h"
#include "led.h"
#include "can.h"
#include "lcd.h"
#include "key.h"
#include "sys.h"
#include "insignal.h"
#include "flash.h"
#include "menu.h"
#include "wdog.h"

int main(void) {
    /** -- 声明 -- by lhb_steven -- 2017/6/14**/
    can_obj can_bus = {
        99,
        {0,0,0,0,0,0,0,0},
        {0},
        &bxcan_init,
        &bxcan_send,
        &bxcan_set_id,
        &bxcan_get_packget,
    };
    StdLcd LCD = {
        {
            {GPIOB, GPIO_Pin_7},
            {GPIOF, GPIO_Pin_1},
            {GPIOF, GPIO_Pin_0},
            {GPIOA, GPIO_Pin_0},
            {GPIOA, GPIO_Pin_1},
            {GPIOA, GPIO_Pin_2},
            {GPIOB, GPIO_Pin_6},
            {GPIOB, GPIO_Pin_5},
            {GPIOA, GPIO_Pin_3},
            {GPIOB, GPIO_Pin_4},
            {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00},
            {0,0},
            0
        },
        &LcdInit,
        &LcdRefresh,
        &LcdSetNum,
        &LcdTogglePoint
    };
    StdKey KEY = {
        {
            {GPIOA, GPIO_Pin_4},
            {GPIOB, GPIO_Pin_3},
            0,
            0
        },
        &KeyInit,
        &KeyRead
    };
    StdSys SYS = {
        &SysInitRcc,
    };
    StdTimer TIMER = {
        {
            {0,0,0,0,0,0,0,0,0,0},
        },
        &TimerInit,
        &TimerGetClock
    };
    Stdinsignal INSIGNED = {
        {
            {GPIOB, GPIO_Pin_1},
            {GPIOB, GPIO_Pin_0},
            {GPIOA, GPIO_Pin_7},
            {GPIOA, GPIO_Pin_6},
            {GPIOA, GPIO_Pin_5},
            {0,0,0,0},
            {0,0,0,0}
        },
        &InsignedInit,
        &InsignedRead
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
            {0x05,0x01,0x01,0x00,0x00,0x01,0x01,0x01,0x01,0x00,},
            {0x00,0x00,0x00,0x00,0x00},
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
    /** -- 时钟初始化 -- by lhb_steven -- 2017/6/19**/
    SYS.SysInitRcc();
    /** -- 数码管初始化 -- by lhb_steven -- 2017/6/17**/
    LCD.init(&LCD.Lcd_n);
    /* --定时器初始化-- */
    TIMER.init();
    TIMER.Timer_n.timer[0] = TIMER.getclock() + 5;
    TIMER.Timer_n.timer[1] = TIMER.getclock() + 1;
    TIMER.Timer_n.timer[2] = TIMER.getclock() + 200;
    TIMER.Timer_n.timer[4] = TIMER.getclock() + 3000;
    /** -- Can初始化 -- by lhb_steven -- 2017/6/15**/
    can_bus.init(&can_bus);
    /** -- 按钮初始化 -- by lhb_steven -- 2017/6/18**/
    KEY.init(&KEY.Key_n);
    /** -- 输入信号初始化 -- by lhb_steven -- 2017/6/19**/
    INSIGNED.Init(&INSIGNED.insignal_n);
    /** -- 内存判断 -- by lhb_steven -- 2017/6/21**/
    {
        uint16_t flag_data;//数据写入标志位
        SFLASH.read(&flag_data, FLASH_ADDR_FLAG(0), 1);
        if(0x55AA != flag_data)
        {
            flag_data = 0x55AA;//将标志位置为"已写入"
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(0), 1);
            flag_data = 99;//将标志位置为"已写入"
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(1), 1);
        }
    }
    can_bus.set_id(&can_bus,SFLASH.readbit(FLASH_ADDR_FLAG(1)));
    LCD.set(&LCD.Lcd_n,0,can_bus.id/10);
    LCD.set(&LCD.Lcd_n,1,can_bus.id%10);
    /** -- 看门狗初始化 -- by lhb_steven -- 2017/6/26**/
    WDOG.init();
    do{
        /** -- 数码管刷新 -- by lhb_steven -- 2017/6/19**/
        if(TIMER.Timer_n.timer[0] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[0] = TIMER.getclock() + 5;
            LCD.refresh(&LCD.Lcd_n);
        }
        //数码管闪烁
        if(TIMER.Timer_n.timer[2] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[2] = TIMER.getclock() + 200;
            if(MENU.getmenu(&MENU.menu_n) == 1) {
                static uint8_t dr = 0;
                /** -- 设置的时候小数点为长亮 -- by lhb_steven -- 2017/7/31**/
                LCD.point(&LCD.Lcd_n,1,1);
                if(dr == 0) {
                    dr = 1;
                    LCD.set(&LCD.Lcd_n,0,10);
                    LCD.set(&LCD.Lcd_n,1,10);
                } else {
                    dr = 0;
                    LCD.set(&LCD.Lcd_n,0,MENU.getvar(&MENU.menu_n)/10);
                    LCD.set(&LCD.Lcd_n,1,MENU.getvar(&MENU.menu_n)%10);
                }
                /** -- 数码管超时不操作，退出 -- by lhb_steven -- 2017/7/31**/
                if(MENU.menu_n.lcd_out_num < 30) {
                    MENU.menu_n.lcd_out_num++;
                } else {
                    MENU.submenu(&MENU.menu_n);
                    LCD.set(&LCD.Lcd_n,0,can_bus.id/10);
                    LCD.set(&LCD.Lcd_n,1,can_bus.id%10);
                }
            } else {
                /** -- 小数点闪烁 -- by lhb_steven -- 2017/7/31**/
                static uint8_t dr = 0;
                dr = dr==0?1:0;
                LCD.point(&LCD.Lcd_n,1,dr);
            }
            /** -- 通讯监听，1个包闪烁一次 -- by lhb_steven -- 2017/8/1**/
            if(MENU.menu_n.message_packet_num > 0) {
                static uint8_t message_dr = 0;
                message_dr = message_dr==0?1:0;
                LCD.point(&LCD.Lcd_n,0,message_dr);
                if(message_dr == 0) {
                    MENU.menu_n.message_packet_num--;
                }
            }
            /** -- 喂狗 -- by lhb_steven -- 2017/6/26**/
            WDOG.reload();
            /** -- 上传送报文 -- by lhb_steven -- 2017/12/13**/
            if(TIMER.Timer_n.timer[4] <= TIMER.getclock()) {
                TIMER.Timer_n.timer[2] = TIMER.getclock() + 3000;
                /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                can_bus.send_msg.send_id = 0xff;	 
                can_bus.send_msg.id = can_bus.id;
                can_bus.send_msg.device_id = 0xd1;	 
                can_bus.send_msg.cmd = 0x01;	
                can_bus.send_msg.len = 1;			
                can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                can_bus.send(&can_bus);
            }
        }
        /** -- 按键检测 -- by lhb_steven -- 2017/6/19**/
        if(TIMER.Timer_n.timer[1] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[1] = TIMER.getclock() + 1;
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI1) == 0) {
                if(INSIGNED.insignal_n.di_count[0] < 51)
                    INSIGNED.insignal_n.di_count[0]++;
                if(INSIGNED.insignal_n.di_count[0] == 50) {
                    //执行动作
                    if(INSIGNED.insignal_n.val_flag[0] == 0) {
                        INSIGNED.insignal_n.val_flag[0] = 1;
                        INSIGNED.insignal_n.val |= (1 << 0);
                        /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                        can_bus.send_msg.send_id = 0xff;	 
                        can_bus.send_msg.id = can_bus.id;
                        can_bus.send_msg.device_id = 0xd1;	 
                        can_bus.send_msg.cmd = 0x01;	
                        can_bus.send_msg.len = 1;			
                        can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                        can_bus.send(&can_bus);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[0] = 0;
                if(INSIGNED.insignal_n.val_flag[0] == 1) {
                    INSIGNED.insignal_n.val_flag[0] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 0);
                     /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                    can_bus.send_msg.send_id = 0xff;	 
                    can_bus.send_msg.id = can_bus.id;
                    can_bus.send_msg.device_id = 0xd1;	 
                    can_bus.send_msg.cmd = 0x01;	
                    can_bus.send_msg.len = 1;			
                    can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                    can_bus.send(&can_bus);
                }
            }
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI2) == 0) {
                if(INSIGNED.insignal_n.di_count[1] < 51)
                    INSIGNED.insignal_n.di_count[1]++;
                if(INSIGNED.insignal_n.di_count[1] == 50) {
                    //执行动作
                    if(INSIGNED.insignal_n.val_flag[1] == 0) {
                        INSIGNED.insignal_n.val_flag[1] = 1;
                        INSIGNED.insignal_n.val |= (1 << 1);
                        /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                        can_bus.send_msg.send_id = 0xff;	 
                        can_bus.send_msg.id = can_bus.id;
                        can_bus.send_msg.device_id = 0xd1;	 
                        can_bus.send_msg.cmd = 0x01;	
                        can_bus.send_msg.len = 1;			
                        can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                        can_bus.send(&can_bus);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[1] = 0;
                if(INSIGNED.insignal_n.val_flag[1] == 1) {
                    INSIGNED.insignal_n.val_flag[1] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 1);
                     /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                    can_bus.send_msg.send_id = 0xff;	 
                    can_bus.send_msg.id = can_bus.id;
                    can_bus.send_msg.device_id = 0xd1;	 
                    can_bus.send_msg.cmd = 0x01;	
                    can_bus.send_msg.len = 1;			
                    can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                    can_bus.send(&can_bus);
                }
            }
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI3) == 0) {
                if(INSIGNED.insignal_n.di_count[2] < 51)
                    INSIGNED.insignal_n.di_count[2]++;
                if(INSIGNED.insignal_n.di_count[2] == 50) {
                    //执行动作
                    if(INSIGNED.insignal_n.val_flag[2] == 0) {
                        INSIGNED.insignal_n.val_flag[2] = 1;
                        INSIGNED.insignal_n.val |= (1 << 2);
                        /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                        can_bus.send_msg.send_id = 0xff;	 
                        can_bus.send_msg.id = can_bus.id;
                        can_bus.send_msg.device_id = 0xd1;	 
                        can_bus.send_msg.cmd = 0x01;	
                        can_bus.send_msg.len = 1;			
                        can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                        can_bus.send(&can_bus);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[2] = 0;
               if(INSIGNED.insignal_n.val_flag[2] == 1) {
                    INSIGNED.insignal_n.val_flag[2] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 2);
                     /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                    can_bus.send_msg.send_id = 0xff;	 
                    can_bus.send_msg.id = can_bus.id;
                    can_bus.send_msg.device_id = 0xd1;	 
                    can_bus.send_msg.cmd = 0x01;	
                    can_bus.send_msg.len = 1;			
                    can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                    can_bus.send(&can_bus);
                }
            }
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI4) == 0) {
                if(INSIGNED.insignal_n.di_count[3] < 51)
                    INSIGNED.insignal_n.di_count[3]++;
                if(INSIGNED.insignal_n.di_count[3] == 50) {
                    //执行动作
                    if(INSIGNED.insignal_n.val_flag[3] == 0) {
                        INSIGNED.insignal_n.val_flag[3] = 1;
                        INSIGNED.insignal_n.val |= (1 << 3);
                        /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                        can_bus.send_msg.send_id = 0xff;	 
                        can_bus.send_msg.id = can_bus.id;
                        can_bus.send_msg.device_id = 0xd1;	 
                        can_bus.send_msg.cmd = 0x01;	
                        can_bus.send_msg.len = 1;			
                        can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                        can_bus.send(&can_bus);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[3] = 0;
                if(INSIGNED.insignal_n.val_flag[3] == 1) {
                    INSIGNED.insignal_n.val_flag[3] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 3);
                     /** -- 上传报文 -- by lhb_steven -- 2017/12/13**/
                    can_bus.send_msg.send_id = 0xff;	 
                    can_bus.send_msg.id = can_bus.id;
                    can_bus.send_msg.device_id = 0xd1;	 
                    can_bus.send_msg.cmd = 0x01;	
                    can_bus.send_msg.len = 1;			
                    can_bus.send_msg.arr[0] = INSIGNED.insignal_n.val;
                    can_bus.send(&can_bus);
                }
            }
            
            if(KEY.read(&KEY.Key_n.key1) == 0) {
                if(KEY.Key_n.key1_count < 2001)
                    KEY.Key_n.key1_count++;
                if(KEY.Key_n.key1_count == 2000) {
                    //长按
                    switch(MENU.getmenu(&MENU.menu_n)) {
                        case 0:
                            MENU.setvar(&MENU.menu_n,can_bus.id);
                            MENU.addmenu(&MENU.menu_n);
                        break;
                        case 1:
                            can_bus.set_id(&can_bus,MENU.getvar(&MENU.menu_n));
                            MENU.submenu(&MENU.menu_n);
                            LCD.set(&LCD.Lcd_n,0,can_bus.id/10);
                            LCD.set(&LCD.Lcd_n,1,can_bus.id%10);
                            SFLASH.write(&can_bus.id, FLASH_ADDR_FLAG(1), 1);
                        break;
                    }
                }
            } else {
                if( (KEY.Key_n.key1_count > 50) && (KEY.Key_n.key1_count < 2000) ) {
                    KEY.Key_n.key1_count = 0;
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
                    }
                }
                KEY.Key_n.key1_count = 0;
            }
            
            if(KEY.read(&KEY.Key_n.key2) == 0) {
                if(KEY.Key_n.key2_count < 2001)
                    KEY.Key_n.key2_count++;
                if(KEY.Key_n.key2_count == 2000) {
                    //长按
//                    CANBUS.can_n.TxMessage.StdId = 10;
//                    CANBUS.can_n.TxMessage.Data[0] = 0x55;
//                    CANBUS.can_n.TxMessage.Data[7] = 0x55;
//                    CANBUS.Send(&CANBUS.can_n);
                }
            } else {
                if( (KEY.Key_n.key2_count > 50) && (KEY.Key_n.key2_count < 2000) ) {
                    KEY.Key_n.key2_count = 0;
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
                    }
                }
                KEY.Key_n.key2_count = 0;
            }
        }
    }while(1);
}