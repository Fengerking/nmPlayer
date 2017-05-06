	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEMetadata.cpp

	Contains:	CVOMEMetadata class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-08-31		JBF			Create file

*******************************************************************************/
//#define LOG_TAG "CVOMEMetadata"

#include <stdio.h>

#ifdef _LINUX
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX

#include "CVOMEMetadata.h"
#include "CVOMEMetadataMediaFormat.h"
#include "voOMX_FilePipe.h"
#include "voCOMXBaseConfig.h"
#include "fVideoHeadDataInfo.h"

#include "cmnFile.h"

#undef LOG_TAG
#define LOG_TAG "CVOMEMetadata"
#include "voLog.h"

#ifdef _WIN32
#pragma warning (disable : 4996)
#endif // _WIN32

// ----------------------------------------------------------------------------

#define ID3V2_HEADER_LEN			10
#define CALCULATE_SIZE_SYNCHSAFE(size_array)	((size_array[0] << 21) | (size_array[1] << 14) | (size_array[2] << 7) | size_array[3])
#define DOUBLE_CHECK_IF_FAIL		2

const VO_U32 s_dwMP3SamplingRates[4][3] = 
{
	{11025, 12000, 8000,  },
	{0,     0,     0,     },
	{22050, 24000, 16000, },
	{44100, 48000, 32000  }
};

const VO_U32 s_dwMP3Bitrates[2][3][15] = 
{
	{
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}
	},
	{
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}
	}
};

const VO_U32 s_dwMP3Coefficients[2][3] = 
{
	{48,144,144},
	{48,144,72}
};


typedef VO_S32 (VO_API * VOSOURCEGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

CVOMEMetadata::CVOMEMetadata ()
	: m_pFilePipe (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_hSourceFile (NULL)
	, m_bDrmCrippled (VO_FALSE)
	, m_pDrmEngine (NULL)
	, m_pDrmformat (NULL)
	, m_nFileFormat (VO_FILE_FFUNKNOWN)
	, m_nDrmMode(VO_DRMMODE_NONE)
{
	strcpy (m_szFile, "");
	memset (&m_Source, 0, sizeof (OMX_VO_FILE_SOURCE));
	m_Source.nMode = OMX_VO_FILE_READ_ONLY;
	
	memset(&m_paramOpen, 0 ,sizeof(m_paramOpen));

	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE | VO_SOURCE_OPENPARAM_FLAG_INFOONLY;
	m_paramOpen.pSource = &m_Source;
	m_paramOpen.pSourceOP = m_pFilePipe;
	m_paramOpen.pMemOP = NULL;
#ifdef _ENABLE_DRM
	memset(&m_drmCB, 0, sizeof(m_drmCB));
	m_drmCB.fCallback  = voSourceDrmCallBack;
	m_drmCB.pUserData  = this;
	m_paramOpen.pDrmCB = (VO_SOURCEDRM_CALLBACK *)&m_drmCB;
#endif
	memset (&m_strMetaText, 0, sizeof (MetaDataString));
}

CVOMEMetadata::~CVOMEMetadata ()
{
	if (m_pDrmformat)
	{
		delete m_pDrmformat; 
		m_pDrmformat = NULL;
	}

	if (m_pDrmEngine)
	{
		delete m_pDrmEngine;
		m_pDrmEngine = NULL;
	}

	CloseFile ();
}

int CVOMEMetadata::SetFilePipe (void * pFilePipe)
{
	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;
	m_paramOpen.pSourceOP = m_pFilePipe;
	return 0;
}

int CVOMEMetadata::SetFileBasedDrm(void* pFileOpt, void* pDrmAPI)
{
	VOLOGI("Set Meta Data DRM APIs");
	m_Source.nReserve = (OMX_U32)pDrmAPI;
	m_paramOpen.pSourceOP = pFileOpt;
	return 0;
}

int CVOMEMetadata::setDataSource(const char *url)
{
	strcpy (m_szFile, url);

	m_Source.nFlag = OMX_VO_FILE_PIPE_NAME;
	m_Source.pSource = (OMX_PTR)m_szFile;

	if (m_hSourceFile != NULL)
		m_funFileRead.Close (m_hSourceFile);

	VO_FILE_FORMAT nFormat = GetFileFormat ();
// 	if (nFormat == VO_FILE_FFAUDIO_OGG || nFormat == VO_FILE_FFAUDIO_MIDI)
// 		return 0;

	if (!LoadSource (nFormat))
	{
		VOLOGE ("LoadSource %d fail", nFormat);
		return 0;
	}

	VO_U32 nRC = m_funFileRead.Open (&m_hSourceFile, &m_paramOpen);
	if(VO_ERR_NONE != nRC)
	{
		VOLOGE ("Source Open fail 0x%08X", (unsigned int)nRC);
	}
	return nRC;
}

int CVOMEMetadata::setDataSource(int nFd, int64_t offset, int64_t length)
{
	m_Source.nFlag = OMX_VO_FILE_PIPE_ID; //OMX_VO_FILE_PIPE_HANDLE;
	m_Source.pSource = (OMX_PTR)nFd;
	m_Source.nOffset = offset;
	m_Source.nLength = length;

	if (m_hSourceFile != NULL)
		m_funFileRead.Close (m_hSourceFile);

	VO_FILE_FORMAT nFormat = GetFileFormat ();

	if (!LoadSource (nFormat))
	{
		VOLOGE ("LoadSource %d fail", nFormat);
		return 0;
	}

	VO_U32 nRC = m_funFileRead.Open (&m_hSourceFile, &m_paramOpen);
	if(VO_ERR_NONE != nRC)
	{
		VOLOGE ("Source Open fail 0x%08X", (unsigned int)nRC);
	}
	return nRC;
}

const char * CVOMEMetadata::GetMetaDataText (int nIndex)
{
	voCOMXAutoLock lock (&mMutex);

	if (METADATA_KEY_IS_DRM_CRIPPLED == nIndex)
	{
		strcpy(m_szMetaData, "false");
		if (VO_DRMMODE_PURCHASE == m_nDrmMode)
			strcpy(m_szMetaData, "purchase");
		else if (VO_DRMMODE_RENTAL_COUNT == m_nDrmMode || VO_DRMMODE_RENTAL_TIME == m_nDrmMode)
			strcpy(m_szMetaData, "rental");
		else if (m_bDrmCrippled == VO_TRUE) 
		{
			strcpy(m_szMetaData, "true");
		}

		return m_szMetaData;
	}

	if (m_hSourceFile == NULL)
		return NULL;

	memset (m_szMetaData, 0, sizeof (m_szMetaData));
	memset (&m_strMetaText, 0, sizeof (MetaDataString));
	VO_U32 nRC = 0;

	switch (nIndex)
	{
	case METADATA_KEY_CD_TRACK_NUMBER:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_ALBUM:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_ALBUM, &m_strMetaText);
		break;

	case METADATA_KEY_ARTIST:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_ARTIST, &m_strMetaText);
		break;

	case METADATA_KEY_AUTHOR:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_OWNER, &m_strMetaText);
		break;

	case METADATA_KEY_COMPOSER:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COMPOSER, &m_strMetaText);
		break;

	case METADATA_KEY_DATE:
        nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_SOURCE_DATE, &m_szMetaData);
		break;

	case METADATA_KEY_GENRE:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_GENRE, &m_strMetaText);
		break;

	case METADATA_KEY_TITLE:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_TITLE, &m_strMetaText);
		break;

	case METADATA_KEY_YEAR:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_YEAR, &m_strMetaText);
		break;

	case METADATA_KEY_DURATION:
	{
		VO_SOURCE_INFO	filInfo;
		nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		sprintf (m_szMetaData, "%d", (int)filInfo.Duration);
	}
		break;

	case METADATA_KEY_NUM_TRACKS:
		{
			 VO_U32 num = -1;
		    nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_SOURCE_NUM_TRACKS, &num);
			if(nRC == VO_ERR_NONE)
		      sprintf (m_szMetaData, "%d",(int) num);
			else{
			 VO_SOURCE_INFO	filInfo;
		     m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		     sprintf (m_szMetaData, "%d", (int)filInfo.Tracks);
			}
		}
		
		break;

	case METADATA_KEY_ALBUMARTIST:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_BAND, &m_strMetaText);
		break;
	case METADATA_KEY_IS_3D:
	{
		 VO_SOURCE_INFO	filInfo;
		 nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		 VO_SOURCE_TRACKINFO	trkInfo;
		 for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if(trkInfo.Type == VO_SOURCE_TT_VIDEO)
			{
                VO_S3D_params sVideoS3d;
		        memset(&sVideoS3d, 0, sizeof(VO_S3D_params));
		        m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_VIDEO_S3D, &sVideoS3d);
				if (sVideoS3d.active) 
				{
					VO_U32 type = 1; //MediaStore.MEDIA_VIDEO_NORMAL
					switch(sVideoS3d.fmt) 
					{
					   case S3D_FORMAT_OVERUNDER:
						   type = 5;
						   break;
					   case S3D_FORMAT_SIDEBYSIDE:
                            switch(sVideoS3d.order)
							 {
                                case 0:
                                     type = 6; // LR
                                        break;
                                 case 1:
                                      type = 7; // RL
                              }
                            break;
                    }
					   sprintf (m_szMetaData, "%d",(int)type);
					   break;

                }
            }
        }
	}
		 break;

	case METADATA_KEY_CODEC:
	{
		VO_SOURCE_INFO	filInfo;
		nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			{
				strcat (m_szMetaData, "Audio ");

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

				strcat (m_szMetaData, szAudio);
				strcat (m_szMetaData, " ");
			}
			else if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				strcat (m_szMetaData, "Video ");

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
				else if (trkInfo.Codec == VO_VIDEO_CodingWMV|| trkInfo.Codec == VO_VIDEO_CodingVC1)
					strcpy (szVideo, "WMV");
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

				strcat (m_szMetaData, szVideo);
				strcat (m_szMetaData, " ");
			}
		}
	}
		break;

	case METADATA_KEY_RATING:
	{
		strcpy (m_szMetaData, "0");

		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			{
				VO_AUDIO_FORMAT fmtAudio;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_AUDIO_FORMAT, &fmtAudio);
				sprintf (m_szMetaData, "%d", (int)fmtAudio.SampleRate);
			}
		}
	}
		break;

	case METADATA_KEY_COMMENT:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COMMENT, &m_strMetaText);
		break;

	case METADATA_KEY_COPYRIGHT:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COPYRIGHT, &m_strMetaText);
		break;

