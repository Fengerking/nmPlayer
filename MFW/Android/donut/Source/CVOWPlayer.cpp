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

#include <stdio.h>
#include <string.h>

#define LOG_TAG "CVOWPlayer"

#include "CVOWPlayer.h"

#include "voLog.h"

// ----------------------------------------------------------------------------

typedef VO_S32 (VO_API * VOWGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

CVOWPlayer::CVOWPlayer(void)
	: CVOMEPlayer ()
	, m_hVOWPlay (NULL)
	, m_nColorType (VO_COLOR_RGB565_PACKED)
	, m_pVideoBuffer (NULL)
	, m_pVideoFormat (NULL)
	, m_bBlackFrame (VO_TRUE)
	, m_nCapturePos (0)
	, m_bVideoCallBack (VO_FALSE)
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
	initParam.nFlag = 0;
	initParam.pFileOP = (VO_FILE_OPERATOR *)m_pFilePipe;
	initParam.pMemOP = NULL;

	memset(&m_drmCB, 0, sizeof(m_drmCB));
	m_drmCB.fCallback = voSourceDrmCallBack;
	m_drmCB.pUserData = this;
	initParam.pDrmCB = (VO_SOURCEDRM_CALLBACK *)&m_drmCB;
//	initParam.pLibOP = (VO_LIB_OPERATOR  *)&m_libOP;
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

int CVOWPlayer::BuildGraph (void)
{
	if (m_nStatus >= 0)
		return 0;

	if (m_hVOWPlay == NULL)
		return -1;

	if (m_nFile == -1 &&  m_hFile == NULL && strlen (m_szFile) <= 0)
		return -1;

	m_bPrepareAsync = false;
	m_bSourceLoading = true;

	VO_U32 nRC = 0;
	if (m_hFile != NULL)
		nRC = m_fVOWAPI.Create (m_hVOWPlay, (VO_PTR) m_hFile, VO_VOMM_CREATE_SOURCE_HANDLE, m_nOffset, m_nLength);
	else if (m_nFile > 0)
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

		if (m_pCallBack != NULL)
		{
			VOA_NOTIFYEVENT event;
			event.msg = MEDIA_SET_VIDEO_SIZE;
			event.ext1 = m_nVideoWidth;
			event.ext2 = m_nVideoHeight;
			//m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		m_bNotifyVideoSize = false;
		m_bSetVideoBuffer = false;
	}

	VO_AUDIO_FORMAT fmtAudio;
	nRC = m_fVOWAPI.GetParam (m_hVOWPlay, VO_VOMMP_PMID_AudioFormat, &fmtAudio);
	if (fmtAudio.SampleRate > 0 && fmtAudio.Channels > 0)
	{
		VOLOGI ("Audio: S %d, C %d \n", fmtAudio.SampleRate, fmtAudio.Channels);

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

int CVOWPlayer::Run (void)
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

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
	event.msg = MEDIA_SEEK_COMPLETE;

	if (m_hVOWPlay == NULL)
    {
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
        return -1;
    }

    if (m_bSeeking)
    {
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
        return 0;
    }

    VO_U32 nRC = m_fVOWAPI.SetCurPos (m_hVOWPlay, nPos);
	if (nRC != VO_ERR_NONE)
    {
        VOLOGI ("seekTo %d Failed!", nPos);
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
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
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		VOLOGI ("seekTo %d COMPLETE 0.", nPos);
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

int CVOWPlayer::Uninit (void)
{
	if (m_hVOWPlay == NULL)
		return 0;

	m_bUninit = true;

	voCOMXAutoLock lock (&m_mutLoad);
	if (m_nStatus >= 1)
		Stop ();

	if (m_nSourceType > 0)
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

	if (m_hFile != NULL)
		fclose (m_hFile);
	m_hFile = NULL;

	return 0;
}

int CVOWPlayer::CaptureFrame (int nPos, int nWidth, int nHeight, VO_IV_COLORTYPE nColor,
							 VO_VIDEO_BUFFER ** ppVideoBuffer, VO_VIDEO_FORMAT ** pVideoFormat)
{
	if (m_hVOWPlay == NULL)
		return -1;

	if (m_hFile == NULL && m_nFile <= 0 && strlen (m_szFile) <= 0)
		return -1;

	m_bBlackFrame = VO_TRUE;

	VO_U32 nValue = 1;
	VO_U32 nRC = m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_PlayFlagAV, &nValue);
	nValue = 0;
	nRC = m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_ThreadNum, &nValue);
	nValue = 1;
	nRC = m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_VOSDK, &nValue);

	VO_BOOL bThumnNail = VO_TRUE;
	nRC = m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_ForThumbnail, &bThumnNail);

	if (m_hFile != NULL)
		nRC = m_fVOWAPI.Create (m_hVOWPlay, (VO_PTR) m_hFile, VO_VOMM_CREATE_SOURCE_HANDLE, m_nOffset, m_nLength);
	else if (m_nFile > 0)
		nRC = m_fVOWAPI.Create (m_hVOWPlay, (VO_PTR) m_nFile, VO_VOMM_CREATE_SOURCE_ID, m_nOffset, m_nLength);
	else
		nRC = m_fVOWAPI.Create (m_hVOWPlay, (VO_PTR) m_szFile, VO_VOMM_CREATE_SOURCE_URL, m_nOffset, m_nLength);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("Create the file graph failed!");
		return -1;
	}

	m_fVOWAPI.GetDuration (m_hVOWPlay, (VO_U32 *)&m_nDuration);

	VOLOGW ("GetDuration: %d", m_nDuration);

	VO_VIDEO_FORMAT fmtVideo;
	nRC = m_fVOWAPI.GetParam (m_hVOWPlay, VO_VOMMP_PMID_VideoFormat, &fmtVideo);
	if (fmtVideo.Width > 0 && fmtVideo.Height > 0)
	{
		m_nVideoWidth = fmtVideo.Width;
		m_nVideoHeight = fmtVideo.Height;

		VO_RECT rcDisp;
		rcDisp.top = 0;
		rcDisp.left = 0;
		rcDisp.right = nWidth;
		rcDisp.bottom = nHeight;
		nRC = m_fVOWAPI.SetViewInfo (m_hVOWPlay, NULL, &rcDisp);

		m_nColorType = nColor;

		m_pVideoBuffer = NULL;
		nValue = 1;
		int nTryTimes = 0;
		int nTrySamples = 0;
		m_nCapturePos = 0;

		while (nTryTimes < 200)
		{
			m_bVideoCallBack = VO_FALSE;

			nRC = m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_Playback, &nValue);
			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("VO_VOMMP_PMID_Playback Error!");
				break;
			}

			nTrySamples++;
			if (!m_bVideoCallBack)
			{
				if (m_nDuration > 2000)
				{
					VO_S32 nPos = 0;
					nRC = m_fVOWAPI.GetCurPos (m_hVOWPlay, &nPos);

					if (nRC != VO_ERR_NONE || nPos + 2000 >= m_nDuration)
						break;;
				}
				else
				{
					if (nTrySamples > 50)
						break;
				}

				continue;
			}
			nTrySamples = 0;

			VOLOGW ("Black: %d		Pos %d", m_bBlackFrame, m_nCapturePos);

			if (m_bBlackFrame && (m_nCapturePos + 2000 < m_nDuration))
			{
				m_nCapturePos += 2000;
				m_fVOWAPI.SetCurPos (m_hVOWPlay, m_nCapturePos);
			}
			else
			{
				if (m_pVideoBuffer != NULL)
				{
					*ppVideoBuffer = m_pVideoBuffer;
					*pVideoFormat = m_pVideoFormat;
					break;
				}
			}

			nTryTimes++;
		}

		if (m_pVideoBuffer != NULL)
		{
			*ppVideoBuffer = m_pVideoBuffer;
			*pVideoFormat = m_pVideoFormat;
		}
		else
		{
			m_bVideoCallBack = VO_FALSE;
			m_bBlackFrame = VO_FALSE;

			m_nCapturePos = 0;
			m_fVOWAPI.SetCurPos (m_hVOWPlay, m_nCapturePos);

			nTrySamples = 0;
			while (nTrySamples < 50)
			{
				nRC = m_fVOWAPI.SetParam (m_hVOWPlay, VO_VOMMP_PMID_Playback, &nValue);
				if (nRC != VO_ERR_NONE)
				{
					VOLOGE ("VO_VOMMP_PMID_Playback Error!");
					break;
				}

				nTrySamples++;
				if (!m_bVideoCallBack)
				{
					if (m_nDuration > 2000)
					{
						VO_S32 nPos = 0;
						nRC = m_fVOWAPI.GetCurPos (m_hVOWPlay, &nPos);

						if (nRC != VO_ERR_NONE || nPos + 2000 >= m_nDuration)
						{
							break;;
						}
					}
				}
				else
				{
					break;
				}
			}

			if (m_pVideoBuffer != NULL)
			{
				*ppVideoBuffer = m_pVideoBuffer;
				*pVideoFormat = m_pVideoFormat;
			}
		}
	}
	else
	{
		return -1;
	}

	return 0;
}

