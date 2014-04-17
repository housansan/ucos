#ifndef _EM_OS_H
#define _EM_OS_H



#include "common.h"
#include "cfg.h"
#include "cpu.h"
#include "list.h"


#define HOLD	((struct tcb *)1)

#define DEBUG

#ifdef DEBUG
#define debug(fmt, arg...)	printf("DEBUG - " fmt, ##arg)
#else
#define debug(fmt, arg...)
#endif


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
	int state;

	// sleep time
	int delay;

	struct list_head list;
};


typedef void (*tsk_fn)(void);


// 管理所有空闲的tcb
// 从这里取得 tcb
struct list_head *tcb_free_head;

// 管理使用中的tcb
// 当task使用delay时遍历task将delay--
struct list_head *tcb_head;

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


extern void tcb_head_init(void);
extern void tcb_exit_rdy(u8 prio);
extern void tcb_enter_rdy(u8 prio);

extern void time_tick(void);

extern void time_dly(int ticks);


/*
 * ERROR NUM
 */
enum ERRNO
{
	SUCCESS = 0,

	// 没有 task 
	NONE,

	// 指定 priority 已经被占用
	USED,
};

/*
 * task status
 */
enum tsk_stat
{
	TSK_RUNNING,
	TSK_RDY,
	TSK_SLEEP,
};

#endif
