#include "MediaBuffer.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_U32 _SetPos(std::list< shared_ptr<_SAMPLE> >* plist, std::list< shared_ptr<_SAMPLE> >::iterator iterP)
{

	struct SAMPLE_WITH_SEQ{
		VO_U32						uSequence;
		shared_ptr<_SAMPLE>	pSample;
	};

	SAMPLE_WITH_SEQ s_Samples[6] = {0};
	//0 VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM
	//1 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT
	//2 VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE
	//3 VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
	//4 VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE
	//5 VO_SOURCE2_FLAG_SAMPLE_EOS

	VO_U32 uSequence = 0;
	std::list< shared_ptr<_SAMPLE> >::iterator iter = plist->begin();

	while (iter != iterP)
	{
		if (VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			s_Samples[0].pSample	= *iter;
			s_Samples[0].uSequence	= uSequence;

			if (s_Samples[1].pSample.get())
			{
				s_Samples[1].pSample	= NULL;
				s_Samples[1].uSequence	= 0;
			}

			if (s_Samples[4].pSample.get())
			{
				s_Samples[4].pSample	= NULL;
				s_Samples[4].uSequence	= 0;
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			s_Samples[1].pSample	= *iter;
			s_Samples[1].uSequence	= uSequence;

			if (s_Samples[4].pSample.get())
			{
				s_Samples[4].pSample	= NULL;
				s_Samples[4].uSequence	= 0;
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			if (s_Samples[1].pSample.get())
			{
				s_Samples[1].pSample	= NULL;
				s_Samples[1].uSequence	= 0;
			}

			s_Samples[4].pSample	= *iter;
			s_Samples[4].uSequence	= uSequence;
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_EOS & (*iter)->uFlag)
		{
			//throw VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM、 VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT、VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE, keep VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE、VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET
			for (VO_S32 i = 0; i < 4; i++)
			{
				if (s_Samples[i].pSample.get())
				{
					s_Samples[i].pSample	= NULL;
					s_Samples[i].uSequence	= 0;
				}
			}
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_HINT_IMAGE & (*iter)->uFlag)
		{
			s_Samples[2].pSample	= *iter;
			s_Samples[2].uSequence	= uSequence;
		}
		else if (VO_SOURCE2_FLAG_SAMPLE_TIMESTAMPRESET & (*iter)->uFlag)
		{
			s_Samples[3].pSample	= *iter;
			s_Samples[3].uSequence	= uSequence;
		}

		iter++;
		uSequence++;
	}

	plist->erase(plist->begin(), iter);

	if (s_Samples[1].pSample.get())
	{
		if (plist->end() == iterP)
		{
			s_Samples[1].pSample->uTime = 0;
		}
		else if (1 <= (*iterP)->uTime)
		{
			s_Samples[1].pSample->uTime = (*iterP)->uTime - 1;
		}
		else
		{
			s_Samples[1].pSample->uTime = 0;
		}
	}

	SAMPLE_WITH_SEQ temp;
	for (VO_S32 i = 6; i > 0; i--)
	{
		for (VO_S32 j = 0; j < i - 1; j++)
		{
			if (s_Samples[j].uSequence > s_Samples[j + 1].uSequence)
			{
				temp = s_Samples[j];
				s_Samples[j] = s_Samples[j + 1];
				s_Samples[j + 1] = temp;
			}
		}
	}

	if (plist->size() && s_Samples[5].pSample.get())
	{
		iter = plist->begin();

		VO_U32 flag = (*iter)->uFlag & s_Samples[5].pSample->uFlag;
		if ( (flag & VO_SOURCE2_FLAG_SAMPLE_NEW_PROGRAM) ||
			(flag & VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT) )
		{
			if (s_Samples[5].pSample.get())
			{
				s_Samples[5].pSample	= NULL;
				s_Samples[5].uSequence	= 0;
			}
		}
	}

	for (int i = 5; i >= 0; i--)
	{
		if (s_Samples[i].pSample.get())
			plist->push_front(s_Samples[i].pSample);
	}

	return VO_RET_SOURCE2_OK;
}

#ifdef _VONAMESPACE
}
#endif //_VONAMESPACE


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

MediaBuffer::MediaBuffer(VO_U32 uMaxDuration/* = 20000*/, VO_U32 m_uBufferingDuration/* = 5000*/, VO_S32 m_uQuikStartDuration/* = 2000*/)
: m_eBufferState(Buffer_Starting)
, m_AudioStreamBuffer(uMaxDuration)
, m_VideoStreamBuffer(uMaxDuration)
, m_SubtitleStreamBuffer(0x7fffffff)
, m_uMaxDuration(uMaxDuration)
, m_uBufferingDuration(m_uBufferingDuration)
, m_uQuikStartDuration(m_uQuikStartDuration)
, m_ullWantedSeekPoint(0)
, m_ullActualSeekPoint(0)
, m_bSoughtVideo(VO_FALSE)
, m_eBufferingStyle(VO_BUFFERING_AV)
{
}

MediaBuffer::~MediaBuffer(void)
{
}


VO_U32 MediaBuffer::PutSample(const shared_ptr<_SAMPLE> pSample, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	voCAutoLock lock(&m_lock);

	VO_U32 uRet = VO_RET_SOURCE2_OK;
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			uRet = m_AudioStreamBuffer.PutSample(pSample);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			uRet = m_VideoStreamBuffer.PutSample(pSample);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			uRet = m_SubtitleStreamBuffer.PutSample(pSample);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	if (Buffer_Seeking == m_eBufferState) //find the nearest usable frame
	{
		if ( m_VideoStreamBuffer.WillAvailable() && !m_bSoughtVideo) // To check video
		{
			if (VO_SOURCE2_TT_VIDEO == eTrackType)
			{
				if (pSample->uTime > m_ullWantedSeekPoint || VO_RET_SOURCE2_NEEDRETRY == uRet)
				{
					m_bSoughtVideo = VO_TRUE;
				}
				else if (VO_SOURCE2_FLAG_SAMPLE_KEYFRAME & pSample->uFlag)
				{
					m_ullActualSeekPoint = pSample->uTime;

					m_VideoStreamBuffer.SetPos(&m_ullActualSeekPoint);
					m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint);
				}
			}
			else if (VO_RET_SOURCE2_NEEDRETRY == uRet)
			{
				m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint);
			}
		}
		else // To check Audio
		{
			if ( m_AudioStreamBuffer.WillAvailable() ) // Audio will coming
			{
				if (VO_SOURCE2_TT_AUDIO == eTrackType/* || VO_RET_SOURCE2_NEEDRETRY == uRet*/)
					if (m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint) != VO_RET_SOURCE2_NEEDRETRY)
					{
						VOLOGI("Buffer from %x to Starting %lld", m_eBufferState, m_ullActualSeekPoint);
						m_eBufferState = Buffer_Starting;
					}
			} 
			else // will no more data
			{
				m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint);
				VOLOGI("Buffer from %x to Starting", m_eBufferState);
				m_eBufferState = Buffer_Starting;
			}
		}
	}

	if (Buffer_Starting == m_eBufferState || Buffer_Buffering == m_eBufferState)
		On(eTrackType);

	VOLOGI("Track %d uRet %x, uSize 0x%x, uFlag 0x%x, uTime \t%lld", eTrackType, uRet, pSample->uSize, pSample->uFlag, pSample->uTime);

	return uRet;
}

