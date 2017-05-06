	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWPlayer.cpp

	Contains:	CVOWPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-08-31		JBF			Create file

*******************************************************************************/
#include "CVOWPlayer.h"
#undef LOG_TAG
#define LOG_TAG "CVOWPlayer"
#include "voLog.h"

typedef VO_S32 (VO_API * VOWGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

CVOWPlayer::CVOWPlayer(void)
	: CVOMEPlayer ()
	, m_hVOWPlay (NULL)
{
}

CVOWPlayer::~CVOWPlayer()
{
	Uninit ();
}

int CVOWPlayer::Init (void)
{
	LoadModule ();
    if (m_hDll == NULL)
	{
		VOLOGE ("It could not load voMMPlay module.");
    	return -1;
	}

	VO_U32 nRC = 0;

	VO_VOMM_INITPARAM initParam;
	memset (&initParam, 0, sizeof (VO_VOMM_INITPARAM));
	initParam.pFileOP = (VO_FILE_OPERATOR *)m_pFilePipe;
	initParam.pDrmCB = (VO_SOURCEDRM_CALLBACK *)&m_drmCallback;
	nRC = m_fVOWAPI.Init(&m_hVOWPlay, &initParam);
	if (m_hVOWPlay == NULL)
	{
		VOLOGE ("It could not init voMMPlay.");
		return -1;
	}

	nRC = m_fVOWAPI.SetCallBack (m_hVOWPlay, vowMessageHandlerProc, this);

	VO_VOMM_CB_VIDEORENDER vrParam;
	vrParam.pCallBack = vowVideoRenderProc;
	vrParam.pUserData = this;
	m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_VideoRender, &vrParam);

	VO_VOMM_CB_AUDIORENDER arParam;
	arParam.pCallBack = vowAudioRenderProc;
	arParam.pUserData = this;
	m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_AudioRender, &arParam);

	return 0;
}

int CVOWPlayer::PrepareAsync()
{
	return vomeLoadSource();
}

int CVOWPlayer::BuildGraph (void)
{
	if (m_nStatus >= 0)
		return 0;

	if (m_hVOWPlay == NULL)
		return -1;

	if (m_nFile == -1 && strlen (m_szFile) <= 0)
		return -1;

	m_bPrepareAsync = false;
	m_bSourceLoading = true;

	VO_U32 nRC = 0;
	if (m_nFile > 0)
		nRC = m_fVOWAPI.Create (m_hVOWPlay, (VO_PTR) m_nFile, VO_VOMM_CREATE_SOURCE_ID, m_nOffset, m_nLength);
	else
		nRC = m_fVOWAPI.Create (m_hVOWPlay, (VO_PTR) m_szFile, VO_VOMM_CREATE_SOURCE_URL, m_nOffset, m_nLength);

	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("It could not create the file graph.");
		m_bSourceLoading = false;
		return -1;
	}

	VO_VIDEO_FORMAT fmtVideo;
	nRC = m_fVOWAPI.GetParam (m_hVOWPlay, VO_VOMMP_PMID_VideoFormat, &fmtVideo);
	if (fmtVideo.Width > 0 && fmtVideo.Height > 0)
	{
		m_nVideoWidth = fmtVideo.Width;
		m_nVideoHeight = fmtVideo.Height;

		//vomeNotifyEvent(MEDIA_SET_VIDEO_SIZE, m_nVideoWidth, m_nVideoHeight);
		m_bNotifyVideoSize = false;
	}

	VO_AUDIO_FORMAT fmtAudio;
	nRC = m_fVOWAPI.GetParam (m_hVOWPlay, VO_VOMMP_PMID_AudioFormat, &fmtAudio);
	if (fmtAudio.SampleRate > 0 && fmtAudio.Channels > 0)
	{
		VOLOGI ("Audio: S %d, C %d \n", (int)fmtAudio.SampleRate, (int)fmtAudio.Channels);

		if (m_pCallBack != NULL)
		{
			m_fmtAudio.nSampleRate = fmtAudio.SampleRate;
			m_fmtAudio.nChannels = fmtAudio.Channels;
			m_fmtAudio.nSampleBits = fmtAudio.SampleBits;
			m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
		}
	}

	m_nDuration = 0;
	GetDuration (&m_nDuration);

	if (m_nSourceType > 0)
		m_bSeeking = true;

	m_nStatus = 0;
	m_bSourceLoading = false;

	return 0;
}

