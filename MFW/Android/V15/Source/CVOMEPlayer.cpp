	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEPlayer.cpp

	Contains:	CVOMEPlayer class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-11-21		JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#ifdef LINUX
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif // _LINUX

#define LOG_TAG "CVOMEPlayer"

#include "CVOMEPlayer.h"
#include "voOMXOSFun.h"

#include "voLog.h"
#include "voSource.h"

typedef OMX_ERRORTYPE (* VOOMXGetEngineAPI) (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag);

int CVOMEPlayer::g_voAndroidStreamSource = 0;

CVOMEPlayer::CVOMEPlayer(void)
	: m_nStatus (-1)
	, m_nLoop (0)
	, m_bUninit (false)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_hModule (NULL)
	, m_hPlay (NULL)
	, m_nFile (-1)
	, m_hFile (NULL)
	, m_nOffset (0)
	, m_nLength (0)
	, m_nSourceType (0)
	, m_bSeeking (false)
	, m_nStartPos (0)
	, m_nDuration (0)
	, m_bPrepareAsync (false)
	, m_bAutoStart (false)
	, m_bStreamSourceCount (false)
	, m_pFilePipe (NULL)
	, m_bNotifyVideoSize (false)
	, m_nRenderVideoCount (0)
	, m_nRenderAudioCount (0)
	, m_bStopLoadSource (false)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_hLoadThread (NULL)
	, m_nLoadThreadID (0)
{
	strcpy (m_szFile, "");
	memset (&m_VideoBuff, 0, sizeof (VOA_VIDEO_BUFFERTYPE));
	memset (&m_dspArea, 0, sizeof (OMX_VO_DISPLAYAREATYPE));
	memset (&m_fmtAudio, 0, sizeof (VOA_AUDIO_FORMAT));

	m_drmCB.fCallback = OMX_voSourceDrmCallBack;
	m_drmCB.pUserData = this;

	m_libOP.LoadLib = OMX_voLoadLib;
	m_libOP.GetAddress = OMX_voGetAddress;
	m_libOP.FreeLib = OMX_voFreeLib;
	m_libOP.pUserData = this;
}

CVOMEPlayer::~CVOMEPlayer()
{
	VOLOGI ();

	Uninit ();
}

int CVOMEPlayer::SetCallBack (VOACallBack pCallBack, void * pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;
	return 0;
}

int CVOMEPlayer::SetFilePipe (void * pFilePipe)
{
	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;
	return 0;
}

int CVOMEPlayer::Init (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	LoadModule ();
    if (m_hModule == NULL)
	{
    	return -1;
	}

	m_fAPI.Init (&m_hPlay, 0, vomeCallBack, this);
	if (m_hPlay == NULL)
	{
    	return -1;
	}

//	m_fAPI.SetParam (m_hPlay, VOME_PID_WorkingPath, (void*)"/data/local/vosdk");
//	m_fAPI.SetParam (m_hPlay, OMX_VO_IndexSourceDrm, &m_drmCB);
//	m_fAPI.SetParam (m_hPlay, OMX_VO_IndexLibraryOperator, &m_libOP);

	m_dspArea.nWidth = 480;
	m_dspArea.nHeight = 320;
//	m_fAPI.SetDisplayArea (m_hPlay, &m_dspArea);

	OMX_VO_CHECKRENDERBUFFERTYPE	audioRender;
	audioRender.pUserData = this;
	audioRender.pCallBack = vomeAudioRenderBufferProc;
	m_fAPI.SetParam (m_hPlay, VOME_PID_AudioRenderBuffer, &audioRender);

	OMX_VO_CHECKRENDERBUFFERTYPE	videoRender;
	videoRender.pUserData = this;
	videoRender.pCallBack = vomeVideoRenderBufferProc;
	m_fAPI.SetParam (m_hPlay, VOME_PID_VideoRenderBuffer, &videoRender);

	return 0;
}

