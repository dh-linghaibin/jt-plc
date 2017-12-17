/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */
#include "w25qxx.h"
#include "ff.h"

FATFS fs; /* FatFs文件系统对象 */
FIL fnew; /* 文件对象 */
FRESULT res_sd; /* 文件操作结果 */
UINT fnum; /* 文件成功读写数量 */
BYTE ReadBuffer[1024]= {0}; /* 读缓冲区 */
BYTE WriteBuffer[] = "欢迎使用野火STM32 F429开发板今天是个好日子，新建文件系统测试文件\r\n";

14 //在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化

15 res_sd = f_mount(&fs,"0:",1);

16

17 /*----------------------- 格式化测试 ---------------------------*/

18 /* 如果没有文件系统就格式化创建创建文件系统 */

19 if (res_sd == FR_NO_FILESYSTEM) {

20 printf("》SD卡还没有文件系统，即将进行格式化...\r\n");

21 /* 格式化 */

22 res_sd=f_mkfs("0:",0,0);

23

24 if (res_sd == FR_OK) {

25 printf("》SD卡已成功格式化文件系统。\r\n");

26 /* 格式化后，先取消挂载 */

27 res_sd = f_mount(NULL,"0:",1);

28 /* 重新挂载 */

29 res_sd = f_mount(&fs,"0:",1);

30 } else {

31 LED_RED;

32 printf("《《格式化失败。》》\r\n");

33 while (1);

34 }

35 } else if (res_sd!=FR_OK) {

36 printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);

37 printf("！！可能原因：SD卡初始化不成功。\r\n");

38 while (1);

39 } else {

40 printf("》文件系统挂载成功，可以进行读写测试\r\n");

41 }

42

43 /*--------------------- 文件系统测试：写测试 -----------------------*/

44 /* 打开文件，如果文件不存在则创建它 */

45 printf("\r\n****** 即将进行文件写入测试... ******\r\n");

46 res_sd=f_open(&fnew,"0:FatFs读写测试文件.txt",FA_CREATE_ALWAYS|FA_WRITE);

47 if ( res_sd == FR_OK ) {

48 printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");

49 /* 将指定存储区内容写入到文件内 */

50 res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);

51 if (res_sd==FR_OK) {

52 printf("》文件写入成功，写入字节数据：%d\n",fnum);

53 printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);

54 } else {

55 printf("！！文件写入失败：(%d)\n",res_sd);

56 }

57 /* 不再读写，关闭文件 */

58 f_close(&fnew);

59 } else {

60 LED_RED;

61 printf("！！打开/创建文件失败。\r\n");

62 }

63

64 /*------------------ 文件系统测试：读测试 --------------------------*/

65 printf("****** 即将进行文件读取测试... ******\r\n");

66 res_sd=f_open(&fnew,"0:FatFs读写测试文件.txt",FA_OPEN_EXISTING|FA_READ);

67 if (res_sd == FR_OK) {

68 LED_GREEN;

69 printf("》打开文件成功。\r\n");

70 res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);

71 if (res_sd==FR_OK) {

72 printf("》文件读取成功,读到字节数据：%d\r\n",fnum);

73 printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);

74 } else {

75 printf("！！文件读取失败：(%d)\n",res_sd);

76 }

77 } else {

78 LED_RED;

79 printf("！！打开文件失败。\r\n");

80 }

81 /* 不再读写，关闭文件 */

82 f_close(&fnew);

83

84 /* 不再使用文件系统，取消挂载文件系统 */

85 f_mount(NULL,"0:",1);

86

87 /* 操作完成，停机 */

88 while (1) {

89 }

90 }

int main(void) {
	 FATFS fs;      /* File system object (volume work area) */
    FIL fil;       /* File object */
    FRESULT res;   /* API result code */
    UINT bw;       /* Bytes written */
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	
	SPI_Flash_Init();
    /* Register work area */
    f_mount(&fs, "0", 0);

    /* Create FAT volume with default cluster size */
    res = f_mkfs("0", FM_ANY, 0, work, sizeof work );
    if (res == FR_OK){
		work[0] = 1;
	}
	
    /* Create a file as new */
    res = f_open(&fil, "hello.txt", FA_CREATE_NEW | FA_WRITE);
    if (res == FR_OK){
		work[0] = 1;
	}

    /* Write a message */
    f_write(&fil, "Hello, World!\r\n", 15, &bw);
    if (bw != 15){
		work[0] = 1;
	}

    /* Close the file */
    f_close(&fil);

    /* Unregister work area */
    f_mount(0, "", 0);
	while(1);
}
