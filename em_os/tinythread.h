#ifndef _TINYTHREAD_H
#define _TINYTHREAD_H


#define TASK_DEF_STK_SIZE	(10240)
#define TASK_STK_SIZE		TASK_DEF_STK_SIZE	
#define TASK_NUM		(3)

#define N					(TASK_DEF_STK_SIZE)

struct task
{
	char *stk;
	int used;

	// priority
	int prio;

	// 使用的时间片
	// time_slice = prio + 1;
	int time_slice;
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


#define ENTER_CRITICAL()	{sigset_t set;	\
								sigemptyset(&set);	\
								sigaddset(&set, SIGALRM);	\
								sigaddset(&set, SIGIO);	\
								sigprocmask(SIG_SETMASK, &set, &cpu_sr);	\
							}

#define EXIT_CRITICAL()		{sigprocmask(SIG_SETMASK, &cpu_sr, NULL);}

#endif
