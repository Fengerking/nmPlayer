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

#include "CDumpPlayFile.h"
#include "CFileFormatCheck.h"

#include "voOSFunc.h"

#include "voLog.h"

#ifdef _WIN32
#pragma warning (disable : 4996)
#endif

CDumpPlayFile::CDumpPlayFile(VO_PTR hInst, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP)
	: CPlayFile (hInst, pMemOP, pFileOP)
{
	strcpy (m_szFileName, (""));

	m_nAudioReadIndex = 0;
	m_nAudioDecIndex = 0;
	m_nAudioRenIndex = 0;

	m_nVideoReadIndex = 0;
	m_nVideoDecIndex = 0;
	m_nVideoRenIndex = 0;

	m_nVideoThreadStart = 0;
	m_nVideoThreadTime = 0;
	m_nAudioThreadStart = 0;
	m_nAudioThreadTime = 0;
	m_nIdleThreadStart = 0;
	m_nIdleThreadTime = 0;

	m_nPlaybackStartTime = 0;
	m_nPlaybackEndTime = 0;

	m_pAudioDecThreadNum = NULL;
	m_pVideoDecThreadNum = NULL;
}

CDumpPlayFile::~CDumpPlayFile ()
{
	ReleaseBuffer ();
}

void CDumpPlayFile::ReleaseBuffer (void)
{
	m_nAudioReadIndex = 0;
	m_nAudioDecIndex = 0;
	m_nAudioRenIndex = 0;

	m_nVideoReadIndex = 0;
	m_nVideoDecIndex = 0;
	m_nVideoRenIndex = 0;

	if (m_pAudioDecThreadNum != NULL)
	{
		delete []m_pAudioReadThreadNum;
		delete []m_pAudioReadSystemNum;
		delete []m_pAudioReadSampleTime;
		delete []m_pAudioReadSampleSize;

		delete []m_pAudioDecThreadNum;
		delete []m_pAudioDecSystemNum;

		delete []m_pAudioRendThreadNum;
		delete []m_pAudioRendSystemNum;
		delete []m_pAudioRendSystemTime;
		delete []m_pAudioRendSampleTime;
		delete []m_pAudioRendPlayingTime;
		delete []m_pAudioRendBufferTime;
	}

	if (m_pVideoDecThreadNum != NULL)
	{
		delete []m_pVideoReadThreadNum;
		delete []m_pVideoReadSystemNum;
		delete []m_pVideoReadSampleTime;
		delete []m_pVideoReadKeyFrame;

		delete []m_pVideoDecThreadNum;
		delete []m_pVideoDecSystemNum;
		delete []m_pVideoPlayTime;
		delete []m_pVideoSampleTime;
		delete []m_pVideoSystemTime;
		delete []m_pVideoKeyFrame;

		delete []m_pVideoRenThreadNum;
		delete []m_pVideoRenSystemNum;
		delete []m_pVideoRenSysTime;
	}

	m_pAudioDecThreadNum = NULL;
	m_pVideoDecThreadNum = NULL;
}

