/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

#include "w25qxx.h"


//w25qxx_obj w25a = {
//	{0,},
//	{0,},
//	&w25qxx_init,
//	&w25qxx_write,
//	&w25qxx_read,
//	&w25qxx_get_id,
//	&w25qxx_erase_chip,
//	&w25qxx_erase_sector,
//	&w25qxx_power_down,
//	&w25qxx_wake_up,
//};

int RAM_disk_status() {
	return RES_OK;
}
int MMC_disk_status() {
	return RES_OK;
}
int USB_disk_status() {
	return RES_OK;
}

int RAM_disk_initialize() {
	return RES_OK;
}
int MMC_disk_initialize() {
	return RES_OK;
}
int USB_disk_initialize() {
	return RES_OK;
}

int RAM_disk_read(BYTE * buff, DWORD sector, UINT count) {
	return RES_OK;
}
int MMC_disk_read(BYTE * buff, DWORD sector, UINT count) {
	return RES_OK;
}
int USB_disk_read(BYTE * buff, DWORD sector, UINT count) {
	return RES_OK;
}

int RAM_disk_write(const BYTE * buff, DWORD sector, UINT count) {
	return RES_OK;
}
int MMC_disk_write(const BYTE * buff, DWORD sector, UINT count) {
	return RES_OK;
}
int USB_disk_write(const BYTE * buff, DWORD sector, UINT count) {
	return RES_OK;
}
DWORD get_fattime() {
	return 55;
}

#define FLASH_SECTOR_SIZE 	512	
//对于W25Q64 
//前6M字节给fatfs用,6M字节后~6M+500K给用户用,6M+500K以后,用于存放字库,字库占用1.5M.		 			    
u16	    FLASH_SECTOR_COUNT=2048*6;//6M字节,默认为W25Q64
#define FLASH_BLOCK_SIZE  	8     //每个BLOCK有8个扇区

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		result = RAM_disk_status();
		
		// translate the reslut code here

		return stat;

	case DEV_MMC :
		result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		//result = RAM_disk_initialize();
		SPI_Flash_Init();
		// translate the reslut code here

		return stat;

	case DEV_MMC :
		result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_OK;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_read(buff, sector, count);
		for(;count>0;count--)
			{
				SPI_Flash_Read(buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_OK;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		// translate the arguments here

		//result = RAM_disk_write(buff, sector, count);
		for(;count>0;count--)
			{										    
				SPI_Flash_Write((u8*)buff,sector*FLASH_SECTOR_SIZE,FLASH_SECTOR_SIZE);
				sector++;
				buff+=FLASH_SECTOR_SIZE;
			}
		// translate the reslut code here

		return res;

	case DEV_MMC :
		// translate the arguments here

		result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;
	int result;

	switch (pdrv) {
	case DEV_RAM :
		switch(cmd)
	    {
		    case CTRL_SYNC:
				res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = FLASH_SECTOR_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = FLASH_BLOCK_SIZE;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = FLASH_SECTOR_COUNT;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
		// Process of the command for the RAM drive

		return res;

	case DEV_MMC :
		*(WORD*)buff = 4096;
		res = RES_OK;
		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :
		*(WORD*)buff = 4096;
		res = RES_OK;
		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}

