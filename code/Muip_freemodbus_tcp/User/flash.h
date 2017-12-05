#ifndef __FLASH_H
#define __FLASH_H

/* 引用头文件 *****************************************************************/
#include "stm32f10x.h"

/* 宏定义 --------------------------------------------------------------------*/
#define FLASH_PAGE_SIZE           ((uint16_t)0x400)         //页大小   - 1K
#define FLASH_TYPE_LENGTH         ((uint16_t)0x002)         //类型大小 - 2字节
#define FLASH_PAGE_LENGTH         (FLASH_PAGE_SIZE/FLASH_TYPE_LENGTH)
#define FLAG_OK                   0x00
#define FLAG_NOOK                 0x01
#define FLASH_ADDR_FLAG(x)           0x08003000+x*2     //FLASH地址 - 标志位
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* 外部数据类型 ***************************************************************/
typedef struct {   
    void (*write)(uint16_t* pBuffer, uint32_t WriteAddr, uint16_t nWord);
    void (*read)(uint16_t* pBuffer, uint32_t ReadAddr, uint16_t nWord);
    uint16_t (*readbit)(uint32_t ReadAddr);
}Stdflash; 
/* 外部常数宏 *****************************************************************/
/* 外部动作宏 *****************************************************************/
/* 外部变量 *******************************************************************/
/* 函数声明 *******************************************************************/
void FLASH_WriteNWord(uint16_t* pBuffer, uint32_t WriteAddr, uint16_t nWord);
void FLASH_ReadNWord(uint16_t* pBuffer, uint32_t ReadAddr, uint16_t nWord);
uint16_t FLASH_ReadNBit(uint32_t ReadAddr);

void WriteFlashOneWord(uint32_t WriteAddress,uint16_t *WriteData,uint8_t num);
int ReadFlashNBtye(uint32_t ReadAddress, uint16_t *ReadBuf, int32_t ReadNum);




#if defined (STM32F10X_LD) || defined (STM32F10X_MD)
  #define PAGE_SIZE  (uint16_t)0x400  /* Page size = 1KByte */
#elif defined (STM32F10X_HD) || defined (STM32F10X_CL)
  #define PAGE_SIZE  (uint16_t)0x800  /* Page size = 2KByte */
#endif

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS    ((uint32_t)0x0800F800) /* EEPROM emulation start address:
                                                  after 62KByte of used Flash memory */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((uint16_t)0x0000)
#define PAGE1                   ((uint16_t)0x0001)

/* No valid page define */
#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
#define RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
#define VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */
#define PAGE_NOT_CORRUPTED      ((uint16_t)0xAAAA)     /* PAGE is NOT corrupted */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL               ((uint8_t)0x80)

extern uint16_t VirtAddVarTab[];

uint16_t ltk_ee_init(void);
uint16_t ltk_ee_read_variable(uint16_t VirtAddress, uint16_t* Data);
uint16_t ltk_ee_write_variable(uint16_t VirtAddress, uint16_t Data);

#endif
/***************************************************************END OF FILE****/
