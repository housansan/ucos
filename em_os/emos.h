#ifndef _EM_OS_H
#define _EM_OS_H



#include "common.h"
#include "cfg.h"
#include "cpu.h"
#include "list.h"


/*
 * declare function
 */
#include "time.h"

#include "task.h"

#include "core.h"

#include "errno.h"

#include "event.h"

#include "pc.h"


//#define DEBUG

#ifdef DEBUG
//#define debug(fmt, arg...)	printf("DEBUG - " fmt, ##arg)
#define debug(fmt, arg...)	do {	\
		printf("%s--", __func__);	\
		printf(fmt, ##arg);	\
		}while(0)
#else
#define debug(fmt, arg...)
#endif



/*
 * ERROR NUM
 */
enum ERRNO
{
	SUCCESS = 0,

	// 没有 task 
	NONE,

	// 指定 priority 已经被占用
	USED,
};


#endif
