#ifndef _TASK_H
#define _TASK_H


#include "cfg.h"



/*
 * task status
 */
enum task_stat
{
	TSK_RUNNING,
	TSK_RDY,
	TSK_SLEEP,
};


/*
 * TASK 
 */
struct tcb
{
	u8 *stk;
	int used;

	u8 prio;
	int time_slice;

	/*
	 * 1. run 
	 * 2. pending
	 * 3. sleep
	 */
	enum task_stat state;

	// sleep time
	int delay;

	struct list_head list;
};



typedef void (*tsk_fn)(void);

// 管理所有空闲的tcb
// 从这里取得 tcb
struct list_head *tcb_free_head;

// 管理rdy中的tcb
struct list_head *tcb_rdy_head;

// delay list
// 当 task 使用 delay 时遍历task将delay--
// 当 dly == 0 时需要从 tcb_dly_head 删除 ptcb
// 而此时正在操作 tcb_dly_head
struct list_head *tcb_dly_head;

struct tcb *cur_tcb;

struct tcb *tcb_prio_tbl[LOWEST_PRIO + 1];
// table oftask control block
struct tcb tcb_tbl[TASK_NUM];

int tsk_thread_id;

sigset_t cpu_sr;

u8 rdy_grp;
// LOWEST_PRIO = 63
u8 rdy_tbl[LOWEST_PRIO/8 + 1];



extern int task_create(tsk_fn func, int prio, u8 *ptos);

extern void start_task(void);

extern int find_next_task(void);


#define HOLD	((struct tcb *)1)

#endif