int CVOMEPlayer::SetSource (const char* pSource)
{
	voCOMXAutoLock lock (&m_mutStatus);

	VOLOGI ("Source: %s", pSource);

    if (m_hModule == NULL)
	{
		if (Init () < 0)
			return -1;
	}

	if (pSource == NULL)
	{
		VOLOGE ();
    	return -1;
	}

	if (strlen (pSource) == 0 || strlen (pSource) >= 1024)
	{
		VOLOGE ();
    	return -1;
	}

    if (!strncmp (pSource, "rtsp://", 7))
    	m_nSourceType = 1;
    else if (!strncmp (pSource, "mms://", 6))
    	m_nSourceType = 1;
    else if (!strncmp (pSource, "http://", 7))
	{
		int nPos = strlen (pSource) - 4;
		if (!strncmp (pSource + nPos, ".sdp", 4))
		   	m_nSourceType = 1;
		else
		   	m_nSourceType = 2;
	}
    else
	{
		m_nSourceType = 0;
	}

	strcpy (m_szFile, pSource);

	m_nOffset = 0;
	m_nLength = 0;

	m_nStatus = -1;
	m_bUninit = false;

	m_nRenderVideoCount = 0;
	m_nRenderAudioCount = 0;

	return 0;
}

int CVOMEPlayer::SetSource (int fd, int64_t offset, int64_t length)
{
	voCOMXAutoLock lock (&m_mutStatus);

	//	VOLOGI ("File ID %d, %d, %d", fd, (int)offset, (int)length);
    
	if (m_hModule == NULL)
	{
		if (Init () < 0)
			return -1;
	}

#ifdef LINUX
	char szLink[1024];
	sprintf(szLink, "/proc/self/fd/%d", fd);
	memset(m_szFile, 0, sizeof(m_szFile));
	readlink(szLink, m_szFile, sizeof(m_szFile));

	m_nFile = dup(fd); 
	if (m_nFile > 0)
	{
		m_hFile = fdopen(m_nFile, "r");
		if (m_hFile == NULL)
		{
			VOLOGE ("fopen error. ID %d,  %s", errno, strerror(errno));
		}
	}
	else
	{
		VOLOGE ("dup error. ID %d,  %s", errno, strerror(errno));
		m_nFile = fd;
	}
#endif // LINUX

	m_nOffset = offset;
	m_nLength = length;
	m_nSourceType = 0;

	m_nStatus = -1;
	m_bUninit = false;
	
	m_nRenderVideoCount = 0;
	m_nRenderAudioCount = 0;

	return 0;
}

OMX_U32 CVOMEPlayer::vomeLoadSourceProc (OMX_PTR pParam)
{
	CVOMEPlayer * pPlayer = (CVOMEPlayer *)pParam;

	return pPlayer->vomeLoadSource ();
}

OMX_U32 CVOMEPlayer::vomeLoadSource (void)
{
	if (m_nSourceType > 0)
	{
		int nTryTimes = 0;
		while (g_voAndroidStreamSource >= 1)
		{
			voOMXOS_Sleep (100);

			if (m_bStopLoadSource)
			{
				VOLOGE ();
 				NotifyOpenSource ();
   				return -1;
			}

			if (nTryTimes % 50 == 0)
			{
				VOLOGI ("Wait for unload the prevous player");
			}

			nTryTimes++;
			if (nTryTimes >= 500)
				break;
		}

		g_voAndroidStreamSource++;
		m_bStreamSourceCount = true;

		VOLOGI ("Count: %d", g_voAndroidStreamSource);

		if (g_voAndroidStreamSource > 1)
		{
			VOLOGE ();
 			NotifyOpenSource ();
   			return -1;
		}
	}

	int nRC = BuildGraph ();

	voCOMXAutoLock lock (&m_mutNotify);
	if (m_nSourceType == 0)
	{
		NotifyOpenSource ();
	}
	else
	{
		if (m_bUninit)
			return 0;

		if (nRC >= 0)
		{
			Run ();
			m_bAutoStart = true;
		}
		else
		{
			NotifyOpenSource ();
		}
	}

	m_hLoadThread = NULL;

	return 0;
}

