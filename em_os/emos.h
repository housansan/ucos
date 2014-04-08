#ifndef _EM_OS_H
#define _EM_OS_H



#include "common.h"
#include "cfg.h"
#include "cpu.h"


/*
 * TASK 
 */

struct task
{
	char *stk;
	int used;

	int prio;
	int time_slice;
};


typedef void (*tsk_fn)(void);


struct task tsk[TASK_NUM];
struct task *cur_tsk;
int tsk_thread_id;


extern int task_create(tsk_fn func, char *ptos);
extern void start_task(void);
extern int find_free_task(void);


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

#endif
