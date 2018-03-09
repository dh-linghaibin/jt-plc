/*
 * This file is part of the 
 *
 * Copyright (c) 2016-2017 linghaibin
 *
 */

#include "stime.h"
#include <string.h>

static struct _stime_obj list_time[BEST_STIME];
static uint16_t timeslice = 0;

void stime_init(void) {
    register int i = 0;
    for(i = 0;i < BEST_STIME;i++) {
        list_time[i].is_enable = ST_DISABLE;
    }
    
}

int stime_create(const char *name,
                 uint16_t sec,
                 stime_type_e type,
                 void (*time_up)(void)) {
    register uint8_t i = 0;
    
    for(i = 0;i < BEST_STIME;i++) {
        if(list_time[i].is_enable == ST_ENABLE) {
            if(strcmp(name,list_time[i].name) == 0) {
                list_time[i].time_up   = time_up;
                list_time[i].end_t     = sec;
                list_time[i].time      = sec;
                list_time[i].type      = type;
                list_time[i].is_enable = ST_ENABLE;
                list_time[i].name      = (char *)name;
                return i;
            }
        }
    }
    
    for(i = 0;i < BEST_STIME;i++) {
        if(list_time[i].is_enable == ST_DISABLE) {
            list_time[i].time_up   = time_up;
            list_time[i].end_t     = sec;
            list_time[i].time      = sec;
            list_time[i].type      = type;
            list_time[i].is_enable = ST_ENABLE;
            list_time[i].name      = (char *)name;
            return i;
        }
    }
    return -1;
}

int stime_delet(const char *name) {
    for(register uint8_t i = 0;i < BEST_STIME;i++) {
        if(list_time[i].is_enable == ST_ENABLE) {
            if(strcmp(name,list_time[i].name) == 0) {
                list_time[i].is_enable = ST_DISABLE;
                return i;
            }
        }
    }
    return -1;
}

void stime_loop(void) {
    register uint8_t i = 0;
    for(i = 0;i < BEST_STIME;i++) {
        if(list_time[i].is_enable == ST_ENABLE) {
            if(list_time[i].end_t == 0) {
                if(list_time[i].time_up != NULL) {
                    if(list_time[i].type == ST_ONCE) {
                        list_time[i].is_enable = ST_DISABLE;
                    } else {
                        list_time[i].end_t = list_time[i].time;
                    }
                    list_time[i].time_up();
                }
            }
        }
    }
}


void Tim2_Overflow(void) {
    timeslice++;
    register uint8_t i = 0;
    for(i = 0;i < BEST_STIME;i++) {
        if(list_time[i].is_enable == ST_ENABLE) {
            if(list_time[i].end_t > 0) {
               list_time[i].end_t--; 
            }
        }
    }
}

