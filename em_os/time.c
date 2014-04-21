#include "emos.h"

static u32 jiffies;

/*
 * 目的:
 */
void time_dly(int ticks)
{
	if (ticks > 0)
	{
		ENTER_CRITICAL();

		tcb_exit_rdy(cur_tcb->prio);
		cur_tcb->delay = ticks;

		//list_add(&cur_tcb->list, tcb_dly_head);

		debug("prio: %d, ticks: %d\n", cur_tcb->prio, ticks);

		EXIT_CRITICAL();

		schedule();
	}
}


/*
 * ticks
 */
void time_get(u32 ticks)
{
	ENTER_CRITICAL();

	jiffies = ticks;

	EXIT_CRITICAL();
}


u32 time_set(void)
{
	u32 ticks;

	ENTER_CRITICAL();

	ticks = jiffies;

	EXIT_CRITICAL();

	return (ticks);
}