int CVOWPlayer::Run (bool bForceSeek /* = false */)
{
   	if (m_hVOWPlay == NULL)
       	return -1;

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);

	if (m_bAutoStart)
		m_bAutoStart = false;

    if (m_fVOWAPI.Run (m_hVOWPlay)== VO_ERR_NONE)
    {
		if (m_nStartPos > 0)
		{
			m_fVOWAPI.SetCurPos (m_hVOWPlay, m_nStartPos);
			m_nStartPos = 0;
		}

    	m_nStatus = 1;
    	return 0;
    }

	return -1;
}

int CVOWPlayer::Pause (void)
{
    if (m_hVOWPlay == NULL)
       	return -1;

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);

    m_fVOWAPI.Pause (m_hVOWPlay);

	m_nStatus = 2;

	return 0;
}

int CVOWPlayer::Stop (void)
{
    if (m_hVOWPlay == NULL)
       	return -1;

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStop, NULL, NULL);

    m_fVOWAPI.Stop (m_hVOWPlay);

	m_nStatus = 3;
	m_nStartPos = 0;

	return 0;
}

int CVOWPlayer::SetPos (int	nPos)
{
//	if (nPos >= m_nDuration)
//		return -1;
	if (nPos >= m_nDuration - 1500)
		nPos = m_nDuration - 1500;

	m_nStartPos = nPos;

	if (m_hVOWPlay == NULL)
    {
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
        return -1;
    }

    if (m_bSeeking)
    {
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
        return 0;
    }

    VO_U32 nRC = m_fVOWAPI.SetCurPos (m_hVOWPlay, nPos);
	if (nRC != VO_ERR_NONE)
    {
        VOLOGW ("seekTo %d Failed!", nPos);
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
        return 0;
    }

    if (m_nSourceType > 0)
    {
     	m_bSeeking = true;
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
    }
    else
    {
		VOLOGI ("seekTo %d COMPLETE 0.", nPos);
		vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
	}

	return 0;
}

int CVOWPlayer::GetPos (int * pPos)
{
    if (m_hVOWPlay == NULL)
    	return -1;

	if (m_nStartPos > 0)
	{
		if (m_nStatus != 1 && m_nStatus != 2)
		{
			*pPos = m_nStartPos;
			return 0;
		}
	}

    m_fVOWAPI.GetCurPos (m_hVOWPlay, (VO_S32 *)pPos);

	return 0;
}

int CVOWPlayer::GetDuration (int * pPos)
{
    if (m_hVOWPlay == NULL)
    	return -1;

 	m_fVOWAPI.GetDuration (m_hVOWPlay, (VO_U32 *) pPos);

	return 0;
}

int CVOWPlayer::Reset (void)
{
	m_nLoop = 0;
	m_nStatus = -1;

	return 0;
}

int CVOWPlayer::Uninit (bool bSuspend /* = false */)
{
	if (m_hVOWPlay == NULL)
		return 0;

	m_bUninit = true;

	if (m_nStatus >= 1)
		Stop ();

	if (m_nSourceType == 1)
	{
		if (m_bStreamSourceCount)
		{
			g_voAndroidStreamSource--;
			m_bStreamSourceCount = false;
		}
		VOLOGI ("Stream Source Count: %d", g_voAndroidStreamSource);
	}

	if (m_hVOWPlay != NULL)
	{
		m_fVOWAPI.Uninit (m_hVOWPlay);
		m_hVOWPlay = NULL;
	}

	if (!bSuspend && m_nFile != -1)
	{
#ifdef LINUX
		close (m_nFile);
#endif	//LINUX
		m_nFile = -1;
	}

	return 0;
}

VO_U32 CVOWPlayer::MessageHandler (VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2)
{
	if (m_pCallBack == NULL || m_bUninit)
		return 0;

	if (nID == VO_VOMMP_CBID_DebugText)
	{
		m_pCallBack (m_pUserData, VOAP_IDC_displayText, pValue1, pValue2);

		return 0;
	}
	else if (nID == VO_VOMMP_CBID_EndOfFile)
	{
		if (m_nLoop > 0)
		{
			Pause ();
			SetPos (0);
			Run ();
		}
		else
		{
			//SetPos (0);
			Stop ();
			vomeNotifyEvent(MEDIA_PLAYBACK_COMPLETE);
		}

		return 0;
	}
	else if (nID == VO_VOMMP_CBID_Error)
	{
        VOLOGW ("VOME Unknown Error!");
		vomeNotifyEvent(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN);
		return 0;
	}

	if (nID == VO_VOMMP_CBID_BuffStart)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
	else if (nID == VO_VOMMP_CBID_Buffering)
	{
		if (m_nSourceType == 1)
			vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, *(int *)pValue1);
	}
	else if (nID == VO_VOMMP_CBID_BuffStop)
	{
		if (m_bPrepareAsync && !m_bSourceLoading)
			NotifyOpenSource ();
		if (m_bAutoStart)
			Pause ();

		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);

		if(m_bSeeking)
		{
			m_bSeeking = false;
			vomeNotifyEvent(MEDIA_SEEK_COMPLETE);
	        VOLOGI ("seekTo  COMPLETE 1");
		}
	}
	else if (nID == VO_VOMMP_CBID_Download)
		vomeNotifyEvent(MEDIA_BUFFERING_UPDATE, *(int *)pValue1);

	return 0;
}

