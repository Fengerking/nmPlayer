#include "SubtitleTrackBuffer.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

SubtitleTrackBuffer::SubtitleTrackBuffer(VO_U32 uMaxDuration)
: TrackBuffer(0x7FFFFFFF)
{
}

SubtitleTrackBuffer::~SubtitleTrackBuffer(void)
{
}

VO_U32 SubtitleTrackBuffer::GetSample(_SAMPLE *pSample)
{
	voCAutoLock lock(&m_lock);

	VO_S64 llWantedTime = pSample->uTime;

	if ( m_listSamples.empty() || static_cast<VO_S64>(m_listSamples.front()->uTime) > llWantedTime)
	{
		VOLOGI("%lld", pSample->uTime);

		return VO_RET_SOURCE2_NEEDRETRY;
	}


	VO_U32 uRet = TrackBuffer::GetSample(pSample);

	if (VO_RET_SOURCE2_OK == uRet && VO_U32(pSample->pReserve2) == __INTERNALSUBTITLE)
	{
		while (!m_listSamples.empty() && m_listSamples.front()->uTime <= llWantedTime)
		{
			uRet = TrackBuffer::GetSample(pSample);
		}
	}

	return uRet;
}
