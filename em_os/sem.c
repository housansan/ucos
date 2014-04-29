
#include "emos.h"


u8 creat_sem()
{
	sem_init();
}


u8 sem_init(u32 cnt)
{
}


/*
 * 1. 可以不使用 is_sched 当得到 sem 时直接返回
 */
u8 down(struct event *sem)
{
	u8 is_sched = 0;
	struct tcb *ptcb = cur_tcb;

	ENTER_CRITICAL();

	if (sem->count > 0)
	{
		--sem->count;
	}
	else 
	{
		is_sched = 1;

		tcb_exit_rdy(ptcb->prio);
		tcb_enter_wait(&sem->event_grp, sem->event_tbl, ptcb->prio)
	}

	EXIT_CRITICAL();

	if (is_sched)
	{
		schedule();
	}
}


u8 up(struct event *sem)
{
	u8 is_sched = 0;
	u8 prio = 0;
	struct tcb *ptcb = cur_tcb;

	ENTER_CRITICAL();

	if (0 ==sem->event_grp)
	{
		++sem->count;
	}
	else 
	{
		is_sched = 1;
		prio = find_next_wait_task(&sem->event_grp, sem->event_tbl);
		tcb_enter_rdy(prio);
		tcb_exit_wait(&sem->event_grp, sem->event_tbl, prio);
	}

	//if (sem->count <= 0)
	//{
		//// 如果有任务 wake up
		//if (sem->count != 0)
		//{
			//is_sched = 1;
			//tcb_enter_rdy();
			//tcb_exit_wait();
		//}
		//else 
		//{
			//++sem->count;
		//}
	//}
	//else 
	//{
		//++sem->count;
	//}

	EXIT_CRITICAL();

	if (is_sched)
	{
		schedule();
	}

}
