/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CPreviewEdit.cpp

Contains:	CPreviewEdit class file

Written by:	   Yu Wei based on bangfei's code

Change History (most recent first):
2010-05-05		YW			Create file

*******************************************************************************/

#include "CPreviewEdit.h"
#include "voOSFunc.h"
#include "../../../Android/jni/source/base/vojnilog.h"
#include "CFileFormatCheck.h"
#include "CFileSource.h"
#include "COutAudioRender.h"

#pragma warning (disable : 4996)


#define LOG_TAG "CPreviewEdit"

CPreviewEdit::CPreviewEdit(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
: CBaseEdit (hInst, pMemOP, pFileOP)
, m_nAudioBytesPerSec (44100)
, m_nAudioRenderSize (0)
, m_nAudioRenderStep (0)
, m_nVideoRenderTime (0)
, m_nVideoRenderCount (0)
, m_nVideoRenderSpeed (0)
, m_nVideoRenderDropTime (80)
, m_nAudioRenderStart (0)
, m_nAudioRenderFill (0)
, m_nAudioRenderCount (0)
{
	memset (&m_videoRenderBuffer, 0, sizeof (VO_VIDEO_BUFFER));
}

CPreviewEdit::~CPreviewEdit ()
{
	
	
}

VO_U32 CPreviewEdit::Run (void)
{


	if (m_pSource == NULL)
		return VO_ERR_WRONG_STATUS;

	if (m_status == VOMM_PLAY_StatusRun)
		return VO_ERR_NONE;

	voCAutoLock lock(&m_csEdit);

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


	return VO_ERR_NONE;
}

VO_U32 CPreviewEdit::Stop (void)
{
	VO_U32 nReturn = __super::Stop();

	m_nVideoRenderTime = 0;
	m_nVideoRenderCount = 0;
	m_bVideoRenderDrop = VO_FALSE;


	m_nAudioRenderStart = 0;
	m_nAudioRenderFill = 0;
	m_nAudioRenderCount = 0;

	return nReturn;
}

VO_U32 CPreviewEdit::PlaybackVideoFrame (void)
{


	return 0;
}

VO_U32 CPreviewEdit::Release (void)
{
	if (m_status == VOMM_PLAY_StatusPause || m_status == VOMM_PLAY_StatusRun)
	{
		Stop ();
	}

	if (m_pSource != NULL && m_status < VOMM_PLAY_StatusStop)
		m_pSource->CloseSource ();

	voCAutoLock lock(&m_csEdit);

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

VO_U32 CPreviewEdit::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{

	JNILOGI("Create");

	Release ();

	voCAutoLock lock(&m_csEdit);

	VO_FILE_FORMAT voFF = VO_FILE_FFUNKNOWN;

	if ((nType & 0X0F) == 1)
	{
		VO_TCHAR szURL[2048];
		vostrcpy (szURL, (VO_PTCHAR)pSource);
		//vostrupr (szURL);

		if (!vostrncmp (szURL, _T("RTSP://"), 6))
			voFF = VO_FILE_FFSTREAMING_RTSP;
		else if (!vostrncmp (szURL, _T("rtsp://"), 6))
			voFF = VO_FILE_FFSTREAMING_RTSP;
		else if (!vostrncmp (szURL, _T("MMS://"), 5))
			voFF = VO_FILE_FFSTREAMING_RTSP;
		else if (!vostrncmp (szURL, _T("mms://"), 5))
			voFF = VO_FILE_FFSTREAMING_RTSP;
		else if (!vostrncmp (szURL, _T("HTTP://"), 6))
			voFF = VO_FILE_FFSTREAMING_HTTPPD;
		else if (!vostrncmp (szURL, _T("http://"), 6))
			voFF = VO_FILE_FFSTREAMING_HTTPPD;
	}

	VO_U32 uFileFlag = VO_FILE_TYPE_NAME;
	if ((nType & 0X0F) == VO_VOMM_CREATE_SOURCE_URL)
		uFileFlag = VO_FILE_TYPE_NAME;
	else if ((nType & 0X0F) == VO_VOMM_CREATE_SOURCE_HANDLE)
		uFileFlag = VO_FILE_TYPE_HANDLE;
	else
		uFileFlag = VO_FILE_TYPE_ID;

	if ( voFF == VO_FILE_FFUNKNOWN)
	{
		CFileFormatCheck filCheck(m_pFileOP , m_pMemOP);

		VO_FILE_SOURCE filSource;
		memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
		filSource.nMode = VO_FILE_READ_ONLY;
		filSource.nFlag = uFileFlag;
		filSource.pSource = pSource;
		filSource.nOffset = nOffset;
		filSource.nLength = nLength;

		voFF = filCheck.GetFileFormat(&filSource , ~FLAG_CHECK_AUDIOREADER);

		if(voFF == VO_FILE_FFUNKNOWN && uFileFlag == VO_FILE_TYPE_NAME)
			voFF = CFileFormatCheck::GetFileFormat((VO_PTCHAR)pSource, FLAG_CHECK_EXTNAME);
	}

	m_pSource = new CFileSource (m_hInst, voFF, m_pMemOP, m_pFileOP, m_pDrmCallBack);

	if (m_pSource == NULL)
	{
		vostrcpy (m_szError, _T("The file source box could not be created!"));
		JNILOGE2 ("%s", m_szError);
		return VO_ERR_OUTOF_MEMORY;
	}
	m_pSource->SetLibOperator (m_pLibOP);
	m_pSource->SetConfig (m_pCodecCfg);
	m_pSource->SetErrorText (m_szError);
	m_pSource->SetVOSDK (m_bVOSDK);
	//	m_pSource->SetCallBack ((VO_PTR)m_cbStreaming.pCallBack, m_cbStreaming.pUserData);

	m_pSource->SetForThumbnail (m_bForThumbnail);


	VO_U32 nRC = m_pSource->LoadSource (pSource, uFileFlag, nOffset, nLength);
	if (nRC != VO_ERR_NONE)
	{
		JNILOGE ("It couuld not load the source!");
		return VO_ERR_VOMMP_FileFormat;
	}

	VO_SOURCE_INFO	filInfo;
	m_pSource->GetSourceInfo (&filInfo);
	if (filInfo.Tracks == 0)
	{
		vostrcpy (m_szError, _T("There is not an available track in file!"));
		JNILOGE2 ("%s", m_szError);
		return VO_ERR_VOMMP_FileFormat;
	}

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	//	if (nType & VO_VOMM_CREATE_SOURCE_ONLY)
	{
		for (i = 0; i < filInfo.Tracks; i++)
		{
			m_pSource->GetTrackInfo (i, &trkInfo);
			if (m_nDuration < trkInfo.Duration)
				m_nDuration = trkInfo.Duration;

			if (trkInfo.Type == VO_SOURCE_TT_AUDIO && m_nAudioTrack == -1)
			{
				//m_nAudioTrack = i;
				m_pSource->GetAudioFormat (i, &m_fmtAudio);
			}
			else if (trkInfo.Type == VO_SOURCE_TT_VIDEO && m_nVideoTrack == -1)
			{
				//	m_nVideoTrack = i;
				m_pSource->GetVideoFormat (i, &m_fmtVideo);
			}
		}


	}

	for (i = 0; i < filInfo.Tracks; i++)
	{
		m_nDuration = 0X7FFFFFFF;
		m_pSource->GetTrackInfo (i, &trkInfo);
		if (m_nDuration > trkInfo.Duration)
			m_nDuration = trkInfo.Duration;
	}

	JNILOGI("after file sink");

	for (i = 0; i < filInfo.Tracks; i++)
	{
		JNILOGI2("filInfo.Tracks %d type : %d", i, trkInfo.Type);



		m_pSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1  )
		{
			JNILOGI("Audio Track");

			m_nAudioTrack = i;
			if (m_nDuration < trkInfo.Duration)
				m_nDuration = trkInfo.Duration;

			m_pSource->GetAudioFormat (m_nAudioTrack, &m_fmtAudio);
			if (m_fmtAudio.Channels > 2)
				m_fmtAudio.Channels = 2;

			m_pAudioDec = new CAudioDecoder (m_hInst, trkInfo.Codec, m_pMemOP);
			if (m_pAudioDec == NULL)
			{
				vostrcpy (m_szError, _T("The audio decoder could not be created!"));
				JNILOGE2 ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}
			VO_U32	nFourCC = 0;
			nRC = m_pSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
			m_pAudioDec->SetFourCC (nFourCC);



			// the head data for WMA is different
			if (trkInfo.Codec == VO_AUDIO_CodingWMA || trkInfo.Codec == VO_AUDIO_CodingADPCM)
			{
				nRC = m_pSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_WAVEFORMATEX, &trkInfo.HeadData);
				if (nRC != VO_ERR_NONE)
					return VO_ERR_VOMMP_AudioDecoder;

				VO_WAVEFORMATEX * pWaveFormat = (VO_WAVEFORMATEX *)trkInfo.HeadData;
				// trkInfo.HeadSize = sizeof (VO_WAVEFORMATEX) + pWaveFormat->cbSize;
				trkInfo.HeadSize = VO_WAVEFORMATEX_STRUCTLEN + pWaveFormat->cbSize;
			}

			m_pAudioDec->SetLibOperator (m_pLibOP);
			m_pAudioDec->SetConfig (m_pCodecCfg);
			m_pAudioDec->SetErrorText (m_szError);
			m_pAudioDec->SetVOSDK (m_bVOSDK);

			JNILOGI("after SetVOSDK");

#ifdef LINUX
			// printf the head info.
			sprintf (m_szError, "VOLOG Audio Track Head Info: Pointer: %x,	Size: %d \r\n", trkInfo.HeadData, trkInfo.HeadSize);
			if (m_pCallBack != NULL)
				m_pCallBack (m_pUserData, VO_VOMMP_CBID_DebugText, (VO_PTR) m_szError, 0);
			printf ("VOLOG Audio Track Head Info: Pointer: %x,	Size: %d \r\n", trkInfo.HeadData, trkInfo.HeadSize);
#endif // LINUX

			if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
			{
				nRC = m_pAudioDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &m_fmtAudio);
			}
			else
			{
				VO_SOURCE_SAMPLE	sample;
				nRC = m_pSource->GetTrackParam (m_nAudioTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);
				if (nRC != VO_ERR_NONE)
				{
					sample.Buffer = NULL;
					sample.Size = 0;
				}
				else
				{
					sample.Size = sample.Size & 0X7FFFFFFF;
				}
				nRC = m_pAudioDec->Init (sample.Buffer, sample.Size, &m_fmtAudio);
			}

			if (nRC != VO_ERR_NONE)
			{
				return VO_ERR_VOMMP_AudioDecoder;
			}

			JNILOGI("before AllocAudioRenderBuffer");
			AllocAudioRenderBuffer ();

			if (m_cbAudioRender.pCallBack == NULL)
				m_pAudioRender = new CAudioRender (m_hInst, m_pMemOP);
			else
				m_pAudioRender = new COutAudioRender (m_hInst, m_pMemOP);

			if (m_pAudioRender == NULL)
			{
				JNILOGE("m_pAudioRender not CREATED");
				vostrcpy (m_szError, _T("The audio render could not be created!"));
				JNILOGE2 ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}
			else
				JNILOGE("m_pAudioRender CREATED");

			m_pAudioRender->SetLibOperator (m_pLibOP);
			m_pAudioRender->SetCallBack (m_cbAudioRender.pCallBack, m_cbAudioRender.pUserData);
			m_pAudioRender->SetConfig (m_pCodecCfg);
			m_pAudioRender->SetErrorText (m_szError);
			m_pAudioRender->SetVOSDK (m_bVOSDK);
			nRC = m_pAudioRender->Init (&m_fmtAudio);
			if (nRC != VO_ERR_NONE)
			{
				return VO_ERR_VOMMP_AudioRender;
			}

			nRC = m_pAudioRender->GetParam (VO_PID_AUDIORENDER_MINBUFF, &m_nAudioBufferTime);
		}
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO) && m_nVideoTrack == -1)
		{
			m_nVideoTrack = i;
			if (m_nDuration < trkInfo.Duration)
				m_nDuration = trkInfo.Duration;

			m_pSource->GetVideoFormat (m_nVideoTrack, &m_fmtVideo);

			m_pVideoDec = new CVideoDecoder (m_hInst, trkInfo.Codec, m_pMemOP);
			if (m_pVideoDec == NULL)
			{
				vostrcpy (m_szError, _T("The video decoder could not be created!"));
				JNILOGE2 ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}

			m_pVideoRender = new CVideoRender (m_hInst, m_hView, m_pMemOP);
			if (m_pVideoRender == NULL)
			{
				vostrcpy (m_szError, _T("The video render could not be created!"));
				JNILOGE2 ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}
			m_pVideoRender->SetLibOperator (m_pLibOP);
			m_pVideoRender->SetParam (1001, (VO_PTR)m_pCallBack);
			m_pVideoRender->SetParam (1002, m_pUserData);

			m_pVideoRender->SetCallBack (m_cbVideoRender.pCallBack, m_cbVideoRender.pUserData);
			m_pVideoRender->SetConfig (m_pCodecCfg);
			m_pVideoRender->SetVOSDK (m_bVOSDK);

			VO_MEM_VIDEO_OPERATOR * pVMOP = NULL;
			nRC = m_pVideoRender->GetVideoMemOP (&pVMOP);

			if (pVMOP != NULL)
			{
				nRC = m_pVideoDec->SetParam (VO_PID_VIDEO_VIDEOMEMOP, pVMOP);
			}

			// set the video decoder parameter.
			m_pVideoDec->SetLibOperator (m_pLibOP);
			m_pVideoDec->SetConfig (m_pCodecCfg);
			m_pVideoDec->SetErrorText (m_szError);
			m_pVideoDec->SetVOSDK (m_bVOSDK);
			m_pVideoDec->SetForThumbnail (m_bForThumbnail);

			VO_U32	nFourCC = 0;
			nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_CODECCC, &nFourCC);
			m_pVideoDec->SetFourCC (nFourCC);

			// the head data for WMV is different
			if (trkInfo.Codec == VO_VIDEO_CodingWMV)
			{
				nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_BITMAPINFOHEADER, &trkInfo.HeadData);
				if (nRC != VO_ERR_NONE)
					return VO_ERR_VOMMP_AudioDecoder;

				VO_BITMAPINFOHEADER * pBmpInfo = (VO_BITMAPINFOHEADER *)trkInfo.HeadData;
				trkInfo.HeadSize = pBmpInfo->biSize;
			}

