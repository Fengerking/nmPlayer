/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Utility.h

	Contains:	Utility header file

	Written by: 	Thomas Liang

	Change History (most recent first):
	2011-03-09		Thomas Liang			Create file

*******************************************************************************/
#ifndef __BASEUTILITY_H__
#define __BASEUTILITY_H__

#include "stdlib.h"
#include "voType.h"
#include "voCMutex.h"

#define RTP_PAYLOAD_SIZE		1344

typedef struct _list_node
{
	struct _list_node *prev;
	struct _list_node *next;
}NODE;

typedef struct _mem_node
{
	NODE 		node;
	VO_BYTE 	buf[RTP_PAYLOAD_SIZE];
	VO_U32   	size;
	VO_U32		readSize;
}MEM_NODE;

typedef struct _miss_node
{
	NODE 		node;
	VO_U16		index;
}MISS_NODE;

typedef unsigned long int DWORD;
DWORD voTimeGetTime();

class CCMCCRTPSink
{
public:
	CCMCCRTPSink(int num);
	~CCMCCRTPSink();

	MEM_NODE * 	GetFreeNode();
	int			InsertFreeNode(MEM_NODE *node);

	MEM_NODE *	GetDataNode();
	int			InsertDataNode(MEM_NODE *node);

	MISS_NODE *	GetMissNode();
	int			InsertMissNode(MISS_NODE *node);
	MISS_NODE * FindAndGetMissNode(unsigned short index);
	
private:
	MEM_NODE *	AllocFreeNode();

private:
	NODE		m_freeList;
	NODE		m_dataList;
	NODE		m_missList;
	
	voCMutex	m_freeListLock;
	voCMutex	m_dataListLock;	
	voCMutex	m_missListLock;	
};
#endif //__BASEUTILITY_H__
