/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CEditGraph.cpp

Contains:	CEditGraph class file

Written by:	   Yu Wei based on Bangfei's code

Change History (most recent first):
2010-04-26		YW			Create file

*******************************************************************************/

#include "cmnFile.h"
#include "CEditGraph.h"
#include "CFileFormatCheck.h"

#include "COutAudioRender.h"
#include "voOSFunc.h"

#include "CRTSPSource.h"
#include "CHTTPPDSource.h"

#include "voLog.h"
#include "../../../Android/jni/source/base/vojnilog.h"


#pragma warning (disable : 4996)


#define LOG_TAG "CEditGraph"

CEditGraph::CEditGraph(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
: CPlayGraph (hInst, pMemOP, pFileOP)
, m_pFileSink(NULL)

, m_pVideoHeader(NULL)
, m_pAudioHeader(NULL)
, m_nVideoHeaderSize(0)
, m_nAudioHeaderSize(0)

, m_nVideoFirstSample(0)
, m_nAudioFirstSample(0)

, m_nDumpVideoTime(0)
, m_nDumpAudioTime(0)
{
	VOLOGF ();

	JNILOGI("Construct");

	memset (&m_Source, 0, sizeof (m_Source));


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

CEditGraph::~CEditGraph ()
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

VO_U32 CEditGraph::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	VOLOGW ();

	JNILOGI("Create");

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

	

	m_nPlayFlag = 0X03;
    //  ==============================================

	JNILOGI("before create FileSink");

	
	if (m_pFileSink == NULL)
	{
		m_pFileSink = new CFileSink (NULL, VO_FILE_FFMOVIE_MP4, m_pMemOP, m_pFileOP);
		if (m_pFileSink == NULL)
			return VO_ERR_OUTOF_MEMORY;

		m_pFileSink->SetLibOperator ((VO_LIB_OPERATOR *)m_pLibOP);
	

		m_Source.nMode    = VO_FILE_WRITE_ONLY;
    	m_Source.nReserve = (VO_U32)m_szDumpTmpDir;


		VO_U32	nAudioCodec = 0;
		VO_U32	nVideoCodec = 0;

		for (i = 0; i < filInfo.Tracks; i++)
		{
			m_pSource->GetTrackInfo (i, &trkInfo);
			 

			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1 && (m_nPlayFlag & 0X02) == 2)
			{
				nAudioCodec = trkInfo.Codec;
			}
			else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO) && m_nVideoTrack == -1 && (m_nPlayFlag & 0X01) == 1)
			{
				nVideoCodec = trkInfo.Codec;
			}
		}




		VO_U32 nRC = m_pFileSink->Init (&m_Source, nAudioCodec, nVideoCodec);
		if (nRC != VO_ERR_NONE) 
		{
			VOLOGE ("Failed to init File Sink. 0X%08X", (unsigned int)nRC);
			return VO_ERR_FAILED;
		}



		nRC = m_pFileSink->SetParam (VO_PID_AUDIO_FORMAT, &m_fmtAudio);
		
		JNILOGI2 ("Name %s. Audio Codec %d Format: S %d, C %d", "Editor", nAudioCodec, (int)m_fmtAudio.SampleRate, (int)m_fmtAudio.Channels);

		nRC = m_pFileSink->SetParam (VO_PID_VIDEO_FORMAT, &m_fmtVideo);
		JNILOGI2 ("Name %s. Video Codec %d Format: W %d, H %d", "Editor", nVideoCodec, (int)m_fmtVideo.Width, (int)m_fmtVideo.Height);


	}
	
	m_pFileSink->Start();



	//  ==============================================

	JNILOGI("after file sink");

	for (i = 0; i < filInfo.Tracks; i++)
	{
		JNILOGI2("filInfo.Tracks %d type : %d", i, trkInfo.Type);
		JNILOGI2("Audiotrack %d Playflag %d", m_nAudioTrack, m_nPlayFlag);

			
		m_pSource->GetTrackInfo (i, &trkInfo);

		if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO) && m_nAudioTrack == -1  && (m_nPlayFlag & 0X02) == 2)
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
				VOLOGE ("%s", m_szError);
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


