#include "StreamCache.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

StreamCache::StreamCache(VO_U32 id, vo_allocator<VO_BYTE>& allocator)
	: m_id(id)
	, m_allocator(allocator)
{
}


StreamCache::~StreamCache(void)
{
	std::list<ChunkDataCache*>::iterator iter	= m_listChunkDataCache.begin();
	std::list<ChunkDataCache*>::iterator itere	= m_listChunkDataCache.end();

	while (iter != itere)
	{
		//VOLOGI("%p", *iter);
		delete *iter;

		iter++;
	}

	m_listChunkDataCache.clear();
}

VO_U32 StreamCache::createChunkDataCache(ChunkDataCache** ph, VO_U32 uMaxDuration)
{
	*ph = new ChunkDataCache(uMaxDuration, m_allocator);

	VOLOGI("%p", *ph);
	m_listChunkDataCache.push_back(*ph);

	return 0;
}

VO_U32 StreamCache::destroyChunkDataCache(ChunkDataCache* h)
{
	VO_U32 ret = VO_RET_SOURCE2_FAIL;
	std::list<ChunkDataCache*>::iterator iter	= m_listChunkDataCache.begin();
	std::list<ChunkDataCache*>::iterator itere	= m_listChunkDataCache.end();

	while (iter != itere)
	{
		if( h && *iter == h)
		{
			m_listChunkDataCache.remove(h);
			delete h;
			ret = VO_RET_SOURCE2_OK;
			break;
		}
		iter++;
	}

	//m_listChunkDataCache.remove(h);

	return ret;
}