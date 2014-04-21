#ifndef _CORE_H
#define _CORE_H

#include "cfg.h"


extern void tcb_head_init(void);
extern void tcb_exit_rdy(u8 prio);
extern void tcb_enter_rdy(u8 prio);

extern void task_enter_rdy(struct tcb *ptcb, u8 prio);
extern void task_exit_rdy(struct tcb *ptcb, u8 prio);

extern void schedule(void);
extern void ctx_sw(void);
extern void tick_isr(void);

extern void time_tick(void);

#endif
