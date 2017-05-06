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
#define LOG_TAG "CVOMEMetadata"

#include <stdio.h>

#ifdef LINUX
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX

#include "CVOMEMetadata.h"
#include "voOMX_FilePipe.h"

#include "cmnFile.h"
#include "voLog.h"

#include "vojnilog.h"

#include <ctype.h>



#ifdef _WIN32
#pragma warning (disable : 4996)
#endif // _WIN32

// ----------------------------------------------------------------------------

typedef VO_S32 (VO_API * VOSOURCEGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

CVOMEMetadata::CVOMEMetadata ()
	: m_pFilePipe (NULL)
	, m_hFile (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_hAVIModule (NULL)
	, m_hASFModule (NULL)
	, m_hMP4Module (NULL)
	, m_hFLVModule (NULL)
	, m_hRealModule (NULL)
	, m_hAudioModule (NULL)
	, m_hSourceFile (NULL)
	, m_nFileFormat (VO_FILE_FFUNKNOWN)
{
	strcpy (m_szFile, "");
	memset (&m_Source, 0, sizeof (OMX_VO_FILE_SOURCE));
	m_Source.nMode = OMX_VO_FILE_READ_ONLY;

	m_paramOpen.nFlag = VO_SOURCE_OPENPARAM_FLAG_FILEOPERATOR | VO_SOURCE_OPENPARAM_FLAG_OPENLOCALFILE;
	m_paramOpen.pSource = &m_Source;
	m_paramOpen.pSourceOP = m_pFilePipe;
	m_paramOpen.pMemOP = NULL;
	m_paramOpen.pDrmCB = NULL;
	m_paramOpen.pLibOP = NULL;
	m_paramOpen.nReserve = NULL;	
	

	memset (&m_strMetaText, 0, sizeof (MetaDataString));
}

CVOMEMetadata::~CVOMEMetadata ()
{
	CloseFile ();

#ifdef _LINUX
	if (m_hAVIModule != NULL)
		dlclose (m_hAVIModule);
	if (m_hASFModule != NULL)
		dlclose (m_hASFModule);
	if (m_hMP4Module != NULL)
		dlclose (m_hMP4Module);
	if (m_hRealModule != NULL)
		dlclose (m_hRealModule);
	if (m_hFLVModule != NULL)
		dlclose (m_hFLVModule);
	if (m_hAudioModule != NULL)
		dlclose (m_hAudioModule);
#endif // _LINUX

	if (m_hFile != NULL)
		fclose (m_hFile);
}

int CVOMEMetadata::SetFilePipe (void * pFilePipe)
{
	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;
	m_paramOpen.pSourceOP = m_pFilePipe;
	return 0;
}

int CVOMEMetadata::setDataSource(const char *url)
{
	strcpy (m_szFile, url);

	m_Source.nFlag = OMX_VO_FILE_PIPE_NAME;
	m_Source.pSource = (OMX_PTR)m_szFile;

      JNILOGI("before close");
	if (m_hSourceFile != NULL)
		m_funFileRead.Close (m_hSourceFile);

JNILOGI("Before get File Format");
	VO_FILE_FORMAT nFormat = GetFileFormat (true);
	if (nFormat == VO_FILE_FFAUDIO_OGG || nFormat == VO_FILE_FFAUDIO_MIDI)
		return 0;

    JNILOGI("BEFORE LOAD SOURCE");
    
    JNILOGI("Before load source");
	if (!LoadSource (nFormat))
		return NULL;

JNILOGI2("BEFORE open %p", m_funFileRead);
	m_funFileRead.Open (&m_hSourceFile, &m_paramOpen);
	JNILOGI("after open");

	
	return 0;
}

int CVOMEMetadata::setDataSource(int fd, int64_t offset, int64_t length)
{
       JNILOGI("SET DATA SOURCE");
#ifdef _LINUX
	char szLink[1024];
	sprintf(szLink, "/proc/self/fd/%d", fd);
	memset(m_szFile, 0, sizeof(m_szFile));
	readlink(szLink, m_szFile, sizeof(m_szFile));
	
	int nFd = dup(fd);
	m_hFile = fdopen (nFd, "r");
	if(nFd != -1)
		close(nFd);
#endif _LINUX

	m_Source.nFlag = OMX_VO_FILE_PIPE_HANDLE;
	m_Source.pSource = (OMX_PTR)m_hFile;
	m_Source.nOffset = offset;
	m_Source.nLength = length;

	JNILOGI("BEFORE CLOSE");

	if (m_hSourceFile != NULL)
		m_funFileRead.Close (m_hSourceFile);

	VO_FILE_FORMAT nFormat = GetFileFormat (true);

	if (!LoadSource (nFormat))
		return NULL;

       JNILOGI2("BEFORE open %p", m_funFileRead);
	m_funFileRead.Open (&m_hSourceFile, &m_paramOpen);
	 JNILOGI("after open");
	 
	return 0;
}

const char * CVOMEMetadata::GetMetaDataText (int nIndex)
{
	voCOMXAutoLock lock (&mMutex);

	if (m_hSourceFile == NULL)
		return NULL;

	memset (m_szMetaData, 0, sizeof (m_szMetaData));
	memset (&m_strMetaText, 0, sizeof (MetaDataString));

	switch (nIndex)
	{
	case METADATA_KEY_CD_TRACK_NUMBER:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_ALBUM:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_ALBUM, &m_strMetaText);
		break;

	case METADATA_KEY_ARTIST:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_ARTIST, &m_strMetaText);
		break;

	case METADATA_KEY_AUTHOR:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_OWNER, &m_strMetaText);
		break;

	case METADATA_KEY_COMPOSER:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COMPOSER, &m_strMetaText);
		break;

	case METADATA_KEY_DATE:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_YEAR, &m_strMetaText);
		break;

	case METADATA_KEY_GENRE:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_GENRE, &m_strMetaText);
		break;

	case METADATA_KEY_TITLE:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_TITLE, &m_strMetaText);
		break;

	case METADATA_KEY_YEAR:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_DURATION:
	{
		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		sprintf (m_szMetaData, "%d", filInfo.Duration);
	}
		break;

	case METADATA_KEY_NUM_TRACKS:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_TRACK, &m_strMetaText);
		break;

	case METADATA_KEY_IS_DRM_CRIPPLED:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_CONDUCTOR, &m_strMetaText);
		break;

	case METADATA_KEY_CODEC:
	{
		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

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
				else if (trkInfo.Codec == VO_VIDEO_CodingWMV)
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
				sprintf (m_szMetaData, "%d", fmtAudio.SampleRate);
			}
		}
	}
		break;

	case METADATA_KEY_COMMENT:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COMMENT, &m_strMetaText);
		break;

	case METADATA_KEY_COPYRIGHT:
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_METADATA_COPYRIGHT, &m_strMetaText);
		break;

	case METADATA_KEY_BIT_RATE:
	{
		VO_S64 llFileSize = 0;
		m_funFileRead.GetSourceParam (m_hSourceFile, VO_PID_SOURCE_FILESIZE, &llFileSize);

		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);
		if (filInfo.Duration == 0)
		{
			strcpy (m_szMetaData, "0");
		}
		else
		{
			VO_U32 uBitRate = (llFileSize * 8) / (filInfo.Duration / 1000);
			sprintf (m_szMetaData, "%d", uBitRate);
		}
	}
		break;

	case METADATA_KEY_FRAME_RATE:
	{
		strcpy (m_szMetaData, "0");


		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_U32 uFrameTime = 0;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_SOURCE_FRAMETIME, &uFrameTime);
				if (uFrameTime > 0)
					sprintf (m_szMetaData, "%d", 1000 / uFrameTime);
			}
		}
		
	}
		break;

	case METADATA_KEY_VIDEO_FORMAT:
	{
		if (m_nFileFormat == VO_FILE_FFUNKNOWN)
			m_nFileFormat = GetFileFormat (true);

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

	case METADATA_KEY_VIDEO_HEIGHT:
	{
		strcpy (m_szMetaData, "0");

		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_VIDEO_FORMAT fmtVideo;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
				sprintf (m_szMetaData, "%d", fmtVideo.Height);
			}
		}
	}
		break;

	case METADATA_KEY_VIDEO_WIDTH:
	{
		strcpy (m_szMetaData, "0");

		VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
				VO_VIDEO_FORMAT fmtVideo;
				m_funFileRead.GetTrackParam (m_hSourceFile, i, VO_PID_VIDEO_FORMAT, &fmtVideo);
				sprintf (m_szMetaData, "%d", fmtVideo.Width);
			}
		}
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
	VO_PTR hModule = NULL;

	char szDllFile[256];
	char szAPIName[128];

	if (nFormat == VO_FILE_FFMOVIE_AVI)
	{
		strcpy (szDllFile, ("voAVIFR.so"));
		strcpy (szAPIName, ("voGetAVIReadAPI"));
		hModule  = m_hAVIModule;
	}
	else if (nFormat == VO_FILE_FFMOVIE_MP4)
	{
		strcpy (szDllFile, ("voMP4FR.so"));
		strcpy (szAPIName, ("voGetMP4ReadAPI"));
		hModule  = m_hMP4Module;
	}
	else if (nFormat == VO_FILE_FFMOVIE_ASF)
	{
		strcpy (szDllFile, ("voASFFR.so"));
		strcpy (szAPIName, ("voGetASFReadAPI"));
		hModule  = m_hASFModule;
	}
	else if (nFormat == VO_FILE_FFMOVIE_FLV)
	{
		strcpy (szDllFile, ("voFLVFR.so"));
		strcpy (szAPIName, ("voGetFLVReadAPI"));
		hModule  = m_hFLVModule;
	}
	else if (nFormat == VO_FILE_FFMOVIE_REAL)
	{
	      JNILOGI("REAL SOURCE HAS BEEN LOADED");
		strcpy (szDllFile, ("voRealFR.so"));
		strcpy (szAPIName, ("voGetRealReadAPI"));
		hModule  = m_hRealModule;
	}
	else if (nFormat == VO_FILE_FFAUDIO_WMA)
	{
		strcpy (szDllFile, ("voASFFR.so"));
		strcpy (szAPIName, ("voGetASFReadAPI"));
		hModule  = m_hASFModule;
	}
	else
	{
		strcpy (szDllFile, ("voAudioFR.so"));
		strcpy (szAPIName, ("voGetAudioReadAPI"));
		hModule  = m_hAudioModule;
	}

