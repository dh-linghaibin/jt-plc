/* 引用头文件 *****************************************************************/
#include "flash.h"
/* 私有数据类型 ***************************************************************/
/* 私有常数宏 *****************************************************************/
/* 私有动作宏 *****************************************************************/
/* 私有变量 *******************************************************************/
/* 全局变量 */
/* 私有函数声明 ***************************************************************/
/* 私有函数  ******************************************************************/
/* 变量 ----------------------------------------------------------------------*/
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

/************************************************
函数名称 ： FLASH_PageWrite
功    能 ： 写一页FLASH
参    数 ： pBuffer ----- 数据缓冲区
WriteAddr --- 写地址
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void FLASH_PageWrite(uint16_t *pBuffer, uint32_t WriteAddr)
{
    uint16_t cnt_tmp;
    for(cnt_tmp=0; (cnt_tmp<FLASH_PAGE_LENGTH) && (FLASHStatus == FLASH_COMPLETE); cnt_tmp++)
    {
        FLASHStatus = FLASH_ProgramHalfWord(WriteAddr, *pBuffer);
        WriteAddr += 2;
        pBuffer++;
    }
}

/************************************************
函数名称 ： FLASH_WriteNWord
功    能 ： 写Flash
参    数 ： pBuffer ----- 数据缓冲区
WriteAddr --- 写地址
nWord ------- 长度
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void FLASH_WriteNWord(uint16_t *pBuffer, uint32_t WriteAddr, uint16_t nWord)
{
    static uint16_t buf_tmp[FLASH_PAGE_LENGTH];
    uint32_t NbrOfPage = 0x00;                     //页数（读写）
    
    uint32_t length_beyond_start;                  //开始页超出的长度(半字)
    uint32_t length_remain_start;                  //开始页剩余的长度(半字)
    
    uint32_t addr_first_page;                      //第一页(起始)地址
    uint32_t addr_last_page;                       //最后页(起始)地址
    uint16_t *pbuf_tmp;                            //buf指针
    uint16_t cnt_length;                           //计数 - 长度
    uint16_t cnt_page;                             //计数 - 页数
    uint32_t prog_addr_start;                      //编程地址
    uint32_t length_beyond_last;                   //最后页超出的长度(半字)
    uint8_t  flag_last_page_fill;                  //最后一页装满标志
    
    
    length_beyond_start = ((WriteAddr % FLASH_PAGE_SIZE) / FLASH_TYPE_LENGTH);
    length_remain_start = FLASH_PAGE_LENGTH - length_beyond_start;
    addr_first_page     = WriteAddr - (WriteAddr % FLASH_PAGE_SIZE);
    
    /* 擦除(写操作)的页数 */
    if(0 == (length_beyond_start + nWord)%FLASH_PAGE_LENGTH)
    {
        flag_last_page_fill = FLAG_OK;               //最后一页刚好
        NbrOfPage = (length_beyond_start + nWord) / FLASH_PAGE_LENGTH;
    }
    else
    {
        flag_last_page_fill = FLAG_NOOK;             //???跨页
        NbrOfPage = (length_beyond_start + nWord) / FLASH_PAGE_LENGTH + 1;
    }
    
    /* 解锁 */
    FLASH_Unlock();
    
    /* 清除标志位 */
    //FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    
    /* 操作第一页 */
    FLASH_ReadNWord(buf_tmp, addr_first_page, FLASH_PAGE_LENGTH);
    FLASHStatus = FLASH_ErasePage(addr_first_page);
    /* 只有1页 */
    if(1 == NbrOfPage)
    {
        pbuf_tmp = pBuffer;                          //复制地址(指针)
        for(cnt_length=length_beyond_start; cnt_length<(length_beyond_start + nWord); cnt_length++)
        {
            buf_tmp[cnt_length] = *pbuf_tmp;
            pbuf_tmp++;
        }
        FLASH_PageWrite(buf_tmp, addr_first_page);
    }
    /* 大于1页 */
    else
    {
        /* 第一页 */
        pbuf_tmp = pBuffer;
        for(cnt_length=length_beyond_start; cnt_length<FLASH_PAGE_LENGTH; cnt_length++)
        {
            buf_tmp[cnt_length] = *pbuf_tmp;
            pbuf_tmp++;
        }
        FLASH_PageWrite(buf_tmp, addr_first_page);
        
        /* 最后一页刚好装满，不用读取最后一页数据 */
        if(FLAG_OK == flag_last_page_fill)
        {
            for(cnt_page=1; (cnt_page<NbrOfPage)  && (FLASHStatus == FLASH_COMPLETE); cnt_page++)
            {                                          //这里编程地址为字节地址(故FLASH_PAGE_SIZE)
                prog_addr_start = addr_first_page + cnt_page*FLASH_PAGE_SIZE;
                FLASHStatus = FLASH_ErasePage(prog_addr_start);
                //(cnt_page - 1):为下一页地址
                FLASH_PageWrite((pBuffer + length_remain_start + (cnt_page - 1)*FLASH_PAGE_LENGTH), prog_addr_start);
            }
        }
        else
        {
            /* 中间页 */
            for(cnt_page=1; (cnt_page<(NbrOfPage - 1))  && (FLASHStatus == FLASH_COMPLETE); cnt_page++)
            {                                          //这里编程地址为字节地址(故FLASH_PAGE_SIZE)
                prog_addr_start = addr_first_page + cnt_page*FLASH_PAGE_SIZE;
                FLASHStatus = FLASH_ErasePage(prog_addr_start);
                //(cnt_page - 1):为下一页地址
                FLASH_PageWrite((pBuffer + length_remain_start + (cnt_page - 1)*FLASH_PAGE_LENGTH), prog_addr_start);
            }
            
            /* 最后一页 */
            addr_last_page = addr_first_page + (NbrOfPage - 1)*FLASH_PAGE_SIZE;
            
            FLASH_ReadNWord(buf_tmp, addr_last_page, FLASH_PAGE_LENGTH);
            FLASHStatus = FLASH_ErasePage(addr_last_page);
            //NbrOfPage - 2: 首页 + 最后一页 共两页(-2)
            pbuf_tmp = pBuffer + length_remain_start + (NbrOfPage - 2)*(FLASH_PAGE_SIZE/2);
            length_beyond_last   = (nWord - length_remain_start) % FLASH_PAGE_LENGTH;
            for(cnt_length=0; cnt_length<length_beyond_last; cnt_length++)
            {
                buf_tmp[cnt_length] = *pbuf_tmp;
                pbuf_tmp++;
            }
            FLASH_PageWrite(buf_tmp, addr_last_page);
        }
    }
}