#ifdef _GingerBread
	case METADATA_KEY_BITRATE:
#endif	//_GingerBread
	case METADATA_KEY_BIT_RATE:
	{
		
		VO_S64 llFileSize = 0;
		VO_S32	result = 0;
		result = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_SOURCE_FILESIZE, &llFileSize);

		VO_SOURCE_INFO	filInfo;
		nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		if(nRC == VO_ERR_NONE && result == VO_ERR_NONE ){
			if (filInfo.Duration == 0)
			{
				strcpy (m_szMetaData, "-1");
			}
			else
			{
				VO_S64 llBitRate = -1;
				const VO_S64 llDuration = filInfo.Duration;

				llBitRate = llFileSize * 8 * 1000 / llDuration ;
				sprintf (m_szMetaData, "%d", (int)llBitRate);
			}
		}
		
	}
		break;

	case METADATA_KEY_FRAME_RATE:
	{
		strcpy (m_szMetaData, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_U32 uFrameTime = 0;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_SOURCE_FRAMETIME, &uFrameTime);
				if (uFrameTime > 0)
					sprintf (m_szMetaData, "%d",(int)(100000 / uFrameTime));	//unit is 1/100ms
			}
		}
	}
		break;

	case METADATA_KEY_VIDEO_FORMAT:
	{
		if (m_nFileFormat == VO_FILE_FFUNKNOWN)
			m_nFileFormat = GetFileFormat ();

		if (m_nFileFormat == VO_FILE_FFMOVIE_MP4)
			strcpy (m_szMetaData, "MP4 (3GP)");
		else if (m_nFileFormat == VO_FILE_FFMOVIE_ASF)
			strcpy (m_szMetaData, "WMV (ASF)");
		else if (m_nFileFormat == VO_FILE_FFMOVIE_AVI)
			strcpy (m_szMetaData, "AVI");
		else if (m_nFileFormat == VO_FILE_FFMOVIE_REAL)
			strcpy (m_szMetaData, "REAL");
		else if (m_nFileFormat == VO_FILE_FFMOVIE_MPG)
			strcpy (m_szMetaData, "MPG");
		else
			strcpy (m_szMetaData, "Unknown");
	}
		break;

#ifdef _GingerBread
	case METADATA_KEY_VIDEOHEIGHT:
#endif	//_GingerBread
	case METADATA_KEY_VIDEO_HEIGHT:
	{
		strcpy (m_szMetaData, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_VIDEO_FORMAT fmtVideo;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
				sprintf (m_szMetaData, "%d", (int)fmtVideo.Height);
			}
		}
	}
		break;

#ifdef _GingerBread
	case METADATA_KEY_VIDEOWIDTH:
#endif	//_GingerBread
	case METADATA_KEY_VIDEO_WIDTH:
	{
		strcpy (m_szMetaData, "0");

		VO_SOURCE_INFO	filInfo;
		nRC = m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_VIDEO_FORMAT fmtVideo;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
				sprintf (m_szMetaData, "%d",(int)fmtVideo.Width);
			}
		}
	}
		break;

	case METADATA_KEY_MIMETYPE:
	{
		if (m_nFileFormat == VO_FILE_FFUNKNOWN)
			m_nFileFormat = GetFileFormat ();

		// check file suffix name
		VO_U32 nLen = strlen(m_szFile);
		char* pExt = NULL;
		if (nLen > 0)
		{	
			pExt = m_szFile + nLen - 1;
			while(*pExt != '.')
				pExt--;
			pExt++;
		}

		VO_BOOL bHasVideo = VO_FALSE;
		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				bHasVideo = VO_TRUE;
				break;
			}
		}

		if(VO_FILE_FFMOVIE_MP4 == m_nFileFormat)
		{
			if(bHasVideo)
			{
				if(pExt && (!strcmp(pExt, "3GP") || !strcmp(pExt, "3gp") || !strcmp(pExt, "3GPP") || !strcmp(pExt, "3gpp")))
					strcpy(m_szMetaData, "video/3gpp");
				else if(pExt && (!strcmp(pExt, "3G2") || !strcmp(pExt, "3g2") || !strcmp(pExt, "3GPP2") || !strcmp(pExt, "3gpp2")))
					strcpy(m_szMetaData, "video/3gpp2");
				else
					strcpy(m_szMetaData, "video/mp4");
			}
			else
			{
				if(pExt && (!strcmp(pExt, "3GP") || !strcmp(pExt, "3gp") || !strcmp(pExt, "3GPP") || !strcmp(pExt, "3gpp")))
					strcpy(m_szMetaData, "audio/3gpp");
				else if(pExt && (!strcmp(pExt, "3G2") || !strcmp(pExt, "3g2") || !strcmp(pExt, "3GPP2") || !strcmp(pExt, "3gpp2")))
					strcpy(m_szMetaData, "audio/3gpp2");
				else
					strcpy(m_szMetaData, "audio/mp4");
			}
		}
		else if(VO_FILE_FFMOVIE_ASF == m_nFileFormat)
		{
			if(bHasVideo)
			{
				if(pExt && (!strcmp(pExt, "WMV") || !strcmp(pExt, "wmv")))
					strcpy(m_szMetaData, "video/x-ms-wmv");
				else
					strcpy(m_szMetaData, "video/x-ms-asf");
			}
			else
				strcpy(m_szMetaData, "audio/x-ms-wma");
		}
		else if(VO_FILE_FFMOVIE_AVI == m_nFileFormat)
#ifdef _MIME_X_MSVIDEO
			strcpy(m_szMetaData, "video/x-msvideo");
#else
			strcpy(m_szMetaData, "video/avi");
