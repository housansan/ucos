/*
 * TASK 相关函数
 */

#include "emos.h"



const u8 map_tbl[] = {
	1, 2, 4, 8, 16, 32, 64, 128,
};

const u8 unmap_tbl[] = {
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 
};


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


void tcb_exit_rdy(u8 prio)
{
	u8 x;
	u8 y;

	x = prio & 0x7;
	y = (prio >> 3) & 0x7;

	if (0 == (rdy_tbl[y] &= ~(map_tbl[x])))
	{
		rdy_grp &= ~(map_tbl[y]);
	}
}


void tcb_enter_rdy(u8 prio)
{
	u8 x;
	u8 y;

	x = prio & 0x7;
	y = (prio >> 3) & 0x7;

	rdy_grp |= map_tbl[y];
	rdy_tbl[y] |= map_tbl[x];
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
	list_add(&ptcb->list, tcb_head);

	tcb_enter_rdy(prio);
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

	tsk_thread_id = find_next_task();

	debug("%s %d\n", __func__, tsk_thread_id);

	cur_tcb = tcb_prio_tbl[tsk_thread_id];

	ucp = (ucontext_t *)cur_tcb->stk;

	setcontext(ucp);
}


int find_next_task(void)
{

	int i = 0;

	ENTER_CRITICAL();

#if TASK_CYCLE


	i = tsk_thread_id;
	i++;
	i %= TASK_NUM;


#elif TASK_PRIORITY

	//for (i = TASK_NUM - 1; i >= 0; --i)
	//{
		//if (0 != tcb_tbl[i].time_slice)
		//{
			//tcb_tbl[i].time_slice--;
			//break;
		//}
	//}

	//if (i < 0)
	//{
		//i = 0;
	//}

	// 选出最高优先级
	u8 y = unmap_tbl[rdy_grp];
	u8 x = unmap_tbl[rdy_tbl[y]];
	u8 prio = (y << 3) | x;
	i = prio;

	debug("%s prio: %d rdy_grp: %d rdy_tbl: %d\n", __func__, prio, rdy_grp, rdy_tbl[y]);

#endif

	EXIT_CRITICAL();


	return i;
}



int find_free_task(void)
{
	int i = 0;
	for (i = 0; i < TASK_NUM; ++i)
	{
		if (0 == tcb_tbl[i].used)
		{
			break;
		}
	}

	return i;
}
