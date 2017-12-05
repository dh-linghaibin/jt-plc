#include "rtc_time.h"


#define RCC_BACKUP_DATA 0xA5A5 /* 随机数，用于指示备份数据是否曾经备份过 */
#define FIRST_YEAR      2000   /* 日历开始年份 */       
#define FIRST_DAY       6      /* 0 = 星期日 */
static const uint8_t days_in_month[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
  * 描述: 将 RTC 计数器格式时间转换为日月年格式时间
  * 参数: cnt: RTC 计数器格式时间，单位为秒，它将会转换为日月年格式时间
  *       *t:  保存RTC 计数器格式时间转换出来的日月年格式时间
  * 返回: 无
  */
static void counter_to_struct(uint32_t cnt, rtc_t *t )
{
    uint16_t day;
    uint8_t year;
    uint16_t dayofyear;
    uint8_t leap400;
    uint8_t month;

    t->sec = cnt % 60;
    cnt /= 60;
    t->min = cnt % 60;
    cnt /= 60;
    t->hour = cnt % 24;
    day = (uint16_t)(cnt / 24);

    t->wday = (day + FIRST_DAY) % 7; /* weekday */

    year = FIRST_YEAR % 100;         /* 0..99 */
    leap400 = 4 - ((FIRST_YEAR - 1) / 100 & 3);  /* 4, 3, 2, 1 */

    for(;;) 
    {
        dayofyear = 365;
        if((year & 3) == 0 ) 
        {
            dayofyear = 366; /* 闰年 */
            if(year == 0 || year == 100 || year == 200) /* 100 倍数年份除外 */
            {
                if( --leap400 ) /* 400 倍数年份除外 */
                {                   
                    dayofyear = 365;
                }
            }
        }
        if(day < dayofyear) 
        {
            break;
        }
        day -= dayofyear;
        year++;                     /* 00..136 / 99..235 */
    }
    t->year = year + FIRST_YEAR / 100 * 100; /* + century */

    if( dayofyear & 1 && day > 58 )    /* 不是闰年并且是2月28日以后 */
    {
        day++;          /* 跳过 2月29日 */
    }

    for(month = 1; day >= days_in_month[month - 1]; month++) 
    {
        day -= days_in_month[month - 1];
    }

    t->month = month;               /* 1..12 */
    t->mday = day + 1;              /* 1..31 */
}

/**
  * 描述: 将日月年结构时间转换为 RTC 计数器格式时间
  * 参数: *t，指向日月年结构的时间
  * 返回: RTC 计数器格式时间
  */
static uint32_t struct_to_counter(const rtc_t *t )
{
    uint8_t i;
    uint32_t result = 0;
    uint16_t idx, year;

    year = t->year;

    /* 计算该年之前有多少天 */
    result = (uint32_t)year * 365;
    if (t->year >= 1) 
    {
        result += (year + 3) / 4;
        result -= (year - 1) / 100;
        result += (year - 1) / 400;
    }

    /* 从 2000年开始 */
    result -= 730485UL;

    /* 年份为数组下标 */
    idx = t->month - 1;

    /* 加上每个月的天数 */
    for (i = 0; i < idx; i++) 
    {
        result += days_in_month[i];
    }

    /* 判断是否为闰年，然后调整 2月份的天数 */
    if (year % 400 == 0 || (year%4 == 0 && year%100 !=0)) 
    {
        ;
    } 
    else 
    {
        if (t->month > 1) 
        {
            result--;
        }
    }

    /* 加上剩余天数 */
    result += t->mday;

    /* 转换为秒数 */
    result = (result-1) * 86400L + (uint32_t)t->hour * 3600 + (uint32_t)t->min * 60 + t->sec;

    return result;
}

/**
  * 描述: 将 RTC 计数器格式时间写入 RTC 计数器寄存器
  * 参数: time_set: 将要写入的 RTC 计数器格式时间
  * 返回: 无
  */
void ltk_rtc_set_counter(uint32_t time_set)
{
    /* 设置 RTC 计数器寄存器 */
    RTC_SetCounter(time_set);
    /* 等待上次写入 RTC 寄存器完成 */
    RTC_WaitForLastTask();
}

/**
  * 描述: 不操作备份区域 (backup domain) 寄存器，将日月年格式时间转换为 RTC
  *       计数器格式，并将其设置到硬件 RTC 计数器寄存器内。
  * 参数: *rtc, 日月年格式时间，用来设置硬件 RTC 时间计数器寄存器
  * 返回: 无
  */
void ltk_rtc_set_time(const rtc_t *rtc)
{
    uint32_t cnt;
    rtc_t ts;

    /* 将日月年格式时间转换为 RTC 计数器格式时间 */
    cnt = struct_to_counter(rtc);
    
    /* 通过 RTC 计数器格式时间得到日月年格式时间的其它变量的值，如星期几 */
    counter_to_struct(cnt, &ts);
    
    /* 将 RTC 计数器格式时间设置到 RTC 计数器寄存器 */
    ltk_rtc_set_counter(cnt);
}

/**
  * 描述: 得到当前时间，该时间为日月年格式
  * 参数: *rtc, 用于保存得到的当前时间
  * 返回: 无
  */
void ltk_rtc_get_time(rtc_t *rtc)
{
    counter_to_struct(RTC_GetCounter(), rtc);
}

/**
  * 描述: 配置 RTC
  * 参数: 无
  * 返回: 无
  */ 
void ltk_rtc_config(void)
{
    /* 使能 PWR 和 BKP 时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* 使能 BKP Domain 的访问 */
    PWR_BackupAccessCmd(ENABLE);

    /* 使能外部时钟 LSE (32.768KHz) */
    RCC_LSEConfig(RCC_LSE_ON);
    /* 等待外部时钟 LSE 就绪 */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* 选择外部时钟 LSE 为 RTC 时钟 */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* 使能 RTC 时钟 */
    RCC_RTCCLKCmd(ENABLE);

    /* 等待 RTC 寄存器同步 */
    RTC_WaitForSynchro();

    /* 等待上次写入 RTC 寄存器完成 */
    RTC_WaitForLastTask();

    /* 设置 RTC 间隔为1秒 计算公式为
       RTC period = RTCCLK / RTC_PR = (32.768 KHz) / (32767 + 1) */
    RTC_SetPrescaler(32767);

    /* 等待上次写入 RTC 寄存器完成 */
    RTC_WaitForLastTask();
}

/**
  * 描述: 初始化 RTC
  * 参数: 无
  * 返回: 无
  */
void ltk_rtc_init(void)
{
    /* 配置 RTC */
    ltk_rtc_config();
    
    /* 判断备份数据是否为 RCC_BACKUP_DATA，如果不相等，则表示备份寄存器
       (backup data register) 数据损坏，或者是该程序第一次执行，备份寄存器
       从来没有写入过，在这两种情况下，备份数据区域的数据都是无效的，可能是
       由于纽扣电池掉电，我们需要重新配置时间 */
    if (BKP_ReadBackupRegister(BKP_DR1) != RCC_BACKUP_DATA)
    {
        rtc_t rtc;

        /* 设置时间 2000-1-1 00:00:00*/
        rtc.year = 2017;
        rtc.month = 8;
        rtc.mday = 1;
        rtc.hour = 10;
        rtc.min = 10;
        rtc.sec = 10;
        
        ltk_rtc_set_time(&rtc);

        /* 往备份数据写入 RCC_BACKUP_DATA，用于指示备份区域数据是否有效 */
        BKP_WriteBackupRegister(BKP_DR1, RCC_BACKUP_DATA);
    }
    
    /* 等待 RTC 寄存器同步 */
    RTC_WaitForSynchro();

    /* 清除 RCC 复位标志(RCC reset flags) */
    RCC_ClearFlag();
}
