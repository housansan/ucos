#ifndef _CORE_H
#define _CORE_H

#ifdef CORE_GLOBAL
#define CORE_EXT
#else
#define CORE_EXT	extern
#endif

#include "cfg.h"


extern void tcb_head_init(void);
extern void tcb_exit_rdy(u8 prio);
extern void tcb_enter_rdy(u8 prio);

extern void tcb_exit_wait(u8 *grp, u8 tbl[], u8 prio);
extern void tcb_enter_wait(u8 *grp, u8 tbl[], u8 prio);

extern void task_enter_rdy(struct tcb *ptcb, u8 prio);
extern void task_exit_rdy(struct tcb *ptcb, u8 prio);


extern int find_next_rdy_task(void);
extern u8 find_next_wait_task(u8 grp, u8 tbl[]);

extern void schedule(void);
extern void ctx_sw(void);
extern void tick_isr(void);

extern void time_tick(void);

extern void os_init(void);

extern void mem_clr(void *ptr, u32 len);

extern void stat_init();

extern void start_task();

#if TIME_GET_SET_EN
extern void time_tick_hook();
#endif


#define TASK_IDLE_STK_SIZE		(10240)
#define OS_VERSION		(100)


// 用于统计空闲任务
CORE_EXT s32 idle_ctr;
CORE_EXT s32 idle_ctr_max;
CORE_EXT u8 task_idle_stk[TASK_IDLE_STK_SIZE];
CORE_EXT u8 task_stat_stk[TASK_IDLE_STK_SIZE];
CORE_EXT u32 lock_nesting;
CORE_EXT u16 int_nesting;
// 统计 task 状态
CORE_EXT u8 stat_rdy;
CORE_EXT s8 cpu_usage;
CORE_EXT sigset_t sigset_mask;





#endif