int CVOMEPlayer::SetLoop (int nLoop)
{
	voCOMXAutoLock lock (&m_mutStatus);

	m_nLoop = nLoop;

	return 0;
}

int CVOMEPlayer::PrepareAsync (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_hLoadThread != NULL)
		return -1;

	m_bPrepareAsync = true;
	m_bStopLoadSource = false;

	voOMXThreadCreate (&m_hLoadThread, &m_nLoadThreadID, (voOMX_ThreadProc)vomeLoadSourceProc, this, 0);
	
	return 0;
}

void CVOMEPlayer::NotifyOpenSource (void)
{
	VOA_NOTIFYEVENT event;
	event.msg = MEDIA_PREPARED;
	event.ext1 = 0;
	event.ext2 = 0;

	if (m_nStatus < 0)
	{
		VOLOGE ("BuildGraph failed!.");

		event.msg = MEDIA_ERROR;
		event.ext1 = MEDIA_ERROR_UNKNOWN;
		event.ext2 = -1;
	}
	else
	{
		event.msg = MEDIA_PREPARED;
		VOLOGI ("BuildGraph successful!.");
	}

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

	m_bPrepareAsync = false;
}

int CVOMEPlayer::BuildGraph (void)
{
	if (m_nStatus >= 0)
		return 0;

	if (m_hPlay == NULL)
	{
		VOLOGE ();
    	return -1;
	}

	if (m_nFile == -1 &&  m_hFile == NULL && strlen (m_szFile) <= 0)
	{
		VOLOGE ();
    	return -1;
	}

	VOME_SOURCECONTENTTYPE Source;
	Source.nSize = sizeof (VOME_SOURCECONTENTTYPE);
	if (m_hFile != NULL)
	{
		Source.pSource = m_hFile;
		Source.nType = VOME_SOURCE_Handle;
	}
	else if (m_nFile > 0)
	{
		Source.pSource = (OMX_PTR)m_nFile;
		Source.nType = VOME_SOURCE_ID;
	}
	else
	{
		Source.pSource = (OMX_PTR)m_szFile;
		Source.nType = VOME_SOURCE_File;
	}

	Source.nOffset = m_nOffset;
	Source.nLength = m_nLength;
	Source.pPipe = m_pFilePipe;
	Source.nFlag = 3; // Audio and Video

    OMX_U32 nRC = m_fAPI.Playback (m_hPlay, &Source);
    if (nRC != OMX_ErrorNone)
	{
		VOLOGE ("m_fAPI.Playback error \n");
		return -1;
	}
	voCOMXAutoLock lock (&m_mutStatus);
	if (m_bUninit)
		return -1;

	VOME_VIDEOSIZETYPE videoSize;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_VideoSize, &videoSize);
	if (nRC == OMX_ErrorNone)
	{
		m_nVideoWidth = videoSize.nWidth;
		m_nVideoHeight = videoSize.nHeight;
		VOLOGI ("Video: W %d, H %d \n", m_nVideoWidth, m_nVideoHeight);

		if (m_pCallBack != NULL)
		{
			VOA_NOTIFYEVENT event;
			event.msg = MEDIA_SET_VIDEO_SIZE;
			event.ext1 = m_nVideoWidth;
			event.ext2 = m_nVideoHeight;
			//m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		m_dspArea.hView = NULL;
		m_dspArea.nWidth = m_nVideoWidth;
		m_dspArea.nHeight = m_nVideoHeight;
//		m_fAPI.SetDisplayArea (m_hPlay, &m_dspArea);

		m_bNotifyVideoSize = false;
	}

	VOME_AUDIOFORMATTYPE audioFormat;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_AudioFormat, &audioFormat);
	if (nRC == OMX_ErrorNone)
	{
		VOLOGI ("Audio: S %d, C %d B: %d \n", audioFormat.nSampleRate, audioFormat.nChannels, audioFormat.nSampleBits);

		if (m_pCallBack != NULL)
		{
			//m_fmtAudio.nSampleRate = audioFormat.nSampleRate;
			//m_fmtAudio.nChannels = audioFormat.nChannels;
			//m_fmtAudio.nSampleBits = audioFormat.nSampleBits;
			// m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
		}
	}

	m_nDuration = 0;
	GetDuration (&m_nDuration);
	m_nStatus = 0;

	return 0;
}

