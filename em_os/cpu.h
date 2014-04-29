#ifndef _CPU_H
#define _CPU_H


#include "common.h"

#define ENTER_CRITICAL()	{sigset_t set;	\
								sigemptyset(&set);	\
								sigaddset(&set, SIGALRM);	\
								sigaddset(&set, SIGIO);	\
								sigprocmask(SIG_SETMASK, &set, &cpu_sr);	\
							}

#define EXIT_CRITICAL()		{sigprocmask(SIG_SETMASK, &cpu_sr, NULL);}


#define task_sw()	{kill(getpid(), SIGUSR1);}


extern void linux_init(void);


#endif
