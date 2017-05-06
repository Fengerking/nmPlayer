	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCMediaPlayer.cpp

	Contains:	voCMediaPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2010-08-18		JBF			Create file

*******************************************************************************/
#ifdef __SYMBIAN32__
#include <stdlib.h>
#include "snprintf.h"
#elif defined _IOS
#include <stdlib.h>
#elif defined _MAC_OS
#include <stdlib.h>
#else
#include <malloc.h>
#endif // __SYMBIAN32__
#include <string.h>
#include <stdio.h>

#include "vompMeta.h"
#include "voFile.h"

#include "voCMediaPlayer.h"

#define LOG_TAG "voCMediaPlayerMeta"
#include "voLog.h"

int voCMediaPlayer::GetMetaData (int nIndex, void * pValue)
{
	if (m_pFileSource == NULL)
		return false;

	memset (m_szMetaText, 0, sizeof (m_szMetaText));
	memset (&m_msMetaInfo, 0, sizeof (m_msMetaInfo));

	int nRC = 0;
	int nID = nIndex;
	switch (nID)
	{
	case VOMP_PID_METADATA_FRONTCOVER:
	{
		MetaDataImage metaData;
		memset(&metaData, 0, sizeof(metaData));
		nRC = m_pFileSource->GetSourceParam (VO_PID_METADATA_FRONTCOVER, &metaData);
		if (metaData.pBuffer != NULL)
		{
			*(VO_PBYTE *)pValue = metaData.pBuffer;
		}

		return metaData.dwBufferSize;
	}

	case VOMP_PID_METADATA_DURATION:
#ifdef __SYMBIAN32__
		snprintf (m_szMetaText, 1024, "%d", m_sFilInfo.Duration);
#else
		sprintf (m_szMetaText, "%d", (int)m_sFilInfo.Duration);
#endif // __SYMBIAN32__
		break;

	case VOMP_PID_METADATA_CODEC:
	{
		VO_SOURCE_INFO	filInfo;
		nRC = m_pFileSource->GetSourceInfo (&filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			nRC = m_pFileSource->GetTrackInfo (i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			{
				strcat (m_szMetaText, "Audio ");

				char szAudio[128];
				if (trkInfo.Codec == VO_AUDIO_CodingPCM)
					strcpy (szAudio, "PCM");
				else if (trkInfo.Codec == VO_AUDIO_CodingADPCM)
					strcpy (szAudio, "ADPCM");
				else if (trkInfo.Codec == VO_AUDIO_CodingAMRNB)
					strcpy (szAudio, "AMR_NB");
				else if (trkInfo.Codec == VO_AUDIO_CodingAMRWB)
					strcpy (szAudio, "AMR_WB");
				else if (trkInfo.Codec == VO_AUDIO_CodingAMRWBP)
					strcpy (szAudio, "AMR_WB+");
				else if (trkInfo.Codec == VO_AUDIO_CodingQCELP13)
					strcpy (szAudio, "QCELP13");
				else if (trkInfo.Codec == VO_AUDIO_CodingEVRC)
					strcpy (szAudio, "EVRC");
				else if (trkInfo.Codec == VO_AUDIO_CodingAAC)
					strcpy (szAudio, "AAC");
				else if (trkInfo.Codec == VO_AUDIO_CodingAC3)
					strcpy (szAudio, "AC3");
				else if (trkInfo.Codec == VO_AUDIO_CodingFLAC)
					strcpy (szAudio, "FLAC");
				else if (trkInfo.Codec == VO_AUDIO_CodingMP3)
					strcpy (szAudio, "MP3");
				else if (trkInfo.Codec == VO_AUDIO_CodingOGG)
					strcpy (szAudio, "OGG");
				else if (trkInfo.Codec == VO_AUDIO_CodingWMA)
					strcpy (szAudio, "WMA");
				else if (trkInfo.Codec == VO_AUDIO_CodingRA)
					strcpy (szAudio, "RealAudio");
				else if (trkInfo.Codec == VO_AUDIO_CodingMIDI)
					strcpy (szAudio, "MIDI");
				else
					strcpy (szAudio, "Unknown");

				strcat (m_szMetaText, szAudio);
				strcat (m_szMetaText, " ");
			}
			else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				strcat (m_szMetaText, "Video ");

				char szVideo[128];
				if (trkInfo.Codec == VO_VIDEO_CodingMPEG2)
					strcpy (szVideo, "MPEG2");
				else if (trkInfo.Codec == VO_VIDEO_CodingH263)
					strcpy (szVideo, "H263");
				else if (trkInfo.Codec == VO_VIDEO_CodingS263)
					strcpy (szVideo, "S263");
				else if (trkInfo.Codec == VO_VIDEO_CodingMPEG4)
					strcpy (szVideo, "MPEG4");
				else if (trkInfo.Codec == VO_VIDEO_CodingH264)
					strcpy (szVideo, "H264");
				else if (trkInfo.Codec == VO_VIDEO_CodingWMV)
					strcpy (szVideo, "WMV");
				else if (trkInfo.Codec == VO_VIDEO_CodingVC1)
					strcpy (szVideo, "VC1");
				else if (trkInfo.Codec == VO_VIDEO_CodingRV)
					strcpy (szVideo, "RealVideo");
				else if (trkInfo.Codec == VO_VIDEO_CodingMJPEG)
					strcpy (szVideo, "MJPEG");
				else if (trkInfo.Codec == VO_VIDEO_CodingDIVX)
					strcpy (szVideo, "DIVX");
				else if (trkInfo.Codec == VO_VIDEO_CodingVP6)
					strcpy (szVideo, "VP6");
				else
					strcpy (szVideo, "Unknown");

				strcat (m_szMetaText, szVideo);
				strcat (m_szMetaText, " ");
			}
		}
	}	
		break;

	case VOMP_PID_METADATA_SAMPLERATE:
	{
		strcpy (m_szMetaText, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_pFileSource->GetSourceInfo (&filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			nRC = m_pFileSource->GetTrackInfo (i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			{
				VO_AUDIO_FORMAT fmtAudio;
				nRC = m_pFileSource->GetTrackParam (i, VO_PID_AUDIO_FORMAT, &fmtAudio);
#ifdef __SYMBIAN32__
				snprintf (m_szMetaText, 1024,  "%d", fmtAudio.SampleRate);
#else
				sprintf (m_szMetaText, "%d", (int)fmtAudio.SampleRate);
#endif // __SYMBIAN32__
			}
		}
	}
		break;

	case VOMP_PID_METADATA_BITRATE:
	{
		VO_S64 llFileSize = 0;
		nRC = m_pFileSource->GetSourceParam (VO_PID_SOURCE_FILESIZE, &llFileSize);

		VO_SOURCE_INFO	filInfo;
		nRC = m_pFileSource->GetSourceInfo (&filInfo);
		if (filInfo.Duration == 0)
		{
			strcpy (m_szMetaText, "0");
		}
		else
		{
			VO_U32 uBitRate = (VO_U32)((llFileSize * 8) / (filInfo.Duration / 1000));
#ifdef __SYMBIAN32__
			snprintf (m_szMetaText, 1024, "%d", uBitRate);
#else
			sprintf (m_szMetaText, "%d", (int)uBitRate);
#endif // __SYMBIAN32__
			VOLOGI ("%d, %s", (int)uBitRate, m_szMetaText);
		}
	}
		break;

	case VOMP_PID_METADATA_FRAMERATE:
	{
		strcpy (m_szMetaText, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_pFileSource->GetSourceInfo (&filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			nRC = m_pFileSource->GetTrackInfo (i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_U32 uFrameTime = 0;
				nRC = m_pFileSource->GetTrackParam (i, VO_PID_SOURCE_FRAMETIME, &uFrameTime);
				if (uFrameTime > 0)
#ifdef __SYMBIAN32__
					snprintf (m_szMetaText, 1024, "%d", 100000 / uFrameTime);	//unit is 1/100ms
#else
					sprintf (m_szMetaText, "%d", (int)(100000 / uFrameTime));	//unit is 1/100ms
#endif // __SYMBIAN32__

			}
		}
	}
		break;

	case VOMP_PID_METADATA_FILEFORMAT:
	{
		if (m_nFF == VO_FILE_FFMOVIE_MP4)
			strcpy (m_szMetaText, "MP4 (3GP)");
		else if (m_nFF == VO_FILE_FFMOVIE_ASF)
			strcpy (m_szMetaText, "WMV (ASF)");
		else if (m_nFF == VO_FILE_FFMOVIE_AVI)
			strcpy (m_szMetaText, "AVI");
		else if (m_nFF == VO_FILE_FFMOVIE_REAL)
			strcpy (m_szMetaText, "REAL");
		else if (m_nFF == VO_FILE_FFMOVIE_MPG)
			strcpy (m_szMetaText, "MPG");
		else if (m_nFF == VO_FILE_FFMOVIE_FLV)
			strcpy (m_szMetaText, "FLV");
		else if (m_nFF == VO_FILE_FFMOVIE_MKV)
			strcpy (m_szMetaText, "MKV");
		else
			strcpy (m_szMetaText, "Audio");
	}
		break;

	case VOMP_PID_METADATA_VIDEOWIDTH:
	{
		strcpy (m_szMetaText, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_pFileSource->GetSourceInfo (&filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			nRC = m_pFileSource->GetTrackInfo (i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_VIDEO_FORMAT fmtVideo;
				nRC = m_pFileSource->GetTrackParam (i, VO_PID_VIDEO_FORMAT, &fmtVideo);
#ifdef __SYMBIAN32__
				snprintf (m_szMetaText, 1024, "%d", fmtVideo.Width);
#else
				sprintf (m_szMetaText, "%d", (int)fmtVideo.Width);
#endif // __SYMBIAN32__
			}
		}
	}
		break;

	case VOMP_PID_METADATA_VIDEOHEIGHT:
	{
		strcpy (m_szMetaText, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_pFileSource->GetSourceInfo (&filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			nRC = m_pFileSource->GetTrackInfo (i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_VIDEO_FORMAT fmtVideo;
				nRC = m_pFileSource->GetTrackParam (i, VO_PID_VIDEO_FORMAT, &fmtVideo);
#ifdef __SYMBIAN32__
				snprintf (m_szMetaText, 1024, "%d", fmtVideo.Height);
#else
				sprintf (m_szMetaText, "%d", (int)fmtVideo.Height);
#endif // __SYMBIAN32__
			}
		}
	}
		break;

	case VOMP_PID_METADATA_DRMCRIPPLED:
		if (m_bDrmCrippled)
			strcpy (m_szMetaText, "true");
		else
			strcpy (m_szMetaText, "false");
		break;

	case VOMP_PID_METADATA_MIME:
		if (m_nVideoTrack >= 0)
		{
			if (m_nFF == VO_FILE_FFMOVIE_MP4)
				strcpy (m_szMetaText,  "video/mp4");
			else if (m_nFF == VO_FILE_FFMOVIE_ASF)
				strcpy (m_szMetaText,  "video/x-ms-wmv");
			else if (m_nFF == VO_FILE_FFMOVIE_AVI)
				strcpy (m_szMetaText,  "video/avi");
			else if (m_nFF == VO_FILE_FFMOVIE_MOV)
				strcpy (m_szMetaText,  "video/mov");
			else if (m_nFF == VO_FILE_FFMOVIE_REAL)
//				strcpy (m_szMetaText,  "video/rmvb");
				strcpy (m_szMetaText,  "video/x-pn-realvideo");
			else if (m_nFF == VO_FILE_FFMOVIE_MPG)
				strcpy (m_szMetaText,  "video/mpeg");
			else if (m_nFF == VO_FILE_FFMOVIE_TS)
				strcpy (m_szMetaText,  "video/ts");
			else if (m_nFF == VO_FILE_FFMOVIE_MKV)
				strcpy (m_szMetaText,  "video/mkv");
			else if (m_nFF == VO_FILE_FFAUDIO_FLAC)
				strcpy (m_szMetaText,  "video/flac");
			else if (m_nFF == VO_FILE_FFMOVIE_FLV)
				strcpy (m_szMetaText,  "video/flv");
			else
				strcpy (m_szMetaText,  "video/unknown");
		}
		else if (m_nAudioTrack >= 0)
		{
			if (m_nFF == VO_FILE_FFMOVIE_MP4)
				strcpy (m_szMetaText,  "audio/mp4");
			else if (m_nFF == VO_FILE_FFMOVIE_ASF)
				strcpy (m_szMetaText,  "audio/x-ms-wma");
			else if (m_nFF == VO_FILE_FFMOVIE_AVI)
				strcpy (m_szMetaText,  "audio/avi");
			else if (m_nFF == VO_FILE_FFMOVIE_MOV)
				strcpy (m_szMetaText,  "audio/mov");
			else if (m_nFF == VO_FILE_FFMOVIE_REAL)
//				strcpy (m_szMetaText,  "audio/ra");
				strcpy (m_szMetaText,  "audio/x-pn-realaudio");
			else if (m_nFF == VO_FILE_FFMOVIE_MPG)
				strcpy (m_szMetaText,  "audio/mpeg");
			else if (m_nFF == VO_FILE_FFMOVIE_TS)
				strcpy (m_szMetaText,  "audio/ts");
			else if (m_nFF == VO_FILE_FFMOVIE_MKV)
				strcpy (m_szMetaText,  "audio/mkv");
			else if (m_nFF == VO_FILE_FFAUDIO_FLAC)
				strcpy (m_szMetaText,  "audio/flac");
			else if (m_nFF == VO_FILE_FFMOVIE_FLV)
				strcpy (m_szMetaText,  "audio/flv");
			else if (m_nFF == VO_FILE_FFAUDIO_APE)
				strcpy (m_szMetaText,  "audio/ape");
			else if (m_nFF == VO_FILE_FFAUDIO_ALAC)
				strcpy (m_szMetaText,  "audio/alac");
			else if (m_nFF == VO_FILE_FFAUDIO_QCP)
				strcpy (m_szMetaText,  "audio/qcp");
			else
				strcpy (m_szMetaText,  "audio/unknown");
		}
		break;

	default:
		nID = nIndex | VO_PID_SOURCE_BASE;
		nRC = m_pFileSource->GetSourceParam (nID, &m_msMetaInfo);
		break;
	}

	if (m_msMetaInfo.pBuffer != NULL)
	{
		if ((m_msMetaInfo.dwFlags & 0XFF) == VO_METADATA_TE_ANSI)
		{
			const unsigned char* src = (const unsigned char *)m_msMetaInfo.pBuffer;
			char* dest = m_szMetaText;
			unsigned int uch;
			for (VO_U32 i = 0; i < m_msMetaInfo.dwBufferSize; i++)
			{
				uch = *src++;
				if (uch & 0x80)
				{
					*dest++ = (uch >> 6) | 0xc0;
					*dest++ = (uch & 0x3f) | 0x80;
				}
				else
				{
					*dest++ = uch;
				}
			}
		}
		else
		{
			memcpy (m_szMetaText, m_msMetaInfo.pBuffer, m_msMetaInfo.dwBufferSize);
		}
	}

	return strlen (m_szMetaText);
}
