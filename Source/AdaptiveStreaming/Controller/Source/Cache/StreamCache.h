#ifndef __STREAMCACHE_H__
#define __STREAMCACHE_H__
#include "ChunkDataCache.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class StreamCache
{
public:
	StreamCache(VO_U32 id, vo_allocator<VO_BYTE>& allocator);
	~StreamCache(void);

	VO_U32 createChunkDataCache(ChunkDataCache**, VO_U32);
	VO_U32 destroyChunkDataCache(ChunkDataCache*);

	VO_U32 id() { return m_id; }

private:
	VO_U32 m_id;

	std::list<ChunkDataCache*> m_listChunkDataCache;

	vo_allocator<VO_BYTE>&	m_allocator;
};


#ifdef _VONAMESPACE
}
#endif

#endif 