int CVOMEPlayer::Run (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_hPlay == NULL)
	{
		VOLOGE ();
    	return -1;
	}

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);

	if (m_bAutoStart)
		m_bAutoStart = false;

	if (m_nStartPos > 0)
	{
		if (m_nSourceType == 0)
		{
			m_fAPI.SetCurPos (m_hPlay, m_nStartPos);
			m_nStartPos = 0;
		}
	}

    if (m_fAPI.Run (m_hPlay)== OMX_ErrorNone)
    {
		if (m_nStartPos > 0)
		{
			if (m_nSourceType > 0)
			{
				m_fAPI.SetCurPos (m_hPlay, m_nStartPos);
				m_nStartPos = 0;
			}
		}
    	m_nStatus = 1;
    	return 0;
    }

	if (m_nSourceType == 2)
		return 0;
	else
	{
		VOLOGE ();
    	return -1;
	}
}

int CVOMEPlayer::Pause (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_hPlay == NULL)
	{
		VOLOGE ();
    	return -1;
	}

    m_fAPI.Pause (m_hPlay);

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
	
	m_nStatus = 2;

	return 0;
}

int CVOMEPlayer::Stop (bool bUninit)
{
	voCOMXAutoLock lock (&m_mutStatus);
	m_bStopLoadSource = true;
	while (m_hLoadThread != NULL)
		voOMXOS_Sleep (10);

	if (m_hPlay == NULL)
	{
    	return 0;
	}

	if (m_nStatus < 1)
		return 0;

    m_fAPI.Stop (m_hPlay);

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStop, NULL, NULL);

	m_nStatus = 3;
	m_nStartPos = 0;

	if (!bUninit)
		Uninit ();

	return 0;
}

bool CVOMEPlayer::IsPlaying (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	if (m_nStatus == 1)
    	return true;
    else
    	return false;
}

int CVOMEPlayer::SetPos (int nPos)
{
	voCOMXAutoLock lock (&m_mutStatus);

    if (m_hPlay == NULL)
	{
		VOLOGE ();
    	return 0;
	}

	if (m_nDuration == 0)
	{
		VOLOGE ();
    	return -1;
	}

	if (nPos >= m_nDuration - 1500)
		nPos = m_nDuration - 1500;
	if (nPos < 0)
		nPos = 0;

	m_nStartPos = nPos;

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
	event.msg = MEDIA_SEEK_COMPLETE;

	if (m_hPlay == NULL || m_nStatus <= 0)
    {
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

		VOLOGE ();
        return -1;
    }

    if (m_bSeeking)
    {
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
        return 0;
    }

    OMX_ERRORTYPE errType = m_fAPI.SetCurPos (m_hPlay, nPos);
    if (errType != OMX_ErrorNone)
    {
        VOLOGI ("seekTo %d Failed!", nPos);
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
        return 0;
    }

	m_nStartPos = 0;
    if (m_nSourceType > 0)
    {
     	m_bSeeking = true;

		m_nRenderVideoCount = 0;
		m_nRenderAudioCount = 0;

		//if (m_pCallBack != NULL)
		//	m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
    }
    else
    {
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		VOLOGI ("seekTo %d COMPLETE 0.", nPos);
	}

	return 0;
}

