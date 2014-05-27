#ifndef _ERRNO_H
#define _ERRNO_H


#define NO_ERR		0

#define ERR_TASK_NO_EXIST			(1u)
#define TASK_SUSPEND_IDLE			(2u)
#define PRIO_INVALID				(3u)
#define TASK_NO_SUSPEND				(4u)


#define ERR_PRIO_USED				(5u)
#define ERR_NO_MORE_TCB				(6u)
#define ERR_TASK_HOLD				(7u)
#define ERR_CHANGE_IDLE_PRIO		(8u)

#define ERR_SEM_OVF					(9u)
#define ERR_PEVENT_NULL				(10u)
#define ERR_EVENT_TYPE				(11u)
#define ERR_TIME_OUT				(12u)


//static char *str_errno[] = 
//{
//	//[] = "Priory used";
//};


// TODO: 定义将 errno 转化成 string
//static inline char *errno_to_str(u8 err)
//{
	//return str_errno[err];
//}


#endif
