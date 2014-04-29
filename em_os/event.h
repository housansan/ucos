#ifndef _EVENT_H
#define _EVENT_H

#include "emos.h"


struct event
{
	u32 count; // 信号量 
	u8 event_grp;
	u8 event_tbl[EVENT_TBL_SIZE]; // 等待信号量的任务
	list_head wait_list; // 可以将等待的任务加入到这里
};



#endif
