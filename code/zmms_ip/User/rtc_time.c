#include "rtc_time.h"


#define RCC_BACKUP_DATA 0xA5A5 /* �����������ָʾ���������Ƿ��������ݹ� */
#define FIRST_YEAR      2000   /* ������ʼ��� */       
#define FIRST_DAY       6      /* 0 = ������ */
static const uint8_t days_in_month[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
  * ����: �� RTC ��������ʽʱ��ת��Ϊ�������ʽʱ��
  * ����: cnt: RTC ��������ʽʱ�䣬��λΪ�룬������ת��Ϊ�������ʽʱ��
  *       *t:  ����RTC ��������ʽʱ��ת���������������ʽʱ��
  * ����: ��
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
            dayofyear = 366; /* ���� */
            if(year == 0 || year == 100 || year == 200) /* 100 ������ݳ��� */
            {
                if( --leap400 ) /* 400 ������ݳ��� */
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

    if( dayofyear & 1 && day > 58 )    /* �������겢����2��28���Ժ� */
    {
        day++;          /* ���� 2��29�� */
    }

    for(month = 1; day >= days_in_month[month - 1]; month++) 
    {
        day -= days_in_month[month - 1];
    }

    t->month = month;               /* 1..12 */
    t->mday = day + 1;              /* 1..31 */
}

/**
  * ����: ��������ṹʱ��ת��Ϊ RTC ��������ʽʱ��
  * ����: *t��ָ��������ṹ��ʱ��
  * ����: RTC ��������ʽʱ��
  */
static uint32_t struct_to_counter(const rtc_t *t )
{
    uint8_t i;
    uint32_t result = 0;
    uint16_t idx, year;

    year = t->year;

    /* �������֮ǰ�ж����� */
    result = (uint32_t)year * 365;
    if (t->year >= 1) 
    {
        result += (year + 3) / 4;
        result -= (year - 1) / 100;
        result += (year - 1) / 400;
    }

    /* �� 2000�꿪ʼ */
    result -= 730485UL;

    /* ���Ϊ�����±� */
    idx = t->month - 1;

    /* ����ÿ���µ����� */
    for (i = 0; i < idx; i++) 
    {
        result += days_in_month[i];
    }

    /* �ж��Ƿ�Ϊ���꣬Ȼ����� 2�·ݵ����� */
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

    /* ����ʣ������ */
    result += t->mday;

    /* ת��Ϊ���� */
    result = (result-1) * 86400L + (uint32_t)t->hour * 3600 + (uint32_t)t->min * 60 + t->sec;

    return result;
}

/**
  * ����: �� RTC ��������ʽʱ��д�� RTC �������Ĵ���
  * ����: time_set: ��Ҫд��� RTC ��������ʽʱ��
  * ����: ��
  */
void ltk_rtc_set_counter(uint32_t time_set)
{
    /* ���� RTC �������Ĵ��� */
    RTC_SetCounter(time_set);
    /* �ȴ��ϴ�д�� RTC �Ĵ������ */
    RTC_WaitForLastTask();
}

/**
  * ����: �������������� (backup domain) �Ĵ��������������ʽʱ��ת��Ϊ RTC
  *       ��������ʽ�����������õ�Ӳ�� RTC �������Ĵ����ڡ�
  * ����: *rtc, �������ʽʱ�䣬��������Ӳ�� RTC ʱ��������Ĵ���
  * ����: ��
  */
void ltk_rtc_set_time(const rtc_t *rtc)
{
    uint32_t cnt;
    rtc_t ts;

    /* ���������ʽʱ��ת��Ϊ RTC ��������ʽʱ�� */
    cnt = struct_to_counter(rtc);
    
    /* ͨ�� RTC ��������ʽʱ��õ��������ʽʱ�������������ֵ�������ڼ� */
    counter_to_struct(cnt, &ts);
    
    /* �� RTC ��������ʽʱ�����õ� RTC �������Ĵ��� */
    ltk_rtc_set_counter(cnt);
}

/**
  * ����: �õ���ǰʱ�䣬��ʱ��Ϊ�������ʽ
  * ����: *rtc, ���ڱ���õ��ĵ�ǰʱ��
  * ����: ��
  */
void ltk_rtc_get_time(rtc_t *rtc)
{
    counter_to_struct(RTC_GetCounter(), rtc);
}

/**
  * ����: ���� RTC
  * ����: ��
  * ����: ��
  */ 
void ltk_rtc_config(void)
{
    /* ʹ�� PWR �� BKP ʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

    /* ʹ�� BKP Domain �ķ��� */
    PWR_BackupAccessCmd(ENABLE);

    /* ʹ���ⲿʱ�� LSE (32.768KHz) */
    RCC_LSEConfig(RCC_LSE_ON);
    /* �ȴ��ⲿʱ�� LSE ���� */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* ѡ���ⲿʱ�� LSE Ϊ RTC ʱ�� */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* ʹ�� RTC ʱ�� */
    RCC_RTCCLKCmd(ENABLE);

    /* �ȴ� RTC �Ĵ���ͬ�� */
    RTC_WaitForSynchro();

    /* �ȴ��ϴ�д�� RTC �Ĵ������ */
    RTC_WaitForLastTask();

    /* ���� RTC ���Ϊ1�� ���㹫ʽΪ
       RTC period = RTCCLK / RTC_PR = (32.768 KHz) / (32767 + 1) */
    RTC_SetPrescaler(32767);

    /* �ȴ��ϴ�д�� RTC �Ĵ������ */
    RTC_WaitForLastTask();
}

/**
  * ����: ��ʼ�� RTC
  * ����: ��
  * ����: ��
  */
void ltk_rtc_init(void)
{
    /* ���� RTC */
    ltk_rtc_config();
    
    /* �жϱ��������Ƿ�Ϊ RCC_BACKUP_DATA���������ȣ����ʾ���ݼĴ���
       (backup data register) �����𻵣������Ǹó����һ��ִ�У����ݼĴ���
       ����û��д�����������������£�����������������ݶ�����Ч�ģ�������
       ����Ŧ�۵�ص��磬������Ҫ��������ʱ�� */
    if (BKP_ReadBackupRegister(BKP_DR1) != RCC_BACKUP_DATA)
    {
        rtc_t rtc;

        /* ����ʱ�� 2000-1-1 00:00:00*/
        rtc.year = 2017;
        rtc.month = 8;
        rtc.mday = 1;
        rtc.hour = 10;
        rtc.min = 10;
        rtc.sec = 10;
        
        ltk_rtc_set_time(&rtc);

        /* ����������д�� RCC_BACKUP_DATA������ָʾ�������������Ƿ���Ч */
        BKP_WriteBackupRegister(BKP_DR1, RCC_BACKUP_DATA);
    }
    
    /* �ȴ� RTC �Ĵ���ͬ�� */
    RTC_WaitForSynchro();

    /* ��� RCC ��λ��־(RCC reset flags) */
    RCC_ClearFlag();
}
