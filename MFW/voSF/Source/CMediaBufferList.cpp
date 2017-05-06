/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CMediaBufferList.cpp

Contains:	CMediaBufferList class file

Written by:	

Change History (most recent first):


*******************************************************************************/
#include <malloc.h>
#include <stdio.h>

#include "CMediaBufferList.h"
#include "CBaseMediaSource.h"

#define LOG_TAG "CMediaBufferList"
#include "voLog.h"


CMediaBufferList::CMediaBufferList(VOSF_MEDIABUFFERTYPE * pMediaBufferOP, VOSF_METADATATYPE * pMetaDataOP)
: m_pMediaBufferOP(pMediaBufferOP)
, m_pMetaDataOP(pMetaDataOP)
, m_pMemanger(new CMemManger())
, m_pHead(NULL)
, m_pCurr(NULL)
, m_pTail(NULL)
, m_nSyncCnt(0)
, m_bEOS(false)
, mnDropCount(0)
{

}

CMediaBufferList::~CMediaBufferList()
{
	releaseList();
	if(m_pMemanger)
	{
		delete m_pMemanger;
		m_pMemanger = NULL;
	}
}

bool CMediaBufferList::addBuffer(void* pBuffer)
{
	BufferItem* pItem;
	void* pMeta = NULL;

	int	  lBufferLen = m_pMediaBufferOP->range_length(pBuffer);
	pMeta = m_pMediaBufferOP->meta_data(pBuffer);
	pItem = allocItem(lBufferLen);

	//VOLOGI("Add pItem = %p" , pItem);

	m_pMetaDataOP->findInt32(pMeta, kKeyIsSyncFrame, &pItem->SyncFrame);
	m_pMetaDataOP->findInt64(pMeta, kKeyTime, &pItem->inputBuffer.Time);	
	pItem->inputBuffer.Length = lBufferLen;

	if(pItem->inputBuffer.Buffer == NULL)
	{
		VOLOGE("Fail! No Memory");
		free(pItem);
		return false;
	}

	if(pItem->SyncFrame==1)
	{
		m_nSyncCnt++;
	}

	memcpy(pItem->inputBuffer.Buffer, (char *)m_pMediaBufferOP->data (pBuffer) + m_pMediaBufferOP->range_offset (pBuffer), pItem->inputBuffer.Length);

	if (m_pHead == NULL)
	{
		m_pHead = m_pCurr = m_pTail = pItem;
	}
	else
	{
		m_pTail->pNext = pItem;
		m_pTail = pItem;
	}

	return true;
}

bool CMediaBufferList::getSample(long long llTime, VO_CODECBUFFER* pBuffer)
{
	BufferItem* pNextSync = getNextSyncItem();

	//VOLOGI("pNextSync = %p " , pNextSync);
	BufferItem* pItem;
	if(pNextSync == NULL || llTime < pNextSync->inputBuffer.Time)
	{
		pItem = m_pCurr;
		if(m_pCurr)
			m_pCurr = m_pCurr->pNext;
	}
	else
	{
		pItem = pNextSync;
		m_pCurr = pNextSync->pNext;
	}

	if(pItem)
	{
		pBuffer->Buffer	= pItem->inputBuffer.Buffer;
		pBuffer->Length	= pItem->inputBuffer.Length;
		pBuffer->Time	= pItem->inputBuffer.Time;
		return true;
	}
	else
		return false;
}

bool CMediaBufferList::isBufferReady()
{
	return (m_nSyncCnt>= 1 || m_bEOS);
}

void CMediaBufferList::arrangeList()
{
	BufferItem* pItem;
	int ii = 0;
	for (pItem = m_pHead; pItem != m_pCurr;)
	{
		if(pItem == NULL)
			break;
		if(m_pHead->SyncFrame == 1)
		{
			m_nSyncCnt--;
		}

		m_pMemanger->returnMem(&pItem->memUnit);
		pItem = pItem->pNext;
		m_pHead = pItem;

		ii ++;
	}

	if(ii > 0)
		mnDropCount = ii - 1;
}

void CMediaBufferList::releaseList()
{
	//BufferItem* pItem;

	m_pMemanger->resetMem();
	m_pHead = NULL;
	m_pCurr = NULL;
	m_pTail = NULL;
	m_nSyncCnt = 0;
	m_bEOS	   = false;

}

