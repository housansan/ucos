
#include "emos.h"



static struct event *sem_init(struct event *pevent, u16 cnt)
{
	if (NULL == pevent)
	{
		return NULL;
	}
	pevent->event_ptr = NULL;
	pevent->event_grp = 0;
	mem_clr(pevent->event_tbl, sizeof(pevent->event_tbl));
	pevent->event_type = EVENT_TYPE_SEM;
	pevent->count = cnt;

	return pevent;
}
/*
 * arguments count: is initial value for the semaphore
 * return 
 * 1. 从event_free_head 中得到 struct event
 */
struct event *sem_create(u16 cnt)
{
#if CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = sigset_mask;
#endif
	
	struct event *pevent;

	ENTER_CRITICAL();

	if (NULL == event_free_head)
	{
		EXIT_CRITICAL();
		return NULL;
	}

	pevent = event_free_head;
	event_free_head = event_free_head->event_ptr;

	EXIT_CRITICAL();

	return (sem_init(pevent, cnt));
}




/*
 * 1. 可以不使用 is_sched 当得到 sem 时直接返回
 * 2. 需要将 stat 设置, 设置什么 stat
 *
 * 这里要注意一个问题就是
 * 1. tick_times 能够减小 delay (等待任务)
 * 2. delay == 0 后要能够参与 schedule
 *
 * Q: why 使用 u8 *err
 * A: 这个函数可能返回不了
 *
 * 3. schedule 后 需要将
 *
 * return 0 if success, else return 剩余的timeout
 */
u8 down_timeout(struct event *sem, u16 timeout)
{
#if CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = sigset_mask;
#endif
	
	struct tcb *ptcb = cur_tcb;
	u8 err = NO_ERR;

	if (NULL == sem)
	{
		return (ERR_PEVENT_NULL);
	}

	if (EVENT_TYPE_SEM != sem->event_type)
	{
		return (ERR_EVENT_TYPE);
	}

	ENTER_CRITICAL();

	if (sem->count > 0)
	{
		--sem->count;

		EXIT_CRITICAL();

		err = NO_ERR;

		return err;
	}

	ptcb->stat |= TASK_STAT_SEM;
	ptcb->delay = timeout;
	tcb_enter_wait(&sem->event_grp, sem->event_tbl, ptcb->prio);
	//tcb_exit_rdy(ptcb->prio);
	//tcb_enter_wait(&sem->event_grp, sem->event_tbl, ptcb->prio);

	EXIT_CRITICAL();

	schedule();

	debug("stat: %d\n", ptcb->stat);

	// 返回后有两种情况
	// 1. 得到信号量
	// 2. timeout
	// 判断 stat
	if (ptcb->stat & TASK_STAT_SEM)
	{
		debug("ERR_TIME_OUT\n");
		err = ERR_TIME_OUT;
	}

	return err;
}


u8 down(struct event *sem)
{
	return (down_timeout(sem, 0));
}


u8 down_trylock(struct event *sem)
{
	if (NULL == sem)
	{
		return ERR_PEVENT_NULL;
	}

	if (EVENT_TYPE_SEM != sem->event_type)
	{
		return ERR_EVENT_TYPE;
	}

	if (sem->count > 0)
	{
		--sem->count;
	}

	return (sem->count);
}


/*
 * 1. 确保 count 不会 overflow
 */
u8 up(struct event *sem)
{
#if CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr = sigset_mask;
#endif
	
	u8 prio = 0;
	struct tcb *ptcb;

	if (NULL == sem)
	{
		return ERR_PEVENT_NULL;
	}

	if (EVENT_TYPE_SEM != sem->event_type)
	{
		return ERR_EVENT_TYPE;
	}

	ENTER_CRITICAL();

	if (0 != sem->event_grp)
	{
		prio = find_next_wait_task(sem->event_grp, sem->event_tbl);
		tcb_exit_wait(&sem->event_grp, sem->event_tbl, prio);
		ptcb = tcb_prio_tbl[prio];

		ptcb->stat &= ~(TASK_STAT_SEM);
		ptcb->delay = 0;

		debug("clear stat: %d\n", ptcb->stat);

		EXIT_CRITICAL();

		schedule();

		return (NO_ERR);
	}

	if (sem->count < 65535u)
	{
		++sem->count;

		EXIT_CRITICAL();

		return (NO_ERR);
	}

	// sem->count overflow
	return (ERR_SEM_OVF);


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
}
