#ifndef	_CFG_H
#define _CFG_H


#define TASK_DEF_STK_SIZE	(10240)
#define TASK_STK_SIZE		(TASK_DEF_STK_SIZE)
#define TASK_NUM			(3)
#define LOWEST_PRIO			(63)
#define TASK_IDLE_PRIO		(LOWEST_PRIO)

#define N					(TASK_DEF_STK_SIZE)


#define TASK_CYCLE			(0)
#define TASK_PRIORITY		(1)


#define TICKS_PER_SEC		(10u)

#define EVENT_TBL_SIZE		(LOWEST_PRIO + 1)
#define MAX_EVENTS			(10)

#define TASK_STAT_EN		(1)

#define TRUE				(1)
#define FALSE				(0)


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u32 os_stk;

#endif