VO_U32 CDumpPlayFile::Create (VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	if ((nType & 0X0F) == VO_VOMM_CREATE_SOURCE_URL)
	{
#ifdef UNICODE
		memset (m_szFileName, 0, sizeof (m_szFileName));
		WideCharToMultiByte (CP_ACP, 0, (TCHAR *)pSource, -1, m_szFileName, sizeof (m_szFileName), NULL, NULL);
#else
		strcpy (m_szFileName, (char *)pSource);
#endif // UNICODE
	}

	VO_U32	nRC = CPlayFile::Create (pSource, nType, nOffset, nLength);
	if (nRC != VO_ERR_NONE)
		return nRC;

	ReleaseBuffer ();

	VO_SOURCE_TRACKINFO	trkInfo;
	if (m_nAudioTrack >= 0)
	{
		m_pSource->GetTrackInfo (m_nAudioTrack, &trkInfo);
		if (trkInfo.Duration == 0)
		{
			VO_SOURCE_INFO	filInfo;
			m_pSource->GetSourceInfo (&filInfo);
			m_nMaxAudioIndex = filInfo.Duration / 10;
		}
		else
		{
			m_nMaxAudioIndex = trkInfo.Duration / 10;
		}
		if (m_nMaxAudioIndex > 102400)
			m_nMaxAudioIndex = 102400;

		m_pAudioReadThreadNum = new VO_S32[m_nMaxAudioIndex];
		m_pAudioReadSystemNum = new VO_S32[m_nMaxAudioIndex];
		m_pAudioReadSampleTime = new VO_S32[m_nMaxAudioIndex];
		m_pAudioReadSampleSize = new VO_S32[m_nMaxAudioIndex];

		m_pAudioDecThreadNum = new VO_S32[m_nMaxAudioIndex];
		m_pAudioDecSystemNum = new VO_S32[m_nMaxAudioIndex];

		m_pAudioRendThreadNum = new VO_S32[m_nMaxAudioIndex];
		m_pAudioRendSystemNum = new VO_S32[m_nMaxAudioIndex];
		m_pAudioRendSystemTime = new VO_S32[m_nMaxAudioIndex];
		m_pAudioRendSampleTime = new VO_S32[m_nMaxAudioIndex];
		m_pAudioRendPlayingTime = new VO_S32[m_nMaxAudioIndex];
		m_pAudioRendBufferTime = new VO_S32[m_nMaxAudioIndex];
	}

	if (m_nVideoTrack >= 0)
	{
		m_pSource->GetTrackInfo (m_nVideoTrack, &trkInfo);
		if (trkInfo.Duration == 0)
		{
			VO_SOURCE_INFO	filInfo;
			m_pSource->GetSourceInfo (&filInfo);
			m_nMaxVideoIndex = filInfo.Duration / 30;
		}
		else
		{
			m_nMaxVideoIndex = trkInfo.Duration / 30;
		}
		if (m_nMaxVideoIndex > 102400)
			m_nMaxVideoIndex = 102400;

		m_pVideoReadThreadNum = new VO_S32[m_nMaxVideoIndex];
		m_pVideoReadSystemNum = new VO_S32[m_nMaxVideoIndex];
		m_pVideoReadSampleTime = new VO_S32[m_nMaxVideoIndex];
		m_pVideoReadKeyFrame = new VO_S32[m_nMaxVideoIndex];

		m_pVideoDecThreadNum = new VO_S32[m_nMaxVideoIndex];
		m_pVideoDecSystemNum = new VO_S32[m_nMaxVideoIndex];
		m_pVideoPlayTime = new VO_S32[m_nMaxVideoIndex];
		m_pVideoSampleTime = new VO_S32[m_nMaxVideoIndex];
		m_pVideoSystemTime = new VO_S32[m_nMaxVideoIndex];
		m_pVideoKeyFrame = new VO_S32[m_nMaxVideoIndex];

		m_pVideoRenThreadNum = new VO_S32[m_nMaxVideoIndex];
		m_pVideoRenSystemNum = new VO_S32[m_nMaxVideoIndex];
		m_pVideoRenSysTime = new VO_S32[m_nMaxVideoIndex];
	}

	return VO_ERR_NONE;
}

VO_U32 CDumpPlayFile::Run (void)
{
	VO_U32 nRC = CPlayFile::Run ();

	m_nPlaybackStartTime = voOS_GetSysTime ();

	m_nPlaybackEndTime = 0;
	m_nIdleThreadTime = 0;

	return nRC;
}

VO_U32 CDumpPlayFile::Stop (void)
{
	m_nPlaybackEndTime = voOS_GetSysTime ();
//	m_nIdleThreadTime = voOS_GetThreadTime (m_hThreadIdle);

	VO_U32 nRC = CPlayFile::Stop ();

	return nRC;
}

#ifdef _WIN32
VO_U32 CDumpPlayFile::PlaybackIdleLoop (void)
{
	m_nIdleThreadStart = voOS_GetThreadTime (NULL);

	VO_U32 nRC = CPlayFile::PlaybackIdleLoop ();

//	m_nIdleThreadTime = voOS_GetThreadTime (NULL);

	return nRC;
}
#endif // _WIN32

VO_U32 CDumpPlayFile::ReadAudioData (VO_U32 nTrack, VO_PTR pTrackData)
{
	VO_U32	nSystemTime = voOS_GetSysTime ();
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);

	if (m_nAudioThreadStart == 0)
		m_nAudioThreadStart = nThreadTime;

	VO_U32 nRC = CPlayFile::ReadAudioData (nTrack, pTrackData);
	if (nRC != VO_ERR_NONE)
		return nRC;

	if (m_nAudioReadIndex < m_nMaxAudioIndex)
	{
		m_pAudioReadThreadNum[m_nAudioReadIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pAudioReadSystemNum[m_nAudioReadIndex] = voOS_GetSysTime () - nSystemTime;
		m_pAudioReadSampleTime[m_nAudioReadIndex] = (VO_U32)(m_audioSourceSample.Time - m_nFirstAudioPos);
		m_pAudioReadSampleSize[m_nAudioReadIndex] = (VO_U32)(m_audioSourceSample.Size & 0X7FFFFFFF);
	}

	m_nAudioReadIndex++;

	return nRC;
}

VO_U32 CDumpPlayFile::DecoderAudioData (VO_CODECBUFFER * pOutput, VO_AUDIO_FORMAT * pAudioFormat)
{
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);
	VO_U32	nSystemTime = voOS_GetSysTime ();

	VO_U32 nRC =  CPlayFile::DecoderAudioData (pOutput, pAudioFormat);
	if (nRC == VO_ERR_INPUT_BUFFER_SMALL)
		return nRC;

	if (m_nAudioDecIndex < m_nMaxAudioIndex)
	{
		m_pAudioDecThreadNum[m_nAudioDecIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pAudioDecSystemNum[m_nAudioDecIndex] = voOS_GetSysTime () - nSystemTime;
	}

	m_nAudioDecIndex++;

	return nRC;
}

