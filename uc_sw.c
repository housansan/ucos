#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define __USE_GNU
#include <ucontext.h>
#include <signal.h>


#include "uc_sw.h"

static char tsk1_stk[TASK_STK_SIZE];
static char tsk2_stk[TASK_STK_SIZE];
static struct task tsk_thread[TASK_NUM];
static struct task *current;
static int tsk_thread_id = 0;


/*
 * 目标: 使用 SIGALRM 实现 thread 的切换
 * 步骤: 
 * 1. 产生 SIGALRM
 */



void tick_isr(void)
{
	ctx_sw();
}


void alarm_act(int signo, siginfo_t *info, void *uc)
{
//	printf("in the %s\n", __func__);
	if ((((ucontext_t*)uc)->uc_mcontext.gregs[REG_EIP] >= (unsigned int)setcontext) &&
			(((ucontext_t*)uc)->uc_mcontext.gregs[REG_EIP] < (unsigned int)(setcontext + 110))) {
		//fprintf(stderr, "sig timer: thread interrupted in setcontext\n");
		return;
	}
	if (((ucontext_t*)uc)->uc_mcontext.fpregs == 0) {
		//fprintf(stderr, "sig timer: uc->uc_mcontext.fpregs == 0\n");
		return;
	}
	current->stk = uc; //stk;


	tick_isr();
	// restore context
}


// tsk: thread
// ptos: 指向 ptos = stack[TASK_STK_SIZE - 1]
char *stk_init(task_fn tsk, char *ptos)
{
	char *stk;
	ucontext_t uc;

	// 20 ?
	int sigsize = 20 + sizeof(uc);

	getcontext(&uc);

	stk = (char *)(ptos - sigsize);
	// uc_link = delay_thread
	uc.uc_link = NULL;
	// Q: REG_EBP
	uc.uc_mcontext.gregs[REG_EBP] = (int)stk;
	uc.uc_stack.ss_sp = (void*)(stk - (TASK_DEF_STK_SIZE) + sigsize);
	uc.uc_stack.ss_size = TASK_DEF_STK_SIZE - sigsize;

	makecontext(&uc, (void *)tsk, 0);
	memcpy(stk, &uc, sizeof(uc));

	ucontext_t *tmp = (ucontext_t *)stk;

	return (stk);
}


// tsk: thread
// ptos = stack[TASK_STK_SIZE - 1];
void task_create(task_fn tsk, char *ptos)
{
	struct task *ptsk;
	char *stk;
	int i = 0;
	for (i = 0; i < TASK_NUM; ++i)
	{
		if (0 == tsk_thread[i].used)
		{
			break;
		}
	}

	ptsk = &tsk_thread[i];

	stk = stk_init(tsk, ptos);

	ptsk->stk = stk;
	ptsk->used = 1;
}


void ctx_sw(void)
{
	tsk_thread_id++;
	tsk_thread_id = tsk_thread_id % TASK_NUM;
	current = &tsk_thread[tsk_thread_id];

	ucontext_t *uc = (ucontext_t *)current->stk;
	setcontext(uc);
}


void ctx_sw_sig_handler(int signo, siginfo_t *info, void *uc)
{
	current->stk = uc;

	ctx_sw();
}


void os_init(void)
{
	struct sigaction act;

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = alarm_act;
	sigaction(SIGALRM, &act, NULL);

	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = ctx_sw_sig_handler;
	sigaction(SIGUSR1, &act, NULL);
}


void tsk1_fn(void)
{
	int cnt = 0;
	int i = 0;

	while (1)
	{
		printf("in the %s cnt = %d\n", __func__, cnt);
		for (i = 0; i < N; ++i);
		for (i = 0; i < N; ++i);
		for (i = 0; i < N; ++i);
		printf("come back %s cnt = %d\n", __func__, cnt);
		cnt++;
	}
}


void tsk2_fn(void)
{
	int i = 0;
	int cnt = 0;

	while (1)
	{
		printf("in the %s tk_cnt = %d\n", __func__, cnt);
		for (i = 0; i < N; ++i);
		for (i = 0; i < N; ++i);
		for (i = 0; i < N; ++i);
		printf("come back %s tk_cnt = %d\n", __func__, cnt);
		cnt++;
	}

}


void start_task(void)
{
	current = &tsk_thread[tsk_thread_id];
	ucontext_t *ucp = (ucontext_t *)current->stk;

	setcontext(ucp);
}


int main(int argc, char *argv[])
{
	useconds_t usec;
	char *ptos;

	os_init();


	usec = 10000;
	ualarm(usec, usec);

	// 创建 task
	
	ptos = &tsk1_stk[TASK_STK_SIZE - 1];
	task_create(tsk1_fn, ptos);

	ptos = &tsk2_stk[TASK_STK_SIZE - 1];
	task_create(tsk2_fn, ptos);

	start_task();


	return 0;
}
