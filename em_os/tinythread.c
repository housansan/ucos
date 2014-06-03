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
#if CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = sigset_mask;
#endif

	while(1) 
	{
		ENTER_CRITICAL();

		idle_ctr++;

		EXIT_CRITICAL();
	}
}


char *ss;
char *s = "原始数据";
u8 y1 = 0, y2 = 0;


/*
 * 在 terminal 上显示随机数字
 */
void tsk_fn1(void)
{
	int cnt = 0;
	u8 row = 80;
	u8 col = 24;
	u8 x = 2;
	u8 y = 2;
	u8 err;

	u8 fg;
	u8 bg;

	u8 num = 0;

	col -= 5;

	pc_dispclrstr(COLOR_WHITE, COLOR_BLACK);
	//pc_dispstr(x, y, "uC/OS-II, The Real-Time Kernel\n", COLOR_WHITE, COLOR_RED);

	while(1) 
	{
		pc_dispstr(x, y, "task_fn1", COLOR_BLACK, COLOR_WHITE);
		y += 2;
		//printf("jiffies = %d\n", jiffies);
		//s = "MyTask 访问共享数据 s";
		//pc_dispstr(5, ++y1, s, COLOR_BLACK, COLOR_WHITE);
		//time_dly(200);

		//x = my_random(row);
		//y = my_random(col);
		//y += 5;

		//fg = my_random(7);
		//bg = my_random(7);

		//num = my_random(9);

		//pc_dispchar(x, y, num + '0', fg, bg);

		//if (x > 5)
		//{
			//x = 0;
			//y += 2;
		//}

		//pc_dispchar(x, y, 'Y', COLOR_BLACK, COLOR_BLUE);

		//x += 2;

		//++cnt;
		//if (cnt == 10)
		//{
			//task_suspend(1);
		//}

		
		//pc_dispdigit(60, 10, cnt);
		//if (20 == cnt)
		//{
			//task_resume(1);
		//}
		time_dly(11);
	}
}

/*
 * show 'M'
 */
void tsk_fn2(void)
{
	int cnt = 0;
	int x = 12;
	int y = 2;
	u8 c = 'M';
	u32 stime = 0;
	char str[32];
	//pc_dispclrstr(COLOR_WHITE, COLOR_BLACK);
	//task_create(tsk_fn1, 5, &tsk_stk1[TASK_STK_SIZE - 1]);

	while(1) 
	{
		pc_dispstr(x, y, "task_fn2", COLOR_RED, COLOR_WHITE);
		y += 2;
		//s = "YouTask 访问共享数据s";
		//pc_dispstr(28, ++y2, s, COLOR_WHITE, COLOR_BLACK);

		//time_set(0);
		//while(jiffies < 500) 
		//{
			////printf("%d\n", jiffies);
			//pc_dispstr(55, y2, s, COLOR_WHITE, COLOR_BLACK);
		//}

		time_dly(7);

		//if (x == 10)
		//{
			//time_set(10);
		//}

		//stime = time_get();
		//sprintf(str, "%5d", stime);
		//pc_dispstr(5, 2, str, COLOR_WHITE, COLOR_BLACK);

		//x += 1;

		////pc_dispchar(x, y, c, COLOR_BLACK, COLOR_WHITE);

		////x += 2;

		//time_dly(TICKS_PER_SEC);
	}
}


/*
 * 测试 统计任务
 */
void tsk_fn3(void)
{
	int cnt = 0;
	u8 err = NO_ERR;
	u32 ticks = 20;
	char str[64];

	u8 x = 22;
	u8 y = 2;

	//pc_dispclrstr();

	stat_init();
	printf("%u\n", idle_ctr_max);

	while(1) 
	{
		//pc_dispstr(x, y, "task_fn3", COLOR_BLUE, COLOR_GREEN);
		//y += 2;
		printf("main -- cpu usage: %d\n", cpu_usage);
		//sprintf(str, "cpu usage: %d idle_ctr: %u\n", cpu_usage, idle_ctr);
		//pc_dispstr(10, 10, str, COLOR_BLACK, COLOR_WHITE);
		//printf("cpu_usage %u, idle_ctr: %d\n", cpu_usage, idle_ctr);
		//printf("in the function %s\n", __func__);
		time_dly(10);
		//printf("in the function %s\n", __func__);
		//++ticks;
	}
}


void sigint(int signum)
{
	printf("sigint signum: %d\n", SIGINT);
}


int main(int argc, char *argv[])
{
	int i = 0;
	u8 err;

	os_init();

	//err = task_create(tsk_fn1, 2, &tsk_stk1[TASK_STK_SIZE - 1]);
	//task_create(tsk_fn2, 4, &tsk_stk2[TASK_STK_SIZE - 1]);
	task_create(tsk_fn3, 6, &tsk_stk3[TASK_STK_SIZE - 1]);

	disp_str_sem = sem_create(1);

	//signal(SIGINT, sigint);

	linux_init();
	start_task();

	return 0;
}