int CVOMEPlayer::GetPos (int * pPos)
{
	voCOMXAutoLock lock (&m_mutStatus);
	*pPos = 0;
    if (m_hPlay == NULL)
	{
		VOLOGE ();
    	return 0;
	}

	if (m_nStartPos > 0)
	{
		if (m_nStatus != 1 && m_nStatus != 2)
		{
			*pPos = m_nStartPos;
			return 0;
		}
	}

    m_fAPI.GetCurPos (m_hPlay, (OMX_S32 *)pPos);

	if (*pPos == 0)
	{
		if (m_nSourceType > 0 && m_bPrepareAsync)
		{
				*pPos = 100;
		}
	}

	return 0;
}

int CVOMEPlayer::GetDuration (int * pPos)
{
 	voCOMXAutoLock lock (&m_mutStatus);
	
	*pPos = 0;
	if (m_hPlay == NULL)
	{
		VOLOGE ();
    	return 0;
	}

 	m_fAPI.GetDuration (m_hPlay, (OMX_S32 *) pPos);

	return 0;
}

int CVOMEPlayer::Reset (void)
{
	voCOMXAutoLock lock (&m_mutStatus);

	m_bStopLoadSource = true;
	while (m_hLoadThread != NULL)
		voOMXOS_Sleep (10);

	m_nLoop = 0;
	m_nStatus = -1;

	return 0;
}

int CVOMEPlayer::Uninit (void)
{
	voCOMXAutoLock lock (&m_mutStatus);
	m_bUninit = true;
//	m_bStopLoadSource = true;
//	while (m_hLoadThread != NULL)
//		voOMXOS_Sleep (10);

	if (m_hPlay == NULL)
		return 0;

	if (m_nStatus == 1 || m_nStatus == 2)
		Stop (true);

	int nStart = voOMXOS_GetSysTime ();

	if (m_hPlay != NULL)
	{
		m_fAPI.Uninit (m_hPlay);
		m_hPlay = NULL;
	}

#ifdef LINUX
	if (m_hModule != NULL)
		dlclose(m_hModule);
	m_hModule = NULL;
#endif	//LINUX

	if (m_hFile != NULL)
		fclose (m_hFile);
	m_hFile = NULL;

	if (m_nSourceType > 0)
	{
		if (m_bStreamSourceCount)
		{
			g_voAndroidStreamSource--;
			m_bStreamSourceCount = false;
		}
		VOLOGI ("Stream Source Count: %d", g_voAndroidStreamSource);
	}

	VOLOGI ("VOME stop and release time is %d", voOMXOS_GetSysTime () - nStart);

	m_nStatus = -1;

	return 0;
}

