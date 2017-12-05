/* 引用头文件 *****************************************************************/
#include "menu.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
void MenuAdd(StdMenu_n* menu_n) {
    menu_n->menu_page++;
}

void MenuSub(StdMenu_n* menu_n) {
    if(menu_n->menu_page > 0)
        menu_n->menu_page--;
}

uint8_t MenuGet(StdMenu_n* menu_n) {
    return menu_n->menu_page;
}

void MenuSetVar(StdMenu_n* menu_n,uint8_t var) {
    menu_n->m_var = var;
    menu_n->lcd_out_num = 0;//操作过后多少次才退出
}

uint8_t MenuGetVar(StdMenu_n* menu_n) {
    return menu_n->m_var;
}

void MenuImplementCmd(StdMenu_n menu_n,Stdcanbus *can_n,uint8_t di_num,Stdflash flash) {
    uint8_t cmd_count = 0;
    
    can_n->can_n.TxMessage.StdId = 254;
    can_n->can_n.TxMessage.Data[0] = 0x00;
    can_n->can_n.TxMessage.Data[1] = 0xb4;
    can_n->can_n.TxMessage.Data[2] = can_n->can_n.id;
    can_n->can_n.TxMessage.Data[3] = di_num;
    can_n->can_n.TxMessage.Data[4] = 0x00;
    can_n->can_n.TxMessage.Data[5] = 0x00;
    can_n->can_n.TxMessage.Data[6] = 0x00;
    can_n->can_n.TxMessage.Data[7] = 0x00;
    can_n->Send(&can_n->can_n);
    
//    if(di_num < 4) {
//        while(cmd_count < 120) {
//            flash.read(menu_n.cmd_merry_flash,FLASH_ADDR_FLAG(10+(di_num*120)+(cmd_count*5)), 6);
//            
//            for(uint8_t cmd = 0;cmd < 5;cmd++) {
//                menu_n.cmd_merry[cmd*2] = (uint8_t)(menu_n.cmd_merry_flash[cmd] >> 8);
//                menu_n.cmd_merry[(cmd*2)+1] = (uint8_t)menu_n.cmd_merry_flash[cmd];
//            }
//            
//             if(menu_n.cmd_merry[0] != 0) {
//                can_n->can_n.TxMessage.StdId = menu_n.cmd_merry[0];
//                can_n->can_n.TxMessage.Data[0] = menu_n.cmd_merry[1];
//                can_n->can_n.TxMessage.Data[1] = menu_n.cmd_merry[2];
//                can_n->can_n.TxMessage.Data[2] = menu_n.cmd_merry[3];
//                can_n->can_n.TxMessage.Data[3] = menu_n.cmd_merry[4];
//                can_n->can_n.TxMessage.Data[4] = menu_n.cmd_merry[5];
//                can_n->can_n.TxMessage.Data[5] = menu_n.cmd_merry[6];
//                can_n->can_n.TxMessage.Data[6] = menu_n.cmd_merry[7];
//                can_n->can_n.TxMessage.Data[7] = menu_n.cmd_merry[8];
//                can_n->Send(&can_n->can_n);
//                if(menu_n.cmd_merry_flash[5] == 0) {
//                    cmd_count = 200;
//                } else {
//                    cmd_count++;
//                }
//            } else {
//                cmd_count = 200;
//            }
//        }
//    }
}

/***************************************************************END OF FILE****/