VO_U32 CEditGraph::PlaybackVideoFrame (void)
{
	//return __super::PlaybackVideoFrame();
	VOLOGF ();

	JNILOGD("PlaybackVideoFrame");

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
		nRC = ReadVideoData (m_nVideoTrack, &m_videoSourceSample);
		if (nRC != VO_ERR_SOURCE_OK)
		{
			if (nRC == VO_ERR_SOURCE_END)
			{
				m_bVideoEndOfStream = VO_TRUE;
				m_bStopVideo = VO_TRUE;
			
				VOLOGE("m_bVideoEndOfStream");
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

	
	//m_videoSourceBuffer.Buffer = m_videoSourceSample.Buffer;
	//m_videoSourceBuffer.Length = m_videoSourceSample.Size & 0X7FFFFFFF;


	
	if (nRC != VO_ERR_NONE)
		return 0;


	



	DumpSample(&m_videoSourceSample, 1);

	return 0;
}



VO_U32 CEditGraph::PlaybackAudioFrame (void)
{
   // return __super::PlaybackAudioFrame();
	VOLOGF ();

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

			m_pFileSink->Stop();

			VOLOGE("m_bAudioEndOfStream");


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



	DumpSample(&m_audioSourceSample, 0);

	return 0;
}


VO_U32	CEditGraph::DumpSample(VO_SOURCE_SAMPLE* pSourceSample, VO_U32 nIsVideo) 
{
	JNILOGI2("Enter %s Sample %d ", (nIsVideo)?"video" : "audio" , pSourceSample->Time);



	m_videoSourceBuffer.Buffer = m_videoSourceSample.Buffer;
	m_videoSourceBuffer.Length = m_videoSourceSample.Size & 0X7FFFFFFF;
	VO_VIDEO_FRAMETYPE ft = m_pVideoDec->GetFrameType (&m_videoSourceBuffer);

	

	static int nTimes = 0;
	nTimes ++;

	static int nTempVideoStop = 0;
	static int nTempAudioStop = 0;




	if (nIsVideo)
	{
		int nSleepTimes = 0;
		while (nSleepTimes < 100 && m_nDumpAudioTime &&  pSourceSample->Time > m_nDumpAudioTime + 200) {
			voOS_Sleep (2);
			nSleepTimes++;
		}

		m_nDumpVideoTime = pSourceSample->Time;
	}
	else
	{
		int nSleepTimes = 0;
		while (nSleepTimes < 100 && m_nDumpVideoTime &&  pSourceSample->Time > m_nDumpVideoTime + 200) {
			voOS_Sleep (2);
			nSleepTimes++;
		}

		m_nDumpAudioTime = pSourceSample->Time;
	}

	/*

	int nTime = pSourceSample->Time & 0x7FFFFFFF;


	if (nTimes == -1)
		return 0;


	while(1){
		if (m_nVideoFirstSample == 0 && nIsVideo == 1)
			break;

		if (m_Controller.CheckSample(pSourceSample, ft, nIsVideo))
		{
			return 0;
		}

	}


	



	if (nTime > 7000 && nTime < 12000)
	{
		if (nIsVideo) {
			if (nTempVideoStop)
				return 0;

			else if (ft == VO_VIDEO_FRAME_I || ft == VO_VIDEO_FRAME_P)
			{
				JNILOGI2("STOP AT %s", (ft == VO_VIDEO_FRAME_I) ? "IFrame" : "PFrame");
				nTempVideoStop = 1;
				//return 0;
			}


		} // else nIsVideo
		else
		{
			if (nTempVideoStop)
				return 0;
		} // end nIsVideo
	}

	if (nTime > 13000 && nTime < 18000 && nTempVideoStop)
		nTempVideoStop = 0;


	if (nTime > 30000)
	{
		nTimes = -1;
		m_pFileSink->Stop();
		return 0;
	}

	if (nTime > 20000 ) 
	{	
		if (nIsVideo) {
			if (nTempVideoStop)
				return 0;
			else if (ft == VO_VIDEO_FRAME_I || ft == VO_VIDEO_FRAME_P)
			{
				nTempVideoStop = 1;
				return 0;
			}
		} // else nIsVideo
		else 
		{

			return 0;

		} // end nIsVideo

	}

	*/

	//*

		if (nTimes == -1)
			return 0;
	

		int nTime = pSourceSample->Time & 0x7FFFFFFF;

		if (nTime < 3000)
		{
			if (m_nVideoFirstSample == 0 && nIsVideo == 1) 
				int aa = 3;
			else
				return 0;
		}

		

		if (nTime > 7000 && nTime < 12000)
		{
			if (nIsVideo) {
				if (nTempVideoStop)
					return 0;
			
				else if (ft == VO_VIDEO_FRAME_I || ft == VO_VIDEO_FRAME_P)
				{
					JNILOGI2("STOP AT %s", (ft == VO_VIDEO_FRAME_I) ? "IFrame" : "PFrame");
					nTempVideoStop = 1;
					//return 0;
				}
						
			
			} // else nIsVideo
			else
			{
				if (nTempVideoStop)
				  return 0;
			} // end nIsVideo
		}
	
		if (nTime > 13000 && nTime < 18000 && nTempVideoStop)
			nTempVideoStop = 0;
		

		if (nTime > 30000)
		{
		   nTimes = -1;
		   m_pFileSink->Stop();
		   return 0;
		}

		if (nTime > 20000 ) 
		{	
			if (nIsVideo) {
					if (nTempVideoStop)
						return 0;
					else if (ft == VO_VIDEO_FRAME_I || ft == VO_VIDEO_FRAME_P)
					{
					   nTempVideoStop = 1;
					   return 0;
					}
			} // else nIsVideo
			else 
			{
				
					return 0;
				
			} // end nIsVideo

		}

	//*/

		
		

	VO_SINK_SAMPLE ss;

	ss.nAV		= nIsVideo;
	ss.Buffer	= pSourceSample->Buffer;	
	ss.Size		= pSourceSample->Size;
	ss.Time		= pSourceSample->Time;
	ss.Duration = pSourceSample->Duration;

//	if (ss.Time & 0x7FFFFFFF > 12000)
	//	 ss.Time -= 5000;


	/*
	if (m_nAudioFirstSample == 0 && nIsVideo == 0) 
	{
		m_nAudioFirstSample = 1;

		m_nAudioHeaderSize = pSourceSample->Size & 0X7FFFFFFF;

		
		if (m_pAudioHeader != NULL) {
			delete m_pAudioHeader;
		}
		m_pAudioHeader = new VO_BYTE[m_nAudioHeaderSize];

		memcpy(m_pAudioHeader, pSourceSample->Buffer , m_nAudioHeaderSize);

		return 0;
	}

	if (m_nAudioFirstSample == 1 && nIsVideo == 0)
	{		
		m_nAudioFirstSample = 2;

		VO_BYTE* pOld = m_pAudioHeader;
		m_pAudioHeader = new VO_BYTE[pSourceSample->Size  & 0X7FFFFFFF + m_nAudioHeaderSize];
		memcpy(m_pAudioHeader, pOld, m_nAudioHeaderSize);
		delete pOld;

	
		memcpy(m_pAudioHeader + m_nAudioHeaderSize, pSourceSample->Buffer, pSourceSample->Size);
	    ss.Buffer = m_pAudioHeader;
		ss.Size   = ss.Size& 0x7FFFFFFF;
		ss.Size	 += m_nAudioHeaderSize + pSourceSample->Size & 0x80000000;
	
	}
	*/

	if (m_nVideoFirstSample == 0 && nIsVideo == 1) 
	{
		m_nVideoFirstSample = 1;
		m_nVideoHeaderSize = pSourceSample->Size & 0X7FFFFFFF;
		
	
		if (m_pVideoHeader)
			delete m_pVideoHeader;
		m_pVideoHeader = new VO_BYTE[m_nVideoHeaderSize];	
			 
		memcpy(m_pVideoHeader, pSourceSample->Buffer , m_nVideoHeaderSize);
		return 0;
	}

	if (m_nVideoFirstSample == 1 && nIsVideo == 1)
	{
		if (pSourceSample->Size & 0X80000000 != 0x80000000)
			return 0;

		m_nVideoFirstSample = 2;

		int nBufferSize = pSourceSample->Size & 0X7FFFFFFF;
		

		VO_BYTE* pOld = m_pVideoHeader;
		m_pVideoHeader = new VO_BYTE[nBufferSize + m_nVideoHeaderSize];
		memcpy(m_pVideoHeader, pOld, m_nVideoHeaderSize);
		delete pOld;


		memcpy(m_pVideoHeader + m_nVideoHeaderSize, pSourceSample->Buffer, nBufferSize);
		ss.Buffer = m_pVideoHeader;
		ss.Size   = ss.Size& 0x7FFFFFFF;
		ss.Size	 += m_nVideoHeaderSize + pSourceSample->Size & 0x80000000;
    }

	//	*/


	
	VOLOGI("Dump %s Sample time :%d ", (nIsVideo)?"video" : "audio" , ss.Time);

	
	

	if (ss.Time > 12000)
	{
		ss.Time -= 4200;
		
	}

	ss.Time -= 2900;

	/*
	JNILOGI2("real Dump %s Sample time :%d ", (nIsVideo)?"video" : "audio" , ss.Time);
	if (nIsVideo && pSourceSample->Size & 0x80000000 == 0x80000000)
		JNILOGI("IFrame");
	*/


	m_pFileSink->AddSample(&ss);

	return 0;
}
