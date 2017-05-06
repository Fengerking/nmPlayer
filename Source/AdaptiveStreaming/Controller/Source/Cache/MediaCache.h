#ifndef __MEDIACACHE_H__
#define __MEDIACACHE_H__
#include "StreamCache.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class MediaCache
{
public:
	MediaCache();
	~MediaCache(void);

	VO_VOID setCapacity(VO_U32 uDuration) { m_MaxDuration = uDuration; };

	VO_U32 createChunkDataCache(VO_U32 ListID, void**h);
	VO_U32 destroyChunkDataCache(void*h);

	VO_U32 flush();
	//release id
	//replace from id to id

private:
	std::list<StreamCache*> m_listStreams;

	VO_U32	m_MaxDuration;

	vo_allocator<VO_BYTE> m_alloc;

	voCMutex m_lock;
};


#ifdef _VONAMESPACE
}
#endif

#endif 