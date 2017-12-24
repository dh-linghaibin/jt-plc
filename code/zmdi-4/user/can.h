#ifndef __CAN_H
#define __CAN_H

/* 引用头文件 *****************************************************************/
#include "stm32f0xx.h"

/* 外部数据类型 ***************************************************************/
typedef enum {
	P_HEAD = 2,
	P_CMD = 5,
	P_LEN = 1,
	P_ADDR = 2,
	P_O0 = 6,
	P_O1 = 7,
	P_O2 = 8,
	P_O3 = 10,
	P_O4 = 11,
	P_O5 = 12,
	P_O6 = 13,
	P_O7 = 14,
}packet_cmd_e;

typedef struct _can_package_obj {
	#define PACKAGE_NUM 20
	uint8_t package[PACKAGE_NUM][69];
	uint8_t (*get_cmd)( struct _can_package_obj* pack,uint8_t num);
}can_package_obj;

typedef struct _can_message_obj {
	uint8_t send_id;
	uint8_t id;
	uint8_t device_id;
	uint8_t len;
	uint8_t cmd;
	uint8_t arr[64];
}can_message_obj;

typedef struct _can_obj{   
	uint16_t id;
    uint8_t send_packed[8];
	can_message_obj send_msg;
    void (*init)(struct _can_obj* can);
    void (*send)(struct _can_obj* can);
    void (*set_id)(struct _can_obj* can,uint8_t id);
	can_package_obj* (*get_packget)(struct _can_obj* can);
}can_obj; 

void bxcan_init(struct _can_obj* can);
void bxcan_send(struct _can_obj* can);
void bxcan_set_id(struct _can_obj* can,uint8_t id);
can_package_obj*  bxcan_get_packget(struct _can_obj* can);


#endif
/***************************************************************END OF FILE****/
