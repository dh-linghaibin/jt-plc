/**
  ******************************************************************************
  * @file SPI/M25P64_FLASH/spi_flash.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  This file provides a set of functions needed to manage the
  *         communication between SPI peripheral and SPI M25P64 FLASH.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "spi_flash.h"

/** @addtogroup StdPeriph_Examples
  * @{
  */

/** @addtogroup SPI_M25P64_FLASH
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Sends a byte through the SPI interface and return the byte
  *   received from the SPI bus.
  * @param byte : byte to send.
  * @retval : The value of the received byte.
  */
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);

  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}


u16 SPI_FLASH_TYPE=0x00;

void SPI_Flash_Init(void) {	
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable SPI1 and GPIO clocks */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB |
	RCC_APB2Periph_GPIO_CS, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB ,ENABLE );

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13  |  GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOB, GPIO_Pin_15);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOB, GPIO_Pin_14);

	/* Configure I/O for Flash Chip select */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIO_CS, &GPIO_InitStructure);

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();

	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	/* Enable SPI1  */
	SPI_Cmd(SPI2, ENABLE);
	SPI_FLASH_TYPE=SPI_Flash_ReadID();

}  

u8 SPI_Flash_ReadSR(void) {  
	u8 byte=0;   
	SPI_FLASH_CS_LOW();                         
	SPI_FLASH_SendByte(W25X_ReadStatusReg);    
	byte=SPI_FLASH_SendByte(0Xff);            
	SPI_FLASH_CS_HIGH();     
	return byte;   
} 

void SPI_FLASH_Write_SR(u8 sr) {   
	SPI_FLASH_CS_LOW();                 
	SPI_FLASH_SendByte(W25X_WriteStatusReg);  
	SPI_FLASH_SendByte(sr);             
	SPI_FLASH_CS_HIGH();                           	      
}   

void SPI_FLASH_Write_Enable(void)   
{
	SPI_FLASH_CS_LOW();                         
    SPI_FLASH_SendByte(W25X_WriteEnable);   
	SPI_FLASH_CS_HIGH();                               
} 

//SPI_FLASH写禁止  
//将WEL清零  
void SPI_FLASH_Write_Disable(void)   
{  
  SPI_FLASH_CS_LOW();                                //使能器件   
  SPI_FLASH_SendByte(W25X_WriteDisable);     //发送写禁止指令    
  SPI_FLASH_CS_HIGH();                         //取消片选             
}           
//读取芯片ID W25X16的ID:0XEF14
u16 SPI_Flash_ReadID(void)
{
  u16 Temp = 0;   
  SPI_FLASH_CS_LOW();               
  SPI_FLASH_SendByte(0x90);//发送读取ID命令     
  SPI_FLASH_SendByte(0x00);       
  SPI_FLASH_SendByte(0x00);       
  SPI_FLASH_SendByte(0x00);            
  Temp|=SPI_FLASH_SendByte(0xFF)<<8;  
  Temp|=SPI_FLASH_SendByte(0xFF);  
  SPI_FLASH_CS_HIGH();        
  return Temp;
}           
//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
  u16 i;                                
  SPI_FLASH_CS_LOW();                               //使能器件   
	SPI_FLASH_SendByte(W25X_ReadData);         //发送读取命令   
	SPI_FLASH_SendByte((u8)((ReadAddr)>>16));  //发送24bit地址    
	SPI_FLASH_SendByte((u8)((ReadAddr)>>8));   
	SPI_FLASH_SendByte((u8)ReadAddr);   
    for(i=0;i<NumByteToRead;i++)
  { 
        pBuffer[i]=SPI_FLASH_SendByte(0XFF);   //循环读数  
    }
  SPI_FLASH_CS_HIGH();                         //取消片选             
}  
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!   
void SPI_Flash_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
  u16 i; 
  u16 numToWrite = NumByteToWrite; 
    SPI_FLASH_Write_Enable();                  //SET WEL 
  SPI_FLASH_CS_LOW();                            //使能器件   
    SPI_FLASH_SendByte(W25X_PageProgram);      //发送写页命令   
    SPI_FLASH_SendByte((u8)((WriteAddr)>>16)); //发送24bit地址    
    SPI_FLASH_SendByte((u8)((WriteAddr)>>8));   
    SPI_FLASH_SendByte((u8)WriteAddr);   
    for(i=0;i<numToWrite;i++)SPI_FLASH_SendByte(pBuffer[i]);//循环写数  
  SPI_FLASH_CS_HIGH();                            //取消片选 
  SPI_Flash_Wait_Busy();             //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{            
  u16 pageremain;
  u16 numToWrite = NumByteToWrite;     
  pageremain=256-WriteAddr%256; //单页剩余的字节数          
  if(numToWrite<=pageremain)pageremain=numToWrite;//不大于256个字节
  while(1)
  {    
    SPI_Flash_Write_Page(pBuffer,WriteAddr,pageremain);
    if(numToWrite==pageremain)break;//写入结束了
    else //NumByteToWrite>pageremain
    {
      pBuffer+=pageremain;
      WriteAddr+=pageremain;  

      numToWrite-=pageremain;       //减去已经写入了的字节数
      if(numToWrite>256)pageremain=256; //一次可以写入256个字节
      else pageremain=numToWrite;     //不够256个字节了
    }
  };      
} 


