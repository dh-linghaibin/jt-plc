#include <dfs_posix.h> /* 当需要使用文件操作时，需要包含这个头文件 */
#include <board.h>
#include <rtthread.h>
#include "led.h"

#ifdef RT_USING_SPI
#include "rt_stm32f10x_spi.h"
#endif 

#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
#include "msd.h"
#endif /* RT_USING_DFS */

#ifdef  RT_USING_COMPONENTS_INIT
	#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
	/* dfs Filesystem APIs */
	#include <dfs_fs.h>
#endif

static void spi_flash_init(void) {
#ifdef RT_USING_SPI2
    /* register spi bus */
    {
        static struct stm32_spi_bus stm32_spi;
        GPIO_InitTypeDef GPIO_InitStructure;

        /* Enable GPIO clock */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
        ENABLE);

        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        stm32_spi_register(SPI2, &stm32_spi, "spi2");
    }

    /* attach cs */
    {
        static struct rt_spi_device spi_device;
        static struct stm32_spi_cs  spi_cs;
        struct stm32_spi_cs  spi_wp, spi_hold;

        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi21: PG10 */
        spi_cs.GPIOx = GPIOC;
        spi_cs.GPIO_Pin = GPIO_Pin_6;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_cs.GPIO_Pin;
        GPIO_SetBits(spi_cs.GPIOx, spi_cs.GPIO_Pin);
        GPIO_Init(spi_cs.GPIOx, &GPIO_InitStructure);


        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

        /* spi21: PG10 */
        spi_wp.GPIOx = GPIOC;
        spi_wp.GPIO_Pin = GPIO_Pin_7;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_wp.GPIO_Pin;
        GPIO_SetBits(spi_wp.GPIOx, spi_wp.GPIO_Pin);
        GPIO_Init(spi_wp.GPIOx, &GPIO_InitStructure);

        spi_hold.GPIOx = GPIOB;
        spi_hold.GPIO_Pin = GPIO_Pin_12;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = spi_hold.GPIO_Pin;
        GPIO_SetBits(spi_hold.GPIOx, spi_hold.GPIO_Pin);
        GPIO_Init(spi_hold.GPIOx, &GPIO_InitStructure);

        rt_spi_bus_attach_device(&spi_device, "wq", "spi2", (void*)&spi_cs);
    }

    {
  		rt_thread_delay(2);
  		{
  			extern rt_err_t w25qxx_init(const char * flash_device_name,
  						const char * spi_device_name);
  			if(w25qxx_init("flash", "wq") != RT_EOK)
  			{
  				rt_kprintf("[error] No such spi flash!\r\n");
  			} else {
  				rt_kprintf("[OK]spi flash!\r\n");
  			}
  		}
    }
#endif /* RT_USING_SPI1 */
}


/* 假设文件操作是在一个线程中完成*/
void file_thread(void)
{
    int fd,size;
    char s[] = "RT-Thread Programmer!\n", buffer[80];
    /* 打开/text.txt 作写入，如果该文件不存在则建立该文件*/
        fd = open("text.txt",O_WRONLY|O_CREAT|O_TRUNC,0664);
        if (fd >= 0)
        {
            write(fd, s, sizeof(s));
            close(fd);
        }
    /* 打开/text.txt 准备作读取动作*/
    fd = open("/text.txt", O_RDONLY,0664);
    if (fd >= 0)
    {
        size=read(fd, buffer, sizeof(buffer));
        close(fd);
    }
    rt_kprintf("%s", buffer);
}

//rt_uint8_t cs[800];

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t flash_stack[ 2048 ];
static struct rt_thread flash_thread;
static void flash_thread_entry(void* parameter) {
    uint16_t count=0;
    
    spi_flash_init();
    /* Filesystem Initialization */
    #if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
        /* initialize the device file system */
        dfs_init();

        /* initialize the elm chan FatFS file system*/
        elm_init();

        /* mount sd card fat partition 1 as root directory */
        if (dfs_mount("flash", "/", "elm", 0, 0) == 0) {
            rt_kprintf("File System initialized!\n");
        } else {
            rt_kprintf("File System initialzation failed!\n");
        }
    #endif /* RT_USING_DFS && RT_USING_DFS_ELMFAT */

    #ifdef RT_USING_FINSH
        /* initialize finsh */
        finsh_system_init();
        finsh_set_device(RT_CONSOLE_DEVICE_NAME);
    #endif
        
    rt_hw_led_init();
    rt_kprintf("2222!\n");
    //file_thread();
    
    {
        int fd,size;
        fd = open("/text.txt", O_RDONLY,0664);
        char buffer[180];
        if (fd >= 0)
        {
            size=read(fd, buffer, sizeof(buffer));
            close(fd);
        }
        rt_kprintf("%s", buffer);
    }
   

    while (1) {
        /* led1 on */
        #ifndef RT_USING_FINSH
            rt_kprintf("led on, count : %d\r\n",count);
        #endif
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

        /* led1 off */
        #ifndef RT_USING_FINSH
            rt_kprintf("led off\r\n");
        #endif
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
        
        {
            int fd,size;
            fd = open("/text.txt", O_RDONLY,0664);
            char buffer[180];
            if (fd >= 0)
            {
                size=read(fd, buffer, sizeof(buffer));
                close(fd);
            }
            rt_kprintf("%s", buffer);
        }
    }
}


int flash_task_init(void) {
    rt_err_t tid;
    
    tid = rt_thread_init(&flash_thread,
                            "flash",
                            flash_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&flash_stack[0],
                            sizeof(flash_stack),
                            5,
                            10);
    if (tid == RT_EOK) rt_thread_startup(&flash_thread);

    return 0;
}

