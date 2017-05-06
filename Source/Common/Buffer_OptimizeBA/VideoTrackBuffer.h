#ifndef __VIDEOTRACKBUFFER_H__
#define __VIDEOTRACKBUFFER_H__

#include "TrackBuffer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class VideoTrackBuffer :
	public TrackBuffer
{
public:
	explicit VideoTrackBuffer(VO_U32 uMaxSize);
	~VideoTrackBuffer(void);

	/*virtual*/ VO_U32	GetSample(_SAMPLE *pSample);

	/*virtual*/ VO_U32	SetPos(VO_U64* pullTimePoint);

	/*virtual*/ VO_U32	Duration() /*const*/;
	
private:
	/*virtual*/ VO_U32	CalculateDuration() { return 0; }

	std::list< shared_ptr<_SAMPLE> >::iterator GetNextKeyFrame(VO_U64 timestamp);

	VO_U32  m_uflag;

	VO_U32	m_uFrameInterval;

	VO_U64	m_ullLatestPrunePoint;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__VIDEOTRACKBUFFER_H__