#include "voSourceBufferManager.h"
#include "voSourceBufferManager_AI.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "voSourceBufferManager"
#endif


voSourceBufferManager::voSourceBufferManager( VO_S32 nBufferingTime , VO_S32 nMaxBufferTime , VO_S32 nStartBufferTime )
: m_eStateManager(Manager_Starting)
, m_nMaxBufferTime(nMaxBufferTime)
, m_BufferingTime(nBufferingTime)
, m_StartBufferTime(nStartBufferTime)
, m_audio(m_alloc, nMaxBufferTime)
, m_video(m_alloc, nMaxBufferTime)
, m_subtitle(m_alloc)
, m_eBuffering_Style(VO_BUFFERING_AV)
, m_ullWantedSeekPos(0)
, m_ullActualSeekPos(0)
{
}

voSourceBufferManager::~voSourceBufferManager()
{
}

VO_U32 voSourceBufferManager::AddBuffer(VO_U32 uType , VO_PTR ptr_obj)
{
	if (NULL == ptr_obj)
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	voCAutoLock lock(&m_lock);

	if (VO_BUFFER_SETTINGS == uType)
	{
		VO_BUFFER_SETTING_SAMPLE * ptr_sample = static_cast<VO_BUFFER_SETTING_SAMPLE *>(ptr_obj);

		switch (ptr_sample->nType)
		{
		case VO_BUFFER_SETTING_STARTBUFFERTIME:
			{
				VO_S32 * ptime = (VO_S32 *)ptr_sample->pObj;

				if( *ptime + 1000 <= m_nMaxBufferTime && *ptime > m_StartBufferTime )
					m_StartBufferTime = *ptime + 1000;
			}
			break;

		case VO_BUFFER_SETTING_BUFFERTIME:
			{
				VO_S32 * ptime = (VO_S32 *)ptr_sample->pObj;

				if( *ptime + 1000 <= m_nMaxBufferTime && *ptime > m_BufferingTime )
					m_BufferingTime = *ptime + 1000;
			}
			break;

		case VO_BUFFER_SETTING_BUFFERING_STYLE:
			{
				VO_BUFFERING_STYLE_TYPE* eBuffering_Style = static_cast<VO_BUFFERING_STYLE_TYPE*>(ptr_sample->pObj);
				m_eBuffering_Style = *eBuffering_Style;
			}
			break;

		default:
			return VO_ERR_NOT_IMPLEMENT;
		}

		return VO_RET_SOURCE2_OK;
	}

	if (VO_SOURCE2_FLAG_SAMPLE_EOS & ( (_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == uType)
		{
			VOLOGI("video track meet VO_SOURCE2_FLAG_SAMPLE_EOS, Flag 0x%x, Time\t%lld, Cached Duration\t%lld", ( (_SAMPLE*)ptr_obj )->uFlag, ( (_SAMPLE*)ptr_obj )->uTime, m_video.GetBuffTime() );
		}
		else if (VO_SOURCE2_TT_AUDIO == uType)
		{
			VOLOGI("audio track meet VO_SOURCE2_FLAG_SAMPLE_EOS, Flag 0x%x, Time\t%lld, Cached Duration\t%lld", ( (_SAMPLE*)ptr_obj )->uFlag, ( (_SAMPLE*)ptr_obj )->uTime, m_video.GetBuffTime() );
		}
	} 
	else if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & ( (_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == uType)
		{
			VOLOGI("video track meet VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, Flag 0x%x, Time\t%lld, Cached Duration\t%lld", ( (_SAMPLE*)ptr_obj )->uFlag, ( (_SAMPLE*)ptr_obj )->uTime, m_video.GetBuffTime() );
		}
		else if (VO_SOURCE2_TT_AUDIO == uType)
		{
			VOLOGI("audio track meet VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, Flag 0x%x, Time\t%lld, Cached Duration\t%lld", ( (_SAMPLE*)ptr_obj )->uFlag, ( (_SAMPLE*)ptr_obj )->uTime, m_video.GetBuffTime() );
		}
	}
	else if (VO_SOURCE2_FLAG_SAMPLE_FORCE_FLUSH & ( (_SAMPLE*)ptr_obj )->uFlag)
	{
		if (VO_SOURCE2_TT_VIDEO == uType)
			m_video.Flush();
		else if (VO_SOURCE2_TT_AUDIO == uType)
			m_audio.Flush();
		else if (VO_SOURCE2_TT_SUBTITLE == uType)
			m_subtitle.Flush();
	}

	VO_U32 uRet = VO_RET_SOURCE2_OUTPUTNOTFOUND;
	switch (uType)
	{
	case VO_SOURCE2_TT_VIDEO:
		uRet = add_video( (_SAMPLE*)ptr_obj );
		break;

	case VO_SOURCE2_TT_AUDIO:
		uRet = add_audio( (_SAMPLE*)ptr_obj );
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		uRet = add_subtitle( (_SAMPLE*)ptr_obj );
		break;
	}

	if (Manager_Seeking == m_eStateManager)
	{
		if ( m_video.WillAvailable() ) // Video will coming
		{
			if (VO_SOURCE2_TT_VIDEO == uType)
			{
				if (m_ullWantedSeekPos < ( (_SAMPLE*)ptr_obj )->uTime)
				{
					m_audio.RemoveTo(m_ullActualSeekPos);
					m_eStateManager = Manager_Starting;
				}
				else if (VO_SOURCE2_FLAG_SAMPLE_KEYFRAME & ( (_SAMPLE*)ptr_obj )->uFlag)
				{
					m_ullActualSeekPos = ( (_SAMPLE*)ptr_obj )->uTime;

					m_video.RemoveTo(m_ullActualSeekPos);
					m_audio.RemoveTo(m_ullActualSeekPos);
				}
			}

			if (VO_RET_SOURCE2_NEEDRETRY == uRet)
			{
				m_audio.RemoveTo(m_ullActualSeekPos);
				m_eStateManager = Manager_Starting;
			}
		}
		else
		{
			if ( m_audio.WillAvailable() ) // only Audio will coming
			{
				if (m_audio.GetBuffTime() >= m_BufferingTime)
					m_audio.RemoveTo(m_ullActualSeekPos);

				if (m_audio.GetBuffTime() >= m_BufferingTime)
					m_eStateManager = Manager_Starting;
			} 
			else // will no more data
			{
				m_audio.RemoveTo(m_ullActualSeekPos);
				m_eStateManager = Manager_Starting;
			}
		}
	}

	if (Manager_Starting == m_eStateManager || Manager_Caching == m_eStateManager)
	{
		if ( m_video.WillAvailable() )
		{
			VO_S32 iBufferingTime = Manager_Starting == m_eStateManager ? m_StartBufferTime : m_BufferingTime;
			if ( m_audio.WillAvailable() ) // A&V will coming
			{
				if ( (m_audio.GetBuffTime() >= iBufferingTime && m_video.GetBuffTime() >= iBufferingTime) ||
					VO_RET_SOURCE2_NEEDRETRY == uRet )
				{
					VOLOGI("m_eStateManager from %x to Manager_Running", m_eStateManager);
					m_eStateManager = Manager_Running;
				}
			} 
			else // only Video will coming
			{
				if (m_video.GetBuffTime() >= iBufferingTime)
				{
					VOLOGI("m_eStateManager from %x to Manager_Running", m_eStateManager);
					m_eStateManager = Manager_Running;
				}
			}
		}
		else
		{
			if ( m_audio.WillAvailable() ) //only Audio will coming
			{
				if (m_audio.GetBuffTime() >= m_BufferingTime)
				{
					VOLOGI("m_eStateManager from %x to Manager_Running", m_eStateManager);
					m_eStateManager = Manager_Running;
				}
			} 
			else // will no more data
			{
				VOLOGI("m_eStateManager from %x to Manager_Running", m_eStateManager);
				m_eStateManager = Manager_Running;
			}
		}
	}

	return uRet;
}

VO_BOOL voSourceBufferManager::CanGetData(VO_U32 uType)
{
	if (Manager_Running != m_eStateManager)
	{
/*		if (VO_SOURCE2_TT_VIDEO == uType)
		{
			if (Manager_Starting == m_eStateManager)
			{
				return VO_TRUE;
			}
		}
		else */if (VO_SOURCE2_TT_AUDIO == uType)
		{
			_SAMPLE* p = m_audio.PeekTheFirst();
			if ( p && 
				(p->uFlag & VO_DATASOURCE_FLAG_FORCEOUTPUT) )
			{
				p->uFlag ^= VO_DATASOURCE_FLAG_FORCEOUTPUT;

				return VO_TRUE;
			}
		}

		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_U32 voSourceBufferManager::GetBuffer(VO_U32 uType, VO_PTR ptr_obj)
{
	voCAutoLock lock(&m_lock);

	if ( !CanGetData(uType) )
		return VO_RET_SOURCE2_NEEDRETRY;
	
	VO_U32 uRet = VO_RET_SOURCE2_OUTPUTNOTFOUND;
	switch (uType)
	{
	case VO_SOURCE2_TT_VIDEO:
		uRet = get_video( (_SAMPLE*)ptr_obj );
		break;

	case VO_SOURCE2_TT_AUDIO:
		uRet = get_audio( (_SAMPLE*)ptr_obj );
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		uRet = get_subtitle( (_SAMPLE*)ptr_obj );
		break;
	}

	if (VO_SOURCE2_TT_SUBTITLE != uType && VO_RET_SOURCE2_NEEDRETRY == uRet)
	{
		switch (m_eBuffering_Style)
		{
		case VO_BUFFERING_A:
			{
				if ( VO_SOURCE2_TT_AUDIO == uType && m_audio.WillAvailable() )
				{
					VOLOGI("m_eStateManager from %x to Manager_Caching", m_eStateManager);
					m_eStateManager = Manager_Caching;
				}
			}
			break;

		case VO_BUFFERING_V:
			{
				if ( VO_SOURCE2_TT_VIDEO == uType && m_video.WillAvailable() )
				{
					VOLOGI("m_eStateManager from %x to Manager_Caching", m_eStateManager);
					m_eStateManager = Manager_Caching;
				}
			}
			break;

		case VO_BUFFERING_AV:
		default:
			{
				if ( ( VO_SOURCE2_TT_AUDIO == uType && m_audio.WillAvailable() ) ||
					( VO_SOURCE2_TT_VIDEO == uType && m_video.WillAvailable() ) )
				{
					VOLOGI("m_eStateManager from %x to Manager_Caching", m_eStateManager);
					m_eStateManager = Manager_Caching;
				}
			}
		}
	}

	return uRet;
}

void voSourceBufferManager::Flush()
{
	m_eStateManager = Manager_Starting;
	m_ullWantedSeekPos = m_ullActualSeekPos = 0;

	m_audio.Flush();
	m_video.Flush();
	m_subtitle.Flush();
}

void voSourceBufferManager::set_pos(VO_U64 ullPos)
{
	VOLOGI("setpos %lld", ullPos);
	m_ullActualSeekPos = m_ullWantedSeekPos = ullPos;

	m_eStateManager = Manager_Seeking;
}

VO_U32 voSourceBufferManager::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_BUFFER_PID_MAX_SIZE:
		{
			if (pParam)
				m_nMaxBufferTime = *(VO_S32*)pParam;

			m_audio.SetBuffSize(m_nMaxBufferTime);
			m_video.SetBuffSize(m_nMaxBufferTime);
		}
		break;

	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 voSourceBufferManager::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_BUFFER_PID_DURATION_A:
		{
			VO_U64* ullDuration = (VO_U64*)pParam;
			*ullDuration = m_audio.GetBuffTime();

			if (*ullDuration >= m_nMaxBufferTime - 200)
			{
				VOLOGI( "Audio Duration %lld (Count %d) is close to Max %d", *ullDuration, m_audio.GetBuffCount(), m_nMaxBufferTime);

				_SAMPLE *pFirst	= m_audio.PeekTheFirst();
				_SAMPLE *pLast	= m_audio.PeekTheLast();

				VOLOGI("First\tuSize 0x%x, uFlag 0x%x, uTime \t%lld; Last\tuSize 0x%x, uFlag 0x%x, uTime \t%lld", pFirst->uSize, pFirst->uFlag, pFirst->uTime, pLast->uSize, pLast->uFlag, pLast->uTime);
			}
		}
		break;

	case VO_BUFFER_PID_DURATION_V:
		{
			VO_U64* ullDuration = (VO_U64*)pParam;
			*ullDuration = m_video.GetBuffTime();

			if (*ullDuration >= m_nMaxBufferTime - 200)
			{
				VOLOGI( "Video Duration %lld (Count %d) is close to Max %d", *ullDuration, m_video.GetBuffCount(), m_nMaxBufferTime);

				_SAMPLE *pFirst	= m_video.PeekTheFirst();
				_SAMPLE *pLast	= m_video.PeekTheLast();

				VOLOGI("First\tuSize 0x%x, uFlag 0x%x, uTime \t%lld; Last\tuSize 0x%x, uFlag 0x%x, uTime \t%lld", pFirst->uSize, pFirst->uFlag, pFirst->uTime, pLast->uSize, pLast->uFlag, pLast->uTime);
			}
		}
		break;

	case VO_BUFFER_PID_DURATION_T:
		{
			*(VO_U64*)pParam = m_subtitle.GetBuffTime();
		}
		break;

	case VO_BUFFER_PID_IS_RUN:
		{
			*(VO_BOOL*)pParam = (m_eStateManager == Manager_Running) ? VO_TRUE : VO_FALSE;
		}
		break;

	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	return VO_RET_SOURCE2_OK;
}