VO_U32 CVOWPlayer::vowMessageHandlerProc (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2)
{
	CVOWPlayer * pVOW = (CVOWPlayer *)pUserData;
	return pVOW->MessageHandler (nID, pValue1, pValue2);
}

int CVOWPlayer::RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize)
{
	if (m_bAutoStart)
	{
		Pause ();
		return 0;
	}

	m_VideoBuff.virBuffer[0] = pVideoBuffer->Buffer[0];
	m_VideoBuff.virBuffer[1] = pVideoBuffer->Buffer[1];
	m_VideoBuff.virBuffer[2] = pVideoBuffer->Buffer[2];

	m_VideoBuff.nStride[0] = pVideoBuffer->Stride[0];
	m_VideoBuff.nStride[1] = pVideoBuffer->Stride[1];
	m_VideoBuff.nStride[2] = pVideoBuffer->Stride[2];

	if (pVideoBuffer->ColorType == VO_COLOR_YUV_PLANAR420)
		m_VideoBuff.nColorType = VOA_COLOR_YUV420;
	else if (pVideoBuffer->ColorType == VO_COLOR_RGB565_PACKED)
		m_VideoBuff.nColorType = VOA_COLOR_RGB565;
	else
		m_VideoBuff.nColorType = VOA_COLOR_NV12;

	if (!m_bNotifyVideoSize)
	{
		VOLOGI ("Video Color %d W %d, H %d\n", pVideoBuffer->ColorType, (int)pVideoSize->Width, (int)pVideoSize->Height);

		m_nVideoWidth = pVideoSize->Width;
		m_nVideoHeight = pVideoSize->Height;

		VOA_NOTIFYEVENT event;
		event.msg = MEDIA_SET_VIDEO_SIZE;
		event.ext1 = m_nVideoWidth;
		event.ext2 = m_nVideoHeight;

		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		m_pCallBack (m_pUserData, VOAP_IDC_setVideoSize, &m_nVideoWidth, &m_nVideoHeight);
	}

	int nRC = m_pCallBack (m_pUserData, VOAP_IDC_renderVideo, &m_VideoBuff, NULL);
	if (nRC >= 0)
	{
		m_bNotifyVideoSize = true;
		return VO_ERR_FINISH;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_S32 CVOWPlayer::vowVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize, VO_S32 nStart)
{
	CVOWPlayer * pVOW = (CVOWPlayer *)pUserData;
	return pVOW->RenderVideo (pVideoBuffer, pVideoSize);
}

int CVOWPlayer::RenderAudio (VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat)
{
	if (m_bAutoStart)
	{
		Pause ();
		return 0;
	}

	if (pAudioFormat->SampleRate != m_fmtAudio.nSampleRate || pAudioFormat->Channels != m_fmtAudio.nChannels)
	{
		m_fmtAudio.nSampleRate = pAudioFormat->SampleRate;
		m_fmtAudio.nChannels = pAudioFormat->Channels;
		m_fmtAudio.nSampleBits = pAudioFormat->SampleBits;
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
	}

	m_pCallBack (m_pUserData, VOAP_IDC_RenderAudio, pAudioBuffer->Buffer, &pAudioBuffer->Length);

	return 0;
}

VO_S32 CVOWPlayer::vowAudioRenderProc (VO_PTR pUserData, VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart)
{
	CVOWPlayer * pVOW = (CVOWPlayer *)pUserData;
	return pVOW->RenderAudio (pAudioBuffer, pAudioFormat);
}

int CVOWPlayer::LoadModule (void)
{
	vostrcpy(m_szDllFile, _T("voMMPlay"));
	vostrcpy(m_szAPIName, _T("vommGetPlayAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
		return -1;

	VOWGETAPI pAPI = (VOWGETAPI) m_pAPIEntry;
	if (pAPI == NULL)
		return -1;

	pAPI (&m_fVOWAPI, 0);

	return 0;
}