int CVOMEPlayer::CallBackFunc (int nID, void * pParam1, void * pParam2)
{
	if (m_pCallBack == NULL || m_bUninit)
		return 0;

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));

	if (nID == VOME_CID_PLAY_DEBUGTEXT)
	{
		m_pCallBack (m_pUserData, VOAP_IDC_displayText, pParam1, pParam2);

		return 0;
	}
	else if (nID == VOME_CID_PLAY_FINISHED)
	{
		if (m_nLoop > 0)
		{
			SetPos (0);
			Run ();
		}
		else
		{
			Stop (true);

			event.msg = MEDIA_PLAYBACK_COMPLETE;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		return 0;
	}
	else if (nID == VOME_CID_PLAY_ERROR)
	{
        VOLOGI ("VOME Unknpwn Error!");

		event.msg = MEDIA_ERROR;
		event.ext1 = MEDIA_ERROR_UNKNOWN;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

		return 0;
	}

	if (nID == VOME_CID_PLAY_BUFFERSTART)
	{
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
		// VOLOGI ("VOME_CID_PLAY_BUFFERSTART");
	}
	else if (nID == VOME_CID_PLAY_BUFFERING)
	{
		if (m_nSourceType == 1)
		{
			event.msg = MEDIA_BUFFERING_UPDATE;
			event.ext1 = *(int *)pParam1;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}
	}
	else if (nID == VOME_CID_PLAY_BUFFERSTOP)
	{
		// VOLOGI ("VOME_CID_PLAY_BUFFERSTOP. PrepareSync: %d", m_bPrepareAsync);
		voCOMXAutoLock lock (&m_mutNotify);

		if (m_bPrepareAsync && m_hLoadThread == NULL)
		{
			NotifyOpenSource ();
		}

		if (m_bAutoStart)
		{
			Pause ();
			m_bAutoStart = false;
		}

		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);
		if(m_bSeeking)
		{
			m_bSeeking = false;

			event.msg = MEDIA_SEEK_COMPLETE;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

	        VOLOGI ("seekTo  COMPLETE 1");
		}
	}
	else if (nID == VOME_CID_PLAY_DOWNLOADPOS)
	{
		event.msg = MEDIA_BUFFERING_UPDATE;
		event.ext1 = *(int *)pParam1;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_PACKETLOST)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_STREAMING_PACKETLOST;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_CODECPASSABLEERR)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_CODEC_PASSABLE_ERROR;
		event.ext2 = (OMX_U32)pParam1;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_MEDIACONTENTTYPE)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_MEDIA_CONTENT_TYPE;
		event.ext2 = (OMX_U32)pParam1;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_MEDIAFOURCC)
	{
		event.msg = MEDIA_INFO;
		if((OMX_U32)pParam1 == 0)
			event.ext1 = MEDIA_INFO_VO_MEDIA_AUDIO_FOURCC;
		else if((OMX_U32)pParam1 == 1)
			event.ext1 = MEDIA_INFO_VO_MEDIA_VIDEO_FOURCC;
		event.ext2 = (OMX_U32)pParam2;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_UNSUPPORTEDFORMAT)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_FORMAT_UNSUPPORTED;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_UNSUPPORTEDCODEC)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_CODEC_UNSUPPORTED;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_FRAMEDROPPED)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_SOURCE_FRAMEDROPPED;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}
	else if (nID == VOME_CID_PLAY_SERVERGOODBYE)
	{
		event.msg = MEDIA_INFO;
		event.ext1 = MEDIA_INFO_VO_STREAMING_SERVERGOODBYE;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
	}

	return 0;
}

