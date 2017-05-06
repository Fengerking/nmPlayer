/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Utility.cpp

	Contains:	Utility header file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-09		Thomas Liang			Create file

*******************************************************************************/
#include "Utility.h"

#ifdef WIN32
#include "Windows.h"
#elif defined LINUX
#include <sys/time.h>
#endif //WIN32

#include "voLog.h"

#define LOG_TAG 		"Utility"

DWORD voTimeGetTime(){
#ifdef WIN32
	return timeGetTime();
#else
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
#endif
}

void list_init(NODE *head)
{
	head->prev = head;
	head->next = head;
}

NODE *list_next(NODE *head)
{
	if(head->next == head)
		return NULL;

	NODE *tmp = head->next;
	
	tmp->next->prev = head;
	head->next=tmp->next;

	return tmp;
}

int list_add(NODE *head, NODE *node)
{
	if(!node)
		return -1;
	
	node->next = head;
	node->prev = head->prev;

	head->prev->next = node;
	head->prev = node;
	return 0;
}

int list_num(NODE *head)
{
	NODE * node=head;
	int num=0;
	
	if(!head)
		return 0;

	while(node->next!=head)
	{
		node=node->next;
		num++;
	}
	return num;
}

CCMCCRTPSink::CCMCCRTPSink(int num)
{	
	MEM_NODE *node = NULL;
	
	list_init(&m_freeList);
	list_init(&m_dataList);
	list_init(&m_missList);
	
	for(int i=0;i<num;i++)
	{
		node = AllocFreeNode();

		list_add(&m_freeList,(NODE *)node);
	}
}

CCMCCRTPSink::~CCMCCRTPSink()
{
	NODE *tmp = NULL;
	MEM_NODE *mem = NULL;
	MISS_NODE *miss = NULL;

	VOLOGI("%s: free remain %d\n", __FUNCTION__,list_num(&m_freeList));
	VOLOGI("%s: data remain %d\n", __FUNCTION__,list_num(&m_dataList));
	VOLOGI("%s: miss remain %d\n", __FUNCTION__,list_num(&m_missList));

	do
	{
		tmp = list_next(&m_missList);
		if(!tmp)
			break;
		miss = (MISS_NODE *)tmp;
		
		delete miss;
		
	}while(1);

	do
	{
		tmp = list_next(&m_dataList);
		if(!tmp)
			break;
		mem = (MEM_NODE *)tmp;
		
		delete mem;
		
	}while(1);

	do
	{
		tmp = list_next(&m_freeList);
		if(!tmp)
			break;
		mem = (MEM_NODE *)tmp;
		
		delete mem;
		
	}while(1);
}

MEM_NODE *	CCMCCRTPSink::AllocFreeNode()
{
	MEM_NODE *node = new MEM_NODE;

	node->node.next = NULL;
	node->node.prev = NULL;

	node->size = RTP_PAYLOAD_SIZE;
	node->readSize = 0;

	memset(node->buf, 0, RTP_PAYLOAD_SIZE);

	return node;
}

int	CCMCCRTPSink::InsertFreeNode(MEM_NODE *node)
{
	voCAutoLock lock(&m_freeListLock);
	VOLOGI("%s: remain %d", __FUNCTION__,list_num(&m_freeList));

	memset(node, 0, sizeof(MEM_NODE));
	node->size = RTP_PAYLOAD_SIZE;
	
	list_add(&m_freeList, (NODE *)node);
	return 0;
}

MEM_NODE * CCMCCRTPSink::GetFreeNode()
{
	voCAutoLock lock(&m_freeListLock);

	MEM_NODE *node = NULL;
	
	node = (MEM_NODE *)list_next(&m_freeList);

	if(!node)
	{
		VOLOGW("%s: free list is empty", __FUNCTION__);
		node = AllocFreeNode();	
	}
	
	return node;
}

int	CCMCCRTPSink::InsertDataNode(MEM_NODE *node)
{
	voCAutoLock lock(&m_dataListLock);

	return list_add(&m_dataList, (NODE *)node);
}

MEM_NODE * CCMCCRTPSink::GetDataNode()
{
	voCAutoLock lock(&m_dataListLock);

	return (MEM_NODE *)list_next(&m_dataList);
}

int	CCMCCRTPSink::InsertMissNode(MISS_NODE *node)
{
	voCAutoLock lock(&m_missListLock);

	return list_add(&m_missList, (NODE *)node);
}

MISS_NODE * CCMCCRTPSink::GetMissNode()
{
	voCAutoLock lock(&m_missListLock);

	return (MISS_NODE *)list_next(&m_missList);
}

MISS_NODE * CCMCCRTPSink::FindAndGetMissNode(unsigned short index)
{
	voCAutoLock lock(&m_missListLock);
	NODE *tmp = m_missList.next;

	/*while(tmp != &m_missList)
	{
		if(tmp->index == index)
		{
			tmp->prev->next = tmp->next;
			tmp->next->prev = tmp->prev;
			
			return tmp;
		}
		tmp = tmp->next;
	}*/
	return NULL;
}