#ifdef _LINUX
	if (hModule == NULL)
	{
		char szLibName[128];
		strcpy (szLibName, szDllFile);
		strcpy (szDllFile, "/data/local/voOMXPlayer/lib/lib");
		strcat (szDllFile, szLibName);
		hModule = dlopen (szDllFile, RTLD_NOW);
		if (hModule == NULL)
		{
			strcpy (szDllFile, "/data/local/voOMXPlayer/lib/");
			strcat (szDllFile, szLibName);
			hModule = dlopen (szLibName, RTLD_NOW);
		}

		if (hModule == NULL)
		{
			strcpy (szDllFile, "/data/data/com.visualon.vome/lib/lib");
			strcat (szDllFile, szLibName);
			hModule = dlopen (szDllFile, RTLD_NOW);
		}
		if (hModule == NULL)
		{
			strcpy (szDllFile, "/data/data/com.visualon.vome/lib/");
			strcat (szDllFile, szLibName);
			hModule = dlopen (szLibName, RTLD_NOW);
		}

		if (hModule == NULL)
		{
			strcpy (szDllFile, "lib/");
			strcat (szDllFile, szLibName);
			hModule = dlopen (szLibName, RTLD_NOW);
		}
		if (hModule == NULL)
			hModule = dlopen (szLibName, RTLD_NOW);

		if (hModule == NULL)
		{
			JNILOGE ("@@@@@@ CVOMEMetadata LoadSource. dlopen error %s \n", dlerror ());
			VOLOGE ("@@@@@@ CVOMEMetadata LoadSource. dlopen error %s \n", dlerror ());
			return VO_FALSE;
		}
		else
			JNILOGI2("FS %s is loaded", szLibName);

		if (nFormat == VO_FILE_FFMOVIE_AVI)
			m_hAVIModule = hModule;
		else if (nFormat == VO_FILE_FFMOVIE_MP4)
			m_hMP4Module = hModule;
		else if (nFormat == VO_FILE_FFMOVIE_ASF)
			m_hASFModule = hModule;
		else if (nFormat == VO_FILE_FFMOVIE_FLV)
			m_hFLVModule = hModule;
		else if (nFormat == VO_FILE_FFMOVIE_REAL)
			m_hRealModule = hModule;
		else
			m_hAudioModule = hModule;
	}

	VOSOURCEGETAPI pAPIEntry = (VOSOURCEGETAPI) dlsym (hModule, szAPIName);
	if (pAPIEntry == NULL)
	{
		VOLOGE ("@@@@@@ CVOMEMetadata LoadSource. dlsym error %s \n", dlerror ());
		return VO_FALSE;
	}

	pAPIEntry (&m_funFileRead, 0);
	if (m_funFileRead.Open == NULL)
	{
		VOLOGE ("@@@@@@ CVOMEMetadata LoadSource. m_funFileRead.Open is null \n");
		return VO_FALSE;
	}