int CVOMEPlayer::RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer)
{
	if (m_nRenderVideoCount == 0)
	{
		voCOMXAutoLock lock (&m_mutNotify);
		if (m_bPrepareAsync && m_hLoadThread == NULL)
		{
			NotifyOpenSource ();
		}

		if(m_bSeeking)
		{
			m_bSeeking = false;
			VOA_NOTIFYEVENT event;
			memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
			event.msg = MEDIA_SEEK_COMPLETE;
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		if (m_bAutoStart)
		{
			Pause ();;
			m_bAutoStart = false;
			return 0;
		}
	}
	m_nRenderVideoCount++;
	

	m_VideoBuff.virBuffer[0] = pVideoBuffer->Buffer[0];
	m_VideoBuff.virBuffer[1] = pVideoBuffer->Buffer[1];
	m_VideoBuff.virBuffer[2] = pVideoBuffer->Buffer[2];

	m_VideoBuff.nStride[0] = pVideoBuffer->Stride[0];
	m_VideoBuff.nStride[1] = pVideoBuffer->Stride[1];
	m_VideoBuff.nStride[2] = pVideoBuffer->Stride[2];

	if (pVideoBuffer->Color == OMX_COLOR_Format_QcomHW)
	{
		m_VideoBuff.nColorType = VOA_COLOR_NV12;
	}
	else if (pVideoBuffer->Color == OMX_COLOR_Format16bitRGB565)
	{
		m_VideoBuff.nColorType = VOA_COLOR_RGB565;
	}
	else
	{
		m_VideoBuff.nColorType = VOA_COLOR_YUV420;
		// return OMX_ErrorNotImplemented;
	}

	if (!m_bNotifyVideoSize)
	{
		// VOLOGI ("Video Color %d W %d, H %d, Disp W %d, H %d\n", pVideoBuffer->Color, pVideoBuffer->Width, pVideoBuffer->Height, m_dspArea.nWidth, m_dspArea.nHeight);

		m_nVideoWidth = pVideoBuffer->Width;
		m_nVideoHeight = pVideoBuffer->Height;

		VOA_NOTIFYEVENT event;
		event.msg = MEDIA_SET_VIDEO_SIZE;
		event.ext1 = m_nVideoWidth;
		event.ext2 = m_nVideoHeight;

		if (m_pCallBack != NULL)
		{
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
			m_pCallBack (m_pUserData, VOAP_IDC_setVideoSize, &m_nVideoWidth, &m_nVideoHeight);
		}
	}

	if (m_pCallBack == NULL)
	{
		m_bNotifyVideoSize = true;
		return OMX_ErrorNone;
	}

	int nRC = m_pCallBack (m_pUserData, VOAP_IDC_renderVideo, &m_VideoBuff, NULL);

	if (nRC >= 0)
	{
		m_bNotifyVideoSize = true;

		return OMX_ErrorNone;
	}
	else
	{
		return OMX_ErrorNotImplemented;
	}
}

OMX_S32 CVOMEPlayer::vomeCallBack (OMX_PTR pUserData, OMX_S32 nID, OMX_PTR pParam1, OMX_PTR pParam2)
{
	return ((CVOMEPlayer*) pUserData)->CallBackFunc (nID, pParam1, pParam2);
}

OMX_ERRORTYPE CVOMEPlayer::vomeVideoRenderBufferProc (OMX_HANDLETYPE hComponent,
										   OMX_PTR pAppData,
										   OMX_BUFFERHEADERTYPE* pBuffer,
										   OMX_IN OMX_INDEXTYPE nType,
										   OMX_IN OMX_PTR pFormat)
{
//	VOLOGW("@@@@@@  CVOMEPlayer vomeVideoRenderBufferProc 000");

	if (nType != (OMX_INDEXTYPE)OMX_VO_IndexVideoBufferType)
		return OMX_ErrorNotImplemented;

	return (OMX_ERRORTYPE) ((CVOMEPlayer *)pAppData)->RenderVideo ((OMX_VO_VIDEOBUFFERTYPE *)pFormat);
	//return OMX_ErrorNone;
}

OMX_ERRORTYPE CVOMEPlayer::vomeAudioRenderBufferProc (OMX_HANDLETYPE hComponent,
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat)
{
//	LOGW("@@@@@@  CVOMEPlayer vomeAudioRenderBufferProc \n");

	CVOMEPlayer *					pVOME = (CVOMEPlayer *)pAppData;
	OMX_AUDIO_PARAM_PCMMODETYPE *	pAudioFormat = (OMX_AUDIO_PARAM_PCMMODETYPE *)pFormat;

	if (pVOME->m_nRenderAudioCount == 0)
	{
		voCOMXAutoLock lock (&pVOME->m_mutNotify);
		if (pVOME->m_bPrepareAsync && pVOME->m_hLoadThread == NULL)
		{
			pVOME->NotifyOpenSource ();
		}

		if(pVOME->m_bSeeking)
		{
			pVOME->m_bSeeking = false;
			VOA_NOTIFYEVENT event;
			memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
			event.msg = MEDIA_SEEK_COMPLETE;
			pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		if (pVOME->m_bAutoStart)
		{
			pVOME->Pause ();
			pVOME->m_bAutoStart = false;
			return OMX_ErrorNone;
		}
	}
	pVOME->m_nRenderAudioCount++;

	if (pVOME->m_pCallBack == NULL)
		return OMX_ErrorNone;

//	VOLOGE (" Audio Format %d, %d, %d", pAudioFormat->nSamplingRate,  pAudioFormat->nChannels,  pAudioFormat->nBitPerSample);
//	VOLOGE (" 11 Audio Format %d, %d, %d", pVOME->m_fmtAudio.nSampleRate,  pVOME->m_fmtAudio.nChannels,  pVOME->m_fmtAudio.nSampleBits);

	if (pAudioFormat->nSamplingRate != pVOME->m_fmtAudio.nSampleRate || pAudioFormat->nChannels != pVOME->m_fmtAudio.nChannels ||
		pAudioFormat->nBitPerSample != pVOME->m_fmtAudio.nSampleBits)
	{
		pVOME->m_fmtAudio.nSampleRate = pAudioFormat->nSamplingRate;
		pVOME->m_fmtAudio.nChannels = pAudioFormat->nChannels;
		pVOME->m_fmtAudio.nSampleBits = pAudioFormat->nBitPerSample;

		VOLOGI ("Audio: S %d, C %d B: %d \n", pAudioFormat->nSamplingRate, pAudioFormat->nChannels, pAudioFormat->nBitPerSample);

		pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_setAudioFormat, &pVOME->m_fmtAudio, NULL);
	}

	pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_RenderAudio, 
									pBuffer->pBuffer + pBuffer->nOffset, &pBuffer->nFilledLen);

	return OMX_ErrorNone;
}


