#ifndef		_LIST_H
#define		_LIST_H


struct list_head
{
	struct list_head *prev;
	struct list_head *next;
};

#define offset(type, member)	((size_t)&(((type *)0)->member))


#define container_of(ptr, type, member)	({	\
		const typeof(((type *)0)->member) *__mptr = ptr;	\
		(type *)((char *)__mptr - offset(type, member)); })


#define list_entry(ptr, type, member)	\
	container_of(ptr, type, member)


#define LIST_HEAD_INIT(name)	{&(name), &(name)}
#define LIST_HEAD(name)	\
	struct list_head name = LIST_HEAD_INIT(name)


static inline void list_init(struct list_head *list)
{
	list->prev = list;
	list->next = list;
}


static inline void list_add(struct list_head *list, struct list_head *head)
{
	struct list_head *prev;
	struct list_head *next;

	prev = head;
	next = head->next;

	prev->next = list;
	list->prev = prev;

	list->next = next;
	next->prev = list;
}


static inline void list_del(struct list_head *entry)
{
	struct list_head *prev;
	struct list_head *next;

	prev = entry->prev;
	next = entry->next;

	prev->next = next;
	next->prev = prev;
}


static inline int list_empty(struct list_head *head)
{
	return (head->next == head);
}


#endif