CMediaBufferList::BufferItem*	CMediaBufferList::getNextSyncItem()
{
	if(m_nSyncCnt == 0)
		return NULL;

	BufferItem* pItem;
	for (pItem = m_pCurr; pItem != NULL; pItem = pItem->pNext)
	{
		//VOLOGI("pItem = %p " , pItem);
		if(pItem != NULL && pItem->SyncFrame == 1)
			return pItem;
	}
	return NULL;
}

CMediaBufferList::BufferItem*	CMediaBufferList::allocItem(int nSize)
{
	BufferItem* pItem;
	MemUnit memUnit;
	pItem = (BufferItem*)m_pMemanger->getMem(sizeof(BufferItem)+nSize, &memUnit);
	memset(pItem,0,sizeof(BufferItem));
	pItem->memUnit.nSize = memUnit.nSize;
	pItem->memUnit.pTrunk= memUnit.pTrunk;
	pItem->inputBuffer.Buffer = (VO_PBYTE)pItem + sizeof(BufferItem);
	pItem->pNext = NULL;

	return pItem;
}


//////////////////////////////////////////////////////////////////////////
#define MEM_TRUNK_SIZE 512000
#define ALLOC_TRUNK(pT) \
{ \
	pT = (MemTrunk*)malloc(sizeof(MemTrunk)+MEM_TRUNK_SIZE); \
	memset(pT, 0, sizeof(MemTrunk)); \
	pT->pMem = (char *)pT + sizeof(MemTrunk);\
}

CMemManger::CMemManger()
: m_pHeadTrunk(NULL)
, m_pTailTrunk(NULL)
, m_pCurrTrunk(NULL)
{
	MemTrunk* pTrunk;
	for(int i = 0 ; i < 3 ; i++)
	{
		ALLOC_TRUNK(pTrunk)
		if(m_pHeadTrunk == NULL)
		{
			m_pCurrTrunk = m_pHeadTrunk = pTrunk;
			m_pTailTrunk = m_pHeadTrunk;
		}
		else
		{
			pTrunk->pPrewTrunk = m_pTailTrunk;
			m_pTailTrunk->pNextTrunk = pTrunk;
			m_pTailTrunk = pTrunk;
		}
	}
	
}

CMemManger::~CMemManger()
{
	release();
}

void*	CMemManger::getMem(int nSize, MemUnit* pMemUnit)
{
	//nSize should 16-bytes aligned 
	int aligned_Size = ((nSize+15)>>4)<<4;
	if(NULL == m_pHeadTrunk)
	{
		ALLOC_TRUNK(m_pHeadTrunk)

		m_pCurrTrunk = m_pHeadTrunk;
		m_pTailTrunk = m_pHeadTrunk;
	}
	if(m_pCurrTrunk&&m_pCurrTrunk->nOffset+aligned_Size > MEM_TRUNK_SIZE)
	{
		//goto next memory trunk
		m_pCurrTrunk = m_pCurrTrunk->pNextTrunk;
	}
	if(m_pCurrTrunk == NULL)
	{
		//allocate new trunk and attach to the tail
		MemTrunk* pTrunk;
		ALLOC_TRUNK(pTrunk)

		pTrunk->pPrewTrunk = m_pTailTrunk;
		m_pTailTrunk->pNextTrunk = pTrunk;

		m_pTailTrunk = pTrunk;
		m_pCurrTrunk = m_pTailTrunk;
	}
	void* pMem = m_pCurrTrunk->pMem+ m_pCurrTrunk->nOffset;
	m_pCurrTrunk->nOffset += aligned_Size;
	//initialize memory unit info
	pMemUnit->pTrunk = m_pCurrTrunk;
	pMemUnit->nSize  = aligned_Size;

	return pMem;
}

