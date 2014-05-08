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


void task_idle(void)
{
	while(1) 
	{
		ENTER_CRITICAL();

		idle_ctr++;

		EXIT_CRITICAL();
	}
}


/*
 * 在 terminal 上显示随机数字
 */
void tsk_fn1(void)
{
	int cnt = 0;
	u8 row = 80;
	u8 col = 24;
	u8 x = 10;
	u8 y = 0;
	u8 err;

	u8 fg;
	u8 bg;

	u8 num = 0;

	col -= 5;

	pc_dispclrstr();
	pc_dispstr(x, y, "uC/OS-II, The Real-Time Kernel\n", WHITE, RED);

	while(1) 
	{
		x = my_random(row);
		y = my_random(col);
		y += 5;

		fg = my_random(7);
		bg = my_random(7);

		num = my_random(9);

		pc_dispchar(x, y, num + '0', fg, bg);
	}
}

/*
 * show 'M'
 */
void tsk_fn2(void)
{
	int cnt = 0;
	int x = 0;
	int y = 0;
	u8 c = 'M';
	//pc_dispclrstr();
	task_create(tsk_fn1, 0, &tsk_stk1[TASK_STK_SIZE - 1]);

	while(1) 
	{
		if (x > 10)
		{
			x = 0;
			y += 2;
		}

		pc_dispchar(x, y, c, BLACK, WHITE);

		x += 1;

		time_dly(3);
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

	//task_create(tsk_fn1, 0, &tsk_stk1[TASK_STK_SIZE - 1]);
	task_create(tsk_fn2, 1, &tsk_stk2[TASK_STK_SIZE - 1]);
	//task_create(tsk_fn3, 2, &tsk_stk3[TASK_STK_SIZE - 1]);

	for (i = 0; i < TASK_NUM; ++i)
	{
		debug("prio: %d, time_slice: %d\n", i, tcb_tbl[i].time_slice);
	}

	disp_str_sem = sem_create(1);

	//signal(SIGINT, sigint);

	linux_init();
	start_task();

	return 0;
}
