#include "CMpegFileIndex.h"
#include "voSource.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMpegFileIndex::CMpegFileIndex(CMemPool* pMemPool)
:CBaseStreamFileIndex(pMemPool)
{
}

CMpegFileIndex::~CMpegFileIndex()
{
	
}

VO_U64 CMpegFileIndex::GetEntryDuration()
{
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	VO_U64 dwMaxTimeStamp = 0;
	VO_U64 dwMinTimeStamp = 0x7FFFFFFFFFFFFFFFLL;
	while(pTmp)
	{
		dwMaxTimeStamp = pTmp->time_stamp > dwMaxTimeStamp ? pTmp->time_stamp : dwMaxTimeStamp;
		dwMinTimeStamp = pTmp->time_stamp < dwMinTimeStamp ? pTmp->time_stamp : dwMinTimeStamp;
		pTmp =pTmp->next;
	}
	return dwMaxTimeStamp - dwMinTimeStamp;
}

VO_U32 CMpegFileIndex::GetFrameInfo(VO_S64 *pNearKeyFramTs,VO_S64 * pMaxTs,VO_S64 * pMinTs,VO_S64 llTimeStamp,VO_BOOL bVideo)
{
	if (!m_pHead)
	{
		return VO_ERR_SOURCE_END;
	}

	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	*pMaxTs = 0;
	*pMinTs = 0x7FFFFFFFFFFFFFFFLL;
	while(pTmp)
	{
		*pMaxTs = pTmp->time_stamp > *pMaxTs ? pTmp->time_stamp : *pMaxTs;
		*pMinTs = pTmp->time_stamp < *pMinTs ? pTmp->time_stamp : *pMinTs;
		if (llTimeStamp != -1 && pTmp->time_stamp <= llTimeStamp && (!bVideo || pTmp->IsKeyFrame()))
		{
			*pNearKeyFramTs = pTmp->time_stamp;
		}
		pTmp =pTmp->next;
	}
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMpegFileIndex::GetEntryByCount(VO_U32 uCount, PBaseStreamMediaSampleIndexEntry* pEntry)
{
	if (pEntry == NULL || uCount >= GetCurrEntryCount())
	{
		return VO_ERR_SOURCE_END;
	}

	*pEntry = m_pHead;
	for (VO_U32 uCnt = 0;uCnt < uCount;++uCnt)
	{
		if (*pEntry == NULL)
		{
			return VO_ERR_SOURCE_END;
		}
		*pEntry = (*pEntry)->next;
	}
	return VO_ERR_SOURCE_OK;
}