/************************************************
函数名称 ： FLASH_ReadNWord
功    能 ： 读N字
参    数 ： pBuffer ----- 数据缓冲区
ReadAddr ---- 读地址
nWord ------- 长度
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void FLASH_ReadNWord(uint16_t* pBuffer, uint32_t ReadAddr, uint16_t nWord)
{
    while(nWord--)
    {
        *pBuffer = (*(__IO uint16_t*)ReadAddr);
        ReadAddr += 2;
        pBuffer++;
    }
}

uint16_t FLASH_ReadNBit(uint32_t ReadAddr) {
    uint16_t pBuffer = 0;
    pBuffer = (*(__IO uint16_t*)ReadAddr );
    return pBuffer;
}


uint16_t DataVar = 0;

/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[] = {0x5555, 0x6666, 0x7777};

static FLASH_Status ltk_ee_Format(void);
static uint16_t ltk_ee_FindValidPage(uint8_t Operation);
static uint16_t ltk_ee_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data);
static uint16_t ltk_ee_PageTransfer(uint16_t VirtAddress, uint16_t Data);

/**
  * @brief  Restore the pages to a known good state in case of page's status
  *         corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  */
uint16_t ltk_ee_init(void)
{
    uint16_t PageStatus0 = 6, PageStatus1 = 6;
    uint16_t PageCorrupted0 = 0, PageCorrupted1 = 0;
    uint16_t VarIdx = 0;
    uint16_t EepromStatus = 0, ReadStatus = 0;
    uint16_t  FlashStatus;

    /* Get Page0 status */
    PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
    /* Get Page1 status */
    PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

    /* Get Page0 corrupted status */
    PageCorrupted0 = (*(__IO uint16_t*)(PAGE0_BASE_ADDRESS + 2));
    /* Get Page1 corrupted status */
    PageCorrupted1 = (*(__IO uint16_t*)(PAGE1_BASE_ADDRESS + 2));

    if((PageStatus0 == ERASED) && (PageCorrupted0 != PAGE_NOT_CORRUPTED))
    {
        //ltk_printf("page 0 first time or corrupted\n");
        /* Erase Page0 */
        FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
            return FlashStatus;
        }
        
        /* Mark Page0 as not corrupted */
        FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS + 2, PAGE_NOT_CORRUPTED);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
            return FlashStatus;
        }
    }
    
    if((PageStatus1 == ERASED) && (PageCorrupted1 != PAGE_NOT_CORRUPTED))
    {
        //ltk_printf("page 1 first time or corrupted\n");
        /* Erase Page1 */
        FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
        /* If erase operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
            return FlashStatus;
        }
        
        /* Mark Page1 as not corrupted */
        FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS + 2, PAGE_NOT_CORRUPTED);
        /* If program operation was failed, a Flash error code is returned */
        if (FlashStatus != FLASH_COMPLETE)
        {
            return FlashStatus;
        }
    }

    /* Get Page0 status */
    PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
    /* Get Page1 status */
    PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

    /* Check for invalid header states and repair if necessary */
    switch (PageStatus0)
    {
        case ERASED:
            if (PageStatus1 == VALID_PAGE) /* Page0 erased, Page1 valid */
            {
            }
            else if (PageStatus1 == RECEIVE_DATA) /* Page0 erased, Page1 receive */
            {
                /* Mark Page1 as valid */
                FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
            }
            else /* First EEPROM access (Page0&1 are erased) or invalid state */
            {
                /* Mark Page0 as valid */
                FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
            }
            break;

        case RECEIVE_DATA:
            if (PageStatus1 == VALID_PAGE) /* Page0 receive, Page1 valid */
            {
                /* Transfer data from Page1 to Page0 */
                for (VarIdx = 0; VarIdx < (sizeof(VirtAddVarTab) / sizeof(VirtAddVarTab[0])); VarIdx++)
                {
                    /* Read the last variables' updates */
                    ReadStatus = ltk_ee_read_variable(VirtAddVarTab[VarIdx], &DataVar);
                    /* In case variable corresponding to the virtual address was found */
                    if (ReadStatus != 0x1)
                    {
                        /* Transfer the variable to the Page0 */
                        EepromStatus = ltk_ee_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
                        /* If program operation was failed, a Flash error code is returned */
                        if (EepromStatus != FLASH_COMPLETE)
                        {
                            return EepromStatus;
                        }
                    }
                }
                /* Mark Page0 as valid */
                FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
                /* Erase Page1 */
                FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
                
                /* Mark Page1 as not corrupted */
                FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS + 2, PAGE_NOT_CORRUPTED);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
                
            }
            else if (PageStatus1 == ERASED) /* Page0 receive, Page1 erased */
            {
                /* Mark Page0 as valid */
                FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
            }
            else /* Invalid state -> format eeprom */
            {
                /* Erase both Page0 and Page1 and set Page0 as valid page */
                FlashStatus = ltk_ee_Format();
                /* If erase/program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
            }
            break;

        case VALID_PAGE:
            if (PageStatus1 == VALID_PAGE) /* Invalid state -> format eeprom */
            {
                /* Erase both Page0 and Page1 and set Page0 as valid page */
                FlashStatus = ltk_ee_Format();
                /* If erase/program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
            }
            else if (PageStatus1 == ERASED) /* Page0 valid, Page1 erased */
            {
            }
            else /* Page0 valid, Page1 receive */
            {
                /* Transfer data from Page0 to Page1 */
                for (VarIdx = 0; VarIdx < (sizeof(VirtAddVarTab) / sizeof(VirtAddVarTab[0])); VarIdx++)
                {
                    /* Read the last variables' updates */
                    ReadStatus = ltk_ee_read_variable(VirtAddVarTab[VarIdx], &DataVar);
                    /* In case variable corresponding to the virtual address was found */
                    if (ReadStatus != 0x1)
                    {
                        /* Transfer the variable to the Page1 */
                        EepromStatus = ltk_ee_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
                        /* If program operation was failed, a Flash error code is returned */
                        if (EepromStatus != FLASH_COMPLETE)
                        {
                            return EepromStatus;
                        }
                    }
                }
                /* Mark Page1 as valid */
                FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS, VALID_PAGE);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
                /* Erase Page0 */
                FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);
                /* If erase operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
                
                /* Mark Page0 as not corrupted */
                FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS + 2, PAGE_NOT_CORRUPTED);
                /* If program operation was failed, a Flash error code is returned */
                if (FlashStatus != FLASH_COMPLETE)
                {
                    return FlashStatus;
                }
            }
            break;

        default:  /* Any other state -> format eeprom */
            /* Erase both Page0 and Page1 and set Page0 as valid page */
            FlashStatus = ltk_ee_Format();
            /* If erase/program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE)
            {
                return FlashStatus;
            }
            break;
    }

    return FLASH_COMPLETE;
}

/**
  * @brief  Returns the last stored variable data, if found, which correspond to
  *         the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - 0: if variable was found
  *           - 1: if the variable was not found
  *           - NO_VALID_PAGE: if no valid page was found.
  */
