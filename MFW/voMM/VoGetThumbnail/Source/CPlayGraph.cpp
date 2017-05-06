	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "voOSFunc.h"
#include "CBaseConfig.h"

#include "CPlayGraph.h"
#include "CRTSPSource.h"

//#define _VOLOG_STATUS
//#define _VOLOG_RUN
//#define _VOLOG_FUNC

#include "voLog.h"

#pragma warning (disable : 4996)

CPlayGraph::CPlayGraph(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: CBasePlay (hInst, pMemOP, pFileOP)
	, m_nThreadNum (2)
	, m_pSource (NULL)
	, m_nAudioTrack (-1)
	, m_nVideoTrack (-1)
	, m_pAudioDec (NULL)
	, m_pVideoDec (NULL)
	, m_pAudioRender (NULL)
	, m_pVideoRender (NULL)
	, m_nZoomMode (VO_ZM_LETTERBOX)
	, m_status (VOMM_PLAY_StatusLoad)
	, m_nPlaySpeed (0X00010001)
	, m_nPlayFlag (0X03)
	, m_nSeekMode (1)
	, m_bVOSDK (VO_FALSE)
	, m_bForThumbnail (VO_FALSE)
	, m_llVideoStartPos (0)
	, m_nStartTime (0)
	, m_nPlayedTime (0)
	, m_nDuration (0)
	, m_nCurPos (0)
	, m_nFirstAudioPos (0XFFFFFFFF)
	, m_nFirstVideoPos (0XFFFFFFFF)
	, m_bAudioEndOfStream (VO_TRUE)
	, m_bVideoEndOfStream (VO_TRUE)
	, m_nSeekTime (0)
	, m_hThreadAudio (NULL)
	, m_bStopAudio (VO_TRUE)
	, m_bAudioPlaying (VO_FALSE)
	, m_hThreadVideo (NULL)
	, m_bStopVideo (VO_TRUE)
	, m_bVideoPlaying (VO_FALSE)
	, m_pAudioRenderData (NULL)
	, m_nAudioRenderSize (0)
	, m_nAudioRenderStep (0)
	, m_nAudioBytesPerSec (44100)
	, m_nAudioBufferTime (400)
	, m_nAudioRenderStart (0)
	, m_nAudioRenderFill (0)
	, m_nAudioRenderCount (0)
	, m_nVideoRenderTime (0)
	, m_nVideoRenderCount (0)
	, m_nVideoRenderSpeed (0)
	, m_nVideoRenderDropTime (80)
	, m_bVideoRenderDrop (VO_FALSE)
	, m_bVideoPlayNextFrame (VO_FALSE)
#ifdef _WIN32
	, m_hThreadIdle (NULL)
	, m_bStopIdle (VO_TRUE)
	, m_nThreadIdle (1)
#endif // _WIN32
{
	VOLOGF ();

	m_fmtAudio.Channels = 2;
	m_fmtAudio.SampleRate = 44100;
	m_fmtAudio.SampleBits = 16;

	m_fmtVideo.Width = 320;
	m_fmtVideo.Height = 240;
	m_fmtVideo.Type = VO_VIDEO_FRAME_NULL;

	m_rcDisp.left = 0;
	m_rcDisp.top = 0;
	m_rcDisp.right = 0;
	m_rcDisp.bottom = 0;

	memset (&m_audioSourceSample, 0, sizeof (VO_SOURCE_SAMPLE));
	memset (&m_audioSourceBuffer, 0, sizeof (VO_CODECBUFFER));
	memset (&m_audioRenderBuffer, 0, sizeof (VO_CODECBUFFER));

	memset (&m_videoSourceSample, 0, sizeof (VO_SOURCE_SAMPLE));
	memset (&m_videoSourceBuffer, 0, sizeof (VO_CODECBUFFER));
	memset (&m_videoRenderBuffer, 0, sizeof (VO_VIDEO_BUFFER));

	memset (&m_videoNextKeySample, 0, sizeof (VO_SOURCE_SAMPLE));

	m_pCodecCfg = new CBaseConfig ();
	m_pCodecCfg->Open (_T("vommcodec.cfg"), m_pFileOP);

	m_cbVideoRender.pCallBack = NULL;
	m_cbVideoRender.pUserData = NULL;

	m_cbAudioRender.pCallBack = NULL;
	m_cbAudioRender.pUserData = NULL;

	m_cbStreaming.pCallBack = NULL;
	m_cbStreaming.pUserData = NULL;

	m_bReleased = VO_FALSE;
	m_nBuffering = 0;

	vostrcpy (m_szError, _T("No Error."));
}

CPlayGraph::~CPlayGraph ()
{
	VOLOGF ();
	m_bReleased = VO_TRUE;

	Release ();

	if (m_pAudioRenderData != NULL)
		m_pMemOP->Free (0, m_pAudioRenderData);

	delete m_pCodecCfg;
}

VO_U32 CPlayGraph::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	VOLOGF ();

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CPlayGraph::SetViewInfo (VO_PTR hView, VO_RECT * pRect)
{
	VOLOGF ();

	VO_U32 nRC = CBasePlay::SetViewInfo (hView, pRect);

	if (m_pVideoRender != NULL)
		m_pVideoRender->SetDispRect (m_hView, &m_rcView);

	return nRC;
}

VO_U32 CPlayGraph::Run (void)
{
	VOLOGF ();

	if (m_pSource == NULL)
		return VO_ERR_WRONG_STATUS;

	if (m_status == VOMM_PLAY_StatusRun)
		return VO_ERR_NONE;

	voCAutoLock lock(&m_csPlay);

	if (m_pAudioRender != NULL)
		m_pAudioRender->Start ();
	if (m_pVideoRender != NULL)
		m_pVideoRender->Start ();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Start ();
	if (m_pVideoDec != NULL)
		m_pVideoDec->Start ();

	if (m_pSource != NULL)
		m_pSource->Start ();

	VO_U32	nRC = 0;
	VO_U32	nID = 0;

	if (m_nThreadNum >= 1)
	{
		if (m_pAudioRender != NULL && m_hThreadAudio == NULL)
		{
			m_bStopAudio = VO_FALSE;

			nRC = voThreadCreate (&m_hThreadAudio, &nID, PlaybackAudioLoopProc, this, 3);
			if (m_hThreadAudio == NULL)
				return VO_ERR_NOT_IMPLEMENT;
		}
	}

	if (m_pVideoRender != NULL)
		m_pVideoRender->Start ();

	if (m_nThreadNum >= 2)
	{
		if (m_pVideoRender != NULL && m_hThreadVideo == NULL)
		{
			m_bStopVideo = VO_FALSE;

			nRC = voThreadCreate (&m_hThreadVideo, &nID, PlaybackVideoLoopProc, this, 0);
			if (m_hThreadVideo == NULL)
				return VO_ERR_NOT_IMPLEMENT;
		}
	}

	m_bAudioEndOfStream = VO_TRUE;
	m_bVideoEndOfStream = VO_TRUE;
	if (m_pAudioRender != NULL)
		m_bAudioEndOfStream = VO_FALSE;
	if (m_pVideoRender != NULL)
		m_bVideoEndOfStream = VO_FALSE;

	m_nStartTime = voOS_GetSysTime ();
	m_status = VOMM_PLAY_StatusRun;

#ifdef _WIN32
	if (m_nThreadIdle > 0 && m_hThreadIdle == NULL)
	{
		m_bStopIdle = VO_FALSE;
		nRC = voThreadCreate (&m_hThreadIdle, &nID, PlaybackIdleLoopProc, this, 0);
	}
#endif //_WIN32

	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::Pause (void)
{
	VOLOGF ();

	if (m_status == VOMM_PLAY_StatusStop)
		return VO_ERR_WRONG_STATUS;

	if (m_status == VOMM_PLAY_StatusLoad || m_status == VOMM_PLAY_StatusPause)
		return VO_ERR_NONE;

	voCAutoLock lock(&m_csPlay);
	m_status = VOMM_PLAY_StatusPause;

	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();
	if (m_pVideoRender != NULL)
		m_pVideoRender->Pause ();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Pause ();
//	if (m_pVideoDec != NULL)
//		m_pVideoDec->Pause ();

	if (m_pSource != NULL)
		m_pSource->Pause ();

	while (m_bAudioPlaying || m_bVideoPlaying)
		voOS_Sleep (10);

	m_nPlayedTime = m_videoSourceSample.Time;

	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::Stop (void)
{
	VOLOGF ();

	if (m_status == VOMM_PLAY_StatusLoad || m_status == VOMM_PLAY_StatusStop)
		return VO_ERR_NONE;

	voCAutoLock lock(&m_csPlay);
	m_status = VOMM_PLAY_StatusStop;

	if (m_pAudioRender != NULL)
		m_pAudioRender->Stop ();
	if (m_pVideoRender != NULL)
		m_pVideoRender->Stop ();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Stop ();
	if (m_pVideoDec != NULL)
		m_pVideoDec->Stop ();

	if (m_pSource != NULL)
		m_pSource->Stop ();

	m_bStopAudio = VO_TRUE;
	m_bStopVideo = VO_TRUE;

	while (m_hThreadAudio != NULL)
		voOS_Sleep (2);

	while (m_hThreadVideo != NULL)
		voOS_Sleep (2);

#ifdef _WIN32
	m_bStopIdle = VO_TRUE;
	while (m_hThreadIdle != NULL)
		voOS_Sleep (2);
#endif // _WIN32

	m_llVideoStartPos = 0;

	m_nStartTime = 0;
	m_nPlayedTime = 0;

	m_videoSourceSample.Time = 0;
	m_audioSourceSample.Time = 0;

	m_nAudioRenderStart = 0;
	m_nAudioRenderFill = 0;
	m_nAudioRenderCount = 0;

	m_nVideoRenderTime = 0;
	m_nVideoRenderCount = 0;
	m_bVideoRenderDrop = VO_FALSE;

	m_videoNextKeySample.Time = 0;
	m_bVideoPlayNextFrame = VO_FALSE;

	m_bAudioEndOfStream = VO_TRUE;
	m_bVideoEndOfStream = VO_TRUE;

	EndOfSource ();

	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::GetDuration (VO_U32 * pDuration)
{
	VOLOGF ();

	*pDuration = m_nDuration;
	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::GetCurPos (VO_S32 * pPos)
{
	VOLOGF ();

	*pPos = m_nCurPos;
	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::SetCurPos (VO_S32 nPos)
{
	VOLOGF ();

	if (m_pSource == NULL)
		return VO_ERR_WRONG_STATUS;

	if (!m_pSource->CanSeek ())
	{
		VOLOGE ("The status is wrong in source!");
		return VO_ERR_WRONG_STATUS;
	}

	if (voOS_GetSysTime () - m_nSeekTime < 1000)
		return VO_ERR_NOT_IMPLEMENT;
	m_nSeekTime = voOS_GetSysTime ();

	VOMMPlAYSTATUS status = m_status;
	if (status == VOMM_PLAY_StatusRun)
		Pause ();

	if (m_pAudioRender != NULL)
		m_pAudioRender->Flush ();
	if (m_pAudioDec != NULL)
		m_pAudioDec->Flush ();
	if (m_pVideoDec != NULL)
		m_pVideoDec->Flush ();

	VO_U32 nRC = 0;
	VO_S64 nNewPos = nPos;
	if (m_nVideoTrack >= 0)
	{
		if (m_nFirstVideoPos != 0XFFFFFFFF)
			nNewPos = m_nFirstVideoPos + nPos;
		else
			nNewPos = nPos;
		m_llVideoStartPos = nNewPos;

		nRC = m_pSource->SetTrackPos (m_nVideoTrack, &nNewPos);
		if (nRC != VO_ERR_NONE && nRC != VO_ERR_SOURCE_END)
		{
			if (status == VOMM_PLAY_StatusRun)
				Run ();
			return nRC;
		}
		m_videoSourceSample.Time = nNewPos;
	}
	if (m_nAudioTrack >= 0)
	{
		if (m_nSeekMode == 1 && m_nVideoTrack >= 0)
			nNewPos = m_llVideoStartPos;

		nRC = m_pSource->SetTrackPos (m_nAudioTrack, &nNewPos);
		if (nRC != VO_ERR_NONE && nRC != VO_ERR_SOURCE_END)
		{
			if (status == VOMM_PLAY_StatusRun)
				Run ();
			return nRC;
		}
		m_audioSourceSample.Time = nNewPos;
	}

	m_nAudioRenderStart = 0;
	m_nAudioRenderFill = 0;
	m_nAudioRenderCount = 0;

	m_nVideoRenderTime = 0;
	m_nVideoRenderCount = 0;
	m_bVideoRenderDrop = VO_FALSE;

	m_videoNextKeySample.Time = 0;
	m_bVideoPlayNextFrame = VO_FALSE;

	if (m_nAudioTrack >= 0)
	{
		if (m_nFirstAudioPos <= nNewPos)
			m_nCurPos = (VO_S32)(nNewPos - m_nFirstAudioPos);
		else
			m_nCurPos = 0;
	}
	else
	{
		if (m_nFirstVideoPos <= nNewPos)
			m_nCurPos = (VO_S32)(nNewPos - m_nFirstVideoPos);
		else
			m_nCurPos = 0;
	}

	m_nStartTime = voOS_GetSysTime ();
	m_nPlayedTime = m_videoSourceSample.Time;

	if (status == VOMM_PLAY_StatusRun)
	{
		Run ();
	}
	else if (status == VOMM_PLAY_StatusPause)
	{
		// display the new pos frame image
		if (m_nVideoTrack >= 0)
		{
			VO_U32 nTryTimes = 0;

			while (m_nVideoRenderCount <= 1)
			{
				PlaybackVideoFrame ();

				nTryTimes++;
				if (nTryTimes > 20)
					break;
			}
		}
	}

	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::SetParam (VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

//	voCAutoLock lock(&m_csPlay);

	if (nID == VO_VOMMP_PMID_Redraw)
	{
		if (pValue != NULL)
		{
			VO_RECT * pRect = (VO_RECT *)pValue;

			if (memcmp (&m_rcDisp, pRect, sizeof (VO_RECT)))
			{
				if (m_pVideoRender != NULL)
					m_pVideoRender->SetDispRect (m_hView, pRect);

				m_nVideoRenderCount = 0;
				m_nVideoRenderSpeed = 0;

				memcpy (&m_rcDisp, pRect, sizeof (VO_RECT));
			}
		}
		if (m_pVideoRender != NULL)
			m_pVideoRender->Redraw ();

		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_SeekMode)
	{
		m_nSeekMode = *(VO_U32 *)pValue;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_PlaySpeed)
	{
		m_nPlaySpeed = *(VO_U32 *)pValue;
		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (nID == VO_VOMMP_PMID_PlayFlagAV)
	{
		m_nPlayFlag = *(VO_U32 *)pValue;
		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (nID == VO_VOMMP_PMID_ThreadNum)
	{
		m_nThreadNum = *(VO_U32 *)pValue;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_VOSDK)
	{
		m_bVOSDK = *(VO_BOOL *)pValue;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_ForThumbnail)
	{
		m_bForThumbnail = *(VO_BOOL *)pValue;
		if (m_bForThumbnail)
			m_nZoomMode = VO_ZM_FITWINDOW;

		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_Playback)
	{
		if (m_nThreadNum != 0)
			return VO_ERR_NOT_IMPLEMENT;

		if (pValue != NULL)
			m_nPlayFlag = *(VO_U32 *)pValue;

		PlaybackAudioVideo (m_nPlayFlag);

		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_VideoRender)
	{
		if (pValue != NULL)
		{
			VO_VOMM_CB_VIDEORENDER * pCallBack = (VO_VOMM_CB_VIDEORENDER *)pValue;
			m_cbVideoRender.pCallBack = pCallBack->pCallBack;
			m_cbVideoRender.pUserData = pCallBack->pUserData;
			if (m_pVideoRender != NULL)
			{
				m_pVideoRender->SetCallBack (m_cbVideoRender.pCallBack, m_cbVideoRender.pUserData);
			}
			return VO_ERR_NONE;
		}

		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (nID == VO_VOMMP_PMID_AudioRender)
	{
		if (pValue != NULL)
		{
			VO_VOMM_CB_AUDIORENDER * pCallBack = (VO_VOMM_CB_AUDIORENDER *)pValue;
			m_cbAudioRender.pCallBack = pCallBack->pCallBack;
			m_cbAudioRender.pUserData = pCallBack->pUserData;
			if (m_pAudioRender != NULL)
			{
				m_pAudioRender->SetCallBack (m_cbAudioRender.pCallBack, m_cbAudioRender.pUserData);
			}
			return VO_ERR_NONE;
		}

		return VO_ERR_NOT_IMPLEMENT;
	}
	else if (nID == VO_VOMMP_PMID_StreamCallBack)
	{
		VO_VOMM_CB_STREAMING * pStreamCB = (VO_VOMM_CB_STREAMING *)pValue;
		m_cbStreaming.pCallBack = pStreamCB->pCallBack;
		m_cbStreaming.pUserData = pStreamCB->pUserData;

		if (m_pSource != NULL)
			m_pSource->SetCallBack ((VO_PTR)m_cbStreaming.pCallBack, m_cbStreaming.pUserData);

		return VO_ERR_NONE;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CPlayGraph::GetParam (VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

//	voCAutoLock lock(&m_csPlay);

	if (pValue == NULL)
		return VO_ERR_NOT_IMPLEMENT;

	if (nID == VO_VOMMP_PMID_VideoFormat)
	{
		VO_VIDEO_FORMAT * pVideoFormat = (VO_VIDEO_FORMAT *)pValue;
		if (m_nVideoTrack < 0)
		{
			pVideoFormat->Width = 0;
			pVideoFormat->Height = 0;
		}
		else
		{
			memcpy (pValue, &m_fmtVideo, sizeof (VO_VIDEO_FORMAT));

			VO_SOURCE_TRACKINFO	trkInfo;
			m_pSource->GetTrackInfo (m_nVideoTrack, &trkInfo);
			pVideoFormat->Type = (VO_VIDEO_FRAMETYPE) trkInfo.Codec;
		}

		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_AudioFormat)
	{
		VO_AUDIO_FORMAT * pAudioFormat = (VO_AUDIO_FORMAT *)pValue;
		if (m_nAudioTrack < 0)
		{
			pAudioFormat->SampleRate = 0;
			pAudioFormat->Channels = 0;
			pAudioFormat->SampleBits = 0;
		}
		else
		{
			memcpy (pValue, &m_fmtAudio, sizeof (VO_AUDIO_FORMAT));

			VO_SOURCE_TRACKINFO	trkInfo;
			m_pSource->GetTrackInfo (m_nAudioTrack, &trkInfo);
			pAudioFormat->SampleBits = trkInfo.Codec;
		}

		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_FileFormat)
	{
		VO_U32 * pFormat = (VO_U32 *)pValue;
		if (m_pSource != NULL)
		{
			*pFormat = m_pSource->GetFormat ();
			return VO_ERR_NONE;
		}
		else
		{
			return VO_ERR_WRONG_STATUS;
		}
	}
	else if (nID == VO_VOMMP_PMID_SeekMode)
	{
		*(VO_U32 *)pValue = m_nSeekMode;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_PlaySpeed)
	{
		*(VO_U32 *)pValue = m_nPlaySpeed;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_PlayFlagAV)
	{
		*(VO_U32 *)pValue = m_nPlayFlag;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_DrawRect)
	{
		if (m_pVideoRender != NULL)
			m_pVideoRender->GetParam (VO_VR_PMID_DrawRect, pValue);

		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_ThreadNum)
	{
		 *(VO_U32 *)pValue = m_nThreadNum;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_LastError)
	{
		*(VO_TCHAR **)pValue = m_szError;
		return VO_ERR_NONE;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CPlayGraph::SetConfig (CBaseConfig * pConfig)
{
	VOLOGF ();

	m_pConfig = pConfig;

	UpdateWithConfig ();

	return 0;
}

VO_U32 CPlayGraph::AllocAudioRenderBuffer (void)
{
	VOLOGF ();

	m_nAudioBytesPerSec =  m_fmtAudio.SampleRate * m_fmtAudio.Channels * m_fmtAudio.SampleBits / 8;
	if (m_nAudioBytesPerSec == 0)
		m_nAudioBytesPerSec = 44100;

	m_nAudioRenderSize = m_nAudioBytesPerSec;
	if (m_nAudioRenderSize < 32000)
		m_nAudioRenderSize = 32000;

	if (m_nAudioRenderStep == 0)
	{
		if (m_nThreadNum >= 2)
			m_nAudioRenderStep = m_nAudioBytesPerSec / 10;
		else
			m_nAudioRenderStep = m_nAudioBytesPerSec / 20;
	}
	else if (m_nAudioRenderStep > 0 && m_nAudioRenderStep < 1000)
	{
		m_nAudioRenderStep = m_nAudioBytesPerSec * m_nAudioRenderStep / 1000;
	}

	if (m_pAudioRenderData != NULL)
		m_pMemOP->Free (0, m_pAudioRenderData);

	VO_MEM_INFO memInfo;
	memInfo.Size = m_nAudioRenderSize;
	m_pMemOP->Alloc (0, &memInfo);
	m_pAudioRenderData = (VO_PBYTE)memInfo.VBuffer;

	return VO_ERR_NONE;
}

VO_S64 CPlayGraph::GetPlayingTime (void)
{
	VOLOGF ();

	VO_S64 nPlayingTime = 0;

	if (((m_nPlayFlag & 0X02) == 0) || m_pAudioRender == NULL)
	{
		if (m_nStartTime == 0)
			m_nStartTime = voOS_GetSysTime ();

		nPlayingTime = voOS_GetSysTime() - m_nStartTime + m_nPlayedTime;
	}
	else
	{
		nPlayingTime = m_pAudioRender->GetPlayingTime ();
	}

	return nPlayingTime;
}

VO_U32 CPlayGraph::VideoWaitForPlayingTime (VO_S64 nVideoTime)
{
	VOLOGF ();

	VO_S64 nPlayingTime = GetPlayingTime () + 30;
	if (nPlayingTime < nVideoTime && !m_bVideoPlayNextFrame)
	{
		VO_S32 nSleepTime = (VO_S32)(nVideoTime - nPlayingTime);
		if (nSleepTime < 10)
		{
			voOS_Sleep (nSleepTime);
		}
		else
		{
			while (GetPlayingTime () + 30 < nVideoTime)
			{
				voOS_Sleep (2);
				if (m_status != VOMM_PLAY_StatusRun)
					break;
				if (m_nBuffering > 0)
					break;
			}
		}
	}

//	if (m_nThreadNum == 2 && m_pAudioRender != NULL)
//		voOS_Sleep (2);

	return 0;
}

VO_U32 CPlayGraph::Release (void)
{
	VOLOGF ();

	//if (m_status == VOMM_PLAY_StatusPause || m_status == VOMM_PLAY_StatusRun)
	//{
	//	Stop ();
	//}

	if (m_pSource != NULL && m_status < VOMM_PLAY_StatusStop)
		m_pSource->CloseSource ();

	voCAutoLock lock(&m_csPlay);

	if (m_pAudioDec != NULL)
		delete m_pAudioDec;
	m_pAudioDec = NULL;

	if (m_pVideoDec != NULL)
		delete m_pVideoDec;
	m_pVideoDec = NULL;

	if (m_pVideoRender != NULL)
		delete m_pVideoRender;
	m_pVideoRender = NULL;

	if (m_pAudioRender != NULL)
		delete m_pAudioRender;
	m_pAudioRender = NULL;

	if (m_pSource != NULL)
		delete m_pSource;
	m_pSource = NULL;

	m_nAudioTrack = -1;
	m_nVideoTrack = -1;

	m_nDuration = 0;
	m_nVideoRenderSpeed = 0;

	return VO_ERR_NONE;
}

VO_U32 CPlayGraph::UpdateWithConfig (void)
{
	VOLOGF ();

	if (m_pConfig == NULL)
		return 0;

	m_nThreadNum = m_pConfig->GetItemValue ("vowPlay", "ThreadNum", m_nThreadNum);
	m_nPlayFlag = m_pConfig->GetItemValue ("vowPlay", "PlayFlag", m_nPlayFlag);
	m_nSeekMode = m_pConfig->GetItemValue ("vowPlay", "SeekMode", m_nSeekMode);

#ifdef _WIN32
	m_nThreadIdle = m_pConfig->GetItemValue ("vowPlay", "ThreadIdle", m_nThreadIdle);
#endif // _WIN32

	m_nAudioBufferTime = m_pConfig->GetItemValue ("vowPlay", "AudioBufferTime", m_nAudioBufferTime);
	m_nAudioRenderStep = m_pConfig->GetItemValue ("vowPlay", "AudioStepTime", 0);

	m_nZoomMode = (VO_IV_ZOOM_MODE)m_pConfig->GetItemValue ("vowPlay", "VideoZoomMode", m_nZoomMode);
	m_nVideoRenderDropTime = m_pConfig->GetItemValue ("vowPlay", "VideoDropTime", m_nVideoRenderDropTime);

	return 1;
}

VO_U32 CPlayGraph::PlaybackAudioLoopProc (VO_PTR pParam)
{
	VOLOGF ();

	CPlayGraph * pPlay = (CPlayGraph *)pParam;

	if (pPlay->m_nThreadNum >= 2)
		pPlay->PlaybackAudioLoop ();
	else
		pPlay->PlaybackAudioVideoLoop ();

	return 0;
}

VO_U32 CPlayGraph::PlaybackAudioLoop (void)
{
	VOLOGF ();

#ifdef _WIN32
	voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif // _WIN32

	m_bAudioEndOfStream = VO_FALSE;

	while (!m_bStopAudio)
	{
		if (m_status == VOMM_PLAY_StatusRun && (m_nPlayFlag & 0X02) == 2)
		{
			if (m_pAudioRender != NULL)
			{
				if (m_pAudioRender->GetBufferTime () <= m_nAudioBufferTime)
					PlaybackAudioFrame ();
				else
					voOS_Sleep (5);
			}

			m_bAudioPlaying = VO_TRUE;
		}
		else
		{
			m_bAudioPlaying = VO_FALSE;
			voOS_Sleep (5);
		}
	}

	m_bAudioPlaying = VO_FALSE;
	m_hThreadAudio = NULL;

	if (m_bVideoEndOfStream && m_status != VOMM_PLAY_StatusStop)
	{
		EndOfSource ();

		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VO_VOMMP_CBID_EndOfFile, 0, 0);
	}

	return 0;
}

VO_U32 CPlayGraph::PlaybackAudioFrame (void)
{
	VOLOGF ();

	// if the seek mode is accurate, wait for the video had decoded reach the start pos.
	if (!m_bVideoEndOfStream && m_llVideoStartPos > m_videoSourceSample.Time)
	{
		voOS_Sleep (5);
		return 0;
	}

	VO_U32	nRC = 0;

	if (m_pSource->HasError () != 0)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VO_VOMMP_CBID_Error, 0, 0);
		return VO_ERR_WRONG_STATUS;
	}

	// Read the audio data from source.
	//nRC = m_pSource->GetTrackData (m_nAudioTrack, &m_audioSourceSample);
	nRC = ReadAudioData (m_nAudioTrack, &m_audioSourceSample);
	m_audioSourceSample.Size = m_audioSourceSample.Size & 0X7FFFFFFF;
	if (nRC != VO_ERR_SOURCE_OK)
	{
		if (nRC == VO_ERR_SOURCE_END)
		{
			//if have not sent at least one frame, we will force send a null frame
			if(!m_nAudioRenderCount && !m_bVideoEndOfStream)
				m_pAudioRender->Render (NULL, 0, m_videoSourceSample.Time);		//m_nAudioRenderFrames can not ++;

			m_bAudioEndOfStream = VO_TRUE;
			if (m_nThreadNum == 2)
				m_bStopAudio = VO_TRUE;

			return 0;
		}
		else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
		{
			voOS_Sleep (2);
			return 0;
		}
		else
		{
			voOS_Sleep (2);
			return 1;
		}
	}

	// set the current pos
	if (m_nFirstAudioPos == 0XFFFFFFFF)
		m_nFirstAudioPos = m_audioSourceSample.Time;
	m_nCurPos = (VO_U32) (m_audioSourceSample.Time - m_nFirstAudioPos);

	m_nAudioRenderStart = m_audioSourceSample.Time - (m_nAudioRenderFill * 1000) / m_nAudioBytesPerSec;

	m_audioSourceBuffer.Buffer = m_audioSourceSample.Buffer;
	m_audioSourceBuffer.Length = m_audioSourceSample.Size;
	m_audioSourceBuffer.Time  = m_audioSourceSample.Time;

	nRC = m_pAudioDec->SetInputData (&m_audioSourceBuffer);

//	printf ("VOLOG CPlayGraph::PlaybackAudioFrame, SetInputData result %d,	Size: %d \r\n", nRC, m_audioSourceBuffer.Length);
//	m_pCallBack (m_pUserData, VO_VOMMP_CBID_DebugText, (VO_PTR) m_szError, 0);

	if (nRC != VO_ERR_NONE)
		return 0;

	m_audioRenderBuffer.Buffer = m_pAudioRenderData + m_nAudioRenderFill;
	m_audioRenderBuffer.Length = m_nAudioRenderSize - m_nAudioRenderFill;
	VO_AUDIO_FORMAT		fmtAudio;
	while (VO_TRUE)
	{
		nRC = DecoderAudioData (&m_audioRenderBuffer, &fmtAudio);

//		printf ("VOLOG CPlayGraph::PlaybackAudioFrame, DecoderAudioData result %d,	Size: %d \r\n", nRC, m_audioSourceBuffer.Length);
//		m_pCallBack (m_pUserData, VO_VOMMP_CBID_DebugText, (VO_PTR) m_szError, 0);

		if (nRC == (VO_ERR_INPUT_BUFFER_SMALL))
			break;

		if (m_nThreadNum > 0)
		{
			if (m_status != VOMM_PLAY_StatusRun)
				break;
		}

		if (nRC == VO_ERR_NONE && fmtAudio.SampleRate != 0)
		{
			if (m_fmtAudio.Channels != fmtAudio.Channels || m_fmtAudio.SampleRate != fmtAudio.SampleRate)
			{
				if (m_nAudioRenderFill > 0)
				{
					RenderAudioData (m_pAudioRenderData, m_nAudioRenderFill, m_nAudioRenderStart);
					m_nAudioRenderStart = m_nAudioRenderStart + (m_nAudioRenderFill * 1000) / m_nAudioBytesPerSec;
				}

				m_fmtAudio.SampleRate = fmtAudio.SampleRate;
				m_fmtAudio.Channels = fmtAudio.Channels;
				m_pAudioRender->SetFormat (&m_fmtAudio);

				if (m_audioRenderBuffer.Length > 0)
				{
					RenderAudioData (m_audioRenderBuffer.Buffer, m_audioRenderBuffer.Length, m_nAudioRenderStart);
					m_nAudioRenderStart = m_nAudioRenderStart + (m_audioRenderBuffer.Length * 1000) / m_nAudioBytesPerSec;
				}

				m_nAudioRenderFill = 0;
				m_audioRenderBuffer.Length = 0;

				AllocAudioRenderBuffer ();
			}
		}

		m_nAudioRenderFill = m_nAudioRenderFill + m_audioRenderBuffer.Length;
		if (m_nAudioRenderFill > m_nAudioRenderStep)
		{
			RenderAudioData (m_pAudioRenderData, m_nAudioRenderFill, m_nAudioRenderStart);
			m_nAudioRenderStart = m_nAudioRenderStart + (m_nAudioRenderFill * 1000) / m_nAudioBytesPerSec;
			m_nAudioRenderFill = 0;
		}

		m_audioRenderBuffer.Buffer = m_pAudioRenderData + m_nAudioRenderFill;
		m_audioRenderBuffer.Length = m_nAudioRenderSize - m_nAudioRenderFill;
	}

	return 0;
}

VO_U32 CPlayGraph::PlaybackVideoLoopProc (VO_PTR pParam)
{
	VOLOGF ();

	CPlayGraph * pPlay = (CPlayGraph *)pParam;

	pPlay->PlaybackVideoLoop ();

	return 0;
}

VO_U32 CPlayGraph::PlaybackVideoLoop (void)
{
	VOLOGF ();

#ifdef _WIN32
	voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_NORMAL);
#endif // _WIN32

	m_bVideoEndOfStream = VO_FALSE;

	while (!m_bStopVideo)
	{
		if (m_status == VOMM_PLAY_StatusRun && (m_nPlayFlag & 0X01) == 1)
		{
			// if (m_pVideoRender != NULL)
			{
				PlaybackVideoFrame ();
			}

			m_bVideoPlaying = VO_TRUE;
		}
		else
		{
			m_bVideoPlaying = VO_FALSE;
			voOS_Sleep (5);
		}
	}

	m_bVideoPlaying = VO_FALSE;
	m_hThreadVideo = NULL;

	if (m_bAudioEndOfStream && m_status != VOMM_PLAY_StatusStop)
	{
		EndOfSource ();

		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VO_VOMMP_CBID_EndOfFile, 0, 0);
	}

	return 0;
}

VO_U32 CPlayGraph::PlaybackVideoFrame (void)
{
	VOLOGF ();

	VO_U32 nRC = VO_ERR_NONE;
	if (m_nThreadNum >= 2)
	{
		// Read the video data from source
		if (m_nVideoRenderCount > 0 && GetPlayingTime () > 0)
		{
			m_videoSourceSample.Time = GetPlayingTime ();
		}

		if (m_pSource->HasError () != 0)
		{
			if (m_pCallBack != NULL)
				m_pCallBack (m_pUserData, VO_VOMMP_CBID_Error, 0, 0);
			return VO_ERR_WRONG_STATUS;
		}
		nRC =ReadVideoData (m_nVideoTrack, &m_videoSourceSample);
		if (nRC != VO_ERR_SOURCE_OK)
		{
			if (nRC == VO_ERR_SOURCE_END)
			{
				m_bVideoEndOfStream = VO_TRUE;
				m_bStopVideo = VO_TRUE;
				return 0;
			}
			else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
			{
				voOS_Sleep (2);
				return 0;
			}
			else
			{
				voOS_Sleep (2);
				return 1;
			}
		}
	}

	// set the current pos
	if (m_nFirstVideoPos == 0XFFFFFFFF)
	{
		m_nFirstVideoPos = m_videoSourceSample.Time;
		m_llVideoStartPos = m_nFirstVideoPos;
	}
	if (m_nAudioTrack < 0 ||( (m_nPlayFlag & 0X02) == 0))
		m_nCurPos = (VO_U32) (m_videoSourceSample.Time - m_nFirstVideoPos);

/*
	// detect to drop the frames or not
	if (m_nVideoRenderCount > 12 && m_nVideoRenderSpeed < m_nVideoRenderDropTime)
	{
		if (m_videoNextKeySample.Time <= m_videoSourceSample.Time)
		{
			m_videoNextKeySample.Time = 0;
			m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_NEXTKEYFRAME, &m_videoNextKeySample);
		}
		if (m_videoNextKeySample.Time > m_videoSourceSample.Time + 200)
		{
			if (GetPlayingTime () > m_videoSourceSample.Time + (m_videoNextKeySample.Time - m_videoSourceSample.Time) / 2)
			{	if (m_nThreadNum > 0)
		{

				if (m_videoNextKeySample.Time + 40 > GetPlayingTime ())
				{
					m_videoSourceSample.Time = m_videoNextKeySample.Time + 40;
					m_bVideoPlayNextFrame = VO_TRUE;
				}
				else
				{
					m_videoSourceSample.Time = GetPlayingTime ();
				}

				return 0;
			}
		}
		if (m_bVideoPlayNextFrame && GetPlayingTime () > m_videoSourceSample.Time)
			m_bVideoPlayNextFrame = VO_FALSE;
	}
*/

	m_videoSourceBuffer.Buffer = m_videoSourceSample.Buffer;
	m_videoSourceBuffer.Length = m_videoSourceSample.Size & 0X7FFFFFFF;

	// detect the frame type. If it was B frame, drop it ot not.
	if (!m_bVideoRenderDrop && GetPlayingTime () > m_videoSourceSample.Time)
	{
		if (m_pVideoDec->GetFrameType (&m_videoSourceBuffer) == VO_VIDEO_FRAME_B)
		{
			m_bVideoRenderDrop = VO_TRUE;
			return 0;
		}
	}

	m_videoSourceBuffer.Time  = m_videoSourceSample.Time;
	nRC = m_pVideoDec->SetInputData (&m_videoSourceBuffer);
	//nRC = SetInputVideoData (&m_videoSourceBuffer);
	if (nRC != VO_ERR_NONE)
		return 0;

	VO_VIDEO_FORMAT		fmtVideo;
	while (VO_TRUE)
	{
		nRC = DecoderVideoData (&m_videoRenderBuffer, &fmtVideo);
		if (nRC == (VO_ERR_INPUT_BUFFER_SMALL))
			break;

		if (m_videoRenderBuffer.Buffer[0] == NULL)
			break;

		if (m_nThreadNum > 0)
		{
			if (m_nVideoRenderCount > 1 && m_status != VOMM_PLAY_StatusRun)
				break;
		}

		if (fmtVideo.Width != m_fmtVideo.Width || fmtVideo.Height != m_fmtVideo.Height)
		{
			m_fmtVideo.Width = fmtVideo.Width;
			m_fmtVideo.Height = fmtVideo.Height;
			m_pVideoRender->SetVideoInfo (m_fmtVideo.Width, m_fmtVideo.Height, VO_COLOR_YUV_PLANAR420);
		}

		if (m_videoRenderBuffer.Time == 0)
		{
			if (m_nVideoRenderTime == m_videoSourceSample.Time)
				m_nVideoRenderTime += 30;
			else
				m_nVideoRenderTime = m_videoSourceSample.Time;
		}
		else
		{
			if (m_nVideoRenderTime == m_videoRenderBuffer.Time)
				m_nVideoRenderTime += 30;
			else
				m_nVideoRenderTime = m_videoRenderBuffer.Time;
		}

		// keep AV sync
		if (m_nVideoRenderCount > 0 && m_llVideoStartPos < m_nVideoRenderTime)
		{
			if (m_nThreadNum == 2)
			{
				VideoWaitForPlayingTime (m_nVideoRenderTime);
			}
			else
			{
				if (m_nPlayFlag & 0X02)
				{
					while (GetPlayingTime () + 30 < m_nVideoRenderTime)
					{
						// if it was single thread, make sure audio have enough data.
						if (m_pAudioRender != NULL)
						{
							if (m_pAudioRender->GetBufferTime () < m_nAudioBufferTime)
								PlaybackAudioFrame ();
						}

						if (m_status != VOMM_PLAY_StatusRun)
							break;
					}
				}
			}
		}

		// detect the video render speed.
		if (m_nVideoRenderCount == 0)
		{
			// render the fist frame immediately
			RenderVideoData (&m_videoRenderBuffer, m_nVideoRenderTime, VO_FALSE);
		}
		else if (m_nVideoRenderCount > 1 && m_nVideoRenderCount < 12)
		{
			// Get the render speed.
			VO_U32 nRenderTime = voOS_GetSysTime ();
			RenderVideoData (&m_videoRenderBuffer, m_nVideoRenderTime, VO_FALSE);
			m_nVideoRenderSpeed = m_nVideoRenderSpeed + voOS_GetSysTime () - nRenderTime;
		}
		else
		{
			// The render speed is slow, drop the vidoe render if it could not play real time
			if (m_nVideoRenderSpeed > m_nVideoRenderDropTime)
			{
				VO_S64 nPlayingTime = GetPlayingTime ();
				if (nPlayingTime > m_nVideoRenderTime + 100)
				{
					if (m_bVideoRenderDrop)
						RenderVideoData (&m_videoRenderBuffer, m_nVideoRenderTime, VO_FALSE);
					else
						m_bVideoRenderDrop = VO_TRUE;
				}
				else
				{
					RenderVideoData (&m_videoRenderBuffer, m_nVideoRenderTime, VO_FALSE);
				}
			}
			else
			{
				RenderVideoData (&m_videoRenderBuffer, m_nVideoRenderTime, VO_FALSE);
			}
		}

		// detect the frame type. If it was B frame, drop it ot not.
		if (!m_bVideoRenderDrop && GetPlayingTime () > m_nVideoRenderTime)
		{
			if (m_pVideoDec->GetFrameType (NULL) == VO_VIDEO_FRAME_B)
			{
				m_videoSourceSample.Time = GetPlayingTime ();
				m_bVideoRenderDrop = VO_TRUE;
				return 0;
			}
		}
	}

	return 0;
}

VO_U32 CPlayGraph::ReadAudioData (VO_U32 nTrack, VO_PTR pTrackData)
{
	VOLOGF ();

	return m_pSource->GetTrackData (nTrack, pTrackData);
}

VO_U32 CPlayGraph::DecoderAudioData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
	VOLOGF ();

	return m_pAudioDec->GetOutputData (pOutput, pAudioFormat);
}

VO_U32 CPlayGraph::RenderAudioData (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart)
{
	VOLOGF ();

	if (m_nBuffering > 0)
	{
		m_nBuffering--;
		// return 0;
	}

	m_nAudioRenderCount++;

	return m_pAudioRender->Render (pBuffer, nSize, nStart);
}

VO_U32 CPlayGraph::ReadVideoData (VO_U32 nTrack, VO_PTR pTrackData)
{
	VOLOGF ();

	return m_pSource->GetTrackData (nTrack, pTrackData);
}

VO_U32 CPlayGraph::SetInputVideoData (VO_CODECBUFFER * pInput)
{
	VOLOGF ();

	return m_pVideoDec->SetInputData (pInput);
}

VO_U32 CPlayGraph::DecoderVideoData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat)
{
	VOLOGF ();

	return  m_pVideoDec->GetOutputData (pOutput, pVideoFormat);
}

VO_U32 CPlayGraph::RenderVideoData (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGF ();

	VO_U32 nRC = 0;

	if (m_nPlayFlag & 0X02)
	{
		if (m_nThreadNum < 2 && m_pAudioRender != NULL)
		{
			if (m_pAudioRender->GetBufferTime () < m_nAudioBufferTime)
				PlaybackAudioFrame ();
			if (m_pAudioRender->GetBufferTime () < m_nAudioBufferTime)
				PlaybackAudioFrame ();
		}
	}

	if (m_llVideoStartPos > m_videoSourceSample.Time)
		return 0;

	m_bVideoRenderDrop = VO_FALSE;

	if (m_pVideoRender != NULL)
	{
		nRC =  m_pVideoRender->Render (pVideoBuffer, nStart, bWait);
	}

	m_nVideoRenderCount++;

	return nRC;
}

VO_U32 CPlayGraph::EndOfSource (void)
{
	VOLOGF ();

	return 0;
}

VO_U32 CPlayGraph::PlaybackAudioVideoLoop (void)
{
	VOLOGF ();

#ifdef _WIN32
	voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_NORMAL);
#endif // _WIN32

	VO_BOOL	bSleep = VO_TRUE;

	while (!m_bStopAudio)
	{
		if (m_status == VOMM_PLAY_StatusRun)
		{
			if (PlaybackAudioVideo (m_nPlayFlag) > 0)
				voOS_Sleep (1);

			m_bAudioPlaying = VO_TRUE;
			if (m_bAudioEndOfStream && m_bVideoEndOfStream)
				break;
		}
		else
		{
			m_bAudioPlaying = VO_FALSE;
			voOS_Sleep (5);
		}
	}

	m_bAudioPlaying = VO_FALSE;
	m_hThreadAudio = NULL;

	if (m_status != VOMM_PLAY_StatusStop)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VO_VOMMP_CBID_EndOfFile, 0, 0);
	}

	return 0;
}

VO_U32 CPlayGraph::PlaybackAudioVideo (VO_U32 nFlag)
{
	VOLOGF ();

	VO_BOOL bSleep = VO_TRUE;

	if (m_pAudioRender != NULL && (nFlag & 0X02))
	{
		if (m_pAudioRender->GetBufferTime () < m_nAudioBufferTime)
		{
			PlaybackAudioFrame ();
			bSleep = VO_FALSE;
		}

		if (m_pAudioRender->GetBufferTime () < m_nAudioBufferTime)
		{
			PlaybackAudioFrame ();
			bSleep = VO_FALSE;
		}
	}

	if (m_pVideoRender != NULL && (nFlag & 0X01))
	{
		VO_U32 nRC = 0;
		if (m_videoSourceSample.Size == 0)
			nRC = ReadVideoData (m_nVideoTrack, &m_videoSourceSample);

		if (nRC == VO_ERR_SOURCE_END)
		{
			m_bVideoEndOfStream = VO_TRUE;
		}
		else if (nRC == VO_ERR_SOURCE_NEEDRETRY)
		{
			voOS_Sleep (2);
		}
		else if (nRC == VO_ERR_SOURCE_OK)
		{
			// for get thumbnail
			if (nFlag == 1 && m_nThreadNum == 0)
			{
				PlaybackVideoFrame ();
				bSleep = VO_FALSE;

				m_videoSourceSample.Size = 0;
			}
			else if (m_videoSourceSample.Time <= GetPlayingTime () + 30)
			{
				PlaybackVideoFrame ();
				bSleep = VO_FALSE;

				m_videoSourceSample.Size = 0;
			}
		}
	}

	if (bSleep)
		return 1;
	else
		return 0;
}

int CPlayGraph::voSourceStreamCallBack (long EventCode, long * EventParam1, long * userData)
{
	CPlayGraph * pGraph = (CPlayGraph *)userData;
	if (pGraph->m_bReleased)
		return 0;

	// voCAutoLock lock(&pGraph->m_csPlay);
	int nValue = 0;
	if (EventCode == VO_STREAM_BUFFERSTART)
	{
		if (pGraph->m_pAudioRender != NULL)
		{
			pGraph->m_pAudioRender->Pause ();
		}

		if (pGraph->m_pCallBack != NULL)
			pGraph->m_pCallBack (pGraph->m_pUserData, VO_VOMMP_CBID_BuffStart, &nValue, 0);
	}
	else if (EventCode == VO_STREAM_BUFFERSTATUS)
	{
		nValue =  *EventParam1;
		if (pGraph->m_pCallBack != NULL)
			pGraph->m_pCallBack (pGraph->m_pUserData, VO_VOMMP_CBID_Buffering, &nValue, 0);
	}
	else if (EventCode == VO_STREAM_BUFFERSTOP)
	{
		if (pGraph->m_pAudioRender != NULL)
			pGraph->m_pAudioRender->Pause ();

		pGraph->m_nBuffering = 5;

		nValue = 100;
		if (pGraph->m_pCallBack != NULL)
			pGraph->m_pCallBack (pGraph->m_pUserData, VO_VOMMP_CBID_BuffStop, &nValue, 0);
	}
	else if (EventCode == VO_STREAM_DOWNLODPOS)
	{
		nValue =  *EventParam1;
		if (pGraph->m_pCallBack != NULL)
			pGraph->m_pCallBack (pGraph->m_pUserData, VO_VOMMP_CBID_Download, &nValue, 0);
	}
	else if (EventCode == VO_STREAM_ERROR)
	{
		VOLOGI ("Stream Error: %d", EventCode);
		if (pGraph->m_pCallBack != NULL)
			pGraph->m_pCallBack (pGraph->m_pUserData, VO_VOMMP_CBID_Error, 0, 0);
	}

	return 0;
}

#ifdef _WIN32
VO_U32 CPlayGraph::PlaybackIdleLoopProc (VO_PTR pParam)
{
	VOLOGF ();

	voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_IDLE);

	CPlayGraph * pGraph = (CPlayGraph *)pParam;

	return pGraph->PlaybackIdleLoop ();
}

VO_U32 CPlayGraph::PlaybackIdleLoop (void)
{
	VOLOGF ();

	int nValue = 0;

	while (!m_bStopIdle)
	{
		nValue++;
		if (nValue > 10000)
			nValue = 0;

		if (m_bStopIdle)
			break;
	}

	m_hThreadIdle = NULL;

	return 0;
}
#endif // _WIN32
