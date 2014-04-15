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
 */
void time_tick(void)
{
	/* code */
}