#ifdef LINUX
			// printf the head info.
			sprintf (m_szError, "VOLOG Video Track Head Info: Pointer: %x,	Size: %d \r\n", trkInfo.HeadData, trkInfo.HeadSize);
			if (m_pCallBack != NULL)
				m_pCallBack (m_pUserData, VO_VOMMP_CBID_DebugText, (VO_PTR) m_szError, 0);
			printf ("VOLOG Video Track Head Info: Pointer: %x,	Size: %d \r\n", trkInfo.HeadData, trkInfo.HeadSize);
#endif // LINUX

			if (trkInfo.HeadData != NULL && trkInfo.HeadSize > 0)
			{
				nRC = m_pVideoDec->Init (trkInfo.HeadData, trkInfo.HeadSize, &m_fmtVideo);
			}
			else
			{
				VO_SOURCE_SAMPLE	sample;
				sample.Buffer = NULL;
				sample.Size = 0;

				nRC = m_pSource->GetTrackParam (m_nVideoTrack, VO_PID_SOURCE_FIRSTFRAME, &sample);

				if (nRC != VO_ERR_NONE)
				{
					sample.Buffer = NULL;
					sample.Size = 0;
				}
				else
				{
					sample.Size = sample.Size & 0X7FFFFFFF;
				}

				//				nRC = m_pVideoDec->Init (sample.Buffer, sample.Size, &m_fmtVideo);
				//if use first sample data to initialize decoder, do not care the return value!!
				//East 2009/12/27
				m_pVideoDec->Init (sample.Buffer, sample.Size, &m_fmtVideo);
				nRC = VO_ERR_NONE;
			}

			if (nRC != VO_ERR_NONE)
			{
				JNILOGE ("Init video decoder failed!");
				return VO_ERR_VOMMP_VideoDecoder;
			}

			// Set the video render parameters
			m_pVideoRender->SetErrorText (m_szError);
			nRC = m_pVideoRender->Init (&m_fmtVideo);
			if (nRC != VO_ERR_NONE)
			{
				JNILOGE ("Init video render failed!");
				return VO_ERR_VOMMP_VideoRender;
			}

			m_pVideoRender->SetDispRect (m_hView, &m_rcView);
			nRC = m_pVideoDec->GetParam (VO_PID_VIDEO_ASPECTRATIO, &m_nRatio);
			if (nRC != VO_ERR_NONE)
				m_nRatio = VO_RATIO_11;
			m_pVideoRender->SetDispType (m_nZoomMode, m_nRatio);
		}
	}




	m_llVideoStartPos = 0;

	m_nStartTime = 0;
	m_nPlayedTime = 0;
	m_nSeekPos = 0;

	m_videoSourceSample.Time = 0;
	m_audioSourceSample.Time = 0;

	m_nAudioRenderStart = 0;
	m_nAudioRenderFill = 0;
	m_nAudioRenderCount = 0;

	m_nVideoRenderTime = 0;
	m_nVideoRenderCount = 0;
	m_bVideoRenderDrop = VO_FALSE;

	if (m_bForThumbnail)
	{
		m_bVideoEndOfStream = VO_FALSE;
	}

	return VO_ERR_NONE;
}