uint16_t ltk_ee_read_variable(uint16_t VirtAddress, uint16_t* Data)
{
    uint16_t ValidPage = PAGE0;
    uint16_t AddressValue = 0x5555, ReadStatus = 1;
    uint32_t Address = PAGE0_BASE_ADDRESS, PageStartAddress = PAGE0_BASE_ADDRESS;

    /* Get active Page for read operation */
    ValidPage = ltk_ee_FindValidPage(READ_FROM_VALID_PAGE);

    /* Check if there is no valid page */
    if (ValidPage == NO_VALID_PAGE)
    {
        return  NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

    /* Get the valid Page end Address */
    Address = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

    /* Check each active page address starting from end */
    while (Address > (PageStartAddress + 2))
    {
        /* Get the current location content to be compared with virtual address */
        AddressValue = (*(__IO uint16_t*)Address);

        /* Compare the read address with the virtual address */
        if (AddressValue == VirtAddress)
        {
            /* Get content of Address-2 which is variable value */
            *Data = (*(__IO uint16_t*)(Address - 2));

            /* In case variable value is read, reset ReadStatus flag */
            ReadStatus = 0;

            break;
        }
        else
        {
            /* Next address location */
            Address = Address - 4;
        }
    }

    /* Return ReadStatus value: (0: variable exist, 1: variable doesn't exist) */
    return ReadStatus;
}

/**
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
uint16_t ltk_ee_write_variable(uint16_t VirtAddress, uint16_t Data)
{
    uint16_t Status = 0;

    /* Write the variable virtual address and value in the EEPROM */
    Status = ltk_ee_VerifyPageFullWriteVariable(VirtAddress, Data);

    /* In case the EEPROM active page is full */
    if (Status == PAGE_FULL)
    {
        /* Perform Page transfer */
        //Status = ltk_ee_PageTransfer(VirtAddress, Data);
    }

    /* Return last operation status */
    return Status;
}

