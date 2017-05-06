#include "CMKVStreamFileIndex.h"
#include "CMKVTrack.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CMKVStreamFileIndex::CMKVStreamFileIndex(CMemPool* pMemPool)
:CBaseStreamFileIndex(pMemPool)
{
}

CMKVStreamFileIndex::~CMKVStreamFileIndex(void)
{
}

VO_S32 CMKVStreamFileIndex::GetEntryByFilePos(VO_BOOL bVideo, VO_U64 filepos, PBaseStreamMediaSampleIndexEntry* ppEntry)
{
	*ppEntry = VO_NULL;
	MKVMediaSampleIndexEntry * pTmp = (MKVMediaSampleIndexEntry*)m_pHead;
	while(pTmp && (pTmp->filepos <= filepos || !(*ppEntry)))
	{
		if(!bVideo || pTmp->IsKeyFrame())
			*ppEntry = pTmp;

		pTmp = (MKVMediaSampleIndexEntry*)pTmp->next;
	}

	if(!(*ppEntry))
		return -1;

	if(((MKVMediaSampleIndexEntry *)(*ppEntry))->filepos < filepos && !(*ppEntry)->next)
		return 1;
	
	return 0;
}

VO_U64 CMKVStreamFileIndex::GetCurrStartFilePos()
{
	return m_pHead ? ((MKVMediaSampleIndexEntry *)m_pHead)->filepos : 0;
}

VO_U64 CMKVStreamFileIndex::GetCurrEndFilePos()
{
	return m_pTail ? ((MKVMediaSampleIndexEntry *)m_pTail)->filepos : 0;
}

VO_U32 CMKVStreamFileIndex::GetCurrStartBufferPos()
{
	return m_pHead ? ((MKVMediaSampleIndexEntry *)m_pHead)->pos_in_buffer & 0x7fffffff : 0;
}

VO_U32 CMKVStreamFileIndex::GetCurrEndBufferPos()
{
	return m_pTail ? ((MKVMediaSampleIndexEntry *)m_pTail)->pos_in_buffer & 0x7fffffff : 0;
}

MKVMediaSampleIndexEntry * CMKVStreamFileIndex::NewEntry( VO_S64 Cluster_TimeCode , VO_S64 Relative_TimeCode , VO_U32 dwPosInBuffer, VO_U32 dwSize)
{
	MKVMediaSampleIndexEntry * pEntry = (MKVMediaSampleIndexEntry *)m_pMemPool->alloc(sizeof(MKVMediaSampleIndexEntry));
	if(!pEntry)
		return VO_NULL;

	pEntry->set_clustertimecode(Cluster_TimeCode);
	pEntry->set_relativetimecode( Relative_TimeCode );
	pEntry->pos_in_buffer = dwPosInBuffer;
	pEntry->size = dwSize;
	pEntry->next = VO_NULL;
	pEntry->pre = VO_NULL;

	return pEntry;
}

VO_VOID CMKVStreamFileIndex::DeleteEntry(MKVMediaSampleIndexEntry * pEntry)
{
	m_pMemPool->free(pEntry, sizeof(MKVMediaSampleIndexEntry));
}

VO_U32 CMKVStreamFileIndex::GetKeyFrameCnt()
{
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;
	VO_U32 nCnt = 0;
	while(pTmp)
	{
		if(pTmp->IsKeyFrame())
		{
			nCnt++;
		}
		pTmp = pTmp->next;
	}
	return nCnt;

}

VO_S32 CMKVStreamFileIndex::GetEntryByTime(VO_BOOL bVideo, VO_S64 nTimeStamp, PBaseStreamMediaSampleIndexEntry* ppEntry)
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

	//if not the key frame and the track is video;return -1
	if(bVideo && !(*ppEntry)->IsKeyFrame()){
		return -1;
	}

	return 0;
}

VO_S32 CMKVStreamFileIndex::PrintIndex()
{
	PBaseStreamMediaSampleIndexEntry pTmp = m_pHead;

	if(NULL == pTmp){
		return -1;
	}
	
	VOLOGI("PrintIndex----Start");
	
	while(pTmp)
	{
		VOLOGI("PrintIndex----time_stamp=%lu, size=%lu",pTmp->time_stamp, pTmp->size);
		pTmp = pTmp->next;
	}

	VOLOGI("PrintIndex----End");
	
	return 0;
}


