#ifndef __SUBTITLETRACKBUFFER_H__
#define __SUBTITLETRACKBUFFER_H__

#include "TrackBuffer.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class SubtitleTrackBuffer :
	public TrackBuffer
{
public:
	SubtitleTrackBuffer(VO_U32 uMaxDuration);
	~SubtitleTrackBuffer(void);

	/*virtual*/ VO_U32 GetSample(_SAMPLE *pSample);
};

#ifdef _VONAMESPACE
}
#endif

#endif //__SUBTITLETRACKBUFFER_H__