VO_U32 CDumpPlayFile::RenderAudioData (VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart)
{
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);
	VO_U32	nSystemTime = voOS_GetSysTime ();

	CPlayFile::RenderAudioData (pBuffer, nSize, nStart);

	if (m_nAudioRenIndex < m_nMaxAudioIndex)
	{
		m_pAudioRendThreadNum[m_nAudioRenIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pAudioRendSystemNum[m_nAudioRenIndex] = voOS_GetSysTime () - nSystemTime;
		m_pAudioRendSystemTime[m_nAudioRenIndex] = voOS_GetSysTime ();
		m_pAudioRendSampleTime[m_nAudioRenIndex] = (VO_U32)nStart;
		m_pAudioRendPlayingTime[m_nAudioRenIndex] = (VO_U32)GetPlayingTime ();
		m_pAudioRendBufferTime[m_nAudioRenIndex] = m_pAudioRender->GetBufferTime ();
	}

	m_nAudioRenIndex++;

	m_nAudioThreadTime = voOS_GetThreadTime (NULL);

	return VO_ERR_NONE;
}

VO_U32 CDumpPlayFile::ReadVideoData (VO_U32 nTrack, VO_PTR pTrackData)
{
	VO_U32	nSystemTime = voOS_GetSysTime ();
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);

	if (m_nVideoThreadStart == 0)
		m_nVideoThreadStart = nThreadTime;

	VO_U32 nRC = CPlayFile::ReadVideoData (nTrack, pTrackData);
	if (nRC != VO_ERR_NONE)
		return nRC;

	if (m_nVideoReadIndex < m_nMaxVideoIndex)
	{
		m_pVideoReadThreadNum[m_nVideoReadIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pVideoReadSystemNum[m_nVideoReadIndex] = voOS_GetSysTime () - nSystemTime;
		m_pVideoReadSampleTime[m_nVideoReadIndex] = (VO_U32)(m_videoSourceSample.Time - m_nFirstVideoPos);
		m_pVideoReadKeyFrame[m_nVideoReadIndex] = m_videoSourceSample.Size;
	}

	m_nVideoReadIndex++;

	return nRC;
}

VO_U32 CDumpPlayFile::SetInputVideoData (VO_CODECBUFFER * pInput)
{
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);
	VO_U32	nSystemTime = voOS_GetSysTime ();

	VO_U32 nRC =  CPlayFile::SetInputVideoData (pInput);
//	if (nRC != VO_ERR_NONE)
//		return nRC;

	if (m_nVideoDecIndex < m_nMaxVideoIndex)
	{
		m_pVideoDecThreadNum[m_nVideoDecIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pVideoDecSystemNum[m_nVideoDecIndex] = voOS_GetSysTime () - nSystemTime;

		m_pVideoSampleTime[m_nVideoDecIndex] = (VO_U32)(m_videoSourceSample.Time - m_nFirstVideoPos);
		m_pVideoPlayTime[m_nVideoDecIndex] = (VO_U32)GetPlayingTime ();
		m_pVideoSystemTime[m_nVideoDecIndex] = voOS_GetSysTime ();

		m_pVideoKeyFrame[m_nVideoDecIndex] = (m_videoSourceSample.Size & 0X80000000) > 0 ? 1 : 0;
	}

	m_nVideoDecIndex++;

//	Sleep (80);

	return nRC;
}

VO_U32 CDumpPlayFile::DecoderVideoData (VO_VIDEO_BUFFER * pOutput, VO_VIDEO_FORMAT * pVideoFormat, VO_BOOL * pMoreOutput)
{
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);
	VO_U32	nSystemTime = voOS_GetSysTime ();

	VO_U32 nRC =  CPlayFile::DecoderVideoData (pOutput, pVideoFormat, pMoreOutput);
	if (nRC == VO_ERR_INPUT_BUFFER_SMALL || pOutput->Buffer[0] == NULL)
		return nRC;

	if (m_nVideoDecIndex < m_nMaxVideoIndex)
	{
		m_pVideoDecThreadNum[m_nVideoDecIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pVideoDecSystemNum[m_nVideoDecIndex] = voOS_GetSysTime () - nSystemTime;

		m_pVideoSampleTime[m_nVideoDecIndex] = (VO_U32)(m_videoSourceSample.Time - m_nFirstVideoPos);
		m_pVideoPlayTime[m_nVideoDecIndex] = (VO_U32)GetPlayingTime ();
		m_pVideoSystemTime[m_nVideoDecIndex] = voOS_GetSysTime ();

		m_pVideoKeyFrame[m_nVideoDecIndex] = (m_videoSourceSample.Size & 0X80000000) > 0 ? 1 : 0;
	}

	m_nVideoDecIndex++;

	return nRC;
}