VO_U32 MediaBuffer::PutSamples(std::list< shared_ptr<_SAMPLE> >* plist, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	voCAutoLock lock(&m_lock);

	if ( !plist->empty())
	{
		VOLOGI("Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", eTrackType, plist->front()->uSize, plist->front()->uFlag, plist->front()->uTime);
		VOLOGI("Track %d, uSize 0x%x, uFlag 0x%x, uTime \t%lld", eTrackType, plist->back()->uSize, plist->back()->uFlag, plist->back()->uTime);
	}

	if (Buffer_Seeking == m_eBufferState) //find the nearest usable frame
	{
		VOLOGI("Buffer_Seeking");
		std::list< shared_ptr<_SAMPLE> >::iterator iter		= plist->begin();
		std::list< shared_ptr<_SAMPLE> >::iterator itere	= plist->end();

		std::list< shared_ptr<_SAMPLE> >::iterator iterP	= plist->end();
		while (iter != itere)
		{
			if (VO_SOURCE2_TT_VIDEO == eTrackType)
			{
				if (m_ullWantedSeekPoint < (*iter)->uTime)
				{
					VOLOGI("Sought Video");
					m_bSoughtVideo = VO_TRUE;

					if (plist->size() >= 2)
					{
						std::list< shared_ptr<_SAMPLE> >::iterator iterCheck = plist->begin();
						if (VO_SOURCE2_FLAG_SAMPLE_TRACKUNAVAILABLE & (*iterCheck)->uFlag)
						{
							iterCheck++;

							if (VO_SOURCE2_FLAG_SAMPLE_NEW_FORMAT & (*iterCheck)->uFlag)
							{
								plist->erase( plist->begin() );
								VOLOGI("remove a unavailabe");
							}
						}
					}

					break;
				}

				if (VO_SOURCE2_FLAG_SAMPLE_KEYFRAME & (*iter)->uFlag)
				{
					VOLOGI("m_ullActualSeekPoint %lld", (*iter)->uTime);
					m_ullActualSeekPoint = (*iter)->uTime;
					iterP = iter;
				}
			}
			else
			{
				if (m_ullActualSeekPoint < (*iter)->uTime)
					break;

				iterP = iter;
			}

			iter++;
		}

		if (plist->end() != iterP)
		{
			VOLOGI("setpos to %lld", (*iterP)->uTime);
			_SetPos(plist, iterP);
		}

		if (VO_SOURCE2_TT_VIDEO == eTrackType)
		{
			m_VideoStreamBuffer.SetPos(&m_ullActualSeekPoint);
		}
		else
		{
			m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint);
		}
	}

	VO_U32 uRet = VO_RET_SOURCE2_OK;
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			uRet = m_AudioStreamBuffer.PutSamples(plist, (m_eBufferState != Buffer_Running && m_AudioStreamBuffer.Duration() < m_uBufferingDuration) ? VO_TRUE : VO_FALSE);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			uRet = m_VideoStreamBuffer.PutSamples(plist, (m_eBufferState != Buffer_Running && m_VideoStreamBuffer.Duration() < m_uBufferingDuration) ? VO_TRUE : VO_FALSE);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			uRet = m_SubtitleStreamBuffer.PutSamples(plist, VO_TRUE);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	if (Buffer_Seeking == m_eBufferState) //find the nearest usable frame
	{
		if ( m_VideoStreamBuffer.WillAvailable() && !m_bSoughtVideo) // To check video
		{
			if (VO_RET_SOURCE2_NEEDRETRY == uRet)
			{
				VOLOGI("Track %d VO_RET_SOURCE2_NEEDRETRY");
				if (VO_SOURCE2_TT_VIDEO == eTrackType)
				{
					m_bSoughtVideo = VO_TRUE;
				} 
				else
				{
					m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint);
				}
			}
		}
		else // To check Audio
		{
			if ( m_AudioStreamBuffer.WillAvailable() ) // Audio will coming
			{
				//if (VO_SOURCE2_TT_AUDIO == eTrackType/* || VO_RET_SOURCE2_NEEDRETRY == uRet*/)
					if (m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint) != VO_RET_SOURCE2_NEEDRETRY)
					{
						VOLOGI("Buffer from %x to Starting %lld", m_eBufferState, m_ullActualSeekPoint);
						m_eBufferState = Buffer_Starting;
					}
			} 
			else // will no more data
			{
				m_AudioStreamBuffer.SetPos(&m_ullActualSeekPoint);
				VOLOGI("Buffer from %x to Starting", m_eBufferState);
				m_eBufferState = Buffer_Starting;
			}
		}
	}

	if (Buffer_Starting == m_eBufferState || Buffer_Buffering == m_eBufferState)
		On(eTrackType);

	return uRet;
}

