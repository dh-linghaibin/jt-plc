/** -- ������ -- by lhb_steven -- 2017/6/15**/
#include "timer.h"
#include "led.h"
#include "usart.h"
#include "can.h"
#include "wdog.h"
#include "sys.h"
#include "flash.h"



int main(void) {
    Stdcanbus CANBUS = {
        {
            {GPIOA, GPIO_Pin_11},
            {GPIOA, GPIO_Pin_12},
            {GPIOA, GPIO_Pin_4},
            253,
            0x1800f001,
        },
        &CanInit,
        &CanSend,
        &CanSetID,
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
    Stdflash SFLASH = {
        &FLASH_WriteNWord,
        &FLASH_ReadNWord,
        &FLASH_ReadNBit,
    };
    Stdwdog WDOG = {
        &WdogInit,
        &WdogReload,
    };
    StdLed LED = {
        {
            {GPIOA, GPIO_Pin_5}
        },
        &LedInit,
        &LedSet,
        &LedTiger,
    };
    StdUart UART = {
        {
            0,
            0,
            0,
            {0,0,},
        },
        &UsartInit,
        &UsartSendByte,
        &UARTReadPackageP,
        &UARTReadPackage,
        &UsartPackInit
    };
    /** -- ϵͳʱ�ӳ�ʼ���� -- by lhb_steven -- 2017/6/26**/
    SYS.SysInitRcc();
    /** -- ���еƳ�ʼ�� -- by lhb_steven -- 2017/6/26**/
    LED.init(&LED.Led_n);
    /** -- ��ʱ����ʼ�� -- by lhb_steven -- 2017/6/26**/
    TIMER.init();
    TIMER.Timer_n.timer[0] = TIMER.getclock() + 200;
    /** -- ���ڳ�ʼ�� -- by lhb_steven -- 2017/6/26**/
    UART.init();
    /** -- canͨѶ��ʼ�� -- by lhb_steven -- 2017/6/30**/
    CANBUS.Init(&CANBUS.can_n);
    /** -- ���Ź���ʼ�� -- by lhb_steven -- 2017/6/26**/
    WDOG.init();
    do{
        /** -- run ϵͳ���� -- by lhb_steven -- 2017/6/30**/
        if(TIMER.Timer_n.timer[0] <= TIMER.getclock()) {
            TIMER.Timer_n.timer[0] = TIMER.getclock() + 200;
            LED.Tiger(&LED.Led_n);
            /** -- ι�� -- by lhb_steven -- 2017/6/30**/
            WDOG.reload();
            /** -- ������ֹ�쳣�� -- by lhb_steven -- 2017/7/7**/
            if(UART.Uart_n.Ble_en > 5) {
                UART.Uart_n.Ble_en = 0;
                //��ʼ������
                UART.packinit();
                //����
                UART.Uart_n.lock = 0;
                UART.Uart_n.pro_lock = 0;
            } else {
                UART.Uart_n.Ble_en++;
            }
            //UART.sendbyte(0xa5);
        }
        /** -- ��ȡ���� -- by lhb_steven -- 2017/6/30**/ 
        if(UART.getpack(&UART.Uart_n)) {
            if(UART.readpack(&UART.Uart_n,8)) {
                //���յ�����
                UART.Uart_n.Ble_en = 0;
                switch(UART.Uart_n.lock) {
                    case 0: {
                        switch(UART.Uart_n.package[7]) {
                            case 0x5a:{
                                //���ؼ���
                                if( (UART.Uart_n.package[7] == 0x5a) && \
                                    (UART.Uart_n.package[6] == 0xa5) && \
                                    (UART.Uart_n.package[5] == 0x5a) && \
                                    (UART.Uart_n.package[4] == 0xa5) && \
                                    (UART.Uart_n.package[3] == 0xaa) && \
                                    (UART.Uart_n.package[2] == 0xa5) && \
                                    (UART.Uart_n.package[1] == 0x5a) && \
                                    (UART.Uart_n.package[0] == 0xa5) ) {
                                    UART.Uart_n.lock = 1;
                                } else {
                                    UART.Uart_n.lock = 0;
                                }
                            }
                                break;
                            case 0x1a:{
                                 //�ж��������� �������ö�ʱ��
                                if( (UART.Uart_n.package[7] == 0x1a) && \
                                    (UART.Uart_n.package[6] == 0x25) && \
                                    (UART.Uart_n.package[5] == 0x3a) && \
                                    (UART.Uart_n.package[4] == 0x45) && \
                                    (UART.Uart_n.package[3] == 0x5a) && \
                                    (UART.Uart_n.package[2] == 0x65) && \
                                    (UART.Uart_n.package[1] == 0x7a) && \
                                    (UART.Uart_n.package[0] == 0x85) ) {
                                    UART.Uart_n.lock = 3;
                                } else {
                                    UART.Uart_n.lock = 0;
                                }
                            }
                                break;
                            case 0xcf:{
                                //��������
                                UART.Uart_n.lock = 5;
                                CANBUS.can_n.TxMessage.StdId = UART.Uart_n.package[2];
                            }
                        }
                    }
                        break;
                    
                    case 1://ȷ�Ͻ���
                        if( (UART.Uart_n.package[7] == 0x55) && \
                            (UART.Uart_n.package[6] == 0x45) && \
                            (UART.Uart_n.package[5] == 0x35) && \
                            (UART.Uart_n.package[4] == 0x25) && \
                            (UART.Uart_n.package[3] == 0x15) && \
                            (UART.Uart_n.package[2] == 0x55) && \
                            (UART.Uart_n.package[1] == 0x54) && \
                            (UART.Uart_n.package[0] == 0x53) ) {
                            UART.Uart_n.lock = 2;
                        } else {
                            UART.Uart_n.lock = 0;
                        }
                    break;
                    
                    case 2: {
                        if( (UART.Uart_n.package[7] == 0xf5) && \
                            (UART.Uart_n.package[6] == 0xf5) && \
                            (UART.Uart_n.package[5] == 0xf5) && \
                            (UART.Uart_n.package[4] == 0xf5) && \
                            (UART.Uart_n.package[3] == 0xf5) && \
                            (UART.Uart_n.package[2] == 0xf5) && \
                            (UART.Uart_n.package[1] == 0xf5) && \
                            (UART.Uart_n.package[0] == 0xf5) ) {
                            //����
                            UART.Uart_n.lock = 0;
                            if(UART.Uart_n.pro_lock > 0) {
                                UART.Uart_n.pro_lock = 0;
                                //��ť���Ӽ���
                                CANBUS.can_n.TxMessage.Data[0] = 0xf1;//��������
                                CANBUS.can_n.TxMessage.Data[1] = 0x87;//������
                                CANBUS.can_n.TxMessage.Data[2] = 0x86;
                                CANBUS.can_n.TxMessage.Data[3] = 0x85;
                                CANBUS.can_n.TxMessage.Data[4] = 0x84;
                                CANBUS.can_n.TxMessage.Data[5] = 0x83;
                                CANBUS.can_n.TxMessage.Data[6] = 0x82;
                                CANBUS.can_n.TxMessage.Data[7] = 0x81;
                                CANBUS.Send(&CANBUS.can_n);
                            }
                        } else {
                            if( (UART.Uart_n.package[7] == 0xa5) && \
                                (UART.Uart_n.package[6] == 0) && \
                                (UART.Uart_n.package[5] == 0) && \
                                (UART.Uart_n.package[4] == 0) && \
                                (UART.Uart_n.package[4] == 0) && \
                                (UART.Uart_n.package[4] == 0) ) {//Ϊ����Ӧ���õ��������
                                //��ť���Ӽ��� �ȼ����������µ�ַ
                                if(UART.Uart_n.pro_lock != 0) {
                                    if( CANBUS.can_n.TxMessage.StdId !=  UART.Uart_n.package[3]) {
                                        CANBUS.can_n.TxMessage.Data[0] = 0xf1;//��������
                                        CANBUS.can_n.TxMessage.Data[1] = 0x87;//������
                                        CANBUS.can_n.TxMessage.Data[2] = 0x86;
                                        CANBUS.can_n.TxMessage.Data[3] = 0x85;
                                        CANBUS.can_n.TxMessage.Data[4] = 0x84;
                                        CANBUS.can_n.TxMessage.Data[5] = 0x83;
                                        CANBUS.can_n.TxMessage.Data[6] = 0x82;
                                        CANBUS.can_n.TxMessage.Data[7] = 0x81;
                                        CANBUS.Send(&CANBUS.can_n);
                                        //�����ٿ���
                                        UART.Uart_n.pro_lock = 0;
                                    }
                                }
                                //���ð�ť��ַ
                                CANBUS.can_n.TxMessage.StdId = UART.Uart_n.package[3];
                            }
                            switch(UART.Uart_n.pro_lock) {
                                case 0x00://����
                                    CANBUS.can_n.TxMessage.Data[0] = 0xf2;//��������
                                    CANBUS.can_n.TxMessage.Data[1] = 0x67;//������
                                    CANBUS.can_n.TxMessage.Data[2] = 0x66;
                                    CANBUS.can_n.TxMessage.Data[3] = 0x65;
                                    CANBUS.can_n.TxMessage.Data[4] = 0x64;
                                    CANBUS.can_n.TxMessage.Data[5] = 0x63;
                                    CANBUS.can_n.TxMessage.Data[6] = 0x62;
                                    CANBUS.can_n.TxMessage.Data[7] = 0x61;
                                    CANBUS.Send(&CANBUS.can_n);
                                    UART.Uart_n.pro_lock = 1;
                                break;
                                    
                                case 0x01:
                                    
                                break;
                            }
                            
                            for(uint8_t i = 0;i < 8;i++) {
                                CANBUS.can_n.TxMessage.Data[i] = UART.Uart_n.package[i];
                            }
                            CANBUS.Send(&CANBUS.can_n);
                        }
                    }
                        break;
                    case 3: {//��ʱ���ڶ��ν���
                         if( (UART.Uart_n.package[7] == 0x55) && \
                            (UART.Uart_n.package[6] == 0x45) && \
                            (UART.Uart_n.package[5] == 0x35) && \
                            (UART.Uart_n.package[4] == 0x25) && \
                            (UART.Uart_n.package[3] == 0x15) && \
                            (UART.Uart_n.package[2] == 0x55) && \
                            (UART.Uart_n.package[1] == 0x54) && \
                            (UART.Uart_n.package[0] == 0x53) ) {
                            UART.Uart_n.lock = 4;
                            CANBUS.can_n.TxMessage.StdId = 254;
                        } else {
                            UART.Uart_n.lock = 0;
                        }
                    }
                        break;
                    case 4: {//��ʱ����ʼ���
                        if( (UART.Uart_n.package[7] == 0xf5) && \
                            (UART.Uart_n.package[6] == 0xf5) && \
                            (UART.Uart_n.package[5] == 0xf5) && \
                            (UART.Uart_n.package[4] == 0xf5) && \
                            (UART.Uart_n.package[3] == 0xf5) && \
                            (UART.Uart_n.package[2] == 0xf5) && \
                            (UART.Uart_n.package[1] == 0xf5) && \
                            (UART.Uart_n.package[0] == 0xf5) ) {
                            //����
                            UART.Uart_n.lock = 0;
                            if(UART.Uart_n.pro_lock > 0) {
                                UART.Uart_n.pro_lock = 0;
                                //��ť���Ӽ���
                                CANBUS.can_n.TxMessage.Data[0] = 0xf1;//��������
                                CANBUS.can_n.TxMessage.Data[1] = 0x87;//������
                                CANBUS.can_n.TxMessage.Data[2] = 0x86;
                                CANBUS.can_n.TxMessage.Data[3] = 0x85;
                                CANBUS.can_n.TxMessage.Data[4] = 0x84;
                                CANBUS.can_n.TxMessage.Data[5] = 0x83;
                                CANBUS.can_n.TxMessage.Data[6] = 0x82;
                                CANBUS.can_n.TxMessage.Data[7] = 0x81;
                                CANBUS.Send(&CANBUS.can_n);
                            }
                        } else {//��ʼ���
                            switch(UART.Uart_n.pro_lock) {
                                case 0x00://����
                                    CANBUS.can_n.TxMessage.Data[0] = 0xf2;//��������
                                    CANBUS.can_n.TxMessage.Data[1] = 255;//������
                                    CANBUS.can_n.TxMessage.Data[2] = 0x66;
                                    CANBUS.can_n.TxMessage.Data[3] = 0x65;
                                    CANBUS.can_n.TxMessage.Data[4] = 0x64;
                                    CANBUS.can_n.TxMessage.Data[5] = 0x63;
                                    CANBUS.can_n.TxMessage.Data[6] = 0x62;
                                    CANBUS.can_n.TxMessage.Data[7] = 0x61;
                                    CANBUS.Send(&CANBUS.can_n);
                                    UART.Uart_n.pro_lock = 1;
                                break;
                                    
                                case 0x01:
                                    
                                break;
                            }
                            
                            for(uint8_t i = 0;i < 8;i++) {
                                CANBUS.can_n.TxMessage.Data[i] = UART.Uart_n.package[i];
                            }
                            CANBUS.Send(&CANBUS.can_n);
                        }
                    }
                        break;
                    case 5:{
                        //���Ƽ̵���
                        CANBUS.can_n.TxMessage.Data[0] = UART.Uart_n.package[0];
                        CANBUS.can_n.TxMessage.Data[1] = UART.Uart_n.package[1];
                        CANBUS.can_n.TxMessage.Data[2] = UART.Uart_n.package[2];
                        CANBUS.can_n.TxMessage.Data[3] = UART.Uart_n.package[3];
                        CANBUS.can_n.TxMessage.Data[4] = UART.Uart_n.package[4];
                        CANBUS.can_n.TxMessage.Data[5] = UART.Uart_n.package[5];
                        CANBUS.can_n.TxMessage.Data[6] = UART.Uart_n.package[6];
                        CANBUS.can_n.TxMessage.Data[7] = UART.Uart_n.package[7];
                        CANBUS.Send(&CANBUS.can_n);
                        UART.Uart_n.lock = 0;
                    }
                        break;
                }
            }
        }
    }while(1);
}
