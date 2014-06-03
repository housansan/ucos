#ifndef	_CFG_H
#define _CFG_H


#define TASK_DEF_STK_SIZE	(10240)
#define TASK_STK_SIZE		(TASK_DEF_STK_SIZE)
#define TASK_NUM			(20)
#define LOWEST_PRIO			(63)
#define TASK_IDLE_PRIO		(LOWEST_PRIO)

#define N					(TASK_DEF_STK_SIZE)


/* 使用不同的调度策略 */
#define TASK_CYCLE			(0)
#define TASK_PRIORITY		(1)

/* time_tic 钩子函数 */
#define TIME_GET_SET_EN		(0)

/* 使用第三中关闭中断方式 */
#define CRITICAL_METHOD		(3)


#define TICKS_PER_SEC		(100u)

#define EVENT_TBL_SIZE		(LOWEST_PRIO + 1)
#define MAX_EVENTS			(10)

#define TASK_STAT_EN		(1)

#define TRUE				(1)
#define FALSE				(0)


typedef unsigned char u8;
typedef char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef u32 os_stk;

typedef sigset_t OS_CPU_SR;



//#define DEBUG

#ifdef DEBUG
//#define debug(fmt, arg...)	printf("DEBUG - " fmt, ##arg)
#define debug(fmt, arg...)	do {	\
		printf("%s--", __func__);	\
		printf(fmt, ##arg);	\
		}while(0)
#else
#define debug(fmt, arg...)
#endif

#endif
