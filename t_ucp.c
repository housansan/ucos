/*
 * memcopy(ucp, &uc, sizeof(uc));
 *
 * setcontext(ucp);
 * 1. 使用 stk 保存 uc
 *  出错原因: stack size 小了
 */


#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __USE_GNU
#include <ucontext.h> 

#define TASK_DEF_STK_SIZE	(10240)
#define TASK_STK_SIZE		TASK_DEF_STK_SIZE	
#define TASK_NUM		(2)

struct task
{
	char *stk;
	int used;
};


typedef void (*tsk_fn)(void);

static ucontext_t *ucp;
static ucontext_t uc;
char stack[TASK_STK_SIZE];

static struct task tsk[TASK_NUM];
static struct task *cur_tsk;



void fn(void)
{
	while(1) 
	{
		printf("in the %s\n", __func__);
		sleep(2);
		printf("%s\n", __func__);
	}
}


char *task_create(tsk_fn func, char *ptos)
{
	ucontext_t uc;
	struct task *ptsk;
	char *stk;
	int sigsize;
	int i;

	for (i = 0; i < TASK_NUM; ++i)
	{
		if (0 == tsk[i].used)
		{
			break;
		}
	}

	tsk[i].used = 1;
	ptsk = &tsk[i];

	sigsize = 20 + sizeof(uc);
	ptsk->stk = stk = ptos - sigsize;

	getcontext(&uc);
	uc.uc_link = NULL;
	uc.uc_mcontext.gregs[REG_EBP] = (int)stk;
	uc.uc_stack.ss_sp = (void *)(stk - TASK_DEF_STK_SIZE + sigsize);
	uc.uc_stack.ss_size = TASK_DEF_STK_SIZE - sigsize;

//	uc.uc_stack.ss_sp = stack;
//	uc.uc_stack.ss_size = sizeof(stack);

	makecontext(&uc, func, 0);


//	memcpy(ucp, &uc, sizeof(uc));

	memcpy(stk, &uc, sizeof(uc));

	return (stk);
}


void start_task(void)
{
	ucontext_t *puc;
	cur_tsk = &tsk[0];

	puc = (ucontext_t *)cur_tsk->stk;

	setcontext(puc);
}


int main(int argc, char *argv[])
{
//	ucp = malloc(sizeof(ucontext_t));
	task_create(fn, &stack[TASK_STK_SIZE - 1]);
//	getcontext(&uc);
//	uc.uc_link = NULL;
//	uc.uc_stack.ss_sp = stack;
//	uc.uc_stack.ss_size = sizeof(stack);
//	makecontext(&uc, fn, 0);
//
//	memcpy(ucp, &uc, sizeof(uc));

	sleep(1);

//	ucontext_t *puc;
//	cur_tsk = &tsk[0];

//	puc = (ucontext_t *)cur_tsk->stk;

	start_task();

	return 0;
}
