#include "voSourceBufferManager_AI.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define VERY_BIG_VALUE 0xffffffffffffffffll

voSourceBufferManager_AI::voSourceBufferManager_AI(VO_S32 nBufferingTime, VO_S32 nMaxBufferTime, VO_S32 nStartBufferTime)
	: voSourceBufferManager(nBufferingTime, nMaxBufferTime, nStartBufferTime)
	, m_bBA_Audio(VO_FALSE)
	, m_bBA_Video(VO_FALSE)
	, m_bPureAudio(VO_FALSE)
	, m_lastaudiotimestamp(VERY_BIG_VALUE)
{
	VOLOGI("nBufferingTime %d, nMaxBufferTime %d, nStartBufferTime %d", nBufferingTime, nMaxBufferTime, nStartBufferTime);
}

voSourceBufferManager_AI::~voSourceBufferManager_AI(void)
{
}

void voSourceBufferManager_AI::Flush()
{
	VOLOGI("Flush");

	m_bBA_Audio = VO_FALSE;
	m_bBA_Video = VO_FALSE;
	m_bPureAudio = VO_FALSE;
	m_lastaudiotimestamp = VERY_BIG_VALUE;
	
	return voSourceBufferManager::Flush();
}

VO_U32 voSourceBufferManager_AI::AddBuffer(VO_U32 uType, VO_PTR ptr_obj)
{
	if (VO_BUFFER_SETTINGS ==  uType)
		return voSourceBufferManager::AddBuffer(uType, ptr_obj);

	if (VO_SOURCE2_FLAG_SAMPLE_BASTART & ( (_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == uType)
		{
			VOLOGI("Video BA start");
			m_bBA_Video = VO_TRUE;
		}
		else
		{
			VOLOGI("Audio BA start");
			m_bBA_Audio = VO_TRUE;
		}
	}
	else if (VO_SOURCE2_FLAG_SAMPLE_BAEND & ( (_SAMPLE*)ptr_obj )->uFlag)
	{
		//video cut off from ( (_SAMPLE*)ptr_obj )->utime
		if (VO_SOURCE2_TT_VIDEO == uType)
		{
			VOLOGI("Video BA end");
			m_bBA_Video = VO_FALSE;
			m_video.CutFrom( ( (_SAMPLE*)ptr_obj )->uTime );
		}
		else
		{
			VOLOGI("Audio BA end");
			m_bBA_Audio = VO_FALSE;
			m_audio.CutFrom( ( (_SAMPLE*)ptr_obj )->uTime );
		}
	}
	else if (VO_SOURCE2_FLAG_SAMPLE_EOS & ( (_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == uType)
			m_bBA_Video = VO_FALSE;
		else
			m_bBA_Audio = VO_FALSE;
	}

	VO_U32 uRet = voSourceBufferManager::AddBuffer(uType, ptr_obj);

	VOLOGR("TRACK_TYPE %d uRet %x, uSize 0x%x, uFlag 0x%x, uTime \t%lld", uType, uRet, ( (_SAMPLE*)ptr_obj )->uSize, ( (_SAMPLE*)ptr_obj )->uFlag, ( (_SAMPLE*)ptr_obj )->uTime);

	return uRet;
}

VO_U32 voSourceBufferManager_AI::GetBuffer(VO_U32 uType, VO_PTR ptr_obj)
{
	if (m_bBA_Audio && VO_SOURCE2_TT_AUDIO == uType)
	{
		_SAMPLE *p = m_audio.PeekTheFirst();
		if (p)
		{
			if (p->uFlag & VO_SOURCE2_FLAG_SAMPLE_BASTART)
			{
				VOLOGR("VO_RET_SOURCE2_NEEDRETRY 0A");
				return VO_RET_SOURCE2_NEEDRETRY;
			}
		}
	}

	if (m_bBA_Video && VO_SOURCE2_TT_VIDEO == uType)
	{
		_SAMPLE *p = m_video.PeekTheFirst();
		if (p)
		{
			if (p->uFlag & VO_SOURCE2_FLAG_SAMPLE_BASTART)
			{
				VOLOGR("VO_RET_SOURCE2_NEEDRETRY 0V");
				return VO_RET_SOURCE2_NEEDRETRY;
			}
		}
	}

	if (m_bPureAudio && VO_SOURCE2_TT_VIDEO == uType)
	{
		_SAMPLE *p = m_video.PeekTheFirst();
		if (p)
		{
			if ( p->uTime == 0 ||
				p->uTime  < m_lastaudiotimestamp ||
				(VO_SOURCE2_FLAG_SAMPLE_EOS & p->uFlag) ||
				(VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & p->uFlag) )
			{
				;
			}
			else if (p->uTime - m_lastaudiotimestamp > 500)
			{
				VOLOGR("VO_RET_SOURCE2_NEEDRETRY 2");
				return VO_RET_SOURCE2_NEEDRETRY;
			} 
			else
			{
				VOLOGI("Not Pure Audio");
				m_bPureAudio = VO_FALSE;
			}
		}
	}

	VO_U32 uRet = voSourceBufferManager::GetBuffer(uType, ptr_obj);

	if (uRet == VO_RET_SOURCE2_OK)
	{
		if (VO_SOURCE2_TT_VIDEO == uType)
		{
			if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & ( (_SAMPLE*)ptr_obj )->uFlag)
			{
				VOLOGI("Pure Audio");
				m_bPureAudio = VO_TRUE;
			}
		}
		else if (VO_SOURCE2_TT_AUDIO == uType)
		{
			m_lastaudiotimestamp = ( (_SAMPLE*)ptr_obj )->uTime;
		}

		if (VO_SOURCE2_FLAG_SAMPLE_EOS & ( (_SAMPLE*)ptr_obj )->uFlag)
		{
			VOLOGI("VO_SOURCE2_FLAG_SAMPLE_EOS TRACK_TYPE %d", uType);
		}
	}		

	VOLOGR("TRACK_TYPE %d uRet %x, uSize 0x%x, uFlag 0x%x, uTime \t%lld", uType, uRet, ( (_SAMPLE*)ptr_obj )->uSize, ( (_SAMPLE*)ptr_obj )->uFlag, ( (_SAMPLE*)ptr_obj )->uTime);

	return uRet;
}
