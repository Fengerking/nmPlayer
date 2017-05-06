	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXTaskQueue.cpp

	Contains:	voCOMXTaskQueue class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-02-16		JBF			Create file

*******************************************************************************/
#include "voCOMXTaskQueue.h"

#include "voOMXMemory.h"
#ifdef _WIN32
#pragma warning(disable: 4996) //Use strxxx_s to replace strxxx
#endif

#define VO_TASKQUEUE_BLOCKCOUNT		16

voCOMXTaskQueue::voCOMXTaskQueue()
	: m_pFirst (NULL)
	, m_pLast (NULL)
	, m_uItems (0)
	, m_pBlock (NULL)
{
	if(m_pObjName)
		strcpy (m_pObjName, __FILE__);

	VO_TASK_BLOCK* pNewBlock = AllocBlock(VO_TASKQUEUE_BLOCKCOUNT);
	if(pNewBlock)
	{
		//add to block list
		m_pBlock = pNewBlock;

		//add to task queue
		pNewBlock->pTaskList[VO_TASKQUEUE_BLOCKCOUNT - 1].pNextItem = pNewBlock->pTaskList;
		m_pFirst = m_pLast = pNewBlock->pTaskList;
	}
}

voCOMXTaskQueue::~voCOMXTaskQueue()
{
	VO_TASK_BLOCK* pFree = NULL;
	while(m_pBlock != NULL)
	{
		pFree = m_pBlock;
		m_pBlock = m_pBlock->pNextBlock;

		FreeBlock(pFree);
	}
}

void voCOMXTaskQueue::Add (OMX_PTR pTask)
{
	voCOMXAutoLock lock (&m_tm);

	if (m_pLast->pTask != NULL)
		return;

	m_pLast->pTask = pTask;

	if(m_pLast->pNextItem == m_pFirst)	//queue is full
	{
		VO_TASK_BLOCK* pNewBlock = AllocBlock(VO_TASKQUEUE_BLOCKCOUNT);
		if(pNewBlock)
		{
			//add to block list
			VO_TASK_BLOCK* pLast = m_pBlock;
			while(pLast->pNextBlock)
				pLast = pLast->pNextBlock;
			pLast->pNextBlock = pNewBlock;

			//add to task queue
			m_pLast->pNextItem = pNewBlock->pTaskList;
			pNewBlock->pTaskList[VO_TASKQUEUE_BLOCKCOUNT - 1].pNextItem = m_pFirst;
		}
	}

	m_pLast = m_pLast->pNextItem;
	m_uItems++;
}

OMX_PTR voCOMXTaskQueue::Remove (void)
{
	voCOMXAutoLock lock (&m_tm);

	if (m_pFirst->pTask == NULL)
		return NULL;

	OMX_PTR pTask = m_pFirst->pTask;
	m_pFirst->pTask = NULL;
	m_pFirst = m_pFirst->pNextItem;
	m_uItems--;

	return pTask;
}

OMX_PTR voCOMXTaskQueue::First (void)
{
	voCOMXAutoLock lock (&m_tm);

	return m_pFirst->pTask;
}

OMX_U32 voCOMXTaskQueue::Count (void)
{
	voCOMXAutoLock lock (&m_tm);

	return m_uItems;
}

VO_TASK_BLOCK* voCOMXTaskQueue::AllocBlock(OMX_U32 uItems)
{
	VO_TASK_BLOCK* pBlock = (VO_TASK_BLOCK*)voOMXMemAlloc(sizeof(VO_TASK_BLOCK));
	if(pBlock == NULL)
		return NULL;

	voOMXMemSet(pBlock, 0, sizeof(VO_TASK_BLOCK));

	pBlock->pTaskList = (VO_TASK_ITEM*)voOMXMemAlloc(sizeof(VO_TASK_ITEM) * uItems);
	if(pBlock->pTaskList == NULL)
	{
		voOMXMemFree(pBlock);

		return NULL;
	}

	voOMXMemSet(pBlock->pTaskList, 0, sizeof(VO_TASK_ITEM) * uItems);

	for(OMX_U32 i = 0; i < uItems - 1; i++)
		pBlock->pTaskList[i].pNextItem = pBlock->pTaskList + i + 1;
	pBlock->pTaskList[uItems - 1].pNextItem = NULL;

	return pBlock;
}

void voCOMXTaskQueue::FreeBlock(VO_TASK_BLOCK* pBlock)
{
	voOMXMemFree(pBlock->pTaskList);
	voOMXMemFree(pBlock);
}
