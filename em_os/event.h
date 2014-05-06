#ifndef _EVENT_H
#define _EVENT_H

#include "emos.h"


#ifdef EVENT_GLOBAL
#define EVENT_EXT
#else 
#define EVENT_EXT	extern
#endif


struct event
{
	u8 event_type;
	// 空闲时用来管理 event
	void *event_ptr;
	u32 count; // 信号量 
	u8 event_grp;
	u8 event_tbl[EVENT_TBL_SIZE]; // 等待信号量的任务
	struct list_head list; // 添加到 event_head
	struct list_head wait_list; // 可以将等待的任务加入到这里
};


EVENT_EXT struct event *event_free_head;
EVENT_EXT struct event event_tbl[MAX_EVENTS];


// event type
#define EVENT_TYPE_UNUSED		(0u)
#define EVENT_TYPE_MBOX			(1u)
#define EVENT_TYPE_Q			(2u)
#define EVENT_TYPE_SEM			(3u)	
#define EVENT_TYPE_MUTEX		(4u)
#define EVENT_TYPE_FLAG			(5u)


extern struct event *sem_create(u16 cnt);
extern u8 down_timeout(struct event *sem, u16 timeout);
extern u8 down(struct event *sem);
extern u8 up();


#endif