/**
  * @brief  Erases PAGE0 and PAGE1 and writes VALID_PAGE header to PAGE0
  * @param  None
  * @retval Status of the last operation (Flash write or erase) done during
  *         EEPROM formating
  */
static FLASH_Status ltk_ee_Format(void)
{
    FLASH_Status FlashStatus = FLASH_COMPLETE;

    /* Erase Page0 */
    FlashStatus = FLASH_ErasePage(PAGE0_BASE_ADDRESS);

    /* If erase operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }

    /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
    FlashStatus = FLASH_ProgramHalfWord(PAGE0_BASE_ADDRESS, VALID_PAGE);

    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }

    /* Erase Page1 */
    FlashStatus = FLASH_ErasePage(PAGE1_BASE_ADDRESS);
    /* If erase operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }
    
    /* Set Page1 as not corrupted */
    FlashStatus = FLASH_ProgramHalfWord(PAGE1_BASE_ADDRESS + 2, PAGE_NOT_CORRUPTED);

    /* Return Page1 not corrupted status */
    return FlashStatus;
}

/**
  * @brief  Find valid Page for write or read operation
  * @param  Operation: operation to achieve on the valid page.
  *         This parameter can be one of the following values:
  *         @arg READ_FROM_VALID_PAGE: read operation from valid page
  *         @arg WRITE_IN_VALID_PAGE: write operation from valid page
  *         @retval Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
  *         of no valid page was found
  */
