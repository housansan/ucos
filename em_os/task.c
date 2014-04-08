/*
 * TASK 相关函数
 */

#include "emos.h"

static char *task_stk_init(tsk_fn func, char *ptos)
{
	ucontext_t uc;
	char *stk;
	int sigsize;

	sigsize = 20 + sizeof(uc);
	stk = ptos - sigsize;

	getcontext(&uc);
	uc.uc_link = NULL;
	uc.uc_mcontext.gregs[REG_EBP] = (int)stk;
	uc.uc_stack.ss_sp = (void *)(stk - TASK_DEF_STK_SIZE + sigsize);
	uc.uc_stack.ss_size = TASK_DEF_STK_SIZE - sigsize;
	makecontext(&uc, func, 0);

	memcpy(stk, &uc, sizeof(uc));

	return stk;
}


int task_create(tsk_fn func, char *ptos)
{
	struct task *ptsk;
	int priority;

	priority = find_free_task();

	tsk[priority].used = 1;
	ptsk = &tsk[priority];

	ptsk->stk = task_stk_init(func, ptos);

	return 0;
}


void start_task(void)
{
	ucontext_t *ucp;
	tsk_thread_id = 0;
	cur_tsk = &tsk[tsk_thread_id];

	ucp = (ucontext_t *)cur_tsk->stk;

	setcontext(ucp);
}

int find_free_task(void)
{
	int i = 0;
	for (i = 0; i < TASK_NUM; ++i)
	{
		if (0 == tsk[i].used)
		{
			break;
		}
	}

	return i;
}
