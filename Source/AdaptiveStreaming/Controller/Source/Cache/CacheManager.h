#ifndef __CACHEMANAGER_H__
#define __CACHEMANAGER_H__
#include "MediaCache.h"
#include "voSourceBufferManager_AI.h"
//store chunk data
/*	manager:
	filter(include BA)
	puts每一个track到输出buffer
*/
#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CacheManager //
{
public:
	CacheManager();
	~CacheManager(void);

	VO_U32 init();
	VO_U32 uninit();

	VO_U32 open();
	VO_U32 close();

	VO_VOID setCapacity(VO_U32 uDuration) { m_MediaCache.setCapacity(uDuration); };

	VO_VOID SetFollowingComponent(VO_PTR pComponent){ m_bufferOutput = (voSourceBufferManager_AI*)pComponent;}

	VO_U32 createChunkDataCache(VO_U32 ListID, void**h) { return m_MediaCache.createChunkDataCache(ListID, h); }
	VO_U32 destroyChunkDataCache(void*h) { return m_MediaCache.destroyChunkDataCache(h); }

	VO_U32 putSample(void* h, const _SAMPLE* pSample, VO_SOURCE2_TRACK_TYPE eTrackType);

	VO_U32 mergeChunks(void* h1, void* h2 = NULL);
	VO_U32 useChunk(void* h1);
	VO_VOID setEOS();

	
	
	//release id
	//replace from id to id

private:
	MediaCache m_MediaCache;

	voSourceBufferManager_AI* m_bufferOutput;

	VO_BOOL m_bStop;

	//voCMutex	m_lock;
};


#ifdef _VONAMESPACE
}
#endif

#endif