static uint16_t ltk_ee_FindValidPage(uint8_t Operation)
{
    uint16_t PageStatus0 = 6, PageStatus1 = 6;

    /* Get Page0 actual status */
    PageStatus0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);

    /* Get Page1 actual status */
    PageStatus1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

    /* Write or read operation */
    switch (Operation)
    {
        case WRITE_IN_VALID_PAGE:   /* ---- Write operation ---- */
            if (PageStatus1 == VALID_PAGE)
            {
                /* Page0 receiving data */
                if (PageStatus0 == RECEIVE_DATA)
                {
                    return PAGE0;         /* Page0 valid */
                }
                else
                {
                    return PAGE1;         /* Page1 valid */
                }
            }
            else if (PageStatus0 == VALID_PAGE)
            {
                /* Page1 receiving data */
                if (PageStatus1 == RECEIVE_DATA)
                {
                    return PAGE1;         /* Page1 valid */
                }
                else
                {
                    return PAGE0;         /* Page0 valid */
                }
            }
            else
            {
                return NO_VALID_PAGE;   /* No valid Page */
            }

        case READ_FROM_VALID_PAGE:  /* ---- Read operation ---- */
            if (PageStatus0 == VALID_PAGE)
            {
                return PAGE0;           /* Page0 valid */
            }
            else if (PageStatus1 == VALID_PAGE)
            {
                return PAGE1;           /* Page1 valid */
            }
            else
            {
                return NO_VALID_PAGE ;  /* No valid Page */
            }

        default:
            return PAGE0;             /* Page0 valid */
    }
}

/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t ltk_ee_VerifyPageFullWriteVariable(uint16_t VirtAddress, uint16_t Data)
{
    FLASH_Status FlashStatus = FLASH_COMPLETE;
    uint16_t ValidPage = PAGE0;
    uint32_t Address = PAGE0_BASE_ADDRESS, PageEndAddress = PAGE1_BASE_ADDRESS;

    /* Get valid Page for write operation */
    ValidPage = ltk_ee_FindValidPage(WRITE_IN_VALID_PAGE);

    /* Check if there is no valid page */
    if (ValidPage == NO_VALID_PAGE)
    {
        return  NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    Address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(ValidPage * PAGE_SIZE));

    /* Get the valid Page end Address */
    PageEndAddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + ValidPage) * PAGE_SIZE));

    /* Check each active page address starting from begining */
    while (Address < PageEndAddress)
    {
        /* Verify if Address and Address+2 contents are 0xFFFFFFFF */
        if ((*(__IO uint32_t*)Address) == 0xFFFFFFFF)
        {
            /* Set variable data */
            FlashStatus = FLASH_ProgramHalfWord(Address, Data);
            /* If program operation was failed, a Flash error code is returned */
            if (FlashStatus != FLASH_COMPLETE)
            {
                return FlashStatus;
            }
            /* Set variable virtual address */
            FlashStatus = FLASH_ProgramHalfWord(Address + 2, VirtAddress);
            /* Return program operation status */
            return FlashStatus;
        }
        else
        {
            /* Next address location */
            Address = Address + 4;
        }
    }

    /* Return PAGE_FULL in case the valid page is full */
    return PAGE_FULL;
}

