#ifndef __CHUNKDATACACHE_H__
#define __CHUNKDATACACHE_H__
#include "TrackCache.h"
#include "voSampleOP.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class ChunkDataCache
{
public:
	ChunkDataCache(VO_U32 uMaxDuration, vo_allocator<VO_BYTE>& allocator);
	~ChunkDataCache(void);

	VO_U32	putSample(const _SAMPLE* pSample, VO_SOURCE2_TRACK_TYPE eTrackType);
	VO_U32	getSample(shared_ptr<_SAMPLE>* pSample, VO_SOURCE2_TRACK_TYPE eTrackType);
	VO_U32	getSamples(std::list< shared_ptr<_SAMPLE> > **plist, VO_SOURCE2_TRACK_TYPE eTrackType);

	VO_U32	setpos(std::list< shared_ptr<_SAMPLE> >::iterator iterP, VO_SOURCE2_TRACK_TYPE eTrackType);

private:
	class deleter {
	public:
		deleter(vo_allocator<VO_BYTE>& _alloc)
			: m_allocator(_alloc)
		{}

		void operator()(_SAMPLE* file)
		{
			DestroySampleOP_T(file, m_allocator);
		}

		vo_allocator<VO_BYTE>&	m_allocator;
	};

	vo_allocator<VO_BYTE>&	m_allocator;

	TrackCache m_audio;
	TrackCache m_video;
	TrackCache m_subtitle;
};


#ifdef _VONAMESPACE
}
#endif

#endif 