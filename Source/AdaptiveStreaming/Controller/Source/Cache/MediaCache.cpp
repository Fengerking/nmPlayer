#include "MediaCache.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

MediaCache::MediaCache()
: m_MaxDuration(25000)
{
}


MediaCache::~MediaCache(void)
{
	flush();
}

VO_U32 MediaCache::flush()
{
	VOLOGI("+flush");
	voCAutoLock lock(&m_lock);

	std::list<StreamCache*>::iterator iter	= m_listStreams.begin();
	std::list<StreamCache*>::iterator itere	= m_listStreams.end();

	while (iter != itere)
	{
		VOLOGI("%p", *iter);
		delete *iter;
		
		iter++;
	}

	m_listStreams.clear();
	VOLOGI("-flush");
	return 0;
}

VO_U32 MediaCache::createChunkDataCache(VO_U32 ListID, void**ph)
{
	VOLOGI("createChunkDataCache");
	voCAutoLock lock(&m_lock);

	std::list<StreamCache*>::iterator iter	= m_listStreams.begin();
	std::list<StreamCache*>::iterator itere	= m_listStreams.end();

	while (iter != itere)
	{
		if ( (*iter)->id() == ListID )
		{
			return (*iter)->createChunkDataCache((ChunkDataCache**)ph, m_MaxDuration);
		}

		iter++;
	}

	StreamCache* pStreamCache = new StreamCache(ListID, m_alloc);
	VOLOGI("%p", pStreamCache);
	m_listStreams.push_back(pStreamCache);

	return pStreamCache->createChunkDataCache((ChunkDataCache**)ph, m_MaxDuration);
}

VO_U32 MediaCache::destroyChunkDataCache(void* h)
{
	VOLOGI("destroyChunkDataCache %p", h);
	voCAutoLock lock(&m_lock);

	std::list<StreamCache*>::iterator iter	= m_listStreams.begin();
	std::list<StreamCache*>::iterator itere	= m_listStreams.end();

	while (iter != itere)
	{
		if( VO_RET_SOURCE2_OK == (*iter)->destroyChunkDataCache((ChunkDataCache*)h) )
		{
			VOLOGI("To Destory OK");
			break;
		}
		iter++;
	}

	return 0;
}