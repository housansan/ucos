#ifndef _TIME_H
#define _TIME_H

#ifdef TIME_GLOBAL
#define TIME_EXT
#else
#define TIME_EXT	extern
#endif

#include "cfg.h"


TIME_EXT u32 jiffies;


extern void time_dly(int ticks);

extern void time_set(u32 ticks);

extern u32 time_get(void);


#endif
