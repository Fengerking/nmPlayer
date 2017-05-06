	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CAudioTrackRender.cpp

	Contains:	CAudioTrackRender class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2012-05-13		JBF			Create file

*******************************************************************************/
#include <media/MediaPlayerInterface.h>

#include "voOSFunc.h"

#include "CAudioTrackRender.h"

#include "voLog.h"

CAudioTrackRender::CAudioTrackRender(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP)
	: CBaseAudioRender (hInst, pMemOP)
	, m_pTrack (NULL)
	, m_lFrameSize (2048)
	, m_llPlayingTime (0)
	, m_llBufferTime (0)
	, m_llSystemTime (0)
{
#ifdef __VONJ_ICS__
	m_nStreamType = AUDIO_STREAM_MUSIC;
#else
	m_nStreamType = android::AudioSystem::MUSIC;
#endif // 

	memset (&m_fmtAudio, 0, sizeof (m_fmtAudio));
}

CAudioTrackRender::~CAudioTrackRender ()
{
	if (m_pTrack != NULL)
	{
		m_pTrack->stop ();
		delete m_pTrack;
		m_pTrack = NULL;
	}
}

VO_U32 CAudioTrackRender::SetFormat (VO_AUDIO_FORMAT * pFormat)
{
	voCAutoLock lock (&m_mtTrack);
	
	if (!memcmp (&m_fmtAudio, pFormat, sizeof (m_fmtAudio)))
		return VO_ERR_NONE;
		
	if (m_pTrack != NULL)
	{
		m_pTrack->stop ();
		delete m_pTrack;
	}

	int sampleRate = pFormat->SampleRate;
	int channelCount = pFormat->Channels;
	int bufferCount = DEFAULT_AUDIOSINK_BUFFERCOUNT;
		
#ifdef 	__VONJ_ICS__
	int format = AUDIO_FORMAT_PCM_16_BIT;
	if (pFormat->SampleBits == 8)
		format = AUDIO_FORMAT_PCM_8_BIT;
#else
	int format = android::AudioSystem::PCM_16_BIT;
	if (pFormat->SampleBits == 8)
		format = android::AudioSystem::PCM_8_BIT;
#endif // __VONJ_ICS__			
			
	memcpy (&m_fmtAudio, pFormat, sizeof (m_fmtAudio));		
	VOLOGI ("Audio format SR %d, CH %d, Bits %d,  BC %d", sampleRate, channelCount, format, bufferCount);
	
	
#if defined __VONJ_ECLAIR__ || defined __VONJ_FROYO__ 
	m_pTrack = new AudioTrack(m_nStreamType, sampleRate, format,
							(channelCount == 2) ? AudioSystem::CHANNEL_OUT_STEREO : AudioSystem::CHANNEL_OUT_MONO, 0, 0);
#elif defined __VONJ_GINGERBREAD__
	m_pTrack = new android::AudioTrack(
							m_nStreamType,
							sampleRate,
							format,
							(channelCount == 2) ? android::AudioSystem::CHANNEL_OUT_STEREO : android::AudioSystem::CHANNEL_OUT_MONO,
							0,
							0,
							NULL,
							NULL,
							0,
							0);
#elif defined __VONJ_CUPCAKE__ || defined __VONJ_DONUT__
	m_pTrack = new AudioTrack(m_nStreamType, sampleRate, format, channelCount, 0, 0);
#elif defined __VONJ_ICS__
    int afSampleRate;
    int afFrameCount;
    int frameCount;

    if (android::AudioSystem::getOutputFrameCount(&afFrameCount, m_nStreamType) != 0)
        return -1;
    
    if (android::AudioSystem::getOutputSamplingRate(&afSampleRate, m_nStreamType) != 0)
        return -1;

    frameCount = (sampleRate*afFrameCount*bufferCount)/afSampleRate;

	m_pTrack = new android::AudioTrack(
							m_nStreamType,
							sampleRate,
							format,
							(channelCount == 2) ? AUDIO_CHANNEL_OUT_STEREO : AUDIO_CHANNEL_OUT_MONO,
							frameCount,
							0,
							NULL,
							NULL,
							0,
							NULL);

#endif
	
	if ((m_pTrack == NULL) || (m_pTrack->initCheck() != 0)) 
	{
		VOLOGE("Unable to create audio track");

		delete m_pTrack;
		m_pTrack = NULL;

		return -1;
	}
	
	
	m_llBufferTime = m_pTrack->latency ();	
	m_lFrameSize = m_pTrack->frameSize () * m_pTrack->frameCount ();
	if (m_lFrameSize > 4096)
		m_lFrameSize = 4096;
	
	VOLOGI ("m_llBufferTime is %d   m_lFrameSize %d", (int)m_llBufferTime, m_lFrameSize);
	

	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::Start (void)
{
	voCAutoLock lock (&m_mtTrack);
	
	if (m_pTrack != NULL)
		m_pTrack->start ();
		
	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::Pause (void)
{
	voCAutoLock lock (&m_mtTrack);
		
	if (m_pTrack != NULL)
		m_pTrack->pause ();
		
	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::Stop (void)
{
	voCAutoLock lock (&m_mtTrack);
		
	if (m_pTrack != NULL)
	{
		m_pTrack->stop ();
		
	//	while (!m_pTrack->stopped ())
			voOS_Sleep (100);
			
//		delete m_pTrack;
//		m_pTrack = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::Render (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{	
	voCAutoLock lock (&m_mtTrack);
	
//	VOLOGI ("Buff %p,  Size %d,  Time %d", pBuffer, nSize, (int)nStart);
	
	voCAutoLock lockTime (&m_mtTime);
	if (nStart >= m_llBufferTime)
		m_llPlayingTime = nStart - m_llBufferTime;
	else
		m_llPlayingTime = 0;
	m_llSystemTime = voOS_GetSysTime ();
	
	if (m_pTrack != NULL)
	{
		int nLen = 0;
		while (nLen < nSize)
		{
			if (nLen + m_lFrameSize < nSize)
				m_pTrack->write (pBuffer + nLen,  m_lFrameSize);
			else
				m_pTrack->write (pBuffer + nLen,  nSize - nLen);				
			nLen += m_lFrameSize;	
		}
		
//		m_pTrack->write (pBuffer,  nSize);	
	}

//	VOLOGI ("Write Done!!!");
	
	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::Flush (void)
{
	voCAutoLock lock (&m_mtTrack);
		
	if (m_pTrack != NULL)
		m_pTrack->flush ();

	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::GetPlayingTime (VO_S64	* pPlayingTime)
{
	voCAutoLock lockTime (&m_mtTime);
	
	if (m_llSystemTime == 0)
		*pPlayingTime = m_llPlayingTime;
	else
		*pPlayingTime = m_llPlayingTime + (voOS_GetSysTime () - m_llSystemTime);
			
	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::GetBufferTime (VO_S32 * pBufferTime)
{
	*pBufferTime = (VO_S32)m_llBufferTime;

	return VO_ERR_NONE;
}

VO_U32 CAudioTrackRender::SetParam (VO_U32 nID, VO_PTR pValue)
{
	return CBaseAudioRender::SetParam (nID, pValue);
}

VO_U32 CAudioTrackRender::GetParam (VO_U32 nID, VO_PTR pValue)
{
	return CBaseAudioRender::GetParam (nID, pValue);
}

