#include "CBaseStreamFileIndex.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CBaseStreamFileIndex::CBaseStreamFileIndex(CMemPool* pMemPool)
	: m_pHead(VO_NULL)
	, m_pTail(VO_NULL)
	, m_pMemPool(pMemPool)
{
}

CBaseStreamFileIndex::~CBaseStreamFileIndex()
{
	RemoveAll();
}

VO_VOID CBaseStreamFileIndex::Add(PBaseStreamMediaSampleIndexEntry pEntry)
{
	if(!m_pTail)
		m_pHead = m_pTail = pEntry;
	else
		m_pTail = m_pTail->next = pEntry;
}

VO_VOID	CBaseStreamFileIndex::AddEntries(PBaseStreamMediaSampleIndexEntry pHead, PBaseStreamMediaSampleIndexEntry pTail)
{
	if(!m_pTail)
		m_pHead = pHead;
	else
		m_pTail->next = pHead;

	m_pTail = pTail;
}

VO_VOID CBaseStreamFileIndex::RemoveAll()
{
	PBaseStreamMediaSampleIndexEntry pCur = m_pHead;
	PBaseStreamMediaSampleIndexEntry pTmp = VO_NULL;
	while(pCur)
	{
		pTmp = pCur;
		pCur = pCur->next;

		DeleteEntry(pTmp);
	}
	m_pHead = m_pTail = VO_NULL;
}

VO_VOID CBaseStreamFileIndex::RemoveUntil(PBaseStreamMediaSampleIndexEntry pEntry)
{
	if(pEntry)
	{
		PBaseStreamMediaSampleIndexEntry pCur = m_pHead;
		PBaseStreamMediaSampleIndexEntry pTmp = VO_NULL;
		while(pCur && pEntry != pCur)
		{
			pTmp = pCur;
			pCur = pCur->next;

			DeleteEntry(pTmp);
		}

		m_pHead = pEntry;
	}
}

VO_VOID CBaseStreamFileIndex::RemoveInclude(PBaseStreamMediaSampleIndexEntry pEntry)
{
	if(pEntry)
	{
		PBaseStreamMediaSampleIndexEntry pCur = m_pHead;
		PBaseStreamMediaSampleIndexEntry pTmp = VO_NULL;

		while(pCur && pEntry->next != pCur)
		{
			pTmp = pCur;
			pCur = pCur->next;

			DeleteEntry(pTmp);
		}

		m_pHead = pEntry->next;
		if(!m_pHead)
			m_pTail = VO_NULL;
	}
}

VO_VOID CBaseStreamFileIndex::RemoveFrom(PBaseStreamMediaSampleIndexEntry pEntry)
{
	if(pEntry == m_pHead)
		RemoveAll();
	else
	{
		PBaseStreamMediaSampleIndexEntry pCur = m_pHead;
		//find the previous entry as tail!!
		while(pCur)
		{
			if(pCur->next == pEntry)
			{
				m_pTail = pCur;
				break;
			}

			pCur = pCur->next;
		}

		//do remove!!
		pCur = pEntry;
		PBaseStreamMediaSampleIndexEntry pTmp = VO_NULL;
		while(pCur)
		{
			pTmp = pCur;
			pCur = pCur->next;

			DeleteEntry(pTmp);
		}
	}
}

VO_BOOL CBaseStreamFileIndex::IsNull()
{
	return m_pHead ? VO_FALSE : VO_TRUE;
}

VO_BOOL CBaseStreamFileIndex::GetEntry(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry, VO_BOOL* pbFrameDropped)
{
	if(!m_pHead)
		return VO_FALSE;

	*ppEntry = m_pHead;

	if(bVideo)		//process notify!!
	{
		PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
		while(nTimeStamp > pTmp->time_stamp ||(-1 == nTimeStamp))//((nTimeStamp > pTmp->time_stamp || nTimeStamp == -1) && pTmp->next)
		{
			if(!pTmp->next){
				break;
			}
		
			pTmp = pTmp->next;
			if(pTmp->IsKeyFrame())
			{
				*ppEntry = pTmp;
				if(pbFrameDropped)
					*pbFrameDropped = VO_TRUE;
			}
		}
	}

	return VO_TRUE;
}

VO_BOOL CBaseStreamFileIndex::GetTail(PBaseStreamMediaSampleIndexEntry*	pTail)
{
	//get the Tail element
	if(!pTail)
		return VO_FALSE;
	
	*pTail = m_pTail;
	return VO_TRUE;
}

VO_S32 CBaseStreamFileIndex::GetEntryByTime(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry)
{
	*ppEntry = VO_NULL;
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	while(pTmp && (pTmp->time_stamp <= nTimeStamp || !(*ppEntry)))
	{
		//audio or video key/last frame
		if( !bVideo || pTmp->IsKeyFrame() )
			*ppEntry = pTmp;
		if (!(*ppEntry) && !pTmp->next)
			*ppEntry = pTmp;

		pTmp = pTmp->next;
	}

	if(!(*ppEntry))
		return -1;

	if((*ppEntry)->time_stamp < nTimeStamp && !(*ppEntry)->next)
		return 1;

	return 0;
}

VO_BOOL CBaseStreamFileIndex::GetKeyFrameEntry(PBaseStreamMediaSampleIndexEntry* ppEntry)
{
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	while(pTmp)
	{
		if(pTmp->IsKeyFrame())
		{
			*ppEntry = pTmp;

			return VO_TRUE;
		}

		pTmp = pTmp->next;
	}

	return VO_FALSE;
}

VO_U32 CBaseStreamFileIndex::GetCurrEntryCount()
{
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	VO_U32 dwCount = 0;
	while(pTmp)
	{
		dwCount++;

		pTmp = pTmp->next;
	}

	return dwCount;
}

PBaseStreamMediaSampleIndexEntry CBaseStreamFileIndex::NewEntry(VO_U32 dwTimeStamp, VO_U32 dwPosInBuffer, VO_U32 dwSize)
{
	PBaseStreamMediaSampleIndexEntry pEntry = (PBaseStreamMediaSampleIndexEntry)m_pMemPool->alloc(sizeof(BaseStreamMediaSampleIndexEntry));
	if(!pEntry)
		return VO_NULL;

	pEntry->time_stamp = dwTimeStamp;
	pEntry->pos_in_buffer = dwPosInBuffer;
	pEntry->size = dwSize;
	pEntry->next = VO_NULL;

	return pEntry;
}

VO_VOID CBaseStreamFileIndex::DeleteEntry(PBaseStreamMediaSampleIndexEntry pEntry)
{
	m_pMemPool->free(pEntry, sizeof(BaseStreamMediaSampleIndexEntry));
}