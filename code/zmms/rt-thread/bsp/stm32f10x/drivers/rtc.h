#ifndef __RTC_H__
#define __RTC_H__

void list_date(void);
u8 rt_hw_rtc_init(void);

#include <time.h>
time_t time(time_t* t);

#endif
