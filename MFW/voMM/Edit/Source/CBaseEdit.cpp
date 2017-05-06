/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseEdit.cpp

Contains:	CBaseEdit class file

Written by:	   Yu Wei based on Bangfei's code

Change History (most recent first):
2010-04-26		YW			Create file

*******************************************************************************/

#include "cmnFile.h"
#include "CBaseEdit.h"
#include "CFileFormatCheck.h"

#include "COutAudioRender.h"
#include "voOSFunc.h"


#include "voLog.h"
#include "../../../Android/jni/source/base/vojnilog.h"

#include "voMMPlay.h"
#include "CFileSource.h"

#pragma warning (disable : 4996)


#define LOG_TAG "CBaseEdit"

CBaseEdit::CBaseEdit(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
: m_hInst (hInst)
, m_pMemOP (pMemOP)
, m_pFileOP (pFileOP)
, m_pVideoHeader(NULL)
, m_pAudioHeader(NULL)
, m_nVideoHeaderSize(0)
, m_nAudioHeaderSize(0)

, m_nVideoFirstSample(0)
, m_nAudioFirstSample(0)

, m_nDumpVideoTime(0)
, m_nDumpAudioTime(0)
, m_nAudioBufferTime (400)
	
, m_nStartTime (0)
, m_nPlayedTime (0)
, m_nZoomMode (VO_ZM_LETTERBOX)

, m_pDrmCallBack (NULL)
, m_pLibOP (NULL)

, m_bVOSDK (VO_FALSE)
, m_bForThumbnail (VO_FALSE)

, m_nDuration(0)
, m_nCurPos(0)

, m_nSeekTime (0)
, m_nSeekPos (0)

, m_nSeekMode (1)
, m_llVideoStartPos (0)





, m_bAudioEndOfStream (VO_TRUE)
, m_bVideoEndOfStream (VO_TRUE)

, m_nThreadNum (2)
, m_hThreadAudio (NULL)
, m_bStopAudio (VO_TRUE)
, m_bAudioPlaying (VO_FALSE)

, m_pCallBack (NULL)

, m_hThreadVideo (NULL)
, m_bStopVideo (VO_TRUE)
, m_bVideoPlaying (VO_FALSE)

, m_nFirstAudioPos (0XFFFFFFFF)
, m_nFirstVideoPos (0XFFFFFFFF)

, m_pAudioRenderData (NULL)

, m_status (VOMM_PLAY_StatusLoad)
, m_bVideoPlayNextFrame (VO_FALSE)

, m_pSource (NULL)
, m_pAudioDec(NULL)
, m_pVideoDec(NULL)
, m_pAudioRender(NULL)
, m_pVideoRender(NULL)

{
	VOLOGF ();

	JNILOGI("Construct");


	g_fileOP.Open = NULL;

	if (m_pMemOP == NULL)
	{
		cmnMemFillPointer (VO_INDEX_MFW_VOMMPLAY);
		m_pMemOP = &g_memOP;
	}

	if (m_pFileOP == NULL)
	{
		if (g_fileOP.Open == NULL)
			cmnFileFillPointer ();
		m_pFileOP = &g_fileOP;
	}

	m_cbAudioRender.pCallBack = NULL;
	m_cbAudioRender.pUserData = NULL;

	m_cbStreaming.pCallBack = NULL;
	m_cbStreaming.pUserData = NULL;

	m_bReleased = VO_FALSE;

	vostrcpy (m_szError, _T("No Error."));


	memset (&m_Source, 0, sizeof (m_Source));
	memset (&m_rcView, 0, sizeof (m_rcView));
	memset (&m_rcDisp, 0, sizeof (m_rcDisp));

	memset (&m_videoSourceSample, 0, sizeof (VO_SOURCE_SAMPLE));
	memset (&m_audioSourceSample, 0, sizeof (VO_SOURCE_SAMPLE));

	memset (&m_videoSourceBuffer, 0, sizeof (VO_CODECBUFFER));
	memset (&m_videoNextKeySample, 0, sizeof (VO_SOURCE_SAMPLE));


	m_pCodecCfg = new CBaseConfig ();
	m_pCodecCfg->Open (_T("vommcodec.cfg"), m_pFileOP);

#ifdef WIN32
	vostrcpy(m_szDumpFilePath, L"d:\\dump.3gp");
	vostrcpy(m_szDumpTmpDir, L"E:\\SVN\\voRelease\\Win32\\Bin\\XP\\");
#else //WIN32
	vostrcpy(m_szDumpFilePath, "/sdcard/dump.3gp");
	vostrcpy(m_szDumpTmpDir, "/sdcard/");
#endif  //WIN32
	
	
	m_Source.pSource = (VO_PTCHAR) m_szDumpFilePath;
	m_Source.nFlag   = VO_FILE_TYPE_NAME;




}

CBaseEdit::~CBaseEdit ()
{
	VOLOGF ();

	if (m_pVideoHeader) {
		delete m_pVideoHeader;
		m_pVideoHeader = NULL;
	}

	if (m_pAudioHeader) {
		delete m_pAudioHeader;
		m_pAudioHeader = NULL;
	}	

	cmnMemShowStatus();
}





VO_U32 CBaseEdit::SetParam (VO_U32 nID, VO_PTR pValue)
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

		// temp		m_nVideoRenderCount = 0;
		// temp		m_nVideoRenderSpeed = 0;

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
	/*
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
		*/
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
		//		if (m_bForThumbnail)
		//			m_nZoomMode = VO_ZM_FITWINDOW;

		return VO_ERR_NONE;
	}
	/*
	else if (nID == VO_VOMMP_PMID_Playback)
	{
		if (m_nThreadNum != 0)
			return VO_ERR_NOT_IMPLEMENT;


		//PlaybackAudioVideo (m_nPlayFlag);
		PlaybackAudioVideo(3);

		return VO_ERR_NONE;
	}
*/
	else if (nID == VO_PID_COMMON_LIBOP)
	{
		m_pLibOP = (VO_LIB_OPERATOR *)pValue;

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






VO_U32 CBaseEdit::SetViewInfo (VO_PTR hView, VO_RECT * pRect)
{
	m_hView = hView;

	if (pRect != NULL)
	{
		m_rcView.left = pRect->left;
		m_rcView.top = pRect->top;
		m_rcView.right = pRect->right;
		m_rcView.bottom = pRect->bottom;
	}

	return VO_ERR_NONE;
}

VO_U32 CBaseEdit::SetCallBack (VOMMPlayCallBack pCallBack, VO_PTR pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;

	return VO_ERR_NONE;
}

VO_S64 CBaseEdit::GetPlayingTime (void)
{
	VOLOGF ();

	VO_S64 nPlayingTime = 0;

	if ( m_pAudioRender == NULL)
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

VO_U32 CBaseEdit::ReadVideoData(VO_U32 nTrack, VO_PTR pTrackData)
{
	VOLOGF ();

	return m_pSource->GetTrackData (nTrack, pTrackData);
}


VO_U32 CBaseEdit::ReadAudioData(VO_U32 nTrack, VO_PTR pTrackData)
{
	VOLOGF ();

	return m_pSource->GetTrackData (nTrack, pTrackData);
}

VO_U32 CBaseEdit::Stop (void)
{
	VOLOGF ();

	if (m_status == VOMM_PLAY_StatusLoad || m_status == VOMM_PLAY_StatusStop)
		return VO_ERR_NONE;

	voCAutoLock lock(&m_csEdit);
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



	m_llVideoStartPos = 0;

	m_nStartTime = 0;
	m_nPlayedTime = 0;
	m_nSeekPos = 0;

	m_videoSourceSample.Time = 0;
	m_audioSourceSample.Time = 0;


	
	m_videoNextKeySample.Time = 0;
	m_bVideoPlayNextFrame = VO_FALSE;

	m_bAudioEndOfStream = VO_TRUE;
	m_bVideoEndOfStream = VO_TRUE;

	EndOfSource ();

	return VO_ERR_NONE;
}



VO_U32 CBaseEdit::EndOfSource (void)
{
	VOLOGF ();

	return 0;
}

VO_U32 CBaseEdit::SetConfig (CBaseConfig * pConfig)
{
	VOLOGF ();

	m_pConfig = pConfig;

	UpdateWithConfig ();

	return 0;
}

VO_U32 CBaseEdit::UpdateWithConfig (void)
{
	VOLOGF ();

	if (m_pConfig == NULL)
		return 0;

	m_nThreadNum = m_pConfig->GetItemValue ("vowPlay", "ThreadNum", m_nThreadNum);
	//m_nPlayFlag = m_pConfig->GetItemValue ("vowPlay", "PlayFlag", m_nPlayFlag);
	m_nSeekMode = m_pConfig->GetItemValue ("vowPlay", "SeekMode", m_nSeekMode);

	m_nAudioBufferTime = m_pConfig->GetItemValue ("vowPlay", "AudioBufferTime", m_nAudioBufferTime);


	m_nZoomMode = (VO_IV_ZOOM_MODE)m_pConfig->GetItemValue ("vowPlay", "VideoZoomMode", m_nZoomMode);
	
	return 1;
}




VO_U32 CBaseEdit::Pause (void)
{
	VOLOGF ();

	if (m_status == VOMM_PLAY_StatusStop)
		return VO_ERR_WRONG_STATUS;

	if (m_status == VOMM_PLAY_StatusLoad || m_status == VOMM_PLAY_StatusPause)
		return VO_ERR_NONE;

	voCAutoLock lock(&m_csEdit);
	m_status = VOMM_PLAY_StatusPause;


	if (m_pAudioRender != NULL)
		m_pAudioRender->Pause ();
	if (m_pVideoRender != NULL)
		m_pVideoRender->Pause ();

	if (m_pAudioDec != NULL)
		m_pAudioDec->Pause ();

	//if (m_pVideoDec != NULL)
	//		m_pVideoDec->Pause ();

	if (m_pSource != NULL)
		m_pSource->Pause ();

	while (m_bAudioPlaying || m_bVideoPlaying)
		voOS_Sleep (10);

	m_nPlayedTime = m_videoSourceSample.Time;

	return VO_ERR_NONE;
}



VO_U32 CBaseEdit::PlaybackAudioLoopProc (VO_PTR pParam)
{
	VOLOGF ();

	CBaseEdit * pPlay = (CBaseEdit *)pParam;

	if (pPlay->m_nThreadNum >= 2)
		pPlay->PlaybackAudioLoop ();


	return 0;
}

VO_U32 CBaseEdit::PlaybackVideoLoopProc (VO_PTR pParam)
{
	VOLOGF ();

	CBaseEdit * pPlay = (CBaseEdit *)pParam;

	pPlay->PlaybackVideoLoop ();

	return 0;
}



VO_U32 CBaseEdit::GetDuration (VO_U32 * pDuration)
{
	VOLOGF ();

	*pDuration = m_nDuration;
	return VO_ERR_NONE;
}

VO_U32 CBaseEdit::GetCurPos (VO_S32 * pPos)
{
	VOLOGF ();

	if (m_bForThumbnail)
	{
		if (m_bVideoEndOfStream)
			*pPos = m_nDuration;
		else
			*pPos = m_nCurPos;
	}
	else
	{
		*pPos = m_nCurPos;
	}

	return VO_ERR_NONE;
}



VO_U32 CBaseEdit::GetParam (VO_U32 nID, VO_PTR pValue)
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
	/*
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
		*/
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
	else if (nID == VO_VOMMP_PMID_Status)
	{
		*(VOMMPlAYSTATUS *)pValue = m_status;
		return VO_ERR_NONE;
	}
	else if (nID == VO_VOMMP_PMID_LastError)
	{
		*(VO_TCHAR **)pValue = m_szError;
		return VO_ERR_NONE;
	}

	return VO_ERR_NOT_IMPLEMENT;
}




VO_U32 CBaseEdit::DecoderVideoData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat)
{
	VOLOGF ();

	return  m_pVideoDec->GetOutputData (pOutput, pVideoFormat);
}