bool CVOWPlayer::CheckBlackFrame (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize)
{
	if (pVideoBuffer == NULL || pVideoSize == NULL)
		return false;

	if (pVideoBuffer->ColorType != VO_COLOR_YUV_PLANAR420)
		return false;

	int nLightPixs = 0;
	int nTotalPixs = 0;

	int	nX = 0;
	int nY = 0;
	int nW = pVideoSize->Width;
	int nH = pVideoSize->Height;

	if (nW > 176)
	{
		nW = 176;
		nX =  (pVideoSize->Width - nW) / 2;
	}
	if (nH > 144)
	{
		nH = 144;
		nY = (pVideoSize->Height - nH) / 2;
	}

	int i = 0;
	int j = 0;
	unsigned char * pVideo = NULL;
	for (i = nY; i < nH + nY ; i++)
	{
		pVideo = pVideoBuffer->Buffer[0] + i * pVideoBuffer->Stride[0];
		for (j = nX; j < nW + nX ; j++)
		{
			if (pVideo[j] > 50)
				nLightPixs++;

			nTotalPixs++;
		}
	}

	if (nLightPixs > nTotalPixs / 10)
		m_bBlackFrame = VO_FALSE;
	else
		m_bBlackFrame = VO_TRUE;

	if (m_bBlackFrame && (nW < pVideoSize->Width || nH < pVideoSize->Height))
	{
		nLightPixs = 0;
		nTotalPixs = 0;

		for (i = 0; i < pVideoSize->Height; i++)
		{
			pVideo = pVideoBuffer->Buffer[0] + i * pVideoBuffer->Stride[0];
			for (j = 0; j <pVideoSize->Width; j++)
			{
				if (pVideo[j] > 50)
					nLightPixs++;

				nTotalPixs++;
			}
		}

		if (nLightPixs > nTotalPixs / 10)
			m_bBlackFrame = VO_FALSE;
		else
			m_bBlackFrame = VO_TRUE;		
	}

	// VOLOGE ("Result L: %d, A: %d, R: %d", nLightPixs, nTotalPixs, m_bBlackFrame);

	return true;
}

