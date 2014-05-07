#include "emos.h"


static void ctx_sw_sig_handler(int signo, siginfo_t *info, void *uc)
{
	cur_tcb->stk = uc;
	ctx_sw();
}


/*
 * 1. save current ucontext
 * 2. 
 */
static void time_tick_sig_handler(int signo, siginfo_t *info, void *uc)
{
	debug("enter %s\n", __func__);
	
	if ((((ucontext_t*)uc)->uc_mcontext.gregs[REG_EIP] >= (unsigned int)setcontext) &&
			(((ucontext_t*)uc)->uc_mcontext.gregs[REG_EIP] < (unsigned int)(setcontext + 110))) {
		fprintf(stderr, "sig timer: thread interrupted in setcontext\n");
		return;
	}
	if (((ucontext_t*)uc)->uc_mcontext.fpregs == 0) {
		fprintf(stderr, "sig timer: uc->uc_mcontext.fpregs == 0\n");
		return;
	}

	cur_tcb->stk = uc;

	tick_isr();
}


static void sig_init(void)
{
	struct sigaction act;
	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = time_tick_sig_handler;
	sigaction(SIGALRM, &act, NULL);

	act.sa_flags = SA_SIGINFO;
	sigemptyset(&act.sa_mask);
	act.sa_sigaction = ctx_sw_sig_handler;
	sigaction(SIGUSR1, &act, NULL);
}


static void alrm_init(void)
{
	useconds_t usec = 1000000;
	ualarm(usec/TICKS_PER_SEC, usec/TICKS_PER_SEC);
}


void linux_init(void)
{
	sig_init();
	alrm_init();
}