#endif // _LINUX

	return VO_TRUE;
}

VO_FILE_FORMAT CVOMEMetadata::GetFileFormat (bool bExt)
{
	if (m_nFileFormat != VO_FILE_FFUNKNOWN)
		return m_nFileFormat;

	VO_FILE_FORMAT nFormat = VO_FILE_FFUNKNOWN;

	if (m_hFile == NULL && strlen (m_szFile) <= 0)
		return nFormat;

  JNILOGI("Before ext");

	if (bExt)
	{
		 JNILOGI("GetFileFormatByName");
		nFormat = GetFileFormatByName ();
		 JNILOGI("GetFileFormatByData");
		if (nFormat == VO_FILE_FFUNKNOWN)
			nFormat = GetFileFormatByData ();
	}
	else
	{
		 JNILOGI("GetFileFormatByData");
		nFormat = GetFileFormatByData ();
		if (nFormat == VO_FILE_FFUNKNOWN)
			nFormat = GetFileFormatByName ();
	}


	m_nFileFormat = nFormat;
	
	JNILOGI("Finished GetFileFormat");

	return nFormat;
}

VO_FILE_FORMAT CVOMEMetadata::GetFileFormatByName (void)
{
	VO_FILE_FORMAT nFormat = VO_FILE_FFUNKNOWN;

  JNILOGI("GetFileFormatByName Before check len");
	if (strlen (m_szFile) <= 0)
		return nFormat;

JNILOGI("Before analysis ext");
	char szFile[256];
	strcpy (szFile, m_szFile);

	VO_U32 nLen = strlen (szFile);
	char * pExt = szFile + nLen - 1;
	while (*pExt != '.')
		pExt--;
	pExt++;


	char *pChar  = pExt;
	JNILOGI2("analysis ext %s", pChar);

	VO_U32 nChars = strlen (pExt);
	for (VO_U32 i = 0; i < nChars; i++)
	{
		
		JNILOGI2("before char %s", pChar);
	  pChar[i] = toupper(pChar[i]);
	//	if ( (*pChar) <= 'z' && (*pChar) >= 'a')
			;//pChar[0] = (*pChar) - ('a' - 'A');
	   
		//pChar++;
		JNILOGI2("CHAR %s", pChar);
	}
	
	
	JNILOGI("Before check ext");

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
	else if (!strcmp (pExt, ("QCP")))
	{
		nFormat = VO_FILE_FFAUDIO_QCP;
	}
	else if (!strcmp (pExt, ("MP3")) || !strcmp (pExt, ("MP2")) ||
			 !strcmp (pExt, ("MP1")) || !strcmp (pExt, ("MPD")) || !strcmp (pExt, ("MP3-DRM")))
	{
		nFormat = VO_FILE_FFAUDIO_MP3;
	}
	else if (!strcmp (pExt, ("SDP"))||!strcmp (pExt, ("SDP2")))
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

	return nFormat;
}