VO_U32 CDumpPlayFile::RenderVideoData (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VO_U32	nThreadTime = voOS_GetThreadTime (NULL);
	VO_U32	nSystemTime = voOS_GetSysTime ();

	VO_U32 nRC = CPlayFile::RenderVideoData (pVideoBuffer, nStart, bWait);

	if (m_nVideoRenIndex < m_nMaxVideoIndex)
	{
		m_pVideoRenThreadNum[m_nVideoRenIndex] = voOS_GetThreadTime (NULL) - nThreadTime;
		m_pVideoRenSystemNum[m_nVideoRenIndex] = voOS_GetSysTime () - nSystemTime;
		m_pVideoRenSysTime[m_nVideoRenIndex] = voOS_GetSysTime ();
	}

	m_nVideoRenIndex++;

	m_nVideoThreadTime = voOS_GetThreadTime (NULL);

//	Sleep (50);

	return nRC;
}

VO_U32 CDumpPlayFile::EndOfSource (void)
{
	if (m_nVideoDecIndex == 0 && m_nAudioDecIndex == 0)
	{
		VOLOGW ("There was no audio and video data!");
		return 0;
	}

	if (m_nPlaybackEndTime == 0)
		m_nPlaybackEndTime = voOS_GetSysTime ();
#ifdef _WIN32
	if (m_nIdleThreadTime == 0)
		m_nIdleThreadTime = voOS_GetThreadTime (m_hThreadIdle);
#endif // _WIN32

	VO_PCHAR pDumpFile = NULL;
	if (m_pConfig != NULL)
		pDumpFile = m_pConfig->GetItemText ((char*)"vowPlay", (char*)"DumpLogFile");

	VO_TCHAR szFile[1024];
#ifdef _WIN32
	if (pDumpFile == NULL)
	{
#ifdef _WIN32_WCE
		vostrcpy (szFile,  _T("\\voMMPlay.txt"));
#else
		vostrcpy (szFile,  _T("C:\\voMMPlay.txt"));
#endif // _WIN32_WCE
	}
	else
	{
		memset (szFile, 0, sizeof (szFile));
		MultiByteToWideChar (CP_ACP, 0, pDumpFile, -1, szFile, sizeof (szFile));
	}
#elif defined _LINUX
	if (pDumpFile == NULL)
		vostrcpy (szFile,  _T("/sdcard/voMMPlay.txt"));
	else
		vostrcpy (szFile,  pDumpFile);
#endif // _WIN32

	VO_FILE_SOURCE filSource;
	memset (&filSource, 0, sizeof (VO_FILE_SOURCE));
	filSource.pSource = szFile;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_WRITE_ONLY;

	VO_PTR hFile = m_pFileOP->Open (&filSource);
	if (hFile == NULL)
	{
		VOLOGE ("It could not open the dump file %s!", pDumpFile);
		//return 0;
	}
	m_pFileOP->Seek (hFile, 0, VO_FILE_END);

	if (m_nAudioReadIndex > m_nMaxAudioIndex)
		m_nAudioReadIndex = m_nMaxAudioIndex;
	if (m_nAudioDecIndex > m_nMaxAudioIndex)
		m_nAudioDecIndex = m_nMaxAudioIndex;
	if (m_nAudioRenIndex > m_nMaxAudioIndex)
		m_nAudioRenIndex = m_nMaxAudioIndex;
	if (m_nVideoReadIndex > m_nMaxVideoIndex)
		m_nVideoReadIndex = m_nMaxVideoIndex;
	if (m_nVideoDecIndex > m_nMaxVideoIndex)
		m_nVideoDecIndex = m_nMaxVideoIndex;
	if (m_nVideoRenIndex > m_nMaxVideoIndex)
		m_nVideoRenIndex = m_nMaxVideoIndex;

	VO_U32 i = 0;
	char szLine[128];

	sprintf (szLine, "File:	%s\r\n", m_szFileName);
	m_pFileOP->Write (hFile, szLine, strlen (szLine));

	double dFrameRate = 0.0;
	if (m_nVideoTrack >= 0)
	{
		VO_SOURCE_TRACKINFO	trkInfo;
		m_pSource->GetTrackInfo (m_nVideoTrack, &trkInfo);

		char szCodec[64];
		if (trkInfo.Codec == VO_VIDEO_CodingMPEG2)
			strcpy (szCodec, "MPEG2");
		else if (trkInfo.Codec == VO_VIDEO_CodingH263)
			strcpy (szCodec, "H263");
		else if (trkInfo.Codec == VO_VIDEO_CodingS263)
			strcpy (szCodec, "S263");
		else if (trkInfo.Codec == VO_VIDEO_CodingMPEG4)
			strcpy (szCodec, "MPEG4");
		else if (trkInfo.Codec == VO_VIDEO_CodingH264)
			strcpy (szCodec, "H264");
		else if (trkInfo.Codec == VO_VIDEO_CodingWMV)
			strcpy (szCodec, "WMV");
		else if (trkInfo.Codec == VO_VIDEO_CodingRV)
			strcpy (szCodec, "RealVideo");
		else if (trkInfo.Codec == VO_VIDEO_CodingMJPEG)
			strcpy (szCodec, "MJPEG");
		else if (trkInfo.Codec == VO_VIDEO_CodingDIVX)
			strcpy (szCodec, "DIVX");
		else if (trkInfo.Codec == VO_VIDEO_CodingVP6)
			strcpy (szCodec, "VP6");
		else
			strcpy (szCodec, "Unknown");

		if (m_nVideoDecIndex > 1)
		{
			int nFrameTime = m_pVideoSampleTime[1] - m_pVideoSampleTime[0];
			nFrameTime = nFrameTime * 100;

			if (nFrameTime > 0)
				dFrameRate = 100000.0 / nFrameTime;
			dFrameRate = (int)dFrameRate;
		}

		double dBitRate = 0.0;
		if (m_nVideoReadIndex > 1)
		{
			VO_U32 uTime = 1;
			VO_U32 uSize = m_pVideoReadKeyFrame[0] & 0X7FFFFFFF;
			for (i = 1; i < m_nVideoReadIndex; i++)
			{
				uSize = uSize + (m_pVideoReadKeyFrame[i] & 0X7FFFFFFF);
				uTime = uTime + (m_pVideoReadSampleTime[i] - m_pVideoReadSampleTime[i-1]);
			}
			dBitRate = uSize * 8.0 / uTime;
		}

		sprintf (szLine, "Video:	Codec: %s	Size: %d X %d, FrameRate: %0.0f F/S	BitRate: %0.2f K/S \r\n", szCodec,(int)m_fmtVideo.Width,(int)m_fmtVideo.Height, dFrameRate, dBitRate);
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
	}

	if (m_nAudioTrack >= 0)
	{
		VO_SOURCE_TRACKINFO	trkInfo;
		m_pSource->GetTrackInfo (m_nAudioTrack, &trkInfo);

		char szCodec[64];
		if (trkInfo.Codec == VO_AUDIO_CodingPCM)
			strcpy (szCodec, "PCM");
		else if (trkInfo.Codec == VO_AUDIO_CodingADPCM)
			strcpy (szCodec, "ADPCM");
		else if (trkInfo.Codec == VO_AUDIO_CodingAMRNB)
			strcpy (szCodec, "AMR_NB");
		else if (trkInfo.Codec == VO_AUDIO_CodingAMRWB)
			strcpy (szCodec, "AMR_WB");
		else if (trkInfo.Codec == VO_AUDIO_CodingAMRWBP)
			strcpy (szCodec, "AMR_WB+");
		else if (trkInfo.Codec == VO_AUDIO_CodingQCELP13)
			strcpy (szCodec, "QCELP13");
		else if (trkInfo.Codec == VO_AUDIO_CodingEVRC)
			strcpy (szCodec, "EVRC");
		else if (trkInfo.Codec == VO_AUDIO_CodingAAC)
			strcpy (szCodec, "AAC");
		else if (trkInfo.Codec == VO_AUDIO_CodingAC3)
			strcpy (szCodec, "AC3");
		else if (trkInfo.Codec == VO_AUDIO_CodingFLAC)
			strcpy (szCodec, "FLAC");
		else if (trkInfo.Codec == VO_AUDIO_CodingMP3)
			strcpy (szCodec, "MP3");
		else if (trkInfo.Codec == VO_AUDIO_CodingOGG)
			strcpy (szCodec, "OGG");
		else if (trkInfo.Codec == VO_AUDIO_CodingWMA)
			strcpy (szCodec, "WMA");
		else if (trkInfo.Codec == VO_AUDIO_CodingRA)
			strcpy (szCodec, "RealAudio");
		else if (trkInfo.Codec == VO_AUDIO_CodingMIDI)
			strcpy (szCodec, "MIDI");
		else
			strcpy (szCodec, "Unknown");

		double dBitRate = 0.0;
		if (m_nAudioReadIndex > 1)
		{
			VO_U32 uTime = 1;
			VO_U32 uSize = m_pAudioReadSampleSize[0] & 0X7FFFFFFF;
			for (i = 1; i < m_nAudioReadIndex; i++)
			{
				uSize = uSize + (m_pAudioReadSampleSize[i] & 0X7FFFFFFF);
				uTime = uTime + (m_pAudioReadSampleTime[i] - m_pAudioReadSampleTime[i-1]);
			}
			dBitRate = uSize * 8.0 / uTime;
		}

		sprintf (szLine, "Audio:	Codec: %s	SampleRate: %d Channels: %d, BitRate: %0.2f K/S \r\n", szCodec,(int)m_fmtAudio.SampleRate, (int)m_fmtAudio.Channels, dBitRate);
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
	}

	VO_U32 nThreadTotalTime = 0;
	VO_U32 nSystemTotalTime = 0;

	if (m_nVideoDecIndex > 0)
	{
		nThreadTotalTime = 0;
		nSystemTotalTime = 0;
		for (i = 0; i < m_nVideoReadIndex; i++)
		{
			nThreadTotalTime += m_pVideoReadThreadNum[i];
			nSystemTotalTime += m_pVideoReadSystemNum[i];
		}
		if (nThreadTotalTime == 0)
			nThreadTotalTime = 1;
		if (nSystemTotalTime == 0)
			nSystemTotalTime = 1;

		sprintf (szLine, "VideoRead:Frames: %d, Speed: %0.2f F/S	%0.2f F/S\r\n",(int)m_nVideoReadIndex,
						(double)(m_nVideoReadIndex * 1000) / nThreadTotalTime, (double)(m_nVideoReadIndex * 1000) / nSystemTotalTime);
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		VOLOGI ("%s", szLine);

		nThreadTotalTime = 0;
		nSystemTotalTime = 0;
		for (i = 0; i < m_nVideoDecIndex; i++)
		{
			nThreadTotalTime += m_pVideoDecThreadNum[i];
			nSystemTotalTime += m_pVideoDecSystemNum[i];
		}
		if (nThreadTotalTime == 0)
			nThreadTotalTime = 1;
		if (nSystemTotalTime == 0)
			nSystemTotalTime = 1;

		sprintf (szLine, "VideoDec: Frames: %d, Speed: %0.2f F/S	%0.2f F/S\r\n", (int)m_nVideoDecIndex,
						(double)(m_nVideoDecIndex * 1000) / nThreadTotalTime, (double)(m_nVideoDecIndex * 1000) / nSystemTotalTime);

		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		VOLOGI ("%s", szLine);

		if (m_nVideoRenIndex > 0)
		{
			nThreadTotalTime = 0;
			nSystemTotalTime = 0;
			for (i = 0; i < m_nVideoRenIndex; i++)
			{
				nThreadTotalTime += m_pVideoRenThreadNum[i];
				nSystemTotalTime += m_pVideoRenSystemNum[i];
			}
			if (nThreadTotalTime == 0)
				nThreadTotalTime = 1;
			if (nSystemTotalTime == 0)
				nSystemTotalTime = 1;

			sprintf (szLine, "VideoRen: Frames: %d, Speed: %0.2f F/S	%0.2f F/S \r\n",(int)m_nVideoRenIndex,
							(double)(m_nVideoRenIndex * 1000) / nThreadTotalTime, (double)(m_nVideoRenIndex * 1000) / nSystemTotalTime);

			m_pFileOP->Write (hFile, szLine, strlen (szLine));

			VOLOGI ("%s", szLine);

			double dPlaySpeed = (double)(m_nVideoDecIndex * 1000) / (m_pVideoSampleTime[m_nVideoDecIndex - 1] - m_pVideoSampleTime[0]);
			if (dPlaySpeed > dFrameRate)
				dPlaySpeed = dFrameRate;
			sprintf (szLine, "PlayBack: Read: %d	Play %0.2f F/S\r\n", (int)m_nVideoReadIndex, dPlaySpeed);
			m_pFileOP->Write (hFile, szLine, strlen (szLine));

			VOLOGI ("%s", szLine);
		}

		if (m_nAudioDecIndex > 0)
		{
			nThreadTotalTime = 0;
			nSystemTotalTime = 0;
			for (i = 0; i < m_nAudioReadIndex; i++)
			{
				nThreadTotalTime += m_pAudioReadThreadNum[i];
				nSystemTotalTime += m_pAudioReadSystemNum[i];
			}
			if (nThreadTotalTime == 0)
				nThreadTotalTime = 1;
			if (nSystemTotalTime == 0)
				nSystemTotalTime = 1;

			sprintf (szLine, "AudioRead:Frames: %d, Speed: %0.2f F/S	%0.2f F/S\r\n", (int)m_nAudioReadIndex,
							(double)(m_nAudioReadIndex * 1000) / nThreadTotalTime, (double)(m_nAudioReadIndex * 1000) / nSystemTotalTime);
			m_pFileOP->Write (hFile, szLine, strlen (szLine));

			nThreadTotalTime = 0;
			nSystemTotalTime = 0;
			for (i = 0; i < m_nAudioDecIndex; i++)
			{
				nThreadTotalTime += m_pAudioDecThreadNum[i];
				nSystemTotalTime += m_pAudioDecSystemNum[i];
			}
			if (nThreadTotalTime == 0)
				nThreadTotalTime = 1;
			if (nSystemTotalTime == 0)
				nSystemTotalTime = 1;

			sprintf (szLine, "AudioDec: Frames: %d, Speed: %0.2f F/S	%0.2f F/S\r\n", (int)m_nAudioDecIndex,
							(double)(m_nAudioDecIndex * 1000) / nThreadTotalTime, (double)(m_nAudioDecIndex * 1000) / nSystemTotalTime);

			m_pFileOP->Write (hFile, szLine, strlen (szLine));

			VOLOGI ("%s", szLine);

			nThreadTotalTime = 0;
			nSystemTotalTime = 0;
			for (i = 0; i < m_nAudioRenIndex; i++)
			{
				nThreadTotalTime += m_pAudioRendThreadNum[i];
				nSystemTotalTime += m_pAudioRendSystemNum[i];
			}
			if (nThreadTotalTime == 0)
				nThreadTotalTime = 1;
			if (nSystemTotalTime == 0)
				nSystemTotalTime = 1;

			sprintf (szLine, "AudioRen: Frames: %d, Speed: %0.2f F/S	%0.2f F/S\r\n", (int)m_nAudioRenIndex,
							(double)(m_nAudioRenIndex * 1000) / nThreadTotalTime, (double)(m_nAudioRenIndex * 1000.0) / nSystemTotalTime);
			m_pFileOP->Write (hFile, szLine, strlen (szLine));
		}

		VO_U32 nPlaybackTime = m_nPlaybackEndTime - m_nPlaybackStartTime;
		if (m_nIdleThreadTime - m_nIdleThreadStart > nPlaybackTime)
			m_nIdleThreadTime = m_nIdleThreadStart + nPlaybackTime;
		sprintf (szLine, "CPU:	PlayBack: %0.2f Video: %0.2f	Audio: %0.2f \r\n", (double)(nPlaybackTime - (m_nIdleThreadTime - m_nIdleThreadStart)) * 100.0 / nPlaybackTime,
													(double)(m_nVideoThreadTime - m_nVideoThreadStart) * 100.0 / nPlaybackTime,
													(double)(m_nAudioThreadTime - m_nAudioThreadStart) * 100.0 / nPlaybackTime);
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
	}

	strcpy (szLine, "\r\n\r\n");
	m_pFileOP->Write (hFile, szLine, strlen (szLine));
	m_pFileOP->Close (hFile);

	int nDumpLevel = 0;
	if (m_pConfig != NULL)
		nDumpLevel = m_pConfig->GetItemValue ((char*)"vowPlay", (char*)"DumpLogLevel", 0);
	if (nDumpLevel < 2)
	{
		m_nVideoDecIndex = 0;
		m_nVideoRenIndex = 0;
		m_nAudioDecIndex = 0;
		m_nAudioRenIndex = 0;

		return 0;
	}

	VO_TCHAR * pExt = vostrrchr (szFile, _T('.'));
	*pExt = 0;
	vostrcat (szFile, _T("_detail.txt"));

	filSource.pSource = szFile;
	filSource.nFlag = VO_FILE_TYPE_NAME;
	filSource.nMode = VO_FILE_WRITE_ONLY;
	hFile = m_pFileOP->Open (&filSource);
	if (hFile == NULL)
		return 0;

	if (m_nVideoDecIndex > 0)
	{
		strcpy (szLine, "****** Video Decoder Speed.******\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
		strcpy (szLine, "Index	DecT	DecS	Sys	Play	Diff	Sample	Step	Diff	Key\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
	}
	for (i = 0; i < m_nVideoDecIndex; i++)
	{
		if (i == 0)
		{
			sprintf (szLine, "%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\r\n",
							(int)i,(int)m_pVideoDecThreadNum[i], (int)m_pVideoDecSystemNum[i],(int)(m_pVideoSystemTime[i] - m_pVideoSystemTime[0]),(int)m_pVideoPlayTime[i],
							(int)(m_pVideoSystemTime[i] - m_pVideoSystemTime[0] - m_pVideoPlayTime[i]),(int) m_pVideoSampleTime[i], 0,
							(int)(m_pVideoPlayTime[i] - m_pVideoSampleTime[i]), (int)m_pVideoKeyFrame[i]);
		}
		else
		{
			sprintf (szLine, "%d	%d	%d	%d	%d	%d	%d	%d	%d	%d\r\n",
							(int)i,(int) m_pVideoDecThreadNum[i], (int)m_pVideoDecSystemNum[i], (int)(m_pVideoSystemTime[i] - m_pVideoSystemTime[0]), (int)m_pVideoPlayTime[i],
							(int)(m_pVideoSystemTime[i] - m_pVideoSystemTime[0] - m_pVideoPlayTime[i]),
							(int)m_pVideoSampleTime[i],(int)( m_pVideoSampleTime[i] - m_pVideoSampleTime[i-1]),
							(int)(m_pVideoPlayTime[i] - m_pVideoSampleTime[i]),(int)m_pVideoKeyFrame[i]);
		}
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
	}

	if (m_nVideoRenIndex > 0)
	{
		strcpy (szLine, "\r\n\r\n****** Video Render Speed ******.\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		strcpy (szLine, "Index	Thread		System		Time		Step \r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		for (i = 0; i < m_nVideoRenIndex; i++)
		{
			if (i == 0)
				sprintf (szLine, "%d	%d		%d		%d		%d\r\n", (int)i,(int) m_pVideoRenThreadNum[i],(int) m_pVideoRenSystemNum[i], 0, 0);
			else
				sprintf (szLine, "%d	%d		%d		%d		%d\r\n", (int)i, (int)m_pVideoRenThreadNum[i],  (int)m_pVideoRenSystemNum[i],
								(int)(m_pVideoRenSysTime[i] - m_pVideoRenSysTime[0]), (int)(m_pVideoRenSysTime[i] - m_pVideoRenSysTime[i-1]));
			m_pFileOP->Write (hFile, szLine, strlen (szLine));
		}
	}

	if (m_nAudioDecIndex > 0)
	{
		strcpy (szLine, "\r\n\r\n****** Audio Decoder Speed.******\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		strcpy (szLine, "Index	Thread		System \r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		for (i = 0; i < m_nAudioDecIndex; i++)
		{
			sprintf (szLine, "%d	%d		%d\r\n", (int)i, (int)m_pAudioDecThreadNum[i], (int)m_pAudioDecSystemNum[i]);
			m_pFileOP->Write (hFile, szLine, strlen (szLine));
		}
	}

	// dump audio render information
	if (m_nAudioRenIndex > 0)
	{
		strcpy (szLine, "\r\n\r\n****** Audio Render Information.******\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		strcpy (szLine, "Index	RenS	Sample	Sys	Play	DiffSys	DiffSmp	Buffer\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		for (i = 0; i < m_nAudioRenIndex; i++)
		{
			VO_U32 nSysTime = m_pAudioRendSystemTime[i] - m_pAudioRendSystemTime[0];

			sprintf (szLine, "%d	%d	%d	%d	%d	%d	%d	%d\r\n", (int)i, (int)m_pAudioRendThreadNum[i], (int)m_pAudioRendSampleTime[i],
								(int)nSysTime, (int)m_pAudioRendPlayingTime[i],(int)(nSysTime - m_pAudioRendPlayingTime[i]),
								(int)(m_pAudioRendSampleTime[i] - m_pAudioRendPlayingTime[i]), (int)m_pAudioRendBufferTime[i]);
			m_pFileOP->Write (hFile, szLine, strlen (szLine));
		}
	}

	// dump video track sample information
	if (m_nVideoReadIndex > 0)
	{
		strcpy (szLine, "\r\n\r\n****** Video Track Infor:****** \r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
		strcpy (szLine, "nIndex	Read, Sample	Step	Key	Size\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		char szKey[12];
		for (i = 0; i < m_nVideoReadIndex; i++)
		{
			if ((m_pVideoReadKeyFrame[i] & 0X80000000) > 0)
				strcpy (szKey, "Key");
			else
				strcpy (szKey, "0");

			if (i == 0)
				sprintf (szLine, "%d	%d	%d	%d	%s	%d\r\n", (int)i, (int)m_pVideoReadThreadNum[i],(int) m_pVideoReadSampleTime[i], 0, szKey,(int)( m_pVideoReadKeyFrame[i] & 0X7FFFFFFF));
			else
				sprintf (szLine, "%d	%d	%d	%d	%s	%d\r\n",(int)i, (int)m_pVideoReadThreadNum[i], (int)m_pVideoReadSampleTime[i],
																(int)(m_pVideoReadSampleTime[i] - m_pVideoReadSampleTime[i-1]), szKey, (int)(m_pVideoReadKeyFrame[i] & 0X7FFFFFFF));
			m_pFileOP->Write (hFile, szLine, strlen (szLine));
		}
	}

	// dump audio track sample information
	if (m_nAudioReadIndex > 0)
	{
		strcpy (szLine, "\r\n\r\n****** Audio Track Infor:****** \r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));
		strcpy (szLine, "nIndex	Read, Sample	Step	Size\r\n");
		m_pFileOP->Write (hFile, szLine, strlen (szLine));

		for (i = 0; i < m_nAudioReadIndex; i++)
		{
			if (i == 0)
				sprintf (szLine, "%d	%d	%d	%d	%d\r\n", (int)i, (int)m_pAudioReadThreadNum[i], (int)m_pAudioReadSampleTime[i], 0,(int)( m_pAudioReadSampleSize[i] & 0X7FFFFFFF));
			else
				sprintf (szLine, "%d	%d	%d	%d	%d\r\n", (int)i, (int)m_pAudioReadThreadNum[i], (int)m_pAudioReadSampleTime[i],
																(int)(m_pAudioReadSampleTime[i] - m_pAudioReadSampleTime[i-1]), (int)(m_pAudioReadSampleSize[i] & 0X7FFFFFFF));
			m_pFileOP->Write (hFile, szLine, strlen (szLine));
		}
	}

	m_pFileOP->Close (hFile);

	m_nVideoDecIndex = 0;
	m_nVideoRenIndex = 0;
	m_nAudioDecIndex = 0;
	m_nAudioRenIndex = 0;

	VOLOGI ("Dump the file");

	return 0;
}

