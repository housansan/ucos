#include "emos.h"


LIST_HEAD(free_head);
LIST_HEAD(use_head);


void mem_clr(void *ptr, int len)
{
	int i = 0;
	char *mptr = ptr;
	for (i = 0; i < len; ++i)
	{
		*mptr++ = 0;
	}
}


/*
 * 1. 将所有的 tcb 加入 tcb_free_head
 * 2. 初始化 tcb_head
 */
void tcb_head_init(void)
{
	int i = 0;

	tcb_free_head = &free_head;
	tcb_head = &use_head;

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

}


/*
 * 遍历 tcb_head 将delay--
 * 注意 tcb 的 stat 要是就绪状态
 */
void time_tick(void)
{
	struct tcb *ptcb;
	int dly;
	u8 prio;

	ENTER_CRITICAL();

	list_for_each_entry(ptcb, tcb_head, list)
	{
		dly = ptcb->delay;
		prio = ptcb->prio;
		if (dly > 0)
		{
			dly--;
			if (0 == dly)
			{
				tcb_enter_rdy(prio);
			}
			ptcb->delay = dly;
			debug("%s prio: %d dly: %d\n", __func__, prio, dly);
		}

	}

	EXIT_CRITICAL();
}


/*
 * 任务的切换
 * 1. 将当前 task 设置tsk_exit_rdy(要不然下一步又是找的当前的task)
 * 2. 找到最高优先级
 * 3. context switch
 */
void schedule(void)
{
	u8 prio;

	//ENTER_CRITICAL();

	prio = cur_tcb->prio;
	tcb_exit_rdy(prio);

	task_sw();

	debug("%s exit critical\n", __func__);

//	EXIT_CRITICAL();
}


void ctx_sw(void)
{
	u8 prio;
	ucontext_t *ucp;

	prio = find_next_task();
	cur_tcb = tcb_prio_tbl[prio];

	ucp = (ucontext_t *)cur_tcb->stk;

	setcontext(ucp);
}