#endif
		else if(VO_FILE_FFMOVIE_MOV == m_nFileFormat)
			strcpy(m_szMetaData, "video/quicktime");
		else if(VO_FILE_FFMOVIE_REAL == m_nFileFormat)
		{
			if(bHasVideo)
				strcpy(m_szMetaData, "video/x-pn-realvideo");
			else
				strcpy(m_szMetaData, "audio/x-pn-realaudio");
		}
		else if(VO_FILE_FFMOVIE_MPG == m_nFileFormat)
			strcpy(m_szMetaData, "video/mpeg");
		else if(VO_FILE_FFMOVIE_TS == m_nFileFormat)
			strcpy(m_szMetaData, "video/mp2ts");
		else if(VO_FILE_FFAUDIO_AAC == m_nFileFormat)
			strcpy(m_szMetaData, "audio/aac");
		else if(VO_FILE_FFAUDIO_AMR == m_nFileFormat)
			strcpy(m_szMetaData, "audio/amr");
		else if(VO_FILE_FFAUDIO_AWB == m_nFileFormat)
			strcpy(m_szMetaData, "audio/amr-wb");
		else if(VO_FILE_FFAUDIO_MP3 == m_nFileFormat)
			strcpy(m_szMetaData, "audio/mpeg");
		else if(VO_FILE_FFAUDIO_QCP == m_nFileFormat)
			strcpy(m_szMetaData, "audio/vnd.qcelp");
		else if(VO_FILE_FFAUDIO_WAV == m_nFileFormat)
			strcpy(m_szMetaData, "audio/x-wav");
		else if(VO_FILE_FFAUDIO_WMA == m_nFileFormat)
			strcpy(m_szMetaData, "audio/x-ms-wma");
		else if(VO_FILE_FFAUDIO_MIDI == m_nFileFormat)
			strcpy(m_szMetaData, "audio/midi");
		else if(VO_FILE_FFAUDIO_OGG == m_nFileFormat)
			strcpy(m_szMetaData, "application/ogg");
		else if(VO_FILE_FFAUDIO_FLAC == m_nFileFormat)
			strcpy(m_szMetaData, "audio/x-flac");
		else if(VO_FILE_FFMOVIE_FLV == m_nFileFormat)
			strcpy(m_szMetaData, "video/x-flv");
		else if(VO_FILE_FFAUDIO_AU == m_nFileFormat)
			strcpy(m_szMetaData, "audio/basic");
		else if(VO_FILE_FFMOVIE_MKV == m_nFileFormat)
		{
			if(bHasVideo)
				strcpy(m_szMetaData, "video/x-matroska");
			else
				strcpy(m_szMetaData, "audio/x-matroska");
		}
		else if(VO_FILE_FFAUDIO_APE == m_nFileFormat)
			strcpy(m_szMetaData, "audio/x-ape");
		else if(VO_FILE_FFAUDIO_ALAC == m_nFileFormat)
			strcpy(m_szMetaData, "audio/alac");
		else if(VO_FILE_FFAUDIO_AC3 == m_nFileFormat)
			strcpy(m_szMetaData, "audio/ac3");
		else
			strcpy(m_szMetaData, "unknown");
	}
		break;

	case METADATA_KEY_DISC_NUMBER:
		nRC = m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_DISK, &m_strMetaText);
		break;

#ifdef _GingerBread
	case METADATA_KEY_COMPILATION:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COMPILATION, &m_strMetaText);
		break;

	case METADATA_KEY_HAS_AUDIO:
	{
		VO_BOOL bHasAudio = VO_FALSE;
		VO_SOURCE_INFO filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			{
				bHasAudio = VO_TRUE;
				break;
			}
		}

		if(bHasAudio)
			strcpy(m_szMetaData, "yes");
	}
		break;
		
	case METADATA_KEY_HAS_VIDEO:
	{
		VO_BOOL bHasVideo = VO_FALSE;
		VO_SOURCE_INFO filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				bHasVideo = VO_TRUE;
				break;
			}
		}

		if(bHasVideo)
			strcpy(m_szMetaData, "yes");
	}
		break;

	case METADATA_KEY_TIMED_TEXT_LANGUAGES:
	{
		// format: "lang1:lang2:lang3:......"
	}
		break;

	case METADATA_KEY_IS_DRM:
	{
		if(m_bDrmCrippled)
			strcpy(m_szMetaData, "1");
	}
		break;
	case METADATA_KEY_LOCATION:
	{
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_SOURCE_LOCATION, &m_szMetaData);
	}
		break;
	case METADATA_KEY_LGE_VIDEO_CONTENT:
	{
		VO_U32 iMinorVersion;
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_SOURCE_MINORVERSION, &iMinorVersion);
		if(memcmp((char*)" EGL", (char*)&iMinorVersion, 4) == 0)
		{
			strcpy(m_szMetaData, "1");
		}
	}
		break;
		
#endif	//_GingerBread

	case METADATA_KEY_DRM_IS_RENTAL:
	{
		strcpy(m_szMetaData, "false");
		if (m_pDrmformat) 
		{
			if (m_pDrmformat->nMode == VO_DRMMODE_RENTAL_COUNT)	
				strcpy(m_szMetaData, "true");
		}
	}
		break;

	case METADATA_KEY_DRM_USE_COUNTER:
	{
		strcpy(m_szMetaData, "0");	
		if (m_pDrmformat) 
		{
			if (m_pDrmformat->nMode == VO_DRMMODE_RENTAL_COUNT)	
			{
				sprintf (m_szMetaData, "%d",(int)m_pDrmformat->uInfo.iRentalCount.nUsed);
			}
		}
	
	}
		break;

    case METADATA_KEY_DRM_USE_LIMIT:
	{
		strcpy(m_szMetaData, "0");	
		if (m_pDrmformat) 
		{
			if (m_pDrmformat->nMode == VO_DRMMODE_RENTAL_COUNT)	
			{
				sprintf (m_szMetaData, "%d",(int)m_pDrmformat->uInfo.iRentalCount.nTotal);
			}
		}
	}
		break;

    case METADATA_KEY_IS_SUPPORTED_AUDIO:
	{
		CVOMEMetadataMediaFormat metaDataMF(m_funFileRead, m_hSourceFile);	
		VO_U32 nResult = metaDataMF.CheckIsSupportedAudio();	
		switch (nResult) 
		{
			case METADATA_MF_SUPPORTED:
				strcpy(m_szMetaData, VO_METADATA_MF_SUPPORTED_STR);	
				break;
			case METADATA_MF_NOTSUPPORTED:
				strcpy(m_szMetaData, VO_METADATA_MF_NOTSUPPORTED_STR);
				break;
			default:
				strcpy(m_szMetaData, VO_METADATA_MF_NOTEXIST_STR);
				break;
		}
	}
		break;

	case METADATA_KEY_IS_SUPPORTED_VIDEO:
	{
		CVOMEMetadataMediaFormat metaDataMF(m_funFileRead, m_hSourceFile);	
		VO_U32 nResult = metaDataMF.CheckIsSupportedVideo();
		switch (nResult) 
		{
			case METADATA_MF_SUPPORTED:
				strcpy(m_szMetaData, VO_METADATA_MF_SUPPORTED_STR);	
				break;
			case METADATA_MF_NOTSUPPORTED:
				strcpy(m_szMetaData, VO_METADATA_MF_NOTSUPPORTED_STR);
				break;
			default:
				strcpy(m_szMetaData, VO_METADATA_MF_NOTEXIST_STR);
				break;
		}
	}		
		break;

	case KEY_PARAMETER_LGEDIVX_RENTAL_MSG:
	{
		if (!m_bDrmCrippled)
		{
			VOLOGE("######   Not DRM clip return NULL");
			return NULL;
		}

		if (!m_pDrmformat)
		{
			VOLOGE("####### KEY_PARAMETER_LGEDIVX_RENTAL_MSG return NULL");
			return NULL;
		}

		sprintf (m_szMetaData, "%d", m_pDrmformat->iOutputProtect.m_btDigitalProtection);

		/*
		if ((m_pDrmformat->nMode == VO_DRMMODE_RENTAL_COUNT) || (m_pDrmformat->nMode == VO_DRMMODE_RENTAL_TIME))
		{
			strcpy(m_szMetaData, "1");
		}
		else
		{
			strcpy(m_szMetaData, "0");
		}
		*/
		VOLOGI("####### KEY_PARAMETER_LGEDIVX_RENTAL_MSG return %s", m_szMetaData);
		return m_szMetaData;
	}
		break;

	case KEY_PARAMETER_LGEDIVX_LICENSE_TYPE:
	{
		if (!m_bDrmCrippled)
		{
			VOLOGE("######## Not DRM clip return NULL");
			return NULL;
		}

		if (!m_pDrmformat)
		{
			strcpy(m_szMetaData, "4");
		}
		else if (m_pDrmformat->nMode == VO_DRMMODE_PURCHASE)
		{
			strcpy(m_szMetaData, "1");
		}
		else if (m_pDrmformat->nMode == VO_DRMMODE_RENTAL_COUNT)
		{
			strcpy(m_szMetaData, "2");
		}
		else if (m_pDrmformat->nMode == VO_DRMMODE_RENTAL_TIME)
		{
			strcpy(m_szMetaData, "3");
		}
		VOLOGI("######### KEY_PARAMETER_LGEDIVX_LICENSE_TYPE return %s", m_szMetaData);
		return m_szMetaData;
	}
		break;

	case KEY_PARAMETER_LGEDIVX_RENTAL_COUNT:
	{
		if (!m_bDrmCrippled)
		{
			VOLOGE("####### Not DRM clip return NULL");
			return NULL;
		}

		if (!m_pDrmformat)
		{
			VOLOGE("####### KEY_PARAMETER_LGEDIVX_RENTAL_COUNT return NULL");
			return NULL;
		}

		sprintf (m_szMetaData, "%d", (int)m_pDrmformat->uInfo.iRentalCount.nUsed);
		VOLOGI("######## KEY_PARAMETER_LGEDIVX_RENTAL_COUNT return %s", m_szMetaData);
		return m_szMetaData;
	}
		break;

	case KEY_PARAMETER_LGEDIVX_TOTAL_COUNT:
	{
		if (!m_bDrmCrippled)
		{
			VOLOGI("######### Not DRM clip return NULL");
			return NULL;
		}

		if (!m_pDrmformat)
		{
			VOLOGI("######### KEY_PARAMETER_LGEDIVX_TOTAL_COUNT return NULL");
			return NULL;
		}

		sprintf (m_szMetaData, "%d",(int)m_pDrmformat->uInfo.iRentalCount.nTotal);
		VOLOGI("######## KEY_PARAMETER_LGEDIVX_TOTAL_COUNT return %s", m_szMetaData);
		return m_szMetaData;
	}
		break;

	case KEY_PARAMETER_LGEDIVX_DIVX_DRM_FILE:
	{
		if (m_bDrmCrippled) 
		{
			strcpy(m_szMetaData, "1");
		}
		else
		{
			strcpy(m_szMetaData, "0");
		}
		VOLOGI("######## KEY_PARAMETER_LGEDIVX_DIVX_DRM_FILE return %s", m_szMetaData);
		return m_szMetaData;
	}
		break;

	default:
		break;
	}

	if (m_strMetaText.pBuffer != NULL)
	{
		if ((m_strMetaText.dwFlags & 0XFF) == VO_METADATA_TE_ANSI)
		{
			const unsigned char* src = (const unsigned char *)m_strMetaText.pBuffer;
			char* dest = m_szMetaData;
			unsigned int uch;
			for (VO_U32 i = 0; i < m_strMetaText.dwBufferSize; i++)
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
			
			memcpy (m_szMetaData, m_strMetaText.pBuffer, m_strMetaText.dwBufferSize);
		}
	}
	if (strlen (m_szMetaData) > 0)
		return m_szMetaData;
	else
		return NULL;
}