VO_U32 CPreviewEdit::PlaybackAudioFrame (void)
{
	// return __super::PlaybackAudioFrame();


	JNILOGD("PlaybackAudioFrame");

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


	nRC = ReadAudioData (m_nAudioTrack, &m_audioSourceSample);
	m_audioSourceSample.Size = m_audioSourceSample.Size & 0X7FFFFFFF;

	if (nRC != VO_ERR_SOURCE_OK)
	{
		if (nRC == VO_ERR_SOURCE_END)
		{

			m_bAudioEndOfStream = VO_TRUE;
			if (m_nThreadNum == 2)
				m_bStopAudio = VO_TRUE;

		

			JNILOGE("m_bAudioEndOfStream");


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




	return 0;
}


VO_U32 CPreviewEdit::PlaybackAudioLoop (void)
{


#ifdef _WIN32
	voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_ABOVE_NORMAL);
#endif // _WIN32

	m_bAudioEndOfStream = VO_FALSE;

	while (!m_bStopAudio)
	{
		if (m_status == VOMM_PLAY_StatusRun)
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


VO_U32 CPreviewEdit::PlaybackVideoLoop (void)
{


#ifdef _WIN32
	voThreadSetPriority (GetCurrentThread (), VO_THREAD_PRIORITY_NORMAL);
#endif // _WIN32

	m_bVideoEndOfStream = VO_FALSE;

	while (!m_bStopVideo)
	{
		if (m_status == VOMM_PLAY_StatusRun )
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

VO_U32 CPreviewEdit::AllocAudioRenderBuffer (void)
{

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

VO_U32 CPreviewEdit::UpdateWithConfig (void)
{
	__super::UpdateWithConfig();

	if (m_pConfig == NULL)
		return 0;

	m_nAudioRenderStep = m_pConfig->GetItemValue ("vowPlay", "AudioStepTime", 0);
	m_nVideoRenderDropTime = m_pConfig->GetItemValue ("vowPlay", "VideoDropTime", m_nVideoRenderDropTime);

	return 1;
}


VO_U32 CPreviewEdit::SetCurPos (VO_S32 nPos)
{


	if (m_pSource == NULL)
		return VO_ERR_WRONG_STATUS;

	if (!m_pSource->CanSeek ())
	{
		JNILOGE ("The status is wrong in source!");
		return VO_ERR_WRONG_STATUS;
	}

	if (m_nSeekPos > 0)
	{
		if ((nPos >  m_nSeekPos - 300) && (nPos < m_nSeekPos + 300))
			return VO_ERR_NOT_IMPLEMENT;
	}
	m_nSeekPos = nPos;

	//	if (voOS_GetSysTime () - m_nSeekTime < 1000)
	//		return VO_ERR_NOT_IMPLEMENT;
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

		if (m_bForThumbnail && nNewPos > 0)
		{
			VO_S32 nIndex = 0;
			nNewPos = m_llVideoStartPos - 1;
			while (nNewPos < m_llVideoStartPos)
			{
				nNewPos = m_llVideoStartPos + nIndex * 1000;
				nIndex++;
				if (nNewPos > m_nDuration)
				{
					m_bVideoEndOfStream = VO_TRUE;
					break;
				}
				nRC = m_pSource->SetTrackPos (m_nVideoTrack, &nNewPos);
				if (nRC != VO_ERR_NONE)
					break;
			}
		}
		else
		{
			nRC = m_pSource->SetTrackPos (m_nVideoTrack, &nNewPos);
		}
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