int CVOMEPlayer::LoadModule (void)
{
#ifdef LINUX
	m_hModule = dlopen ("/data/local/voOMXPlayer/lib/libvoOMXME.so", RTLD_NOW);
	if (m_hModule == NULL)
		m_hModule = dlopen ("/data/local/voOMXPlayer/lib/voOMXME.so", RTLD_NOW);
	if (m_hModule == NULL)
		m_hModule = dlopen ("libvoOMXME.so", RTLD_NOW);
	if (m_hModule == NULL)
		m_hModule = dlopen ("voOMXME.so", RTLD_NOW);

	if (m_hModule == NULL)
	{
		LOGE ("@@@@@@ Load Module Error %s \n", dlerror ());
		return -1;
	}

	VOOMXGetEngineAPI pAPI = (VOOMXGetEngineAPI) dlsym(m_hModule, "voOMXGetEngineAPI");
	if (pAPI == NULL)
	{
		LOGE ("@@@@@@ Get function address Error %s \n", dlerror ());
		return -1;
	}

	pAPI (&m_fAPI, 0);
#endif // LINUX
	return 0;
}



OMX_PTR CVOMEPlayer::OMX_voLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag)
{
	OMX_PTR	hModule = NULL;
	char	szDll[256];

#ifdef LINUX
	strcpy (szDll, "/data/local/voOMXPlayer/lib/lib");
	strcat (szDll, pLibName);
	hModule = dlopen (szDll, nFlag);
	if (hModule == NULL)
	{
		strcpy (szDll, "/data/local/voOMXPlayer/lib/");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
	}
	if (hModule == NULL)
	{
		strcpy (szDll, "lib");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
	}
	if (hModule == NULL)
	{
		strcpy (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
	}
#endif // LINUX

//	VOLOGE (" Name: %s, Result: %d ******************************", szDll, (int)hModule);

	return hModule;
}

OMX_PTR CVOMEPlayer::OMX_voGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return NULL;

	OMX_PTR pFunc = NULL;
	
#ifdef LINUX
	pFunc = dlsym(hLib, pFuncName);
#endif // LINUX

	return pFunc;
}

OMX_S32 CVOMEPlayer::OMX_voFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return -1;

#ifdef LINUX
	dlclose (hLib);
#endif // LINUX

//	VOLOGE (" Handle: %d ******************************",  (int)hLib);

	return 0;
}

OMX_U32 CVOMEPlayer::OMX_voSourceDrmCallBack (OMX_PTR pUserData, OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	return 0;
}