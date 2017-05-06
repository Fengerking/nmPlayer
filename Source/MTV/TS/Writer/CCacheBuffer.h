#ifndef _CCACHE_BUFFER_H_
#define _CCACHE_BUFFER_H_

#include "voType.h"
#include "voSink.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

const VO_U32 MAX_CACHE_FRAME_COUNT = 20;

typedef struct CacheBuffer
{
	VO_SINK_SAMPLE						pBuf;
	VO_BOOL								bKey;
	struct CacheBuffer *				pNext;
	struct CacheBuffer *				pPre;
}CacheBufferItem,*pCacheBufferItem;

class CacheBufferList
{
public:
	CacheBufferList(VO_U32 nMaxItemCount);
	virtual ~CacheBufferList();
	VO_VOID								AddEntry(VO_SINK_SAMPLE *pBuffer);
	pCacheBufferItem					GetEntry(VO_BOOL bForce = VO_FALSE);
	pCacheBufferItem					PeekEntry(VO_BOOL bForce = VO_FALSE);
	VO_U32								GetSmapleCount();
	VO_BOOL								IsCacheBufferFull();
	VO_VOID								DeleteEntry(pCacheBufferItem pItem);
	VO_U32								GetDuration(VO_U64* llTs);
private:
	pCacheBufferItem					NewEntry(VO_SINK_SAMPLE *pBuffer);
	VO_VOID								RemoveAll();
	VO_VOID								CopyItem(VO_SINK_SAMPLE * stDstItem,VO_SINK_SAMPLE * SrcItem);
public:
	pCacheBufferItem					m_pCacheBufHead;
	pCacheBufferItem					m_pCacheBufTail;
	pCacheBufferItem					m_pTmpCacheItem;
	VO_U32								m_nMaxItemCount;
};

#ifdef _VONAMESPACE
}
#endif


#endif //_CCACHE_BUFFER_H_
