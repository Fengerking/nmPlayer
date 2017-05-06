#include "CCacheBuffer.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CacheBufferList::CacheBufferList(VO_U32 nMaxItemCount)
:m_pCacheBufHead(NULL)
,m_pCacheBufTail(NULL)
,m_pTmpCacheItem(NULL)
,m_nMaxItemCount(nMaxItemCount)
{

}

CacheBufferList::~CacheBufferList()
{
	RemoveAll();
}

VO_VOID CacheBufferList::RemoveAll()
{
	pCacheBufferItem pCur = m_pCacheBufHead;
	pCacheBufferItem pTmp = NULL;
	while(pCur)
	{
		pTmp = pCur;
		pCur = pCur->pNext;
		VOLOGE("remove pTmp");
		DeleteEntry(pTmp);
	}
	m_pCacheBufHead = m_pCacheBufTail = NULL;
	if (m_pTmpCacheItem)
	{
		VOLOGE("remove m_pTmpCacheItem");
		DeleteEntry(m_pTmpCacheItem);
		m_pTmpCacheItem = NULL;
	}
	VOLOGE("remove all ok");
}

VO_VOID CacheBufferList::CopyItem(VO_SINK_SAMPLE * stDstItem,VO_SINK_SAMPLE * SrcItem)
{
	if (!stDstItem || !SrcItem)
	{
		return;
	}
	stDstItem->Size = SrcItem->Size;
	stDstItem->Time = SrcItem->Time;
	stDstItem->DTS  = SrcItem->Time;///<take the PTS as DTS 
	stDstItem->Duration = SrcItem->Duration;
	stDstItem->nAV = SrcItem->nAV;

	if (stDstItem->Buffer && SrcItem->Buffer)
	{
		memcpy(stDstItem->Buffer,SrcItem->Buffer,SrcItem->Size);
	}
}
pCacheBufferItem CacheBufferList::NewEntry(VO_SINK_SAMPLE *pBuffer)
{
	VO_BOOL bKeyFrame = (pBuffer->Size & 0x80000000) ? VO_TRUE : VO_FALSE;
	pBuffer->Size &= 0x7FFFFFFF;

	pCacheBufferItem pTempItem;
	if (!m_pTmpCacheItem) 
	{
		pTempItem = new CacheBufferItem;
		pTempItem->pNext = NULL;
		pTempItem->pPre = NULL;
		pTempItem->bKey = bKeyFrame;

		pTempItem->pBuf.Buffer = new VO_BYTE[pBuffer->Size];
		CopyItem(&pTempItem->pBuf,pBuffer);
	}
	else
	{	
		if (m_pTmpCacheItem->pBuf.Size < pBuffer->Size)
		{
			delete []m_pTmpCacheItem->pBuf.Buffer;
			m_pTmpCacheItem->pBuf.Buffer = new VO_BYTE[pBuffer->Size];
		}
		CopyItem(&m_pTmpCacheItem->pBuf,pBuffer);
		m_pTmpCacheItem->bKey = bKeyFrame;
		pTempItem = m_pTmpCacheItem;
		m_pTmpCacheItem = NULL;
	}
	return pTempItem;
}

VO_VOID CacheBufferList::DeleteEntry(pCacheBufferItem pItem)
{
	if (pItem->pBuf.Buffer)
	{
		delete []pItem->pBuf.Buffer;
	}
	delete pItem;
}

VO_U32 CacheBufferList::GetSmapleCount()
{
	VO_U32 nItemCount = 0;
	pCacheBufferItem pTmp = m_pCacheBufHead;
	while (pTmp)
	{
		nItemCount++;
		pTmp = pTmp->pNext;
	}
	return nItemCount;
}

VO_VOID CacheBufferList::AddEntry(VO_SINK_SAMPLE *pBuffer)
{
	pCacheBufferItem pTmpItem = NewEntry(pBuffer);
	
	if (!m_pCacheBufHead && !m_pCacheBufTail)
	{
		m_pCacheBufHead  = m_pCacheBufTail = pTmpItem;
	}
	else
	{
		m_pCacheBufTail->pNext = pTmpItem;
		pTmpItem->pPre = m_pCacheBufTail;
		m_pCacheBufTail = pTmpItem;

		VO_S64 nDTSTime = pTmpItem->pBuf.DTS;
		pTmpItem = pTmpItem->pPre;

		while( pTmpItem )
		{
			if( pTmpItem->pBuf.DTS <= nDTSTime )
				break;
			else
			{
				pTmpItem->pNext->pBuf.DTS = pTmpItem->pBuf.DTS;
				pTmpItem->pBuf.DTS = nDTSTime;
			}

			pTmpItem = pTmpItem->pPre;
		}
	}
}

pCacheBufferItem CacheBufferList::GetEntry(VO_BOOL bForce)
{
	if (m_pCacheBufHead && (GetSmapleCount() >= m_nMaxItemCount + 1 || bForce))
	{
		pCacheBufferItem pTmpEntry = m_pCacheBufHead;
		m_pCacheBufHead = m_pCacheBufHead->pNext;
		if (!m_pCacheBufHead)
		{
			m_pCacheBufTail = NULL;
		}
		else
			m_pCacheBufHead->pPre = NULL;

		pTmpEntry->pNext = NULL;
		pTmpEntry->pPre = NULL;
		
		if (m_pTmpCacheItem)
		{
			DeleteEntry(m_pTmpCacheItem);
		}
		m_pTmpCacheItem = pTmpEntry;
		return m_pTmpCacheItem;
	}
	return NULL;
}

pCacheBufferItem CacheBufferList::PeekEntry(VO_BOOL bForce)
{
	if (m_pCacheBufHead && (GetSmapleCount() >= m_nMaxItemCount + 1 || bForce))
	{
		return m_pCacheBufHead;
	}
	return NULL;
}

VO_BOOL CacheBufferList::IsCacheBufferFull()
{
	return GetSmapleCount() > m_nMaxItemCount ? VO_TRUE : VO_FALSE; 
}

VO_U32 CacheBufferList::GetDuration(VO_U64* llTs)
{
	if (!m_pCacheBufHead)
	{
		return VO_ERR_FAILED;
	}
	pCacheBufferItem pTmp = m_pCacheBufHead;
	VO_U64 llMaxTs = m_pCacheBufHead->pBuf.DTS;
	VO_U64 llMinTs = m_pCacheBufHead->pBuf.DTS;
	while (pTmp)
	{
		if (pTmp->pBuf.DTS > llMaxTs)
		{
			llMaxTs = pTmp->pBuf.DTS;
		}
		if (pTmp->pBuf.DTS < llMinTs)
		{
			llMinTs = pTmp->pBuf.DTS;
		}
		pTmp = pTmp->pNext;
	}
	if (llMaxTs > llMinTs)
	{
		*llTs = (llMaxTs - llMinTs)/90;
	}
	else
		*llTs = 0;

	return VO_ERR_NONE;
}