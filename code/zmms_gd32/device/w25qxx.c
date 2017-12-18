/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "w25qxx.h"

#define GPIO_CS                  GPIOC
#define RCC_APB2Periph_GPIO_CS   RCC_APB2Periph_GPIOC
#define GPIO_Pin_CS              GPIO_Pin_6

/* Exported macro ------------------------------------------------------------*/
/* Select SPI FLASH: Chip Select pin low  */
#define W25QXX_CS_LOW()       GPIO_ResetBits(GPIO_CS, GPIO_Pin_CS)
/* Deselect SPI FLASH: Chip Select pin high */
#define W25QXX_CS_HIGH()      GPIO_SetBits(GPIO_CS, GPIO_Pin_CS)

#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

/**
  * @brief  Sends a byte through the SPI interface and return the byte
  *   received from the SPI bus.
  * @param byte : byte to send.
  * @retval : The value of the received byte.
  */
static uint8_t w25qxx_send_byte(uint8_t byte) {
  /* Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  /* Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI2, byte);
  /* Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

//static void w25qxx_read_b(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead) { 
//	u16 i;                                
//	W25QXX_CS_LOW();					//使能器件   
//	w25qxx_send_byte(W25X_ReadData);         //发送读取命令   
//	w25qxx_send_byte((u8)((ReadAddr)>>16));  //发送24bit地址    
//	w25qxx_send_byte((u8)((ReadAddr)>>8));   
//	w25qxx_send_byte((u8)ReadAddr);   
//	for(i=0;i<NumByteToRead;i++) { 
//		pBuffer[i]=w25qxx_send_byte(0XFF);   //循环读数  
//	}
//	W25QXX_CS_HIGH();		//取消片选             
//}  

static u8 w25qxx_read_sr(void) {  
	u8 byte=0;   
	W25QXX_CS_LOW();                         
	w25qxx_send_byte(W25X_ReadStatusReg);    
	byte=w25qxx_send_byte(0Xff);            
	W25QXX_CS_HIGH();     
	return byte;   
} 

//static void w25qxx_write_sr(u8 sr) {   
//	W25QXX_CS_LOW();                 
//	w25qxx_send_byte(W25X_WriteStatusReg);  
//	w25qxx_send_byte(sr);             
//	W25QXX_CS_HIGH();                           	      
//}   

static void w25qxx_write_enable(void) {
	W25QXX_CS_LOW();                         
    w25qxx_send_byte(W25X_WriteEnable);   
	W25QXX_CS_HIGH();                               
} 

////SPI_FLASH写禁止  
////将WEL清零  
//static void w25qxx_write_disable(void) {  
//	W25QXX_CS_LOW();                                //使能器件   
//	w25qxx_send_byte(W25X_WriteDisable);     //发送写禁止指令    
//	W25QXX_CS_HIGH();                         //取消片选             
//}   
//等待空闲
static void w25qxx_wait_busy(void) {   
	while ((w25qxx_read_sr()&0x01)==0x01);   // 等待BUSY位清空
}  

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!   
static void w25qxx_wait_page(const u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite) {
	u16 i; 
	u16 numToWrite = NumByteToWrite; 
	w25qxx_write_enable();                  //SET WEL 
	W25QXX_CS_LOW();                            //使能器件   
	w25qxx_send_byte(W25X_PageProgram);      //发送写页命令   
	w25qxx_send_byte((u8)((WriteAddr)>>16)); //发送24bit地址    
	w25qxx_send_byte((u8)((WriteAddr)>>8));   
	w25qxx_send_byte((u8)WriteAddr);   
	for(i=0;i<numToWrite;i++)w25qxx_send_byte(pBuffer[i]);//循环写数  
	W25QXX_CS_HIGH();                            //取消片选 
	w25qxx_wait_busy();             //等待写入结束
} 

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
static void w25qxx_write_nocheck(const u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite) {            
	u16 pageremain;
	u16 numToWrite = NumByteToWrite;     
	pageremain=256-WriteAddr%256; //单页剩余的字节数          
	if(numToWrite<=pageremain)pageremain=numToWrite;//不大于256个字节
	while(1) {    
		w25qxx_wait_page(pBuffer,WriteAddr,pageremain);
		if(numToWrite==pageremain)break;//写入结束了
		else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;  

			numToWrite-=pageremain;       //减去已经写入了的字节数
			if(numToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=numToWrite;     //不够256个字节了
		}
	}  
} 


int w25qxx_init(struct _w25qxx_obj* w25qxx) {
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
	W25QXX_CS_HIGH();

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
	return 0;
}

int w25qxx_write(struct _w25qxx_obj* w25qxx,const uint8_t * buff, unsigned long sector, uint16_t count) {
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * SPI_FLASH_BUF;	  
   	SPI_FLASH_BUF=w25qxx->SPI_FLASH_BUF;	     
 	secpos=sector/4096;
	secoff=sector%4096;
	secremain=4096-secoff;
 	if(count<=secremain)secremain=count;
	while(1) 
	{	
		w25qxx->read(w25qxx,SPI_FLASH_BUF,secpos*4096,4096);
		for(i=0;i<secremain;i++)
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;  
		}
		if(i<secremain)
		{
			w25qxx->erase_sector(w25qxx,secpos);
			for(i=0;i<secremain;i++)	  
			{
				SPI_FLASH_BUF[i+secoff]=buff[i];	  
			}
			w25qxx_write_nocheck(SPI_FLASH_BUF,secpos*4096,4096);

		}else w25qxx_write_nocheck(buff,sector,secremain);			   
		if(count==secremain)break;
		else
		{
			secpos++;
			secoff=0;

		   	buff+=secremain;  
			sector+=secremain;
		   	count-=secremain;				
			if(count>4096)secremain=4096;	
			else secremain=count;		
		}	 
	};	 
	return 0;
}	

int w25qxx_read(struct _w25qxx_obj* w25qxx, uint8_t * buff, unsigned long sector, uint16_t count) {
	 u16 i;                                
	W25QXX_CS_LOW();                               
	w25qxx_send_byte(W25X_ReadData);       
	w25qxx_send_byte((u8)((sector)>>16));  
	w25qxx_send_byte((u8)((sector)>>8));   
	w25qxx_send_byte((u8)sector);   
	for(i=0;i<count;i++) { 
		buff[i]=w25qxx_send_byte(0XFF);  
	}
	W25QXX_CS_HIGH();   
	return 0;
}

uint16_t w25qxx_get_id(struct _w25qxx_obj* w25qxx) {
	u16 Temp = 0;   
	W25QXX_CS_LOW();               
	w25qxx_send_byte(0x90);
	w25qxx_send_byte(0x00);       
	w25qxx_send_byte(0x00);       
	w25qxx_send_byte(0x00);            
	Temp|=w25qxx_send_byte(0xFF)<<8;  
	Temp|=w25qxx_send_byte(0xFF);  
	W25QXX_CS_HIGH();        
	return Temp;
}

int w25qxx_erase_chip(struct _w25qxx_obj* w25qxx) {
	w25qxx_write_enable();                  //SET WEL 
	w25qxx_wait_busy();   
	W25QXX_CS_LOW();                               //使能器件   
	w25qxx_send_byte(W25X_ChipErase);        //发送片擦除命令  
	W25QXX_CS_HIGH();                             //取消片选             
	w25qxx_wait_busy();             //等待芯片擦除结束
	return 0;
}

int w25qxx_erase_sector(struct _w25qxx_obj* w25qxx,unsigned long sector) {
	//监视falsh擦除情况,测试用    
	sector*=4096;
    w25qxx_write_enable();                   //SET WEL  
    w25qxx_wait_busy();   
    W25QXX_CS_LOW();                              //使能器件   
    w25qxx_send_byte(W25X_SectorErase);       //发送扇区擦除指令 
    w25qxx_send_byte((u8)((sector)>>16));   //发送24bit地址    
    w25qxx_send_byte((u8)((sector)>>8));   
    w25qxx_send_byte((u8)sector);  
    W25QXX_CS_HIGH();                              //取消片选          
    w25qxx_wait_busy();       //等待擦除完成
	return 0;
}

void w25qxx_power_down(struct _w25qxx_obj* w25qxx) {
	W25QXX_CS_LOW();                               //使能器件   
	w25qxx_send_byte(W25X_PowerDown);        //发送掉电命令  
	W25QXX_CS_HIGH();                      //取消片选                               
}

void w25qxx_wake_up(struct _w25qxx_obj* w25qxx) {
	W25QXX_CS_LOW();                              //使能器件   
	w25qxx_send_byte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	W25QXX_CS_HIGH();                        //取消片选                  
}
