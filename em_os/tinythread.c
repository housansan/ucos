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

static struct event *pevent;



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
	u8 err;
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

		if (10 == cnt)
		{
			err = down(pevent);
			if (ERR_TIME_OUT == err)
			{
				printf("down semaphore time out\n");
			}
			else 
			{
				printf("get semaphore\n");
			}
			//task_suspend(PRIO_SELF);
			cnt = 0;
			printf("%s jiffies %d\n", __func__, time_get());
		}

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

		if (5 == cnt)
		{
			up(pevent);
			printf("%s time_dly back\n", __func__);
		}
	}
}


void tsk_fn3(void)
{
	int cnt = 0;
	u8 err = NO_ERR;
	while(1) 
	{
		printf("in the %s cnt = %d\n", __func__, cnt);

		sleep(3);

		printf("come back %s cnt = %d\n", __func__, cnt);

		cnt++;


		if (7 == cnt)
		{
			err = change_task_prio(2, 0);
			if (NO_ERR == err)
			{
				printf("chang task prio success new_prio: %d\n", 0);
			}
		}
	}
}


void sigint(int signum)
{
	printf("sigint signum: %d\n", SIGINT);
	up(pevent);
}


int main(int argc, char *argv[])
{
	int i = 0;

	os_init();

	task_create(tsk_fn1, 0, &tsk_stk1[TASK_STK_SIZE - 1]);
	task_create(tsk_fn2, 1, &tsk_stk2[TASK_STK_SIZE - 1]);
	//task_create(tsk_fn3, 2, &tsk_stk3[TASK_STK_SIZE - 1]);

	for (i = 0; i < TASK_NUM; ++i)
	{
		debug("prio: %d, time_slice: %d\n", i, tcb_tbl[i].time_slice);
	}

	pevent = sem_create(2);

	signal(SIGINT, sigint);

	linux_init();
	start_task();

	return 0;
}
