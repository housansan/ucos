

#define EVENT_GLOBAL
#define CORE_GLOBAL

#include "emos.h"


LIST_HEAD(free_head);
LIST_HEAD(rdy_head);
LIST_HEAD(dly_head);


static const u8 map_tbl[] = {
	1, 2, 4, 8, 16, 32, 64, 128,
};

static const u8 unmap_tbl[] = {
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


void mem_clr(void *ptr, u32 len)
{
	u32 i = 0;
	char *mptr = ptr;
	for (i = 0; i < len; ++i)
	{
		*mptr++ = 0;
	}
}


void mem_cpy(void *dest, void *src, u32 len)
{
	//
}


/*
 * 1. 将所有的 tcb 加入 tcb_free_head
 * 2. 初始化 tcb_head
 */
void tcb_head_init(void)
{
	int i = 0;

	tcb_free_head = &free_head;
	tcb_rdy_head = &rdy_head;
	tcb_dly_head = &dly_head;


	// tcb_tbl[i] = 0;
	mem_clr(&tcb_tbl[0], sizeof(tcb_tbl));

	// tcb_prio_tbl[i] == NULL;
	mem_clr(&tcb_prio_tbl[0], sizeof(tcb_prio_tbl));

	// n = TASK_NUM - 1
	// 确保 空闲任务 不在 tcb_free_head
	for (i = 0; i < TASK_NUM; ++i)
	{
		list_add(&tcb_tbl[i].list, tcb_free_head);
	}

	event_free_head = &event_tbl[0];
	for (i = 0; i < MAX_EVENTS - 1; ++i)
	{
		event_tbl[i].event_ptr = &event_tbl[i+1];
	}
	event_tbl[MAX_EVENTS - 1].event_ptr = NULL;

}


// 空闲 task
static void task_idle(void)
{
	while(1) 
	{
		ENTER_CRITICAL();

		idle_ctr++;

		EXIT_CRITICAL();

//		task_idle_hook();
	}
}


static void task_idle_init(void)
{
	task_create(task_idle, LOWEST_PRIO, &task_idle_stk[TASK_IDLE_STK_SIZE - 1]);
}


/*
 * 统计 task
 * Q: 怎么统计任务
 */
static void task_stat_init(void)
{
	/* code */
}


/*
 * TODO: 建立空闲任务
 */
void os_init(void)
{
	tcb_head_init();
	task_idle_init();
}


/*
 * 遍历 tcb_head 将delay--
 * 注意 tcb 的 stat 要是就绪状态
 */
void time_tick(void)
{
	struct tcb *ptcb;
	u8 prio;

	//ENTER_CRITICAL();
	

	/*
	 * FIXME: 
	 * 1. 会把空闲idle_task也的delay
	 * 2. task suspend 将不能进入 rdy
	 */
	list_for_each_entry(ptcb, tcb_rdy_head, list)
	{
		prio = ptcb->prio;
		if (ptcb->delay > 0)
		{
			if (0 == --ptcb->delay)
			{
				/*
				 * Q: why 不直接 TASK_STAT_RDY == ptcb->stat
				 * A: 可能有其他 status
				 *
				 */
				if (TASK_STAT_RDY == (ptcb->stat & TASK_STAT_SUSPEND))
				{
					tcb_enter_rdy(prio);
				}
			}
			debug("%s prio: %d dly: %d\n", __func__, prio, ptcb->delay);
		}

	}

	jiffies++;
	//EXIT_CRITICAL();
}


void int_ctx_sw(void)
{
	high_rdy = find_next_rdy_task();
	if (high_rdy == cur_prio)
	{
		return;
	}

	tcb_high_rdy = tcb_prio_tbl[high_rdy];

	ctx_sw();
}



void tick_isr(void)
{
	time_tick();
	int_ctx_sw();
}


/*
 * 任务的切换
 * 1. FIXME 将当前 task 设置tsk_exit_rdy(要不然下一步又是找的当前的task)
 *		找到最高优先级与当前比较如果一样则不 schedule 否则 schedule
 * 2. 找到最高优先级
 * 3. context switch
 */
void schedule(void)
{
	ENTER_CRITICAL();

	/*
	 * 调度不一定要把自己exit_rdy
	 */
	high_rdy = find_next_rdy_task();
	/*
	 * why 比较 high_rdy != cur_prio
	 * 而不比较 tcb_high_rdy != cur_tcb
	 * 这是因为在一些 8bit 和 16bit 比较慢
	 */
	if (high_rdy != cur_prio)
	{
		tcb_high_rdy = tcb_prio_tbl[high_rdy];

		task_sw();

		EXIT_CRITICAL();
	}
	//prio = cur_tcb->prio;
	//tcb_exit_rdy(prio);

	//EXIT_CRITICAL();

	//task_sw();

	debug("%s exit critical\n", __func__);
}


void ctx_sw(void)
{
	ucontext_t *ucp;

	cur_prio = high_rdy;
	cur_tcb = tcb_high_rdy;

	ucp = (ucontext_t *)cur_tcb->stk;

	setcontext(ucp);
}


static inline void tcb_exit_tbl(u8 *grp, u8 tbl[], u8 prio)
{
	u8 x;
	u8 y;

	x = prio & 0x7;
	y = (prio >> 3) & 0x7;


	if (0 == (tbl[y] &= ~map_tbl[x]))
	{
		*grp &= ~map_tbl[y];
	}
}


static inline void tcb_enter_tbl(u8 *grp, u8 tbl[], u8 prio)
{
	u8 x;
	u8 y;

	x = prio & 0x7;
	y = (prio >> 3) & 0x7;

	*grp |= map_tbl[y];
	tbl[y] |= map_tbl[x];
}


void tcb_exit_rdy(u8 prio)
{
	tcb_exit_tbl(&rdy_grp, rdy_tbl, prio);
}


void tcb_enter_rdy(u8 prio)
{
	tcb_enter_tbl(&rdy_grp, rdy_tbl, prio);
}


void tcb_exit_wait(u8 *grp, u8 tbl[], u8 prio)
{
	//prio = find_next_wait_task(grp, tbl);
	tcb_enter_rdy(prio);
	tcb_exit_tbl(grp, tbl, prio);
}


void tcb_enter_wait(u8 *grp, u8 tbl[], u8 prio)
{
	tcb_exit_rdy(prio);
	tcb_enter_tbl(grp, tbl, prio);
}



/*
 * 需要调用者使用 ENTER_CRITICAL EXIT_CRITICAL
 * TODO: 使用 find_next_task
 * find_next_rdy_task
 * find_next_wait_task
 */
//int find_next_task(void)
//{

	//int i = 0;

////	ENTER_CRITICAL();

//#if TASK_CYCLE


	//i = cur_prio;
	//i++;
	//i %= TASK_NUM;


//#elif TASK_PRIORITY

	////for (i = TASK_NUM - 1; i >= 0; --i)
	////{
		////if (0 != tcb_tbl[i].time_slice)
		////{
			////tcb_tbl[i].time_slice--;
			////break;
		////}
	////}

	////if (i < 0)
	////{
		////i = 0;
	////}

	//// 选出最高优先级
	//u8 y = unmap_tbl[rdy_grp];
	//u8 x = unmap_tbl[rdy_tbl[y]];
	//u8 prio = (y << 3) | x;
	//i = prio;

	//debug("%s prio: %d rdy_grp: %d rdy_tbl[%d]: %d\n", __func__, prio, rdy_grp, y, rdy_tbl[y]);

//#endif

////	EXIT_CRITICAL();


	//return i;
//}


static u8 find_next_task(u8 grp, u8 tbl[])
{
	u8 y = unmap_tbl[grp];
	u8 x = unmap_tbl[tbl[y]];
	u8 prio = (y << 3) | x;

	return prio;
}



u8 find_next_wait_task(u8 grp, u8 tbl[])
{
	return (find_next_task(grp, tbl));
}


int find_next_rdy_task(void)
{
	return (find_next_task(rdy_grp, rdy_tbl));
}



u16 version(void)
{
	return (OS_VERSION);
}
