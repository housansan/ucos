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


#define TASK_IDLE_STK_SIZE		(10240)
#define OS_VERSION		(100)


// 用于统计空闲任务
CORE_EXT u32 idle_ctr;
CORE_EXT u8 task_idle_stk[TASK_IDLE_STK_SIZE];





#endif
