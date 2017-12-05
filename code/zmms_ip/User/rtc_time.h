#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

#include "stm32f10x.h"

typedef struct 
{
    uint16_t year;  /* 1..4095 */
    uint8_t  month; /* 1..12 */
    uint8_t  mday;  /* 1..31 */
    uint8_t  wday;  /* 0..6, Sunday = 0*/
    uint8_t  hour;  /* 0..23 */
    uint8_t  min;   /* 0..59 */
    uint8_t  sec;   /* 0..59 */
    uint8_t  dst;   /* 0 Winter, !=0 Summer */
}rtc_t;

void ltk_rtc_init(void); 
void ltk_rtc_get_time (rtc_t*);              /* Get time */
void ltk_rtc_set_time (const rtc_t*);        /* Set time */


#endif
