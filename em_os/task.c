/*
 * TASK 相关函数
 */

#define TASK_GLOBAL

#include "emos.h"



/*
 * return struct tcb *
 */
static struct tcb *find_free_tcb(void)
{
	struct tcb *ptcb = NULL;
	struct list_head *list;

	if (list_empty(tcb_free_head))
	{
		goto done;
	}

	list = tcb_free_head->next;
	// 要不要放在这里 ?
	// 就放在这里
	list_del(list);
	ptcb = list_entry(list, struct tcb, list);

done:
	return ptcb;
}


static u8 *task_stk_init(tsk_fn func, u8 *ptos)
{
	ucontext_t uc;
	u8 *stk;
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


static struct tcb *tcb_get_init(int prio, u8 *ptos)
{
	struct tcb *ptcb = NULL;
	ptcb = find_free_tcb();
	if (NULL == ptcb)
	{
		goto no_more_tcb;
	}

	ptcb->stk = ptos;
	ptcb->prio = prio;
	ptcb->delay = 0;
	ptcb->time_slice = prio + 10;

	return ptcb;

no_more_tcb:
	return NULL;
}





/*
 * 加入 tcb_head 的管理
 */
static void tcb_manage(struct tcb *ptcb)
{
	u8 prio = ptcb->prio;
	// 将高优先级
	if (cur_tcb->prio <= ptcb->prio)
	{
		
	}

	tcb_enter_rdy(prio);
	list_add(&ptcb->list, tcb_rdy_head);
}

/*
 * @func: task
 * @prio: priority
 * @ptos: pointer to stack top
 *
 * 返回错误码
 *
 */
int task_create(tsk_fn func, int prio, u8 *ptos)
{
	struct tcb *ptcb;
	u8 *pstk;
	
	if (tcb_prio_tbl[prio])
	{
		goto tcb_used;
	}

	tcb_prio_tbl[prio] = HOLD;

	// 与machine相关
	pstk = task_stk_init(func, ptos);

	ptcb = tcb_get_init(prio, pstk);
	// disable interrupt

	if (NULL == ptcb)
	{
		// ...
	}
	
	tcb_prio_tbl[prio] = ptcb;

	// 添加到 tcb_head 进行管理
	tcb_manage(ptcb);

	return 0;

tcb_used:
	return -1;
}


//int task_create(tsk_fn func, u8 *ptos)
//{
	//struct tcb *ptcb;
	//int prio;

	//prio= find_free_task();

	//tcb_tbl[prio].used = 1;

	//// 运行的次数
	//tcb_tbl[prio].time_slice = prio + 10;
	//tcb_tbl[prio].prio = prio;

	////printf("prio: %d %d\n", prio, tcb_tbl[prio].time_slice);

	//ptcb = &tcb_tbl[prio];

	//ptcb->stk = task_stk_init(func, ptos);

	//return 0;
//}


void start_task(void)
{
	ucontext_t *ucp;

	cur_prio = find_next_task();

	debug("%s %d\n", __func__, cur_prio);

	cur_tcb = tcb_prio_tbl[cur_prio];

	high_rdy = cur_prio;
	tcb_high_rdy = cur_tcb;

	ucp = (ucontext_t *)cur_tcb->stk;

	setcontext(ucp);
}


u8 task_suspend(u8 prio)
{
	struct tcb *ptcb;
	u8 self;

	/*
	 * prio == TASK_IDLE_PRIO
	 * 不允许 suspend idle task
	 */
	if (prio == TASK_IDLE_PRIO)
	{
		return TASK_SUSPEND_IDLE;
	}

	/*
	 * invalid prio
	 */
	if (prio > LOWEST_PRIO)
	{
		if (PRIO_SELF != prio)
		{
			return PRIO_INVALID;
		}
	}


	ENTER_CRITICAL();

	if (prio == PRIO_SELF)
	{
		prio = cur_tcb->prio;
		self = 1;
	}
	else if (prio == cur_tcb->prio)
	{
		self = 1;
	}
	else
	{
		self = 0;
	}

	ptcb = tcb_prio_tbl[prio];

	if (NULL == ptcb)
	{
		goto task_no_exist;
	}

	if (HOLD == ptcb)
	{
		goto task_no_exist;
	}

	ptcb->stat |= TASK_SUSPEND;

	tcb_exit_rdy(prio);

	EXIT_CRITICAL();


	if (self)
	{
		schedule();
	}

	return NO_ERR;

task_no_exist:
	EXIT_CRITICAL();
	return TASK_NO_EXIST;
}


u8 task_resume(u8 prio)
{
	struct tcb *ptcb;

	if (prio > LOWEST_PRIO)
	{
		return PRIO_INVALID;
	}

	ENTER_CRITICAL();

	ptcb = tcb_prio_tbl[prio];
	if (NULL == ptcb)
	{
		goto task_no_exist;
	}
	else if (HOLD == ptcb)
	{
		goto task_no_exist;
	}

	/*
	 * task 必须要有 TASK_SUSPEND
	 */
	if (TASK_RDY != (ptcb->stat & TASK_SUSPEND))
	{
		ptcb->stat &= ~TASK_SUSPEND;
		if (TASK_RDY == ptcb->stat)
		{
			if (ptcb->delay == 0)
			{
				tcb_enter_rdy(prio);
				EXIT_CRITICAL();
				schedule();
			}
			else 
			{
				EXIT_CRITICAL();
			}
		}
		else 
		{
			EXIT_CRITICAL();
		}

		return NO_ERR;
	}
	

	EXIT_CRITICAL();
	return TASK_NO_SUSPEND;

task_no_exist:

	EXIT_CRITICAL();
	return TASK_NO_EXIST;
}


/*
 * 查询 task
 */
u8 task_query(u8 prio, struct tcb *p_task_data)
{
	struct tcb *ptcb;

	if (prio > LOWEST_PRIO)
	{
		if (prio != PRIO_SELF)
		{
			return PRIO_INVALID;
		}
	}

	if (NULL == p_task_data)
	{
//		return ERR_PDATA_NULL;
	}

	ENTER_CRITICAL();

	if (prio == PRIO_SELF)
	{
		prio = cur_tcb->prio;
		ptcb = tcb_prio_tbl[prio];
	}

	ptcb = tcb_prio_tbl[prio];

	if (NULL == ptcb || HOLD == ptcb)
	{
		goto task_no_exist;
	}

	memcpy(p_task_data, ptcb, sizeof(struct tcb));

	EXIT_CRITICAL();
	
	return NO_ERR;

task_no_exist:

	EXIT_CRITICAL();
	return TASK_NO_EXIST;

}