//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
//监视falsh擦除情况,测试用    
	Dst_Addr*=4096;
    SPI_FLASH_Write_Enable();                   //SET WEL  
    SPI_Flash_Wait_Busy();   
    SPI_FLASH_CS_LOW();                              //使能器件   
    SPI_FLASH_SendByte(W25X_SectorErase);       //发送扇区擦除指令 
    SPI_FLASH_SendByte((u8)((Dst_Addr)>>16));   //发送24bit地址    
    SPI_FLASH_SendByte((u8)((Dst_Addr)>>8));   
    SPI_FLASH_SendByte((u8)Dst_Addr);  
    SPI_FLASH_CS_HIGH();                              //取消片选          
    SPI_Flash_Wait_Busy();       //等待擦除完成
} 

//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)        
u8 SPI_FLASH_BUF[4096]={0};
u8 *spiFlashBuf = SPI_FLASH_BUF;
u8 SPI_FLASH_BUF2[4096]={0};
u8 *spiFlashBuf2 = SPI_FLASH_BUF2;
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
  u32 secpos;
  u32 secoff;
  u32 secremain;     
  u32 i;  
  u32 numToWrite = NumByteToWrite; 

  secpos=WriteAddr/4096;//扇区地址 0~511 for w25x16
  secoff=WriteAddr%4096;//在扇区内的偏移
  secremain=4096-secoff;//扇区剩余空间大小   

  if(numToWrite<=secremain)secremain=numToWrite;//不大于4096个字节
  while(1) 
  { 
    SPI_Flash_Read(spiFlashBuf,secpos*4096,4096);//读出整个扇区的内容
    for(i=0;i<secremain;i++)//校验数据
    {
      if(spiFlashBuf[secoff+i]!=0XFF)break;//需要擦除     
    }
    if(i<secremain)//需要擦除
    {
      W25QXX_Erase_Sector(secpos);//擦除这个扇区
      //检测是否已擦除
      SPI_Flash_Read(spiFlashBuf2,secpos*4096,4096);
      for(i=0;i<secremain;i++)     //复制
      {
        *(spiFlashBuf+i+secoff)=pBuffer[i];   
      }
      SPI_Flash_Write_NoCheck(spiFlashBuf,secpos*4096,4096);//写入整个扇区  

    }else{ 
		SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);
	}//写已经擦除了的,直接写入扇区剩余区间.           
    if(numToWrite==secremain) {
		break;//写入结束了
	} else {//写入未结束
		secpos++;//扇区地址增1
		secoff=0;//偏移位置为0    

		pBuffer+=secremain;  //指针偏移
		WriteAddr+=secremain;//写地址偏移     
		numToWrite-=secremain;        //字节数递减
		if(numToWrite>4096)secremain=4096;  //下一个扇区还是写不完
		else secremain=numToWrite;      //下一个扇区可以写完了
    }  
  };     
}

//擦除整个芯片
//整片擦除时间:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//等待时间超长...
void SPI_Flash_Erase_Chip(void)   
{                                             
	SPI_FLASH_Write_Enable();                  //SET WEL 
	SPI_Flash_Wait_Busy();   
	SPI_FLASH_CS_LOW();                               //使能器件   
	SPI_FLASH_SendByte(W25X_ChipErase);        //发送片擦除命令  
	SPI_FLASH_CS_HIGH();                             //取消片选             
	SPI_Flash_Wait_Busy();             //等待芯片擦除结束
}   
////擦除一个扇区
////Dst_Addr:扇区地址 0~511 for w25x16
////擦除一个山区的最少时间:150ms
//void SPI_Flash_Erase_Sector(u32 Dst_Addr)   
//{ 
//	unsigned long int dstAddr;
//	dstAddr = Dst_Addr;  
//	dstAddr *= dstAddr;
//	SPI_FLASH_Write_Enable();                  //SET WEL   
//	SPI_Flash_Wait_Busy();   
//	SPI_FLASH_CS_LOW();                             //使能器件   
//	SPI_FLASH_SendByte(W25X_SectorErase);      //发送扇区擦除指令 
//	SPI_FLASH_SendByte((u8)((dstAddr)>>16));  //发送24bit地址    
//	SPI_FLASH_SendByte((u8)((dstAddr)>>8));   
//	SPI_FLASH_SendByte((u8)dstAddr);  
//	SPI_FLASH_CS_HIGH();                          //取消片选             
//	SPI_Flash_Wait_Busy();             //等待擦除完成
//}  
//等待空闲
void SPI_Flash_Wait_Busy(void)   
{   
  while ((SPI_Flash_ReadSR()&0x01)==0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void SPI_Flash_PowerDown(void)   
{ 
    SPI_FLASH_CS_LOW();                               //使能器件   
    SPI_FLASH_SendByte(W25X_PowerDown);        //发送掉电命令  
  SPI_FLASH_CS_HIGH();                      //取消片选             
    //delay_us(3);                               //等待TPD  
}   
//唤醒
void SPI_Flash_WAKEUP(void)   
{  
    SPI_FLASH_CS_LOW();                              //使能器件   
    SPI_FLASH_SendByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
  SPI_FLASH_CS_HIGH();                        //取消片选             
    //delay_us(3);                               //等待TRES1
}   


/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