VO_U32 CVOWPlayer::MessageHandler (VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2)
{
	if (m_pCallBack == NULL || m_bUninit)
		return 0;

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));

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
			// SetPos (0);
			Stop ();

			event.msg = MEDIA_PLAYBACK_COMPLETE;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		return 0;
	}
	else if (nID == VO_VOMMP_CBID_Error)
	{
        VOLOGI ("VOME Unknpwn Error!");

		event.msg = MEDIA_ERROR;
		event.ext1 = MEDIA_ERROR_UNKNOWN;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

		return 0;
	}

	voCOMXAutoLock lock (&m_mutLoad);
	if (nID == VO_VOMMP_CBID_BuffStart)
	{
		// VOLOGI ("VOME_CID_PLAY_BUFFERSTART");
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
	}
	else if (nID == VO_VOMMP_CBID_Buffering)
	{
		if (m_nSourceType == 1)
		{
			event.msg = MEDIA_BUFFERING_UPDATE;
			event.ext1 = *(int *)pValue1;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}
	}
	else if (nID == VO_VOMMP_CBID_BuffStop)
	{
		// VOLOGI ("VOME_CID_PLAY_BUFFERSTOP. PrepareSync: %d", m_bPrepareAsync);
		if (m_bPrepareAsync && !m_bSourceLoading)
			NotifyOpenSource ();
		if (m_bAutoStart)
			Pause ();;

		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);

		if(m_bSeeking)
		{
			m_bSeeking = false;

			event.msg = MEDIA_SEEK_COMPLETE;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

	        VOLOGI ("seekTo  COMPLETE 1");
		}
	}
	else if (nID == VO_VOMMP_CBID_Download)
	{
		event.msg = MEDIA_BUFFERING_UPDATE;
		event.ext1 = *(int *)pValue1;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}

	return 0;
}

VO_U32 CVOWPlayer::vowMessageHandlerProc (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2)
{
	CVOWPlayer *		pVOW = (CVOWPlayer *)pUserData;

	return pVOW->MessageHandler (nID, pValue1, pValue2);
}

