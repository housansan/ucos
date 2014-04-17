/*
 * memcopy(ucp, &uc, sizeof(uc));
 *
 * setcontext(ucp);
 * 1. 使用 stk 保存 uc
 *  出错原因: stack size 小了
 *
 * 2. 增加 ualarm
 * 3. 实现优先级时间片 
 *		time_sliec = prio + 1
 */


#include "emos.h"

extern struct tcb tcb_tbl[TASK_NUM];
extern struct tcb *cur_tcb;
extern int tsk_thread_id;

static u8 tsk_stk1[TASK_STK_SIZE];
static u8 tsk_stk2[TASK_STK_SIZE];
static u8 tsk_stk3[TASK_STK_SIZE];




void delay(int n)
{
	int i = 0;
	for (i = 0; i < n; ++i)
	{
	}
}


void tsk_fn1(void)
{
	int cnt = 0;
	while(1) 
	{
		ENTER_CRITICAL();
		printf("in the %s cnt = %d\n", __func__, cnt);
		EXIT_CRITICAL();
		// ...
		sleep(5);

		ENTER_CRITICAL();
		printf("come back %s cnt = %d\n", __func__, cnt);
		EXIT_CRITICAL();
		cnt++;
	}
}


void tsk_fn2(void)
{
	int cnt = 0;
	while(1) 
	{
		printf("in the %s cnt = %d\n", __func__, cnt);

		sleep(2);

		printf("come back %s cnt = %d\n", __func__, cnt);
		cnt++;
	}
}


void tsk_fn3(void)
{
	int cnt = 0;
	while(1) 
	{
		printf("in the %s cnt = %d\n", __func__, cnt);

		sleep(3);

		printf("come back %s cnt = %d\n", __func__, cnt);

		cnt++;
	}
}



/*
 * 1. save current ucontext
 * 2. 
 */
void time_tick_sig_handler(int signo, siginfo_t *info, void *uc)
{
	ucontext_t *ucp;
	u8 next_prio;
	printf("enter %s\n", __func__);
	
	if ((((ucontext_t*)uc)->uc_mcontext.gregs[REG_EIP] >= (unsigned int)setcontext) &&
			(((ucontext_t*)uc)->uc_mcontext.gregs[REG_EIP] < (unsigned int)(setcontext + 110))) {
		fprintf(stderr, "sig timer: thread interrupted in setcontext\n");
		return;
	}
	if (((ucontext_t*)uc)->uc_mcontext.fpregs == 0) {
		fprintf(stderr, "sig timer: uc->uc_mcontext.fpregs == 0\n");
		return;
	}

	time_tick();

	next_prio = find_next_task();
	if (next_prio == tsk_thread_id)
	{
		return;
	}

	tsk_thread_id = next_prio;

	cur_tcb->stk = uc;
	
	cur_tcb = &tcb_tbl[tsk_thread_id];

	ucp = (ucontext_t *)cur_tcb->stk;

	setcontext(ucp);
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


void alrm_init(void)
{
	useconds_t usec = 100000;
	ualarm(usec, usec);
}


int main(int argc, char *argv[])
{
	int i = 0;
	sig_init();

	tcb_head_init();

	task_create(tsk_fn1, 0, &tsk_stk1[TASK_STK_SIZE - 1]);
	task_create(tsk_fn2, 1, &tsk_stk2[TASK_STK_SIZE - 1]);
	task_create(tsk_fn3, 2, &tsk_stk3[TASK_STK_SIZE - 1]);

	for (i = 0; i < TASK_NUM; ++i)
	{
		debug("prio: %d, time_slice: %d\n", i, tcb_tbl[i].time_slice);
	}

	alrm_init();
	start_task();

	return 0;
}
