#include "ChunkDataCache.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

ChunkDataCache::ChunkDataCache(VO_U32 uMaxDuration, vo_allocator<VO_BYTE>& allocator)
: m_allocator(allocator)
, m_audio(uMaxDuration)
, m_video(uMaxDuration)
, m_subtitle(uMaxDuration)
{
}


ChunkDataCache::~ChunkDataCache(void)
{
}

VO_U32 ChunkDataCache::putSample(const _SAMPLE* pSample, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	shared_ptr<_SAMPLE> pCopy( CreateSampleOP_T(pSample, m_allocator), deleter(m_allocator) );

	VO_U32 uRet = VO_RET_SOURCE2_OK;
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			uRet = m_audio.PutSample(pCopy);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			uRet = m_video.PutSample(pCopy);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			uRet = m_subtitle.PutSample(pCopy);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	//VOLOGI("Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", eTrackType, pSample->uSize, pSample->uFlag, pSample->uTime);

	return uRet;
}

VO_U32 ChunkDataCache::getSample(shared_ptr<_SAMPLE>* ppSample, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	VO_U32 uRet = VO_RET_SOURCE2_OK;
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			uRet = m_audio.GetSample(ppSample);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			uRet = m_video.GetSample(ppSample);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			uRet = m_subtitle.GetSample(ppSample);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	if (VO_RET_SOURCE2_OK == uRet)
	{
		VOLOGI("Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", eTrackType, (*ppSample)->uSize, (*ppSample)->uFlag, (*ppSample)->uTime);
	}

	return uRet;
}

VO_U32 ChunkDataCache::getSamples(std::list< shared_ptr<_SAMPLE> > **plist, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			return m_audio.GetSamples(plist);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			return m_video.GetSamples(plist);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			return m_subtitle.GetSamples(plist);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 ChunkDataCache::setpos(std::list< shared_ptr<_SAMPLE> >::iterator iterP, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			return m_audio.SetPos(iterP);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			return m_video.SetPos(iterP);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			return m_subtitle.SetPos(iterP);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	return VO_RET_SOURCE2_OK;
}