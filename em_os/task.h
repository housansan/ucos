#ifndef _TASK_H
#define _TASK_H


#include "cfg.h"


/*
 * 目的:
 * 在 *.c 中定义
 * 在 *.h 中声明
 */
#ifdef	TASK_GLOBAL
#define TASK_EXT
#else
#define TASK_EXT	extern
#endif



/*
 * task status
 */
enum task_stat
{
	TASK_RDY,
//	TASK_SEM,
//	TASK_MBOX,
//	TASK_Q,
	TASK_SUSPEND,
//	TASK_MUTEX,
};


/*
 * TASK 
 */
struct tcb
{
	u8 *stk;
	int used;

	u8 prio;
#if TASK_CREATE_EXT_EN
	// 使用id来唯一确定task
	u16 tcb_pid;
#endif
	int time_slice;

	/*
	 * 1. run 
	 * 2. pending
	 * 3. sleep
	 */
	u8 stat;
	u8 pend_to; // flag indicating PEND time out, TRUE time_out, otherwise 得到 semaphore or event etc.

	// sleep time
	int delay;

	struct list_head list;
};



// TODO: argument
// 使用参数的好处: 可以通过其他task创建程序时
// 传递参数
typedef void (*tsk_fn)(void);

// 管理所有空闲的tcb
// 从这里取得 tcb
TASK_EXT struct list_head *tcb_free_head;

// 管理rdy中的tcb
TASK_EXT struct list_head *tcb_rdy_head;

// delay list
// 当 task 使用 delay 时遍历task将delay--
// 当 dly == 0 时需要从 tcb_dly_head 删除 ptcb
// 而此时正在操作 tcb_dly_head
TASK_EXT struct list_head *tcb_dly_head;

TASK_EXT struct tcb *cur_tcb;

// current priority
TASK_EXT u8 cur_prio;

// the highest priority
TASK_EXT u8 high_rdy_prio;

TASK_EXT struct tcb *tcb_high_rdy;

TASK_EXT struct tcb *tcb_prio_tbl[LOWEST_PRIO + 1];
// table oftask control block
TASK_EXT struct tcb tcb_tbl[TASK_NUM];

//TASK_EXT sigset_t cpu_sr;

TASK_EXT u8 rdy_grp;
// LOWEST_PRIO = 63
TASK_EXT u8 rdy_tbl[LOWEST_PRIO/8 + 1];

// os is running
TASK_EXT u8 os_running;




extern u8 task_create(tsk_fn func, int prio, u8 *ptos);

extern u8 task_suspend(u8 prio);

extern u8 task_resume(u8 prio);

extern u8 change_task_prio(u8 old_prio, u8 new_prio);

// TODO 移到合适的位置
extern u8 general_check_prio(u8 prio);


#define HOLD	((struct tcb *)1)


// task status
#define TASK_STAT_RDY		0
// time_dly 中设置
// 任务处于 挂起 
#define TASK_STAT_SUSPEND	(1 << 1)
// task 等待 semaphore
#define TASK_STAT_SEM		(1 << 2)
// task wait mbox
#define TASK_STAT_MBOX		(1 << 3)
// task wait queue
#define TASK_STAT_Q			(1 << 4)
// task wait mutex
#define TASK_STAT_MUTEX		(1 << 5)

#define TASK_STAT_PEND_ANY	(TASK_STAT_SEM | TASK_STAT_MBOX | TASK_STAT_Q | TASK_STAT_MUTEX)


#define PRIO_SELF		(0xff)

#endif
