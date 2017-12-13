/** -- ������ -- by lhb_steven -- 2017/6/15**/
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
    /** -- ���� -- by lhb_steven -- 2017/6/14**/
    Stdcanbus CANBUS = {
        {
            {GPIOA, GPIO_Pin_11},
            {GPIOA, GPIO_Pin_12},
            {GPIOA, GPIO_Pin_15},
            99,
            0x1800f001,
            B_10K,
            {0,0,0,0,0,0,0,0},
            {0},
            0,
        }, 
        &CanInit,
        &CanSend,
        &CanSetID,
        &CanSetBlt,
        &CanReadPackage,
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
            {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x7c},
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
        &TimerGetClock,
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
        &MenuImplementCmd,
    };
    Stdwdog WDOG = {
        &WdogInit,
        &WdogReload,
    };
    /** -- ʱ�ӳ�ʼ�� -- by lhb_steven -- 2017/6/19**/
    SYS.SysInitRcc();
    /** -- ����ܳ�ʼ�� -- by lhb_steven -- 2017/6/17**/
    LCD.init(&LCD.Lcd_n);
    /* --��ʱ����ʼ��-- */
    TIMER.init();
    TIMER.Timer_n.timer[0] = TIMER.getclock() + 5;
    TIMER.Timer_n.timer[1] = TIMER.getclock() + 1;
    TIMER.Timer_n.timer[2] = TIMER.getclock() + 200;
    TIMER.Timer_n.timer[5] = TIMER.getclock() + 3000;
    /** -- Can��ʼ�� -- by lhb_steven -- 2017/6/15**/
    CANBUS.Init(&CANBUS.can_n);
    /** -- ��ť��ʼ�� -- by lhb_steven -- 2017/6/18**/
    KEY.init(&KEY.Key_n);
    /** -- �����źų�ʼ�� -- by lhb_steven -- 2017/6/19**/
    INSIGNED.Init(&INSIGNED.insignal_n);
    /** -- �ڴ��ж� -- by lhb_steven -- 2017/6/21**/
    {
        uint16_t flag_data;//����д���־λ
        SFLASH.read(&flag_data, FLASH_ADDR_FLAG(0), 1);
        if(0x555A != flag_data)
        {
            flag_data = 0x555A;//����־λ��Ϊ"��д��"
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(0), 1);
            flag_data = 99;//����־λ��Ϊ"��д��"
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(1), 1);
            flag_data = B_10K;//Ĭ�ϼ̵���״̬
            SFLASH.write(&flag_data, FLASH_ADDR_FLAG(3), 1);
        }
    }
    CANBUS.setid(&CANBUS.can_n,SFLASH.readbit(FLASH_ADDR_FLAG(1)));//��ȡID
    CANBUS.set_btl(&CANBUS.can_n,(btl_e)SFLASH.readbit(FLASH_ADDR_FLAG(3)));//��ȡID
    LCD.set(&LCD.Lcd_n,0,CANBUS.can_n.id/10);
    LCD.set(&LCD.Lcd_n,1,CANBUS.can_n.id%10);
    /** -- ���Ź���ʼ�� -- by lhb_steven -- 2017/6/26**/
    WDOG.init();
    do{
        /** -- �����ˢ�� -- by lhb_steven -- 2017/6/19**/
        if(TIMER.Timer_n.timer[0] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[0] = TIMER.getclock() + 5;
            LCD.refresh(&LCD.Lcd_n);
        }
        //�������˸
        if(TIMER.Timer_n.timer[2] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[2] = TIMER.getclock() + 200;
            if(MENU.getmenu(&MENU.menu_n) > 0) {
                static uint8_t dr = 0;
                /** -- ���õ�ʱ��С����Ϊ���� -- by lhb_steven -- 2017/7/31**/
                LCD.point(&LCD.Lcd_n,1,1);
                if(dr == 0) {
                    dr = 1;
                    LCD.set(&LCD.Lcd_n,0,10);
                    LCD.set(&LCD.Lcd_n,1,10);
                } else {
                    dr = 0;
                    if(MENU.getmenu(&MENU.menu_n) == 1) {
                        LCD.set(&LCD.Lcd_n,0,MENU.getvar(&MENU.menu_n)/10);
                        LCD.set(&LCD.Lcd_n,1,MENU.getvar(&MENU.menu_n)%10);
                    } else {
                        LCD.set(&LCD.Lcd_n,0,11);
                        LCD.set(&LCD.Lcd_n,1,MENU.menu_n.m_var);
                    }
                }
                /** -- ����ܳ�ʱ���������˳� -- by lhb_steven -- 2017/7/31**/
                if(MENU.menu_n.lcd_out_num < 30) {
                    MENU.menu_n.lcd_out_num++;
                } else {
                    MENU.submenu(&MENU.menu_n);
                    LCD.set(&LCD.Lcd_n,0,CANBUS.can_n.id/10);
                    LCD.set(&LCD.Lcd_n,1,CANBUS.can_n.id%10);
                }
            } else {
                /** -- С������˸ -- by lhb_steven -- 2017/7/31**/
                static uint8_t dr = 0;
                dr = dr==0?1:0;
                LCD.point(&LCD.Lcd_n,1,dr);
            }
            /** -- ͨѶ������1������˸һ�� -- by lhb_steven -- 2017/8/1**/
            if(MENU.menu_n.message_packet_num > 0) {
                static uint8_t message_dr = 0;
                message_dr = message_dr==0?1:0;
                LCD.point(&LCD.Lcd_n,0,message_dr);
                if(message_dr == 0) {
                    MENU.menu_n.message_packet_num--;
                }
            }
            /** -- ι�� -- by lhb_steven -- 2017/6/26**/
            WDOG.reload();
            /** -- �Զ����� -- by lhb_steven -- 2017/7/7**/
            if(TIMER.Timer_n.timer[3] > 5) {
                TIMER.Timer_n.timer[3] = 0;
                //����
                CANBUS.can_n.lock = 0;
            } else {
                TIMER.Timer_n.timer[3]++;
            }
            /** -- �ϴ��ͱ��� -- by lhb_steven -- 2017/12/13**/
            if(TIMER.Timer_n.timer[5] <= TIMER.getclock()) {
                TIMER.Timer_n.timer[5] = TIMER.getclock() + 3000;
                /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                CANBUS.Send(&CANBUS.can_n);
            }
        }
        /** -- ������� -- by lhb_steven -- 2017/6/19**/
        if(TIMER.Timer_n.timer[1] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[1] = TIMER.getclock() + 1;
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI1) == 0) {
                if(INSIGNED.insignal_n.di_count[0] < 51)
                    INSIGNED.insignal_n.di_count[0]++;
                if(INSIGNED.insignal_n.di_count[0] == 50) {
                    //ִ�ж���
                    MENU.implement(MENU.menu_n,&CANBUS,0,SFLASH);
                    if(INSIGNED.insignal_n.val_flag[0] == 0) {
                        INSIGNED.insignal_n.val_flag[0] = 1;
                        INSIGNED.insignal_n.val |= (1 << 0);
                        /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                        CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                        CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                        CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                        CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                        CANBUS.Send(&CANBUS.can_n);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[0] = 0;
                if(INSIGNED.insignal_n.val_flag[0] == 1) {
                    INSIGNED.insignal_n.val_flag[0] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 0);
                     /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                    CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                    CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                    CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                    CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                    CANBUS.Send(&CANBUS.can_n);
                }
            }
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI2) == 0) {
                if(INSIGNED.insignal_n.di_count[1] < 51)
                    INSIGNED.insignal_n.di_count[1]++;
                if(INSIGNED.insignal_n.di_count[1] == 50) {
                    //ִ�ж���
                    MENU.implement(MENU.menu_n,&CANBUS,1,SFLASH);
                    if(INSIGNED.insignal_n.val_flag[1] == 0) {
                        INSIGNED.insignal_n.val_flag[1] = 1;
                        INSIGNED.insignal_n.val |= (1 << 1);
                        /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                        CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                        CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                        CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                        CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                        CANBUS.Send(&CANBUS.can_n);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[1] = 0;
                if(INSIGNED.insignal_n.val_flag[1] == 1) {
                    INSIGNED.insignal_n.val_flag[1] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 1);
                     /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                    CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                    CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                    CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                    CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                    CANBUS.Send(&CANBUS.can_n);
                }
            }
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI3) == 0) {
                if(INSIGNED.insignal_n.di_count[2] < 51)
                    INSIGNED.insignal_n.di_count[2]++;
                if(INSIGNED.insignal_n.di_count[2] == 50) {
                    //ִ�ж���
                    MENU.implement(MENU.menu_n,&CANBUS,2,SFLASH);
                    if(INSIGNED.insignal_n.val_flag[2] == 0) {
                        INSIGNED.insignal_n.val_flag[2] = 1;
                        INSIGNED.insignal_n.val |= (1 << 2);
                        /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                        CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                        CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                        CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                        CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                        CANBUS.Send(&CANBUS.can_n);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[2] = 0;
               if(INSIGNED.insignal_n.val_flag[2] == 1) {
                    INSIGNED.insignal_n.val_flag[2] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 2);
                     /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                    CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                    CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                    CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                    CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                    CANBUS.Send(&CANBUS.can_n);
                }
            }
            
            if(INSIGNED.read(&INSIGNED.insignal_n.DI4) == 0) {
                if(INSIGNED.insignal_n.di_count[3] < 51)
                    INSIGNED.insignal_n.di_count[3]++;
                if(INSIGNED.insignal_n.di_count[3] == 50) {
                    //ִ�ж���
                    MENU.implement(MENU.menu_n,&CANBUS,3,SFLASH);
                    if(INSIGNED.insignal_n.val_flag[3] == 0) {
                        INSIGNED.insignal_n.val_flag[3] = 1;
                        INSIGNED.insignal_n.val |= (1 << 3);
                        /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                        CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                        CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                        CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                        CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                        CANBUS.Send(&CANBUS.can_n);
                    }
                }
            } else {
                INSIGNED.insignal_n.di_count[3] = 0;
                if(INSIGNED.insignal_n.val_flag[3] == 1) {
                    INSIGNED.insignal_n.val_flag[3] = 0;
                    INSIGNED.insignal_n.val &= ~(1 << 3);
                     /** -- �ϴ����� -- by lhb_steven -- 2017/12/13**/
                    CANBUS.can_n.TxMessage.StdId = 254;//������ַ
                    CANBUS.can_n.TxMessage.Data[0] = CANBUS.can_n.id;//������ַ
                    CANBUS.can_n.TxMessage.Data[1] = 0x61;//���ֵ
                    CANBUS.can_n.TxMessage.Data[7] = INSIGNED.insignal_n.val ;//�Ĵ���ֵ
                    CANBUS.Send(&CANBUS.can_n);
                }
            }
            
            if(KEY.read(&KEY.Key_n.key1) == 0) {
                if(KEY.Key_n.key1_count < 2001)
                    KEY.Key_n.key1_count++;
                if(KEY.Key_n.key1_count == 2000) {
                    //����
                    switch(MENU.getmenu(&MENU.menu_n)) {
                        case 0:
                            MENU.setvar(&MENU.menu_n,CANBUS.can_n.id);
                            MENU.addmenu(&MENU.menu_n);
                        break;
                        case 1:
                            CANBUS.setid(&CANBUS.can_n,MENU.getvar(&MENU.menu_n));
                            MENU.submenu(&MENU.menu_n);
                            LCD.set(&LCD.Lcd_n,0,CANBUS.can_n.id/10);
                            LCD.set(&LCD.Lcd_n,1,CANBUS.can_n.id%10);
                            CANBUS.setid(&CANBUS.can_n,CANBUS.can_n.id);
                            SFLASH.write(&CANBUS.can_n.id, FLASH_ADDR_FLAG(1), 1);
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
                        case 2:
                            if(MENU.getvar(&MENU.menu_n) < 5) {
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
                    //����
//                    CANBUS.can_n.TxMessage.StdId = 10;
//                    CANBUS.can_n.TxMessage.Data[0] = 0x55;
//                    CANBUS.can_n.TxMessage.Data[7] = 0x55;
//                    CANBUS.Send(&CANBUS.can_n);
                    //����
                    switch(MENU.getmenu(&MENU.menu_n)) {
                        case 0:
                            MENU.setvar(&MENU.menu_n,CANBUS.can_n.btl);
                            MENU.menu_n.menu_page = 2;
                        break;
                        case 2:
                            CANBUS.set_btl(&CANBUS.can_n,(btl_e)MENU.getvar(&MENU.menu_n));
                            MENU.menu_n.menu_page = 0;
                            LCD.set(&LCD.Lcd_n,0,CANBUS.can_n.id/10);
                            LCD.set(&LCD.Lcd_n,1,CANBUS.can_n.id%10);
                            uint16_t blt = CANBUS.can_n.btl;
                            SFLASH.write(&blt, FLASH_ADDR_FLAG(3), 1);
                        break;
                    }
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
                        case 2:
                            if(MENU.getvar(&MENU.menu_n) > 0) {
                                MENU.setvar(&MENU.menu_n,MENU.getvar(&MENU.menu_n)-1);
                            } else {
                                MENU.setvar(&MENU.menu_n,5);
                            }
                        break;
                    }
                }
                KEY.Key_n.key2_count = 0;
            }
        }
        /** -- can��� -- by lhb_steven -- 2017/6/21**/
        if( CANBUS.readpack(&CANBUS.can_n) ) {
            /** -- ͨѶ������ -- by lhb_steven -- 2017/8/1**/
            MENU.menu_n.message_packet_num++;
            /** -- �������� -- by lhb_steven -- 2017/6/30**/
            switch(CANBUS.can_n.lock) {
                case 0x00://����״̬
                    switch(CANBUS.can_n.package[0]) {
                        case 0xf2://��������
                            if( (CANBUS.can_n.package[1] == 0x67) && \
                                (CANBUS.can_n.package[2] == 0x66) && \
                                (CANBUS.can_n.package[3] == 0x65) && \
                                (CANBUS.can_n.package[4] == 0x64) && \
                                (CANBUS.can_n.package[5] == 0x63) && \
                                (CANBUS.can_n.package[6] == 0x62) && \
                                (CANBUS.can_n.package[7] == 0x61) ) {
                                //����������ȷ ����
                                CANBUS.can_n.lock = 1;
                            }
                        break;
                        
                        case 0xf1://��������
                            if( (CANBUS.can_n.package[1] == 0x87) && \
                                (CANBUS.can_n.package[2] == 0x86) && \
                                (CANBUS.can_n.package[3] == 0x85) && \
                                (CANBUS.can_n.package[4] == 0x84) && \
                                (CANBUS.can_n.package[5] == 0x83) && \
                                (CANBUS.can_n.package[6] == 0x82) && \
                                (CANBUS.can_n.package[7] == 0x81) ) {
                                //����������ȷ ����
                                CANBUS.can_n.lock = 0;
                            }
                        break;
                        
                        default:
                        
                        break;
                    }
                break;
                
                case 0x01://�����ɹ�
                    if( (CANBUS.can_n.package[0] == 0xf1) && \
                    (CANBUS.can_n.package[1] == 0x87) && \
                    (CANBUS.can_n.package[2] == 0x86) && \
                    (CANBUS.can_n.package[3] == 0x85) && \
                    (CANBUS.can_n.package[4] == 0x84) && \
                    (CANBUS.can_n.package[5] == 0x83) && \
                    (CANBUS.can_n.package[6] == 0x82) && \
                    (CANBUS.can_n.package[7] == 0x81) ) {
                        //����������ȷ ����
                        CANBUS.can_n.lock = 0;
                    } else {
                        TIMER.Timer_n.timer[3] = 0;
                        if( (CANBUS.can_n.package[7] == 0) && (CANBUS.can_n.package[6] == 0) && 
                        (CANBUS.can_n.package[5] == 0) && (CANBUS.can_n.package[4] == 0) &&
                        (CANBUS.can_n.package[2] == 0) && (CANBUS.can_n.package[1] == 0) ) {
                            //�����ĸ���ť����
                            MENU.menu_n.cmd_merry_switch = CANBUS.can_n.package[0] - 1;
                            MENU.menu_n.cmd_merry_count = 0;
                        } else {
                            if(MENU.menu_n.cmd_merry_switch < 4) {
                                /** -- �Ÿ��ֽ�---����ӵ�ַ -- by lhb_steven -- 2017/7/7**/
                                MENU.menu_n.cmd_merry[0] = CANBUS.can_n.package[2];
                                for(uint8_t cmd = 0;cmd < 8;cmd++) {
                                    MENU.menu_n.cmd_merry[cmd+1] = CANBUS.can_n.package[cmd];
                                }
                                /** -- ��ӽ������ţ����滹������Ḳ�� -- by lhb_steven -- 2017/7/7**/
                                MENU.menu_n.cmd_merry[9] = 0x00;
                                
                                for(uint8_t cmd = 0;cmd < 5;cmd++) {
                                    MENU.menu_n.cmd_merry_flash[cmd] = (uint16_t)(MENU.menu_n.cmd_merry[cmd*2] << 8);
                                    MENU.menu_n.cmd_merry_flash[cmd] |= (uint16_t)MENU.menu_n.cmd_merry[(cmd*2)+1];
                                }
                                MENU.menu_n.cmd_merry_flash[5] = 0x00;
                                SFLASH.write(MENU.menu_n.cmd_merry_flash, FLASH_ADDR_FLAG(10+(MENU.menu_n.cmd_merry_switch*120)+(MENU.menu_n.cmd_merry_count*5)), 6);
                                //�ڼ�����
                                MENU.menu_n.cmd_merry_count++;
                            }
                        }
                    }
                break;
            }
        }
    }while(1);
}