/**
  * @brief  Transfers last updated variables data from the full Page to
  *         an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t ltk_ee_PageTransfer(uint16_t VirtAddress, uint16_t Data)
{
    FLASH_Status FlashStatus = FLASH_COMPLETE;
    uint32_t NewPageAddress = PAGE1_BASE_ADDRESS, OldPageAddress = PAGE0_BASE_ADDRESS;
    uint16_t ValidPage = PAGE0, VarIdx = 0;
    uint16_t EepromStatus = 0, ReadStatus = 0;

    /* Get active Page for read operation */
    ValidPage = ltk_ee_FindValidPage(READ_FROM_VALID_PAGE);

    if (ValidPage == PAGE1)       /* Page1 valid */
    {
        /* New page address where variable will be moved to */
        NewPageAddress = PAGE0_BASE_ADDRESS;

        /* Old page address where variable will be taken from */
        OldPageAddress = PAGE1_BASE_ADDRESS;
    }
    else if (ValidPage == PAGE0)  /* Page0 valid */
    {
        /* New page address where variable will be moved to */
        NewPageAddress = PAGE1_BASE_ADDRESS;

        /* Old page address where variable will be taken from */
        OldPageAddress = PAGE0_BASE_ADDRESS;
    }
    else
    {
        return NO_VALID_PAGE;       /* No valid Page */
    }

    /* Set the new Page status to RECEIVE_DATA status */
    FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, RECEIVE_DATA);
    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }

    /* Write the variable passed as parameter in the new active page */
    EepromStatus = ltk_ee_VerifyPageFullWriteVariable(VirtAddress, Data);
    /* If program operation was failed, a Flash error code is returned */
    if (EepromStatus != FLASH_COMPLETE)
    {
        return EepromStatus;
    }

    /* Transfer process: transfer variables from old to the new active page */
    for (VarIdx = 0; VarIdx < (sizeof(VirtAddVarTab) / sizeof(VirtAddVarTab[0])); VarIdx++)
    {
        if (VirtAddVarTab[VarIdx] != VirtAddress)  /* Check each variable except the one passed as parameter */
        {
            /* Read the other last variable updates */
            ReadStatus = ltk_ee_read_variable(VirtAddVarTab[VarIdx], &DataVar);
            /* In case variable corresponding to the virtual address was found */
            if (ReadStatus != 0x1)
            {
                /* Transfer the variable to the new active page */
                EepromStatus = ltk_ee_VerifyPageFullWriteVariable(VirtAddVarTab[VarIdx], DataVar);
                /* If program operation was failed, a Flash error code is returned */
                if (EepromStatus != FLASH_COMPLETE)
                {
                    return EepromStatus;
                }
            }
        }
    }

    /* Erase the old Page: Set old Page status to ERASED status */
    FlashStatus = FLASH_ErasePage(OldPageAddress);
    /* If erase operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }
    
    /* Mark old Page as not corrupted */
    FlashStatus = FLASH_ProgramHalfWord(OldPageAddress + 2, PAGE_NOT_CORRUPTED);
    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }

    /* Set new Page status to VALID_PAGE status */
    FlashStatus = FLASH_ProgramHalfWord(NewPageAddress, VALID_PAGE);
    /* If program operation was failed, a Flash error code is returned */
    if (FlashStatus != FLASH_COMPLETE)
    {
        return FlashStatus;
    }

    /* Return last operation flash status */
    return FlashStatus;
}


void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
	if(startAddress<FLASH_BASE||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_PAGE_SIZE)))
	{
		return;
	}
	FLASH_Unlock();        
	uint32_t offsetAddress=startAddress-FLASH_BASE;              
	uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;            

	uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;  

	FLASH_ErasePage(sectorStartAddress);

	uint16_t dataIndex;
	for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
	{
		FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
	}
	FLASH_Lock();
}

void WriteFlashOneWord(uint32_t WriteAddress,uint16_t *WriteData,uint8_t num)
{
	//FLASH_WriteMoreData(WriteAddress,WriteData,num);
//    uint8_t cdm_num= 0;
//    while(num != cdm_num) {
//        FLASHStatus = ltk_ee_write_variable(WriteAddress, WriteData[cdm_num]);    //flash.c 中API函数
//        cdm_num++;
//        WriteAddress += 2;
//    }
} 

/****************************************************************
*Name:		ReadFlashNBtye
*Function:	从内部Flash读取N字节数据
*Input:		ReadAddress：数据地址（偏移地址）ReadBuf：数据指针	ReadNum：读取字节数
*Output:	读取的字节数  
*Author:    ValerianFan
*Date:		2014/04/09
*E-Mail:	fanwenjingnihao@163.com
*Other:		
****************************************************************/
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
  return*(__IO uint16_t*)address;
}

uint32_t FLASH_ReadWord(uint32_t address)
{
  uint32_t temp1,temp2;
 temp1=*(__IO uint16_t*)address;
 temp2=*(__IO uint16_t*)(address+2);
  return(temp2<<16)+temp1;
}

void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
  uint16_t dataIndex;
 for(dataIndex=0;dataIndex<countToRead;dataIndex++)
  {
   readData[dataIndex]=FLASH_ReadHalfWord(startAddress+dataIndex*2);
  }
}
 


int ReadFlashNBtye(uint32_t ReadAddress, uint16_t *ReadBuf, int32_t ReadNum) 
{
    int DataNum = 0;
    while(DataNum < ReadNum) 
    {
        //*(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++;
        ltk_ee_read_variable( ReadAddress,(ReadBuf + DataNum));
        DataNum++;
        ReadAddress += 2;
    }
    return ReadNum;
}
/***************************************************************END OF FILE****/
