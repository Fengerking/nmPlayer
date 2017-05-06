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
#include "cmnFile.h"
#include "CPlayFile.h"
#include "CFileFormatCheck.h"

#include "COutAudioRender.h"
#include "voOSFunc.h"

#include "CRTSPSource.h"
#include "CHTTPPDSource.h"

#include "voLog.h"

#pragma warning (disable : 4996)


#define LOG_TAG "CPlayFile"

CPlayFile::CPlayFile(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: CPlayGraph (hInst, pMemOP, pFileOP)
{
	VOLOGF ();
}

CPlayFile::~CPlayFile ()
{
	VOLOGF ();
	cmnMemShowStatus();
}

VO_U32 CPlayFile::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	VOLOGW ();

	Release ();

	voCAutoLock lock(&m_csPlay);

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

	if (voFF == VO_FILE_FFSTREAMING_RTSP || voFF == VO_FILE_FFAPPLICATION_SDP)
		m_pSource = new CRTSPSource (m_hInst, voFF, m_pMemOP, m_pFileOP, m_pDrmCallBack);
	else if (voFF == VO_FILE_FFSTREAMING_HTTPPD)
		m_pSource = new CHTTPPDSource (m_hInst, voFF, m_pMemOP, m_pFileOP, m_pDrmCallBack);
	else
		m_pSource = new CFileSource (m_hInst, voFF, m_pMemOP, m_pFileOP, m_pDrmCallBack);
	if (m_pSource == NULL)
	{
		vostrcpy (m_szError, _T("The file source box could not be created!"));
		VOLOGE ("%s", m_szError);
		return VO_ERR_OUTOF_MEMORY;
	}
	m_pSource->SetLibOperator (m_pLibOP);
	m_pSource->SetConfig (m_pCodecCfg);
	m_pSource->SetErrorText (m_szError);
	m_pSource->SetVOSDK (m_bVOSDK);
//	m_pSource->SetCallBack ((VO_PTR)m_cbStreaming.pCallBack, m_cbStreaming.pUserData);
	m_pSource->SetCallBack ((VO_PTR)voSourceStreamCallBack, (VO_PTR)this);
	m_pSource->SetForThumbnail (m_bForThumbnail);

	VO_U32 nRC = m_pSource->LoadSource (pSource, uFileFlag, nOffset, nLength);
	if (nRC != VO_ERR_NONE)
	{
		VOLOGE ("It couuld not load the source!");
		return VO_ERR_VOMMP_FileFormat;
	}

	VO_SOURCE_INFO	filInfo;
	m_pSource->GetSourceInfo (&filInfo);
	if (filInfo.Tracks == 0)
	{
		vostrcpy (m_szError, _T("There is not an available track in file!"));
		VOLOGE ("%s", m_szError);
		return VO_ERR_VOMMP_FileFormat;
	}

	VO_U32				i = 0;
	VO_SOURCE_TRACKINFO	trkInfo;
	if (nType & VO_VOMM_CREATE_SOURCE_ONLY)
	{
		for (i = 0; i < filInfo.Tracks; i++)
		{
			m_pSource->GetTrackInfo (i, &trkInfo);
			if (m_nDuration < trkInfo.Duration)
				m_nDuration = trkInfo.Duration;

			if (trkInfo.Type == VO_SOURCE_TT_AUDIO && m_nAudioTrack == -1)
			{
				m_nAudioTrack = i;
				m_pSource->GetAudioFormat (m_nAudioTrack, &m_fmtAudio);
			}
			else if (trkInfo.Type == VO_SOURCE_TT_VIDEO && m_nVideoTrack == -1)
			{
				m_nVideoTrack = i;
				m_pSource->GetVideoFormat (m_nVideoTrack, &m_fmtVideo);
			}
		}

		return VO_ERR_NONE;
	}

	for (i = 0; i < filInfo.Tracks; i++)
	{
		m_nDuration = 0X7FFFFFFF;
		m_pSource->GetTrackInfo (i, &trkInfo);
		if (m_nDuration > trkInfo.Duration)
			m_nDuration = trkInfo.Duration;
	}

	for (i = 0; i < filInfo.Tracks; i++)
	{
		m_pSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1 && (m_nPlayFlag & 0X02) == 2)
		{
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
				VOLOGE ("%s", m_szError);
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

			AllocAudioRenderBuffer ();

			if (m_cbAudioRender.pCallBack == NULL)
				m_pAudioRender = new CAudioRender (m_hInst, m_pMemOP);
			else
				m_pAudioRender = new COutAudioRender (m_hInst, m_pMemOP);

			if (m_pAudioRender == NULL)
			{
				vostrcpy (m_szError, _T("The audio render could not be created!"));
				VOLOGE ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}

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
		else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO) && m_nVideoTrack == -1 && (m_nPlayFlag & 0X01) == 1)
		{
			m_nVideoTrack = i;
			if (m_nDuration < trkInfo.Duration)
				m_nDuration = trkInfo.Duration;

			m_pSource->GetVideoFormat (m_nVideoTrack, &m_fmtVideo);

			m_pVideoDec = new CVideoDecoder (m_hInst, trkInfo.Codec, m_pMemOP);
			if (m_pVideoDec == NULL)
			{
				vostrcpy (m_szError, _T("The video decoder could not be created!"));
				VOLOGE ("%s", m_szError);
				return VO_ERR_OUTOF_MEMORY;
			}

			m_pVideoRender = new CVideoRender (m_hInst, m_hView, m_pMemOP);
			if (m_pVideoRender == NULL)
			{
				vostrcpy (m_szError, _T("The video render could not be created!"));
				VOLOGE ("%s", m_szError);
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
				VOLOGE ("Init video decoder failed!");
				return VO_ERR_VOMMP_VideoDecoder;
			}

			// Set the video render parameters
			m_pVideoRender->SetErrorText (m_szError);
			nRC = m_pVideoRender->Init (&m_fmtVideo);
			if (nRC != VO_ERR_NONE)
			{
				VOLOGE ("Init video render failed!");
				return VO_ERR_VOMMP_VideoRender;
			}

			m_pVideoRender->SetDispRect (m_hView, &m_rcView);
			nRC = m_pVideoDec->GetParam (VO_PID_VIDEO_ASPECTRATIO, &m_nRatio);
			if (nRC != VO_ERR_NONE)
				m_nRatio = VO_RATIO_11;
			m_pVideoRender->SetDispType (m_nZoomMode, m_nRatio);
		}
	}

	VOLOGW ();

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