int CVOWPlayer::RenderVideo (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize)
{
	if (m_bPrepareAsync)
	{
		m_mutAVRender.Lock ();
		NotifyOpenSource ();
		m_mutAVRender.Unlock ();
	}

	if(m_bSeeking)
	{
		m_mutAVRender.Lock ();
		m_bSeeking = false;
		VOA_NOTIFYEVENT event;
		memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
		event.msg = MEDIA_SEEK_COMPLETE;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		m_mutAVRender.Unlock ();
	}

	if (m_bAutoStart)
	{
		Pause ();;
		return 0;
	}

	if (m_pCallBack == NULL)
	{
		m_bVideoCallBack = VO_TRUE;

		if (m_bBlackFrame && pVideoBuffer->ColorType == VO_COLOR_YUV_PLANAR420)
			CheckBlackFrame (pVideoBuffer, pVideoSize);

		if (m_bBlackFrame && (m_nCapturePos + 4100 < m_nDuration))
			return VO_ERR_FINISH;

		if (pVideoBuffer->ColorType != m_nColorType)
			return VO_ERR_NOT_IMPLEMENT;

		m_pVideoBuffer = pVideoBuffer;
		m_pVideoFormat = pVideoSize;

		return VO_ERR_FINISH;
	}

	m_VideoBuff.virBuffer[0] = pVideoBuffer->Buffer[0];
	m_VideoBuff.virBuffer[1] = pVideoBuffer->Buffer[1];
	m_VideoBuff.virBuffer[2] = pVideoBuffer->Buffer[2];

	m_VideoBuff.nStride[0] = pVideoBuffer->Stride[0];
	m_VideoBuff.nStride[1] = pVideoBuffer->Stride[1];
	m_VideoBuff.nStride[2] = pVideoBuffer->Stride[2];

	if (pVideoBuffer->ColorType == VO_COLOR_YUV_PLANAR420)
	{
		m_VideoBuff.nColorType = VOA_COLOR_YUV420;
	}
	else if (pVideoBuffer->ColorType == VO_COLOR_RGB565_PACKED)
	{
		m_VideoBuff.nColorType = VOA_COLOR_RGB565;
	}
	else
	{
		m_VideoBuff.nColorType = VOA_COLOR_NV12;
		// return OMX_ErrorNotImplemented;
	}

	if (!m_bNotifyVideoSize)
	{
		VOLOGI ("Video Color %d W %d, H %d\n", pVideoBuffer->ColorType, pVideoSize->Width, pVideoSize->Height);

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

/*
	if (!m_bSetVideoBuffer && nRC >= 0)
	{
		OMX_VO_VIDEOBUFFERTYPE videoBuff;
		memset (&videoBuff, 0, sizeof (OMX_VO_VIDEOBUFFERTYPE));

		m_pCallBack (m_pUserData, VOAP_IDC_getVideoBuffer, &videoBuff.Buffer[0], NULL);
		videoBuff.Stride[0] = m_nVideoWidth * 2;

		// VOLOGI ("VOAP_IDC_getVideoBuffer: %d, %d", videoBuff.Buffer[0], videoBuff.Stride[0]);

		m_fAPI.SetParam (m_hVOWPlay, VOME_PID_VideoDataBuffer, &videoBuff);
	}
*/
	if (nRC >= 0)
	{
		m_bNotifyVideoSize = true;
		m_bSetVideoBuffer = true;

		return VO_ERR_FINISH;
	}
	else
	{
		return VO_ERR_NOT_IMPLEMENT;
	}
}

VO_S32 CVOWPlayer::vowVideoRenderProc (VO_PTR pUserData, VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_FORMAT * pVideoSize, VO_S32 nStart)
{
	CVOWPlayer *	pVOW = (CVOWPlayer *)pUserData;

	return pVOW->RenderVideo (pVideoBuffer, pVideoSize);
}

VO_S32 CVOWPlayer::vowAudioRenderProc (VO_PTR pUserData, VO_CODECBUFFER * pAudioBuffer, VO_AUDIO_FORMAT * pAudioFormat, VO_S32 nStart)
{
	CVOWPlayer *	pVOW = (CVOWPlayer *)pUserData;

	if (pVOW->m_pCallBack == NULL)
		return OMX_ErrorNone;

	if (pVOW->m_bPrepareAsync)
	{
		pVOW->m_mutAVRender.Lock ();
		pVOW->NotifyOpenSource ();
		pVOW->m_mutAVRender.Unlock ();
	}

	if(pVOW->m_bSeeking)
	{
		pVOW->m_mutAVRender.Lock ();
		pVOW->m_bSeeking = false;
		VOA_NOTIFYEVENT event;
		memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
		event.msg = MEDIA_SEEK_COMPLETE;
		pVOW->m_pCallBack (pVOW->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		pVOW->m_mutAVRender.Unlock ();
	}

	if (pVOW->m_bAutoStart)
	{
		pVOW->Pause ();;
		return 0;
	}

	if (pAudioFormat->SampleRate != pVOW->m_fmtAudio.nSampleRate || pAudioFormat->Channels != pVOW->m_fmtAudio.nChannels)
	{
		pVOW->m_fmtAudio.nSampleRate = pAudioFormat->SampleRate;
		pVOW->m_fmtAudio.nChannels = pAudioFormat->Channels;
		pVOW->m_fmtAudio.nSampleBits = pAudioFormat->SampleBits;
		pVOW->m_pCallBack (pVOW->m_pUserData, VOAP_IDC_setAudioFormat, &pVOW->m_fmtAudio, NULL);
	}

	pVOW->m_pCallBack (pVOW->m_pUserData, VOAP_IDC_RenderAudio, pAudioBuffer->Buffer, &pAudioBuffer->Length);

	return 0;
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

VO_U32 CVOWPlayer::voSourceDrmCallBack (VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved)
 {
	 return 0;
 }
