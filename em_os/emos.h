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