VO_FILE_FORMAT CVOMEMetadata::GetFileFormatByData (void)
{
	VO_FILE_FORMAT nFormat = VO_FILE_FFUNKNOWN;

	OMX_VO_FILE_OPERATOR *	pFilePipe = m_pFilePipe;
	if (pFilePipe == NULL)
	{
		cmnFileFillPointer ();
		pFilePipe = (OMX_VO_FILE_OPERATOR *)&g_fileOP;
	}

	OMX_PTR hFile = pFilePipe->Open (&m_Source);
	if (hFile == NULL)
		return nFormat;

	VO_S32		nHeadSize = 64 * 1024;
	VO_PBYTE	pHeadData = (VO_PBYTE)malloc (nHeadSize);

	pFilePipe->Seek (hFile, 0, OMX_VO_FILE_BEGIN);
	nHeadSize = pFilePipe->Read (hFile, pHeadData, nHeadSize);
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

	free (pHeadData);

	return nFormat;
}


VO_BOOL CVOMEMetadata::IsMP4 (VO_PBYTE pHeadData, VO_S32 nHeadSize)
{
	if (pHeadData == NULL || nHeadSize < 6)
		return VO_FALSE;

	VO_PBYTE p = pHeadData + 4;
	if (p[0] != 'f') return VO_FALSE;
	if (p[1] != 't') return VO_FALSE;
	if (p[2] != 'y') return VO_FALSE;
	if (p[3] != 'p') return VO_FALSE;
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
	if ( ((p[4] == 'i') ||  (p[4] == 'I')) && ((p[5] == 's') || (p[5] == 'S')) ) //mp4, m4v
		return VO_TRUE;
	if ( ((p[4] == 'm') || (p[4] == 'M')) && (p[5] == '4') ) //mp4, mp4
		return VO_TRUE;
	if ( (p[4] == 's') && ((p[5] == 'k') || (p[5] == 'm')) ) //3gp, 3g2
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



const char * CVOMEMetadata::GetVideoFormat ()
{
	  strcpy(m_szMetaData, "none");
	  		
   	VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		

		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
		 
		  if ((trkInfo.Type == VO_SOURCE_TT_VIDEO || trkInfo.Type == VO_SOURCE_TT_RTSP_VIDEO))
			{
      
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

				strcpy (m_szMetaData, szVideo);			
			}
		}
	
	CloseFile();
		return m_szMetaData;
}

	
const char * CVOMEMetadata::GetAudioFormat ()
{

	  strcpy(m_szMetaData, "none");
	  
   	VO_SOURCE_INFO	filInfo;
		m_funFileRead.GetSourceInfo (m_hSourceFile, &filInfo);

		VO_SOURCE_TRACKINFO	trkInfo;
		for (VO_U32 i = 0; i < filInfo.Tracks; i++)
		{
			m_funFileRead.GetTrackInfo (m_hSourceFile, i, &trkInfo);
			if ((trkInfo.Type == VO_SOURCE_TT_AUDIO || trkInfo.Type == VO_SOURCE_TT_RTSP_AUDIO))
			{			

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

				strcpy (m_szMetaData, szAudio);
		
			}
			
		}
			CloseFile();
		return m_szMetaData;
}

	