int CVOMEMetadata::GetAlbumArt (unsigned char ** ppBuffer, int * pBuffSize)
{
	voCOMXAutoLock lock (&mMutex);

	if (m_hSourceFile == NULL)
		return -1;
	
	MetaDataImage metaData;
	memset(&metaData, 0, sizeof(metaData));
	m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_FRONTCOVER, &metaData);
	if (metaData.pBuffer != NULL)
	{
		*ppBuffer = metaData.pBuffer;
		*pBuffSize = metaData.dwBufferSize;
	}
	return 0;
}

void CVOMEMetadata::CloseFile (void)
{
	voCOMXAutoLock lock (&mMutex);

	if (m_hSourceFile != NULL)
		m_funFileRead.Close (m_hSourceFile);

	m_hSourceFile = NULL;
	m_nFileFormat = VO_FILE_FFUNKNOWN;
}

VO_BOOL CVOMEMetadata::LoadSource (VO_FILE_FORMAT nFormat)
{
	VO_CHAR szCfgItem[128];
	if (nFormat == VO_FILE_FFMOVIE_AVI)
	{
		strcpy (szCfgItem, "Source_File_AVI");
		vostrcpy (m_szDllFile, _T("voAVIFR"));
		vostrcpy (m_szAPIName, _T("voGetAVIReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_MP4 || nFormat == VO_FILE_FFMOVIE_MOV)
	{
		strcpy (szCfgItem, "Source_File_MP4");
		vostrcpy (m_szDllFile, _T("voMP4FR"));
		vostrcpy (m_szAPIName, _T("voGetMP4ReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_ASF)
	{
		strcpy (szCfgItem, "Source_File_ASF");
		vostrcpy (m_szDllFile, _T("voASFFR"));
		vostrcpy (m_szAPIName, _T("voGetASFReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_FLV)
	{
		strcpy (szCfgItem, "Source_File_FLV");
		vostrcpy (m_szDllFile, _T("libvoFLVFR"));
		vostrcpy (m_szAPIName, _T("voGetFLVReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_REAL)
	{
		strcpy (szCfgItem, "Source_File_REAL");
		vostrcpy (m_szDllFile, _T("voRealFR"));
		vostrcpy (m_szAPIName, _T("voGetRealReadAPI"));
	}
	else if (nFormat == VO_FILE_FFAUDIO_WMA)
	{
		strcpy (szCfgItem, "Source_File_ASF");
		vostrcpy (m_szDllFile, _T("voASFFR"));
		vostrcpy (m_szAPIName, _T("voGetASFReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_MKV)
	{
		strcpy (szCfgItem, "Source_File_MKV");
		vostrcpy (m_szDllFile, _T("voMKVFR"));
		vostrcpy (m_szAPIName, _T("voGetMKVReadAPI"));
	}
	else if (nFormat == VO_FILE_FFAUDIO_OGG)
	{
		strcpy (szCfgItem, "Source_File_OGG");
		vostrcpy (m_szDllFile, _T("voOGGFR"));
		vostrcpy (m_szAPIName, _T("voGetOGGReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_TS)
	{
		strcpy (szCfgItem, "Source_File_TS");
		vostrcpy (m_szDllFile, _T("voTsParser"));
		vostrcpy (m_szAPIName, _T("voGetMTVReadAPI"));
	}
	else if (nFormat == VO_FILE_FFMOVIE_MPG)
	{
		strcpy (szCfgItem, "Source_File_MPG");
		vostrcpy (m_szDllFile, _T("voMPGFR"));
		vostrcpy (m_szAPIName, _T("voGetMPGReadAPI"));
	}
	else
	{
		strcpy (szCfgItem, "Source_File_AUDIO");
		vostrcpy (m_szDllFile, _T("voAudioFR"));
		vostrcpy (m_szAPIName, _T("voGetAudioReadAPI"));
	}

	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;
	voCOMXBaseConfig cfg;
	if(OMX_TRUE == cfg.Open((OMX_STRING)_T("vommcodec.cfg")))
	{
		pDllFile = cfg.GetItemText (szCfgItem, (char*)"File");
		pApiName = cfg.GetItemText (szCfgItem, (char*)"Api");
	}

#if defined _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat(m_szDllFile, _T(".Dll"));
	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));
	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
		return VO_FALSE;

	VOSOURCEGETAPI pAPIEntry = (VOSOURCEGETAPI) m_pAPIEntry;
	if (pAPIEntry == NULL)
		return VO_FALSE;

	pAPIEntry (&m_funFileRead, 0);
	if (m_funFileRead.Open == NULL)
	{
		VOLOGE ("@@@@@@ CVOMEMetadata LoadSource. m_funFileRead.Open is null \n");
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_FILE_FORMAT CVOMEMetadata::GetFileFormat ()
{
	if (m_nFileFormat != VO_FILE_FFUNKNOWN)
		return m_nFileFormat;

	m_nFileFormat = GetFileFormatByData ();
	if (m_nFileFormat != VO_FILE_FFUNKNOWN)
		return m_nFileFormat;

	m_nFileFormat = GetFileFormatByName ();

	return m_nFileFormat;
}

VO_FILE_FORMAT CVOMEMetadata::GetFileFormatByName (void)
{
	VO_FILE_FORMAT nFormat = VO_FILE_FFUNKNOWN;

	if (strlen (m_szFile) <= 0)
		return nFormat;

	char szFile[256];
	strcpy (szFile, m_szFile);

	VO_U32 nLen = strlen (szFile);
	char * pExt = szFile + nLen - 1;
	while (*pExt != '.')
		pExt--;
	pExt++;

	char * pChar = pExt;
	VO_U32 nChars = strlen (pExt);
	for (VO_U32 i = 0; i < nChars; i++)
	{
		if (*pChar <= 'z' && *pChar >= 'a')
			*pChar = *pChar - ('a' - 'A');
		pChar++;
	}

	if (!strcmp (pExt, ("ASF")) || !strcmp (pExt, ("ASFD")) ||
		!strcmp (pExt, ("WMV")) || !strcmp (pExt, ("WMVD")) ||
		!strcmp (pExt, ("ASX")) || !strcmp (pExt, ("WAX")) ||
		!strcmp (pExt, ("WVX")) || !strcmp (pExt, ("WMX")))
	{
		nFormat = VO_FILE_FFMOVIE_ASF;
	}
	else if (!strcmp (pExt, ("AVI")) || !strcmp (pExt, ("DIVX")))
	{
		nFormat = VO_FILE_FFMOVIE_AVI;
	}
	else if (!strcmp (pExt, ("MOV")))
	{
		nFormat = VO_FILE_FFMOVIE_MOV;
	}
	else if (!strcmp (pExt, ("RM")) || !strcmp (pExt, ("RA"))  || !strcmp (pExt, ("RMVB")))
	{
		nFormat = VO_FILE_FFMOVIE_REAL;
	}
	else if (!strcmp (pExt, ("MPG")) || !strcmp (pExt, ("MPEG")) ||
		!strcmp (pExt, ("DAT")) || !strcmp (pExt, ("MPV")) || !strcmp (pExt, ("MPE")))
	{
		nFormat = VO_FILE_FFMOVIE_MPG;
	}
	else if (!strcmp (pExt, ("TS")))
	{
		nFormat = VO_FILE_FFMOVIE_TS;
	}
	else if (!strcmp (pExt, ("MP4")) || !strcmp (pExt, ("MP4")) ||
			 !strcmp (pExt, ("M4A")) || !strcmp (pExt, ("M4V")) ||
			 !strcmp (pExt, ("M4V")) || !strcmp (pExt, ("3GP")) ||
			 !strcmp (pExt, ("3G2")) || !strcmp (pExt, ("3GPP")))
	{
		nFormat = VO_FILE_FFMOVIE_MP4;
	}
	else if (!strcmp (pExt, ("WAV")) || !strcmp (pExt, ("WAD")))
	{
		nFormat = VO_FILE_FFAUDIO_WAV;
	}
	else if (!strcmp (pExt, ("WMA")) || !strcmp (pExt, ("WMAD")))
	{
		nFormat = VO_FILE_FFAUDIO_WMA;
	}
	else if (!strcmp (pExt, ("MID")) || !strcmp (pExt, ("MIDI")) ||
			 !strcmp (pExt, ("MMF")) || !strcmp (pExt, ("MIDD")) ||
			 !strcmp (pExt, ("XMF")) || !strcmp (pExt, ("MXMF")) ||
			 !strcmp (pExt, ("MA1")) || !strcmp (pExt, ("MA2")) || !strcmp (pExt, ("MA3")) ||
			 !strcmp (pExt, ("IMY")) || !strcmp (pExt, ("SMF")))
	{
		nFormat = VO_FILE_FFAUDIO_MIDI;
	}
	else if (!strcmp (pExt, ("AMR")))
	{
		nFormat = VO_FILE_FFAUDIO_AMR;
	}
	else if (!strcmp (pExt, ("AWB")))
	{
		nFormat = VO_FILE_FFAUDIO_AWB;
	}
	else if (!strcmp (pExt, ("AAC")))
	{
		nFormat = VO_FILE_FFAUDIO_AAC;
	}
	else if (!strcmp (pExt, ("AC3")))
	{
		nFormat = VO_FILE_FFAUDIO_AC3;
	}
	else if (!strcmp (pExt, ("QCP")))
	{
		nFormat = VO_FILE_FFAUDIO_QCP;
	}
	else if (!strcmp (pExt, ("MP3")) || !strcmp (pExt, ("MP2")) ||
			 !strcmp (pExt, ("MP1")) || !strcmp (pExt, ("MPD")) || !strcmp (pExt, ("MP3-DRM")))
	{
		nFormat = VO_FILE_FFAUDIO_MP3;
	}
	else if (!strcmp (pExt, ("SDP")) || !strcmp (pExt, ("SDP2")))
	{
		nFormat = VO_FILE_FFAPPLICATION_SDP;
	}
	else if (!strcmp (pExt, ("OGG")))
	{
		nFormat = VO_FILE_FFAUDIO_OGG;
	}
	else if (!strcmp (pExt, ("FLAC")))
	{
		nFormat = VO_FILE_FFAUDIO_FLAC;
	}
	else if (!strcmp (pExt, ("FLV")))
	{
		nFormat = VO_FILE_FFMOVIE_FLV;
	}
	else if (!strcmp (pExt, ("MKV")) || !strcmp (pExt, ("WEBM")))
	{
		nFormat = VO_FILE_FFMOVIE_MKV;
	}

	return nFormat;
}

VO_FILE_FORMAT CVOMEMetadata::GetFileFormatByData (void)
{
	OMX_VO_FILE_OPERATOR * pFilePipe = m_pFilePipe;
	if(m_Source.nReserve != 0)
		pFilePipe = (OMX_VO_FILE_OPERATOR*)m_paramOpen.pSourceOP;
	else if (pFilePipe == NULL)
	{
		cmnFileFillPointer ();
		pFilePipe = (OMX_VO_FILE_OPERATOR *)&g_fileOP;
	}

	VO_FILE_FORMAT nFormat = VO_FILE_FFUNKNOWN;
	OMX_PTR hFile = pFilePipe->Open (&m_Source);
	if (hFile == NULL)
		return nFormat;

	VO_S32		nHeadSize = 64 * 1024;
	VO_PBYTE	pHeadData = (VO_PBYTE)malloc (nHeadSize);

	pFilePipe->Seek (hFile, 0, OMX_VO_FILE_BEGIN);
	nHeadSize = pFilePipe->Read (hFile, pHeadData, nHeadSize);

	VO_S64 llFileHeaderSize = 0;
	while(SkipID3V2Header(hFile, pHeadData, nHeadSize, llFileHeaderSize)) {}

	pFilePipe->Seek (hFile, 0, OMX_VO_FILE_BEGIN);
	pFilePipe->Close (hFile);

	if (nHeadSize <= 0)
	{
		free (pHeadData);
		return nFormat;
	}

	if (IsMP4 (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_MP4;
	else if (IsMOV (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_MOV;
	else if (IsASF (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_ASF;
	else if (IsAVI (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_AVI;
	else if (IsREAL (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_REAL;
	else if (IsAMR (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_AMR;
	else if (IsAWB (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_AWB;
	else if (IsQCP (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_QCP;
	else if (IsWAV (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_WAV;
	else if (IsMID (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_MIDI;
	else if (IsFLAC (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_FLAC;
	else if (IsOGG (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_OGG;
	else if (IsFLV (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_FLV;
	else if (IsMKV (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_MKV;
	else if(IsMPG(pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_MPG;
	else if (IsMP3 (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_MP3;
	else if (IsAAC (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_AAC;
	else if (IsAC3 (pHeadData, nHeadSize))
		nFormat = VO_FILE_FFAUDIO_AC3;
	else if(IsTS(pHeadData, nHeadSize))
		nFormat = VO_FILE_FFMOVIE_TS;

	free (pHeadData);

	return nFormat;
}


VO_BOOL CVOMEMetadata::IsMP4 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if(pHeadData == NULL || nHeadSize < 8)
		return VO_FALSE;

	VO_U32 dwFileTypeBoxSize = 0;
	VO_PBYTE p = pHeadData + 4;
	if(p[0] == 'f' && p[1] == 't' && p[2] == 'y' && p[3] == 'p')
	{
		if(nHeadSize < 12)
			return VO_FALSE;

		if ( (p[4] == '3') && ((p[5] == 'g') || (p[5] == 'G')) ) //3gp, 3g2
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'p') || (p[5] == 'P')) ) //mp4, mp42
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'o') || (p[5] == 'O')) ) //mp4, MOV
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'm') || (p[5] == 'M')) ) //MMP4
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 's') || (p[5] == 'S')) ) //MSNV
			return VO_TRUE;
		if ( ((p[4] == 'w') || (p[4] == 'W')) && ((p[5] == 'm') || (p[5] == 'M')) ) //MSNV
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && ((p[5] == 'v') || (p[5] == 'V')) ) //mp4, MV4
			return VO_TRUE;
		if ( ((p[4] == 'a') || (p[4] == 'A')) && ((p[5] == 'v') || (p[5] == 'V')) ) //mp4, MV4
			return VO_TRUE;
		if ( ((p[4] == 'i') || (p[4] == 'I')) && ((p[5] == 's') || (p[5] == 'S')) ) //mp4, m4v
			return VO_TRUE;
		if ( ((p[4] == 'm') || (p[4] == 'M')) && (p[5] == '4') ) //mp4, mp4
			return VO_TRUE;
		if ( (p[4] == 's') && ((p[5] == 'k') || (p[5] == 'm')) ) //3gp, 3g2
			return VO_TRUE;
		if ( (p[4] == 'k') && (p[5] == '3') && (p[6] == 'g') ) //k3g1...
			return VO_TRUE;

		// 3GPP2 EZmovie for KDDI 3G cellphones
		if((p[4] == 'k' || p[4] == 'K') && 
			(p[5] == 'd' || p[5] == 'D') && 
			(p[6] == 'd' || p[6] == 'D') && 
			(p[7] == 'i' || p[7] == 'I'))
			return VO_TRUE;

		dwFileTypeBoxSize = (pHeadData[0] << 24) | (pHeadData[1] << 16) | (pHeadData[2] << 8) | pHeadData[3];
	}
	else if((p[0] == 's' && p[1] == 'k' && p[2] == 'i' && p[3] == 'p') || 
		(p[0] == 'f' && p[1] == 'r' && p[2] == 'e' && p[3] == 'e'))	// some mp4 file use 'free' box instead of 'skip' box, East 20111008
		dwFileTypeBoxSize = (pHeadData[0] << 24) | (pHeadData[1] << 16) | (pHeadData[2] << 8) | pHeadData[3];

	//if major brand can not be identified, we add check to 'moov' or 'mdat'
	//East 2010/02/26
	if((VO_U32)nHeadSize < dwFileTypeBoxSize + 8)
		return VO_FALSE;

	p = pHeadData + dwFileTypeBoxSize + 4;
	if((p[0] == 'm' && p[1] == 'o' && p[2] == 'o' && p[3] == 'v') || 
		(p[0] == 'm' && p[1] == 'd' && p[2] == 'a' && p[3] == 't'))
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL CVOMEMetadata::IsMOV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 6)
		return VO_FALSE;

	VO_PBYTE p = pHeadData + 4;
	if (p[0] != 'f') return VO_FALSE;
	if (p[1] != 't') return VO_FALSE;
	if (p[2] != 'y') return VO_FALSE;
	if (p[3] != 'p') return VO_FALSE;
	if ( (p[4] == 'q') && (p[5] == 't') ) //qt
		return VO_TRUE;

	return VO_FALSE;
}

VO_BOOL CVOMEMetadata::IsASF (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 16)
		return VO_FALSE;

	//{75b22630-668e-11cf-a6d9-00aa0062ce6c}
	VO_PBYTE p = pHeadData;
	if(p[0] != 0x30) return VO_FALSE;
	if(p[1] != 0x26) return VO_FALSE;
	if(p[2] != 0xb2) return VO_FALSE;
	if(p[3] != 0x75) return VO_FALSE;
	if(p[4] != 0x8e) return VO_FALSE;
	if(p[5] != 0x66) return VO_FALSE;
	if(p[6] != 0xcf) return VO_FALSE;
	if(p[7] != 0x11) return VO_FALSE;
	if(p[8] != 0xa6) return VO_FALSE;
	if(p[9] != 0xd9) return VO_FALSE;
	if(p[10] != 0x00) return VO_FALSE;
	if(p[11] != 0xaa) return VO_FALSE;
	if(p[12] != 0x00) return VO_FALSE;
	if(p[13] != 0x62) return VO_FALSE;
	if(p[14] != 0xce) return VO_FALSE;
	if(p[15] != 0x6c) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsAVI (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"AVI "
	if(p[0] != 0x41) return VO_FALSE;
	if(p[1] != 0x56) return VO_FALSE;
	if(p[2] != 0x49) return VO_FALSE;
	if(p[3] != 0x20) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsREAL (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//".RMF"
	if(p[0] != 0x2e) return VO_FALSE;
	if(p[1] != 0x52) return VO_FALSE;
	if(p[2] != 0x4d) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsAMR (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 6)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"#!AMR\n"
	if(p[0] != 0x23) return VO_FALSE;
	if(p[1] != 0x21) return VO_FALSE;
	if(p[2] != 0x41) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;
	if(p[4] != 0x52) return VO_FALSE;
	if(p[5] != 0x0a) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsAWB (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 9)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"#!AMR-WB\n"
	if(p[0] != 0x23) return VO_FALSE;
	if(p[1] != 0x21) return VO_FALSE;
	if(p[2] != 0x41) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;
	if(p[4] != 0x52) return VO_FALSE;
	if(p[5] != 0x2d) return VO_FALSE;
	if(p[6] != 0x57) return VO_FALSE;
	if(p[7] != 0x42) return VO_FALSE;
	if(p[8] != 0x0a) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsQCP (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"QLCM"
	if(p[0] != 0x51) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x43) return VO_FALSE;
	if(p[3] != 0x4d) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsWAV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 12)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"RIFF"
	if(p[0] != 0x52) return VO_FALSE;
	if(p[1] != 0x49) return VO_FALSE;
	if(p[2] != 0x46) return VO_FALSE;
	if(p[3] != 0x46) return VO_FALSE;
	p += 8;
	//"WAVE"
	if(p[0] != 0x57) return VO_FALSE;
	if(p[1] != 0x41) return VO_FALSE;
	if(p[2] != 0x56) return VO_FALSE;
	if(p[3] != 0x45) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsMID (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	if (p[0] != 'M') return VO_FALSE;
	if (p[1] != 'T') return VO_FALSE;
	if (p[2] != 'h') return VO_FALSE;
	if (p[3] != 'd') return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsFLAC (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"fLaC"
	if(p[0] != 0x66) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x61) return VO_FALSE;
	if(p[3] != 0x43) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsOGG (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"OggS"
	if(p[0] != 0x4f) return VO_FALSE;
	if(p[1] != 0x67) return VO_FALSE;
	if(p[2] != 0x67) return VO_FALSE;
	if(p[3] != 0x53) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsFLV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 3)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"FLV"
	if(p[0] != 0x46) return VO_FALSE;
	if(p[1] != 0x4c) return VO_FALSE;
	if(p[2] != 0x56) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsMPG (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (!pHeadData || nHeadSize < 15)///<4 bytes's syncword + 8 bytes's MPEG1 Packer header+ next 3 bytes sync word
	{
		return VO_FALSE;
	}
	VO_PBYTE pMPGHead = MPGFindHeadInBuffer(pHeadData,nHeadSize);

	if(!pMPGHead || !MPGParseHeader(pMPGHead,nHeadSize - (pMPGHead - pHeadData)))
		return VO_FALSE;

	return VO_TRUE; 
}

VO_BOOL CVOMEMetadata::IsMKV (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 3)
		return VO_FALSE;

	VO_PBYTE p = pHeadData;
	//"FLV"
	if(p[0] != 0x1A) return VO_FALSE;
	if(p[1] != 0x45) return VO_FALSE;
	if(p[2] != 0xDF) return VO_FALSE;
	if(p[3] != 0xA3) return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::IsMP3 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pMP3Head = NULL;
	VO_BYTE btMP3Head[4];

	VO_U32 dwFrameSize = 0;
	VO_S32 nDoubleCheck = 0;
	while((pMP3Head = MP3FindHeadInBuffer(pBuf, dwBufSize)) != NULL)
	{
		dwBufSize -= (pMP3Head - pBuf);
		pBuf = pMP3Head;

		if(MP3ParseHeader(pMP3Head, &dwFrameSize))
		{
			memcpy(btMP3Head, pMP3Head, sizeof(btMP3Head));
			btMP3Head[2] &= 0x0C;
			btMP3Head[3] &= 0x8C;

			//check next header!!
			if(dwBufSize < dwFrameSize + 4)
				return VO_FALSE;

			pMP3Head = pBuf + dwFrameSize;

			if(!memcmp(btMP3Head, pMP3Head, 2) && btMP3Head[2] == (pMP3Head[2] & 0x0C) && btMP3Head[3] == (pMP3Head[3] & 0x8C))	//this header is what we want to find!!
			{
				nDoubleCheck--;
				if(nDoubleCheck < 0)
					return VO_TRUE;
			}
			else
				nDoubleCheck = DOUBLE_CHECK_IF_FAIL;
		}
		else
			nDoubleCheck = DOUBLE_CHECK_IF_FAIL;

		dwBufSize--;
		pBuf++;
	}

	return VO_FALSE;
}

VO_BOOL CVOMEMetadata::IsAAC (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 4)
		return VO_FALSE;

	//check if ADIF format!!
	if(pHeadData[0] == 0x41 && pHeadData[1] == 0x44 && pHeadData[2] == 0x49 && pHeadData[3] == 0x46)
		return VO_TRUE;

	VO_PBYTE pBuf = pHeadData;
	VO_U32 dwBufSize = nHeadSize;
	VO_PBYTE pAACHead = NULL;
	VO_BYTE btAACHead[4];

	VO_U32 dwFrameSize = 0;
	VO_S32 nDoubleCheck = 0;
	while((pAACHead = AACFindHeadInBuffer(pBuf, dwBufSize)) != NULL )
	{
		dwBufSize -= (pAACHead - pBuf);
		pBuf = pAACHead;
		if(AACParseHeader(pAACHead, &dwFrameSize))
		{
			memcpy(btAACHead, pAACHead, sizeof(btAACHead));
			btAACHead[3] &= 0xF0;

			//check next header!!
			if(dwBufSize < dwFrameSize + 6)
				return VO_FALSE;

			pAACHead = pBuf + dwFrameSize;
			if(!memcmp(btAACHead, pAACHead, 3) && btAACHead[3] == (pAACHead[3] & 0xF0))	//this header is what we want to find!!
			{
				nDoubleCheck--;
				if(nDoubleCheck < 0)
					return VO_TRUE;
			}
			else
				nDoubleCheck = DOUBLE_CHECK_IF_FAIL;
		}
		else
			nDoubleCheck = DOUBLE_CHECK_IF_FAIL;

		dwBufSize--;
		pBuf++;
	}

	return VO_FALSE;
}

VO_BOOL CVOMEMetadata::IsAC3 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	const VO_U16 vo_ac3_frame_size_tab[38][3] = { { 64,   69,   96   },  { 64,   70,   96   },{ 80,   87,   120  }, { 80,   88,   120  },
		{ 96,   104,  144  },   { 96,   105,  144  },   { 112,  121,  168  },  { 112,  122,  168  }, { 128,  139,  192  },  { 128,  140,  192  },
		{ 160,  174,  240  }, { 160,  175,  240  }, { 192,  208,  288  }, { 192,  209,  288  }, { 224,  243,  336  },  { 224,  244,  336  },
		{ 256,  278,  384  },{ 256,  279,  384  },{ 320,  348,  480  },{ 320,  349,  480  },{ 384,  417,  576  },{ 384,  418,  576  },
		{ 448,  487,  672  }, { 448,  488,  672  },{ 512,  557,  768  },{ 512,  558,  768  },{ 640,  696,  960  },{ 640,  697,  960  }, { 768,  835,  1152 },
		{ 768,  836,  1152 }, { 896,  975,  1344 },{ 896,  976,  1344 },{ 1024, 1114, 1536 },{ 1024, 1115, 1536 },{ 1152, 1253, 1728 },  { 1152, 1254, 1728 },
		{ 1280, 1393, 1920 },  { 1280, 1394, 1920 },
	};
	VO_U16 FirstFrmPos = 0;
	VO_U16 syn_word = 0;
	VO_U8 frmsizecode = 0;
	VO_U8 streadid = 0;
	VO_U16 framesize = 0;
	VO_PBYTE p = pHeadData;
	while(VO_TRUE){
		syn_word = (((syn_word&0x0000)|(p[FirstFrmPos]))<<8)|(((syn_word&0xFF00)|p[FirstFrmPos+1]));
		if(0x0B77 == syn_word)
		{
			// VO_U8 strmid_frmsize = 0;
			frmsizecode = p[FirstFrmPos + 4]&0x3F;
			streadid = (p[FirstFrmPos + 4]&0xB0)>>6;
			framesize = vo_ac3_frame_size_tab[frmsizecode][streadid]*2;
			syn_word = (((syn_word&0x0000)|(p[FirstFrmPos + framesize]))<<8)|(((syn_word&0xFF00)|p[FirstFrmPos + framesize + 1]));
			if(0x0B77 == syn_word)// a ac3 format file
			{
				break;
			}
		}
		if((FirstFrmPos + 2) < nHeadSize)
			FirstFrmPos += 2;
		else
			break;
	}	
	if(FirstFrmPos + 2 >= nHeadSize)
		return VO_FALSE;

	return VO_TRUE;
}

VO_BOOL CVOMEMetadata::SkipID3V2Header(OMX_PTR hFile, VO_PBYTE pHeadData, VO_S32& nHeadSize, VO_S64& llFileHeaderSize)
{
	if(nHeadSize < ID3V2_HEADER_LEN)
		return VO_FALSE;

	static const VO_CHAR byteID3[3] = {0x49, 0x44, 0x33};
	if(memcmp(pHeadData, byteID3, 3))	//no ID3v2 information!!
		return VO_FALSE;

	OMX_VO_FILE_OPERATOR *	pFilePipe = m_pFilePipe;
	if(m_Source.nReserve != 0)
		pFilePipe = (OMX_VO_FILE_OPERATOR *)m_paramOpen.pSourceOP;
	else if (pFilePipe == NULL)
	{
		cmnFileFillPointer ();
		pFilePipe = (OMX_VO_FILE_OPERATOR *)&g_fileOP;
	}

	//calculate the size and skip it!!
	VO_U32 dwSize = CALCULATE_SIZE_SYNCHSAFE((pHeadData + 6));
	llFileHeaderSize += (dwSize + ID3V2_HEADER_LEN);
	if(-1 == pFilePipe->Seek(hFile, llFileHeaderSize, OMX_VO_FILE_BEGIN))
	{
		nHeadSize = 0;
		return VO_FALSE;
	}

	VO_S32 nRes = -2;
	while(-2 == nRes)
		nRes = pFilePipe->Read(hFile, pHeadData, 64 * 1024);

	nHeadSize = (nRes > 0) ? nRes : 0;

	return VO_TRUE;
}

VO_PBYTE CVOMEMetadata::MP3FindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 4)
		return NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 3;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xE0 != (pHead[1] & 0xE0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 111XXXXX found!!
		return pHead;
	}

	return NULL;
}

VO_BOOL CVOMEMetadata::MP3ParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize)
{
	//check version
	VO_BYTE ver = (pHeader[1] >> 3) & 0x03;
	if(1 == ver)
		return VO_FALSE;

	//check layer
	VO_BYTE layer = 3 - ((pHeader[1] >> 1) & 0x03);
	if(3 == layer)
		return VO_FALSE;

	//check sampling rate
	VO_BYTE idx = (pHeader[2] >> 2) & 0x03;
	if(0x03 == idx)	//reserved
		return VO_FALSE;

	VO_U32 dwSamplesPerSec = s_dwMP3SamplingRates[ver][idx];

	//check bit rate
	idx = (pHeader[2] >> 4) & 0x0F;
	if(0xF == idx)	//free or bad
		return VO_FALSE;

	VO_BYTE bLSF = (3 != ver) ? 1 : 0;
	VO_U32 dwBitrate = s_dwMP3Bitrates[bLSF][layer][idx] * 1000;	//KBit to Bit
	if(!dwBitrate)
		return VO_FALSE;

	if(pdwFrameSize)
	{
		//padding bit
		VO_BYTE btPaddingBit = (pHeader[2] >> 1) & 0x01;

		//frame size;
		if(0 == layer)
			*pdwFrameSize = (12 * dwBitrate / dwSamplesPerSec + btPaddingBit) * 4;
		else
			*pdwFrameSize = s_dwMP3Coefficients[bLSF][layer] * dwBitrate / dwSamplesPerSec + btPaddingBit;
	}

	return VO_TRUE;
}

VO_PBYTE CVOMEMetadata::AACFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	if(dwLen < 6)
		return NULL;

	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen - 5;
	while(pHead < pTail)
	{
		if(0xFF != pHead[0])
		{
			pHead++;
			continue;
		}

		//0x11111111 found!!
		if(0xF0 != (pHead[1] & 0xF0))
		{
			pHead += 2;
			continue;
		}

		//0x11111111 1111XXXX found!!
		return pHead;
	}

	return NULL;
}

VO_BOOL CVOMEMetadata::AACParseHeader(VO_PBYTE pHeader, VO_U32* pdwFrameSize)
{
	//check ID
	//if(!(pHeader[1] & 0x8))
	//	return VO_FALSE;

	//check Layer
	//if(pHeader[1] & 0x6)
	//	return VO_FALSE;

	//check Profile
	if(0x3 == (pHeader[2] >> 6))
		return VO_FALSE;

	//check Sampling rate frequency index
	if(((pHeader[2] >> 2) & 0xF) > 0xB)
		return VO_FALSE;

	VO_U32 dwFrameSize = ((pHeader[3] & 0x03) << 11) | (pHeader[4] << 3) | (pHeader[5] >> 5);
	if(dwFrameSize < 6)
		return VO_FALSE;

	if(pdwFrameSize)
		*pdwFrameSize = dwFrameSize;

	return VO_TRUE;
}

VO_PBYTE CVOMEMetadata::MPGFindHeadInBuffer(VO_PBYTE pBuffer, VO_U32 dwLen)
{
	VO_PBYTE pHead = pBuffer;
	VO_PBYTE pTail = pBuffer + dwLen;
	VO_U32 LeftData = 0xFFFFFFFF;
	const VO_U32 wAVCSyncWord = 0x000001BA;///<or 0x000001
	while(pHead < pTail)
	{
		LeftData <<= 8;
		LeftData |= *pHead++;
		if (LeftData == wAVCSyncWord)
		{
			return pHead;
		}
	}
	return NULL;
}

VO_BOOL CVOMEMetadata::MPGParseHeader(VO_PBYTE pHeader, VO_U32 dwLen)
{
	if (dwLen < 11)///<8 bytes's MPEG1 Packer header+ next 3 bytes sync word
	{
		return VO_FALSE;
	}
	VO_PBYTE pHead = pHeader;
	VO_BOOL beMPEG2 = ((pHead[0] >> 6)==0x01)?VO_TRUE:VO_FALSE;
	if(!beMPEG2 && (pHead[0] >> 4) != 0x2)///<not mpeg1
		return VO_FALSE;

	if (beMPEG2)
	{
		if (dwLen < 13)///<10 bytes's MPEG1 Packer header+ next 3 bytes's sync word
		{
			return VO_FALSE;
		}
		if(((pHead[0]&0x4) != 0x4) || ((pHead[2]&0x4) != 0x4) 
			||((pHead[4]&0x4) != 0x4) || ((pHead[5]&0x1) != 0x1) 
			|| ((pHead[8] & 0x3) != 0x3))
		{
			return VO_FALSE;
		}
		pHead += 10;

	}
	else
	{
		if (((pHead[0] & 0x1) != 0x1) || ((pHead[2] & 0x1) != 0x1)
			|| ((pHead[4] & 0x1) != 0x1) || ((pHead[5] & 0x80) != 0x80)|| ((pHead[7] & 0x1) != 0x1)
			)
		{
			return VO_FALSE;
		}
		pHead += 8;
	}
	if (pHead[0] != 0x0 || pHead[1] != 0x0 || pHead[2] != 0x1)///<next synec work 0x000001
	{
		return VO_FALSE;
	}
	return VO_TRUE;
}

VO_PBYTE CVOMEMetadata::FindTSPacketHeader(VO_PBYTE pData, VO_S32 nDataSize, VO_S32 nPacketSize)
{
	VO_PBYTE p1 = pData;
	VO_PBYTE p2 = pData + nDataSize - nPacketSize;
	while((p1 < p2) && (p1 - pData <= nPacketSize))
	{
		if((*p1 == 0x47) && (*(p1 + nPacketSize) == 0x47))
			return p1;
		p1++;
	}
	return NULL;
}

VO_BOOL CVOMEMetadata::FindTSPacketHeader2(VO_PBYTE pData, VO_S32 cbData, VO_S32 packetSize)
{
	VO_PBYTE p = FindTSPacketHeader(pData, cbData, packetSize);
	if (p)
	{
		VO_PBYTE p2 = p + packetSize * 2;
		if (p2 < pData + cbData)
		{
			if (*p2 == 0x47)
				return VO_TRUE;
		}
	}
	return VO_FALSE;
}

VO_BOOL CVOMEMetadata::IsTS (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL)
		return VO_FALSE;

	if (nHeadSize >= 189 && FindTSPacketHeader2(pHeadData, nHeadSize, 188))
		return VO_TRUE;

	if (nHeadSize >= 193 && FindTSPacketHeader2(pHeadData, nHeadSize, 192))
		return VO_TRUE;

	if (nHeadSize >= 205 && FindTSPacketHeader2(pHeadData, nHeadSize, 204))
		return VO_TRUE;

	return VO_FALSE;
}

VO_U32 CVOMEMetadata::voSourceDrmCallBack (VO_PTR pUserData, VO_U32 nFlag, VO_PTR pParam, VO_U32 nReserved)
{
	return ((CVOMEMetadata *)pUserData)->vomeOnSourceDrm (nFlag, pParam, nReserved);
}

VO_U32 CVOMEMetadata::vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	if(VO_SOURCEDRM_FLAG_DRMINFO == nFlag)
	{
		m_bDrmCrippled = VO_TRUE;
		VO_SOURCEDRM_INFO * pDrmInfo = (VO_SOURCEDRM_INFO *)pParam;
		if(VO_DRMTYPE_DIVX == pDrmInfo->nType)
		{
			if(m_pDrmEngine)
				delete m_pDrmEngine;

			m_pDrmEngine = new CDrmEngine(VO_DRMTYPE_DIVX);
			if(!m_pDrmEngine)
				return VO_ERR_OUTOF_MEMORY;

			VO_U32 result = m_pDrmEngine->LoadLib(NULL);
			if(1 != result)
			{
				VOLOGE ("Load DRM Engine fail");
				return VO_ERR_DRM_MODULENOTFOUND;
			}

			result = m_pDrmEngine->Open();
			if(VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Open fail: 0x%08X", (unsigned int)result);
				return result;
			}

			result = m_pDrmEngine->SetDrmInfo(0, pDrmInfo->pDrmInfo);
			/*if(VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->SetDrmInfo fail: 0x%08X", result);
				return result;
			}*/

			if (!m_pDrmformat) 
			{
				m_pDrmformat = new VO_DRM_FORMAT();
				if(!m_pDrmformat)
					return VO_ERR_OUTOF_MEMORY;

				memset(m_pDrmformat, 0, sizeof(VO_DRM_FORMAT));
			}

			result = m_pDrmEngine->GetDrmFormat(m_pDrmformat);
			if (VO_ERR_DRM_OK == result)
			{
				m_nDrmMode = m_pDrmformat->nMode;
			}
			else
			{
				VOLOGE ("m_pDrmEngine->GetDrmFormat fail: 0x%08X", (unsigned int)result);
				return result;
			}

			m_pDrmEngine->Close();
	
			VOLOGI ("mode: %d, total: 0x%08X, used: 0x%08X", m_pDrmformat->nMode, (unsigned int)m_pDrmformat->uInfo.iRentalCount.nTotal, 
					(unsigned int)m_pDrmformat->uInfo.iRentalCount.nUsed);
			
			return VO_ERR_DRM_OK;
		}
	}

	return 0;	
}