void CMemManger::returnMem(MemUnit* pMemUnit)
{
	if(NULL == pMemUnit)
		return;

	MemTrunk*  pTrunk = pMemUnit->pTrunk;
	pTrunk->nRecyleSize += pMemUnit->nSize;
	if(pTrunk->nRecyleSize >= pTrunk->nOffset)
	{
		if(pTrunk == m_pHeadTrunk)
		{
			if(m_pCurrTrunk == m_pHeadTrunk)
			{
				//VOLOGI("m_pCurrTrunk == m_pHeadTrunk");
				m_pHeadTrunk->nOffset = 0;
				m_pHeadTrunk->nRecyleSize = 0;
			}
			else
			{
				//VOLOGI("Should here pTrunk == m_pHeadTrunk");
				MemTrunk*  pTempTrunk = m_pHeadTrunk;
				m_pHeadTrunk = m_pHeadTrunk->pNextTrunk;
				m_pHeadTrunk->pPrewTrunk = NULL;

				m_pTailTrunk->pNextTrunk = pTempTrunk;
				pTempTrunk->pPrewTrunk = m_pTailTrunk;
				m_pTailTrunk = pTempTrunk;
				m_pTailTrunk->pNextTrunk = NULL;
				m_pTailTrunk->nOffset = m_pTailTrunk->nRecyleSize = 0;
			}
		}
		else if(pTrunk == m_pCurrTrunk || pTrunk == m_pTailTrunk)
		{
			//VOLOGI("pTrunk == m_pCurrTrunk || pTrunk == m_pTailTrunk");
			m_pTailTrunk->nOffset = m_pTailTrunk->nRecyleSize = 0;
		}
		else
		{
			//VOLOGI("Shouldn't here !!!");
			pTrunk->pPrewTrunk->pNextTrunk = pTrunk->pNextTrunk;
			pTrunk->pNextTrunk->pPrewTrunk = pTrunk->pPrewTrunk;

			m_pTailTrunk->pNextTrunk = pTrunk;
			pTrunk->pPrewTrunk = m_pTailTrunk;
			pTrunk->pNextTrunk = NULL;
		}
	}

	//if(pMemUnit->pTrunk != m_pHeadTrunk)
	//{
	//	VOLOGE("Memory manage error, pMemUnit->pTrunk = %p  m_pHeadTrunk = %p should not be here" , pMemUnit->pTrunk , m_pHeadTrunk);
	//	return;
	//}

	//m_pHeadTrunk->nRecyleSize += pMemUnit->nSize;

	////VOLOGI("nRecyleSize = %d  offset = %d" , m_pHeadTrunk->nRecyleSize , m_pHeadTrunk->nOffset);
	//if(m_pHeadTrunk->nRecyleSize >= m_pHeadTrunk->nOffset &&
	//	((m_pHeadTrunk->pNextTrunk&&m_pHeadTrunk != m_pCurrTrunk)||m_pHeadTrunk->pNextTrunk==NULL))
	//{
	//	//VOLOGI("reuse buffer %p ", m_pHeadTrunk);
	//	//reuse memory trunk, move to the tail
	//	MemTrunk* pTrunk = m_pHeadTrunk;
	//	
	//	if(m_pHeadTrunk->pNextTrunk != NULL)
	//	{
	//		m_pHeadTrunk = m_pHeadTrunk->pNextTrunk;
	//		pTrunk->pNextTrunk = NULL;
	//		m_pTailTrunk->pNextTrunk = pTrunk;
	//		m_pTailTrunk = pTrunk;
	//		pTrunk->nOffset = 0;
	//		pTrunk->nRecyleSize = 0;
	//	}
	//	else
	//	{
	//		pTrunk->nOffset = 0;
	//		pTrunk->nRecyleSize = 0;
	//	}
	//
	//}
}

void CMemManger::resetMem()
{
	//VOLOGI("resetMem   m_pHeadTrunk = %p " , m_pHeadTrunk);
	for (MemTrunk* pTrunk = m_pHeadTrunk; pTrunk != NULL; pTrunk = pTrunk->pNextTrunk)
	{
		pTrunk->nOffset = 0;
		pTrunk->nRecyleSize = 0;
	}

	m_pCurrTrunk = m_pHeadTrunk;	
}

void CMemManger::release()
{
	while (m_pHeadTrunk)
	{
		MemTrunk* pTrunk = m_pHeadTrunk->pNextTrunk;

		free(m_pHeadTrunk);
		m_pHeadTrunk = pTrunk;
	}
}