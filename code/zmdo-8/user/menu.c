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
/***************************************************************END OF FILE****/
