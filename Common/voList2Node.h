#ifndef _VO_LIST2NODE_H
#define _VO_LIST2NODE_H

/************************************************************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2012				*
	*																		*
	************************************************************************/
/************************************************************************************************************************
	File:		voList2Node.h

	Contains:	voList2Node implementation file

	description: it is a list utility tool.
				 generally , we add/delete/access node to list,so we have to care the list operations for every kind of nodes.it may cause problem sometimes.
				 so why not "insert" list to node?
				 as we know, for a struct or class,the memory map of instance is some how like this:
				 Instance A
				 {
					member1;
					member2;
					.....
					membern;
				 }
				 so when we know the address of Instance A, then we can access the member of Instance A. for example:
				 we can access member2 of A like this:  A.member2
				 vice versa,when we know the address of member2, then we can know the address of Instance A, if we know 
				 the offset of the member2. 
				 the address of Instance A = the address of member2 - offset of member2

				 so in order to support list operation for general structure or class, we just need to add one member to it:
				 Instance A
				 {
					member1;
					member2;
					list_node;
					.....
					membern;
				 }
				 the list_node member is node of list, and it will be add/delete/access to list as normal list operation.

	
	usage: 1,define head node of list: list_node	m_headnode;

		   2,init head node : list_init_ptr(&m_headnode);

		   3,insert list_node to normal structure : 
		   struct testnode
		   {
				VO_S32 value1;
				VO_U32 value2;
				list_node  i_list;
		   };

		   4,add normal structure node to list
		   testnode * pnewbs = new testnode;
		   ...
		   vo_list_add_tail(&pnewbs->i_list,&m_headnode);

		   5,access object instance and related member
		   struct list_node *p = NULL;
		   testnode* pbs = NULL;
		   vo_list_for_each(p,&m_headnode)
		   {
				pbs = vo_list_entry(p,struct testnode,i_list);
				....
				pbs->value1;
		   }

		   6,access and delete object instance from list
		    struct list_node *p = NULL;
			struct list_node *pprev = NULL;
		    testnode* pbs = NULL;
			vo_list_for_each(p,&m_headnode)
			{
				pbs = vo_list_entry(p,struct testnode,i_list);
				pprev = p->next;
				vo_list_del(p);
				delete pbs;
				p = pprev->prev;
			}

	Written by:	Stony

	Change History (most recent first):
	2012-09-25		Stony			Create file
	2012-09-26		Stony			add usage demo

*******************************************************************************/

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#else
#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */
#endif

#ifndef	WIN32
#ifndef _WIN32_WCE
#define vo_offsetof_field(TYPE, FIELD) ((size_t) &((TYPE *)0)->FIELD)
#endif
#endif


#ifdef WIN32
#define vo_instance_addrof(ptr_field, type, field) ((type*)((PCHAR)(ptr_field) - (PCHAR)(&((type*)0)->field)))
#elif defined _WIN32_WCE
#define vo_instance_addrof(ptr_field, type, field) ((type*)((PCHAR)(ptr_field) - (PCHAR)(&((type*)0)->field)))
#else
#define vo_instance_addrof(ptr_field, type, field) ( { 			\
        const typeof( ((type *)0)->field ) *__mptr_field = (ptr_field); 	\
        (type *)( (char *)__mptr_field - vo_offsetof_field(type,field) ); } )
#endif

//the node for list operation 
struct list_node {
    struct list_node *next;
	struct list_node *prev;
};

#define list_init(name) { &(name), &(name) }

#define list_init_temp(name) \
    struct list_node name = list_init(name)

#define list_init_ptr(ptr) do { \
    (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)


static inline void __list_add(struct list_node *newobj,
        struct list_node *prev,
        struct list_node *next)
{
    next->prev = newobj;
    newobj->next = next;
    newobj->prev = prev;
    prev->next = newobj;
}


static inline void vo_list_add(struct list_node *newobj, struct list_node *head)
{
    __list_add(newobj, head, head->next);
}


static inline void vo_list_add_tail(struct list_node *newobj, struct list_node *head)
{
    __list_add(newobj, head->prev, head);
}

static inline void __list_del(struct list_node * prev, struct list_node * next)
{
    next->prev = prev;
    prev->next = next;
}

static inline void vo_list_del(struct list_node *entry)
{
    __list_del(entry->prev, entry->next);
    entry->next = (struct list_node*)((void *) 0x00000000);
    entry->prev = (struct list_node*)((void *) 0x00000000);
}

static inline void vo_list_del_init(struct list_node *entry)
{
    __list_del(entry->prev, entry->next);
    list_init_ptr(entry);
}

static inline void vo_list_move(struct list_node *list, struct list_node *head)
{
    __list_del(list->prev, list->next);
    vo_list_add(list, head);
}

static inline void vo_list_move_tail(struct list_node *list,
        struct list_node *head)
{
    __list_del(list->prev, list->next);
    vo_list_add_tail(list, head);
}

static inline int vo_list_empty(const struct list_node *head)
{
    return head->next == head;
}

static inline int vo_list_empty_careful(const struct list_node *head)
{
    struct list_node *next = head->next;
    return (next == head) && (next == head->prev);
}

static inline void __list_splice(struct list_node *list, struct list_node *head)
{
    struct list_node *first = list->next;
    struct list_node *last = list->prev;
    struct list_node *at = head->next;

    first->prev = head;
    head->next = first;

    last->next = at;
    at->prev = last;
}


//vo_list_splice: join two lists
static inline void vo_list_splice(struct list_node *list, struct list_node *head)
{
    if (!vo_list_empty(list))
        __list_splice(list, head);
}


//vo_list_splice_init:join two lists and reinitialise the emptied list.
static inline void vo_list_splice_init(struct list_node *list,
        struct list_node *head)
{
    if (!vo_list_empty(list)) {
        __list_splice(list, head);
        list_init_ptr(list);
    }
}

//get the instance address by the field address and its offset
#define vo_list_entry(ptr_field, type, field) vo_instance_addrof(ptr_field, type, field)

//retrieve through the list in sequence
#define vo_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); \
            pos = pos->next)

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _VONAMESPACE */


#endif
