#include <rtthread.h>
#include "rtc.h"
#include <time.h>


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t rtc_stack[ 512 ];
static struct rt_thread rtc_thread;
static void rtc_thread_entry(void* parameter) {
    
    if(rt_hw_rtc_init() == 1) {
        set_date(2017,10,24);
        set_time(16,44,12);
    }
    
    while(1) {
        {
            time_t now;

            time(&now);
            rt_kprintf("%s\n", ctime(&now));
        }
        rt_thread_delay( RT_TICK_PER_SECOND);
    }
}


int rtc_task_init(void) {
    rt_err_t tid;
    
    tid = rt_thread_init(&rtc_thread,
                            "rtc",
                            rtc_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&rtc_stack[0],
                            sizeof(rtc_stack),
                            10,
                            5);
    if (tid == RT_EOK) rt_thread_startup(&rtc_thread);

    return 0;
}