VO_U32 MediaBuffer::GetSample(_SAMPLE *pSample, VO_SOURCE2_TRACK_TYPE eTrackType)
{
	voCAutoLock lock(&m_lock);

	if (Buffer_Running != m_eBufferState)
	{
		VO_BOOL bCanGetSample = VO_FALSE;
		if (VO_SOURCE2_TT_AUDIO == eTrackType)
		{
			const shared_ptr<_SAMPLE> p = m_AudioStreamBuffer.Peek();
			if ( p && 
				(p->uDataSource_Flag & VO_DATASOURCE_FLAG_FORCEOUTPUT) )
			{
				//p->uFlag ^= VO_DATASOURCE_FLAG_FORCEOUTPUT;

				bCanGetSample = VO_TRUE;
			}
		}

		if (!bCanGetSample)
			return VO_RET_SOURCE2_NEEDRETRY;
	}

	VO_U32 uRet = VO_RET_SOURCE2_OK;
	switch (eTrackType)
	{
	case VO_SOURCE2_TT_AUDIO:
		{
			uRet = m_AudioStreamBuffer.GetSample(pSample);
		}
		break;

	case VO_SOURCE2_TT_VIDEO:
		{
			uRet = m_VideoStreamBuffer.GetSample(pSample);
		}
		break;

	case VO_SOURCE2_TT_SUBTITLE:
		{
			uRet = m_SubtitleStreamBuffer.GetSample(pSample);
		}
		break;

	default:
		return VO_RET_SOURCE2_TRACKNOTFOUND;
	}

	if (VO_RET_SOURCE2_NEEDRETRY == uRet)
		Off(eTrackType);

	VOLOGR("Track %d uRet %x, uSize 0x%x, uFlag 0x%x, uTime \t%lld", eTrackType, uRet, pSample->uSize, pSample->uFlag, pSample->uTime);

	return uRet;
}

