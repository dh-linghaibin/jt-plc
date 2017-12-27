void test(void) {
	FATFS fs; /* FatFs文件系统对象 */
	FIL fnew; /* 文件对象 */
	FRESULT res_sd; /* 文件操作结果 */
	UINT fnum; /* 文件成功读写数量 */
	BYTE ReadBuffer[1024]= {0}; /* 读缓冲区 */
	BYTE WriteBuffer[] = "linghaibin very best ha on the world \r\n";//
	BYTE work[FF_MAX_SS]; /* Work area (larger is better for processing time) */
	//在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
	res_sd = f_mount(&fs,"0:",0);
	/*----------------------- 格式化测试 ---------------------------*/
	/* 如果没有文件系统就格式化创建创建文件系统 */
	if (res_sd == FR_NO_FILESYSTEM) {
		//printf("》SD卡还没有文件系统，即将进行格式化...\r\n");
		/* 格式化 */
		res_sd=f_mkfs("0:",FM_FAT,0,work, sizeof work );
		if (res_sd == FR_OK) {
			//printf("》SD卡已成功格式化文件系统。\r\n");
			/* 格式化后，先取消挂载 */
			res_sd = f_mount(0,"0:",0);
			/* 重新挂载 */
			res_sd = f_mount(&fs,"0:",0);
		} else {
			//printf("《《格式化失败。》》\r\n");
			while (1);
		}
	} else if (res_sd!=FR_OK) {
		//printf("！！SD卡挂载文件系统失败。(%d)\r\n",res_sd);
		//printf("！！可能原因：SD卡初始化不成功。\r\n");
		while (1);
	} else {
		//printf("》文件系统挂载成功，可以进行读写测试\r\n");
	}
	/*--------------------- 文件系统测试：写测试 -----------------------*/
	/* 打开文件，如果文件不存在则创建它 */
	//printf("\r\n****** 即将进行文件写入测试... ******\r\n");
	res_sd=f_open(&fnew,"0:lhb6.txt",FA_CREATE_ALWAYS|FA_WRITE);
	if ( res_sd == FR_OK ) {
		//printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
		/* 将指定存储区内容写入到文件内 */
		res_sd=f_write(&fnew,WriteBuffer,sizeof(WriteBuffer),&fnum);
		if (res_sd==FR_OK) {
			//printf("》文件写入成功，写入字节数据：%d\n",fnum);
			//printf("》向文件写入的数据为：\r\n%s\r\n",WriteBuffer);
		} else {
			//printf("！！文件写入失败：(%d)\n",res_sd);
		}
		/* 不再读写，关闭文件 */
		f_close(&fnew);
	} else {
		//printf("！！打开/创建文件失败。\r\n");

	}
	/*------------------ 文件系统测试：读测试 --------------------------*/
	//printf("****** 即将进行文件读取测试... ******\r\n");
	res_sd=f_open(&fnew,"0:lhb6.txt",FA_OPEN_EXISTING|FA_READ);
	if (res_sd == FR_OK) {
		//printf("》打开文件成功。\r\n");
		res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
		if (res_sd==FR_OK) {
			//printf("》文件读取成功,读到字节数据：%d\r\n",fnum);
			//printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
		} else {
			//printf("！！文件读取失败：(%d)\n",res_sd);
		}
	} else {
		//printf("！！打开文件失败。\r\n");
	}

	/* 不再读写，关闭文件 */
	f_close(&fnew);
	/* 不再使用文件系统，取消挂载文件系统 */
	f_mount(0,"0:",0);
	/* 操作完成，停机 */
	while (1) {

	}
}


void theTimerCallback(TimerHandle_t pxTimer)
{
	//motor_durationTick();
	led.tager(&led,L_CAN);
	
}

void theTimerInit(int msCount)
{
	TickType_t timertime = (msCount/portTICK_PERIOD_MS);
	TimerHandle_t theTimer = xTimerCreate("theTimer", timertime , pdTRUE, 0, theTimerCallback );
	if( xTimerStart(theTimer, 0) != pdPASS )
	{
		//debugU("Timer failed to start");
	}
}
//theTimerInit(500);

static const char program[] =
"10 gosub 100\n\
//20 for i = 1 to 10\n\
//30 print i\n\
//40 next i\n\
//50 print \"end\"\n\
//60 end\n\
//100 print \"subroutine\"\n\
//101 if 10 > 0 then print 1000\n\
110 return\n";

static const char program2[] =
"1 v=1\n\
30 l=1000\n\
2 for p = 0 to 7\n\
3 write \"do_8\"\,0,p,v\n\
4 wait l\n\
5 next p\n\
6 for p = 0 to 7\n\
7 write \"do_8\"\,1,p,v\n\
8 wait l\n\
9 next p\n\
10 for p = 0 to 7\n\
11 write \"do_8\"\,2,p,v\n\
12 wait l\n\
13 next p\n\
14 for p = 0 to 7\n\
15 write \"do_8\"\,3,p,v\n\
16 wait l\n\
17 next p\n\
18 for p = 0 to 7\n\
19 write \"do_8\"\,4,p,v\n\
20 wait l\n\
21 next p\n\
22 for p = 0 to 7\n\
23 write \"do_8\"\,5,p,v\n\
24 wait l\n\
25 next p\n\
26 if v=0 then goto 1\n\
27 if v=1 then v=0\n\
28 goto 2 ";

//	ubasic_init(program);
//	do {
//		ubasic_run();
//	} while(!ubasic_finished());
