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

enum ERRNO 
{  
	SUCCESS = 0,

	// 没有 thread, 用完了
	NONE,

	// 指定的 priory 已经被占用
	USED,
};


typedef void (*tsk_fn)(void);

static struct task tsk[TASK_NUM];
static struct task *cur_tsk;

static char tsk_stk1[TASK_STK_SIZE];
static char tsk_stk2[TASK_STK_SIZE];



void tsk_fn1(void)
{
	int cnt = 0;
	while(1) 
	{
		printf("in the %s cnt = %d\n", __func__, cnt);
		printf("come back %s cnt = %d\n", __func__, cnt);
		cnt++;
	}
}


void tsk_fn2(void)
{
	int cnt = 0;
	while(1) 
	{
		printf("in the %s cnt = %d\n", __func__, cnt);
		printf("come back %s cnt = %d\n", __func__, cnt);
		cnt++;
	}
}


char *task_stk_init(tsk_fn func, char *ptos)
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
	cur_tsk = &tsk[0];

	ucp = (ucontext_t *)cur_tsk->stk;

	setcontext(ucp);
}


void time_tick_sig_handler(int signo, siginfo_t *info, void *uc)
{

}


void ctx_sw_sig_handler(int signo, siginfo_t *info, void *uc)
{
}


void sig_init(void)
{
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = time_tick_sig_handler;
	sigaction(SIGALRM, &act, NULL);

	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = ctx_sw_sig_handler;
	sigaction(SIGUSR1, &act, NULL);
}


int main(int argc, char *argv[])
{
	sig_init();
	task_create(tsk_fn1, &tsk_stk1[TASK_STK_SIZE - 1]);
	task_create(tsk_fn2, &tsk_stk2[TASK_STK_SIZE - 1]);

	start_task();

	return 0;
}
