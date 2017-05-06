
#include "CMTVStreamFileIndex.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMTVStreamFileIndex::CMTVStreamFileIndex(CMemPool* pMemPool)
: CBaseStreamFileIndex(pMemPool)
{
}

CMTVStreamFileIndex::~CMTVStreamFileIndex(void)
{
}


VO_BOOL CMTVStreamFileIndex::GetEntry(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry)
{
	if(!m_pHead)
		return VO_FALSE;

	*ppEntry = m_pHead;

	if(bVideo)		//process notify!!
	{
		PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
		while(nTimeStamp > pTmp->time_stamp && pTmp->next)
		{
			pTmp = pTmp->next;
			if(pTmp->IsKeyFrame())
				*ppEntry = pTmp;
		}
	}

	return VO_TRUE;
}


VO_S32 CMTVStreamFileIndex::GetEntryByTime(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry)
{
	*ppEntry = VO_NULL;
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	while(pTmp && (pTmp->time_stamp <= nTimeStamp || !(*ppEntry)))
	{
		//VOLOGI("entry time : %d", pTmp->time_stamp);

		// 20100616, open it
		if(!bVideo || pTmp->IsKeyFrame())	
			*ppEntry = pTmp;

// 		if (bVideo)
// 		{
// 			if (pTmp->IsKeyFrame())
// 			{
// 				*ppEntry = pTmp;
// 			}
// 		}
// 		else
// 		{
// 			*ppEntry = pTmp;
// 		}

		pTmp = pTmp->next;
	}

	if(!(*ppEntry))
		return -1;

	if((*ppEntry)->time_stamp < nTimeStamp && !(*ppEntry)->next)
    {   
        VOLOGI("I Frame at Last Position!")
        return 0;
    }   

	return 0;

	/*

	*ppEntry = VO_NULL;
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	while(pTmp && (pTmp->time_stamp <= nTimeStamp || !(*ppEntry)))
	{
		//if(!bVideo || pTmp->IsKeyFrame())
		*ppEntry = pTmp;

		//VOLOGI("stream index: ts = %u, key frame = %d", pTmp->time_stamp, pTmp->IsKeyFrame()?1:0);

		pTmp = pTmp->next;
	}

	if(!(*ppEntry))
		return 1;

	if((*ppEntry)->time_stamp < nTimeStamp)
		return 1;

	return 0;
*/
}
