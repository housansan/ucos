#include "emos.h"

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

		EXIT_CRITICAL();

		schedule();
	}
}