VO_U32 MediaBuffer::SetPos(VO_U64* pullTimePoint)
{
	VOLOGI("Seek %lld", *pullTimePoint);

	Flush();

	m_ullActualSeekPoint = m_ullWantedSeekPoint = *pullTimePoint;

	m_bSoughtVideo = VO_FALSE;
	m_eBufferState = Buffer_Seeking;

	return VO_RET_SOURCE2_OK;
}

VO_U32 MediaBuffer::Flush()
{
	voCAutoLock lock(&m_lock);
	m_eBufferState = Buffer_Starting;
	m_ullWantedSeekPoint = m_ullActualSeekPoint = 0;
	m_bSoughtVideo = VO_FALSE;

	m_AudioStreamBuffer.Flush();
	m_VideoStreamBuffer.Flush();
	m_SubtitleStreamBuffer.Flush();

	return VO_RET_SOURCE2_OK;
}

VO_U32 MediaBuffer::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	voCAutoLock lock(&m_lock);

	switch (uID)
	{
	case VO_BUFFER_PID_MAX_SIZE:
		{
			VO_U32* uDuration = static_cast<VO_U32*>(pParam);
			if (*uDuration <= m_uBufferingDuration || *uDuration <= m_uQuikStartDuration)
			{
				VOLOGW("Max %d, Buffering %d, QuikStart %d", *uDuration, m_uBufferingDuration, m_uQuikStartDuration);
				return VO_RET_SOURCE2_FAIL;
			}

			m_uMaxDuration = *uDuration;
			VOLOGI("Max Duration %d", m_uMaxDuration);

			m_AudioStreamBuffer.ReCapacity(m_uMaxDuration);
			m_VideoStreamBuffer.ReCapacity(m_uMaxDuration);
			//m_SubtitleStreamBuffer.ReCapacity(m_uMaxDuration);
		}
		break;

	case VO_BUFFER_PID_STYLE:
		{
			VO_BUFFERING_STYLE_TYPE* eBuffering_Style = static_cast<VO_BUFFERING_STYLE_TYPE*>(pParam);
			m_eBufferingStyle = *eBuffering_Style;
			VOLOGI("Buffering Style %d", m_eBufferingStyle);
		}
		break;

	case VO_BUFFER_PID_Buffering_Duration:
		{
			VO_U32* uDuration = static_cast<VO_U32*>(pParam);
			if (*uDuration >= m_uMaxDuration)
			{
				VOLOGW("Max %d, Buffering %d", m_uMaxDuration, *uDuration);
				return VO_RET_SOURCE2_FAIL;
			}

			m_uBufferingDuration = *uDuration;
			VOLOGI("Buffering Duration %d", m_uBufferingDuration);
		}
		break;

	case VO_BUFFER_PID_Quik_Start_Duration:
		{
			VO_U32* uDuration = static_cast<VO_U32*>(pParam);
			if (*uDuration >= m_uMaxDuration)
			{
				VOLOGW("Max %d, QuikStart %d", m_uMaxDuration, *uDuration);
				return VO_RET_SOURCE2_FAIL;
			}

			m_uQuikStartDuration = *uDuration;
			VOLOGI("Quik Start Duration %d", m_uQuikStartDuration);
		}
		break;

	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 MediaBuffer::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	voCAutoLock lock(&m_lock);

	switch (uID)
	{
	case VO_BUFFER_PID_DURATION_A:
		{
			VO_U32* ullDuration = static_cast<VO_U32*>(pParam);
			*ullDuration = m_AudioStreamBuffer.Duration();

			if (*ullDuration >= m_uMaxDuration - 200)
			{
				VOLOGI( "Audio Duration %d is close to Max %d", *ullDuration, m_uMaxDuration);

				//_SAMPLE *pFirst	= m_AudioStreamBuffer.PeekTheFirst();
				//VOLOGI("First\tuSize 0x%x, uFlag 0x%x, uTime \t%lld; Last\tuSize 0x%x, uFlag 0x%x, uTime \t%lld", pFirst->uSize, pFirst->uFlag, pFirst->uTime, pLast->uSize, pLast->uFlag, pLast->uTime);
			}
		}
		break;

	case VO_BUFFER_PID_DURATION_V:
		{
			VO_U32* ullDuration = static_cast<VO_U32*>(pParam);
			*ullDuration = m_VideoStreamBuffer.Duration();

			if (*ullDuration >= m_uMaxDuration - 200)
			{
				VOLOGI( "Video Duration %d is close to Max %d", *ullDuration, m_uMaxDuration);

				//_SAMPLE *pFirst	= m_VideoStreamBuffer.PeekTheFirst();
				//VOLOGI("First\tuSize 0x%x, uFlag 0x%x, uTime \t%lld; Last\tuSize 0x%x, uFlag 0x%x, uTime \t%lld", pFirst->uSize, pFirst->uFlag, pFirst->uTime, pLast->uSize, pLast->uFlag, pLast->uTime);
			}
		}
		break;

	case VO_BUFFER_PID_EFFECTIVE_DURATION:
		{
			VO_U32* ullDuration = static_cast<VO_U32*>(pParam);

			if ( m_VideoStreamBuffer.WillAvailable() )
			{
				VO_U32 uVDuration = m_VideoStreamBuffer.Duration();

				if ( m_AudioStreamBuffer.WillAvailable() ) // A&V will coming
				{
					VO_U32 uADuration = m_AudioStreamBuffer.Duration();

					*ullDuration = uADuration < uVDuration ? uADuration : uVDuration;
				} 
				else // only Video will coming
				{
					*ullDuration = uVDuration;
				}
			}
			else
			{
				VO_U32 uADuration = m_AudioStreamBuffer.Duration();

				if ( m_AudioStreamBuffer.WillAvailable() ) //only Audio will coming
				{
					*ullDuration = uADuration;
				} 
				else // will no more data
				{
					VO_U32 uVDuration = m_VideoStreamBuffer.Duration();

					*ullDuration = uADuration < uVDuration ? uADuration : uVDuration;
				}
			}

			VOLOGI( "Effective Duration %d. Audio is %d, status %d; Video is %d, status %d", *ullDuration, m_AudioStreamBuffer.Duration(), m_AudioStreamBuffer.WillAvailable(), m_VideoStreamBuffer.Duration(), m_VideoStreamBuffer.WillAvailable() );
		}
		break;

	case VO_BUFFER_PID_DURATION_T:
		{
			VO_U32* ullDuration = static_cast<VO_U32*>(pParam);
			*ullDuration = m_SubtitleStreamBuffer.Duration();
		}
		break;

	case VO_BUFFER_PID_IS_RUN:
		{
			VO_BOOL* bBufferRunning = static_cast<VO_BOOL*>(pParam);
			*bBufferRunning = (Buffer_Running== m_eBufferState) ? VO_TRUE : VO_FALSE;
		}
		break;

	default:
		return VO_RET_SOURCE2_NOIMPLEMENT;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 MediaBuffer::On(VO_SOURCE2_TRACK_TYPE eTrackType)
{
	if ( m_VideoStreamBuffer.WillAvailable() )
	{
		VO_U32 uBufferingDuration = Buffer_Starting == m_eBufferState ? m_uQuikStartDuration : m_uBufferingDuration;
		if ( m_AudioStreamBuffer.WillAvailable() ) // A&V will coming
		{
			VO_U32 uADuration = m_AudioStreamBuffer.Duration();
			VO_U32 uVDuration = m_VideoStreamBuffer.Duration();
			if (uADuration >= uBufferingDuration && uVDuration >= uBufferingDuration)
			{
				VOLOGI("Buffer from %x to Running", m_eBufferState);
				m_eBufferState = Buffer_Running;
			}
		} 
		else // only Video will coming
		{
			if (m_VideoStreamBuffer.Duration() >= uBufferingDuration)
			{
				VOLOGI("Buffer from %x to Running", m_eBufferState);
				m_eBufferState = Buffer_Running;
			}
		}
	}
	else
	{
		if ( m_AudioStreamBuffer.WillAvailable() ) //only Audio will coming
		{
			if (m_AudioStreamBuffer.Duration() >= m_uBufferingDuration)
			{
				VOLOGI("Buffer from %x to Running", m_eBufferState);
				m_eBufferState = Buffer_Running;
			}
		} 
		else // will no more data
		{
			VOLOGI("Buffer from %x to Running", m_eBufferState);
			m_eBufferState = Buffer_Running;
		}
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 MediaBuffer::Off(VO_SOURCE2_TRACK_TYPE eTrackType)
{
	if (VO_SOURCE2_TT_SUBTITLE == eTrackType)
		return VO_RET_SOURCE2_OK;

	switch (m_eBufferingStyle)
	{
	case VO_BUFFERING_A:
		{
			if ( ( VO_SOURCE2_TT_AUDIO == eTrackType && m_AudioStreamBuffer.WillAvailable() ) ||
				( VO_SOURCE2_TT_VIDEO == eTrackType && !m_AudioStreamBuffer.WillAvailable() && m_VideoStreamBuffer.WillAvailable() ) )
			{
				VOLOGI("Buffer from %x to Buffering", m_eBufferState);
				m_eBufferState = Buffer_Buffering;
			}
		}
		break;

	case VO_BUFFERING_V:
		{
			if ( ( VO_SOURCE2_TT_VIDEO == eTrackType && m_VideoStreamBuffer.WillAvailable() ) ||
				( VO_SOURCE2_TT_AUDIO == eTrackType && !m_VideoStreamBuffer.WillAvailable() && m_AudioStreamBuffer.WillAvailable() ) )
			{
				VOLOGI("Buffer from %x to Buffering", m_eBufferState);
				m_eBufferState = Buffer_Buffering;
			}
		}
		break;

	case VO_BUFFERING_AV:
	default:
		{
			if ( ( VO_SOURCE2_TT_AUDIO == eTrackType && m_AudioStreamBuffer.WillAvailable() ) ||
				( VO_SOURCE2_TT_VIDEO == eTrackType && m_VideoStreamBuffer.WillAvailable() ) )
			{
				VOLOGI("Buffer from %x to Buffering", m_eBufferState);
				m_eBufferState = Buffer_Buffering;
			}
		}
	}

	return VO_RET_SOURCE2_OK;
}
