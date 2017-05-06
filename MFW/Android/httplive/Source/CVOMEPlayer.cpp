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

#ifdef _LINUX
#include <dlfcn.h>
#endif // _LINUX

#define LOG_TAG "CVOMEPlayer"

#include "CVOMEPlayer.h"
#include "voOMXOSFun.h"
#include "voSource.h"

#include "OMX_DolbyExt.h"

#include "voLog.h"

typedef OMX_ERRORTYPE (* VOOMXGetEngineAPI) (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag);
typedef OMX_ERRORTYPE (* VOOMXGetComponentAPI) (VOOMX_COMPONENTAPI * pComponent, OMX_U32 uFlag);

int CVOMEPlayer::g_voAndroidStreamSource = 0;

CVOMEPlayer::CVOMEPlayer(void)
	: m_nStatus (-1)
	, m_nLoop (0)
	, m_bUninit (false)
	, m_pDM3 (NULL)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
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
	, m_bSourceLoading (false)
	, m_bAudioPaused4Seek(false)
	, m_pFilePipe (NULL)
	, m_bNotifyVideoSize (false)
	, m_bSetVideoBuffer (false)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_hLoadThread (NULL)
	, m_nLoadThreadID (0)
	, m_pDrmEngine (NULL)
{
	strcpy (m_szFile, "");
	memset (&m_VideoBuff, 0, sizeof (VOA_VIDEO_BUFFERTYPE));
	memset (&m_dspArea, 0, sizeof (OMX_VO_DISPLAYAREATYPE));
	memset (&m_fmtAudio, 0, sizeof (VOA_AUDIO_FORMAT));
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
	LoadModule ();
	if (m_hDll == NULL)
	{
		VOLOGE ("LoadModule Fail");
		return -1;
	}

	m_fAPI.Init (&m_hPlay, 0, vomeCallBack, this);
	if (m_hPlay == NULL)
	{
		VOLOGE ("Init Player Fail");
		return -1;
	}

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
	VOLOGI ("Source: %s", pSource);

	if (m_hLoadThread != NULL)
		return -1;
	if (pSource == NULL)
		return -1;
	if (strlen (pSource) == 0)
		return -1;

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
	m_bPrepareAsync = false;
	m_bUninit = false;

	if (m_nSourceType > 0)
	{
		g_voAndroidStreamSource++;
		m_bStreamSourceCount = true;

		VOLOGI ("Count: %d", g_voAndroidStreamSource);

		if (g_voAndroidStreamSource > 1)
			return -1;
	}

	VOLOGE ("Source Type %d, Call Back %d", m_nSourceType, m_pCallBack);

	if (m_pCallBack != NULL)
		voOMXThreadCreate (&m_hLoadThread, &m_nLoadThreadID, (voOMX_ThreadProc)vomeLoadSourceProc, this, 0);

	return 0;
}

int CVOMEPlayer::SetSource (int fd, int64_t offset, int64_t length)
{
//	VOLOGI ("File ID %d, %d, %d", fd, (int)offset, (int)length);

	if (m_hLoadThread != NULL)
		return -1;

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
	m_bPrepareAsync = false;
	m_bUninit = false;

	if (m_pCallBack != NULL)
		voOMXThreadCreate (&m_hLoadThread, &m_nLoadThreadID, (voOMX_ThreadProc)vomeLoadSourceProc, this, 0);
	
	return 0;
}

OMX_U32 CVOMEPlayer::vomeLoadSourceProc (OMX_PTR pParam)
{
	CVOMEPlayer * pPlayer = (CVOMEPlayer *)pParam;

	return pPlayer->vomeLoadSource ();
}

OMX_U32 CVOMEPlayer::vomeLoadSource (void)
{
	VOLOGE (" 000 ");

	int nRC = BuildGraph ();

	voCOMXAutoLock lock (&m_mutLoad);
	m_hLoadThread = NULL;

	if (m_bPrepareAsync)
	{
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
	}

	return 0;
}

OMX_U32 CVOMEPlayer::vomeWaitLoadSource (int nWaitTime)
{
	if (nWaitTime <= 0)
		nWaitTime = 10000;

	int nStartTime = voOMXOS_GetSysTime ();
	while (m_hLoadThread != NULL)
	{
		voOMXOS_Sleep (2);

		if (voOMXOS_GetSysTime () - nStartTime > nWaitTime)
			break;
	}

	return 0;
}

int CVOMEPlayer::SetLoop (int nLoop)
{
	if (m_pDM3 != NULL)
	{
		int nEnable = 1;
		if (nLoop > 0)
		{
			m_pDM3->SetConfig (m_pDM3, (OMX_INDEXTYPE)OMX_IndexConfigDAK_DEFAULT, &nEnable);
			VOLOGI ("Enable DM3");
		}
		else
		{
			nEnable = 0;
			m_pDM3->SetConfig (m_pDM3, (OMX_INDEXTYPE)OMX_IndexConfigDAK_DEFAULT, &nEnable);
			VOLOGI ("Disable DM3");
		}
	}
	else
	{
		m_nLoop = nLoop;
	}

	return 0;
}

int CVOMEPlayer::PrepareAsync (void)
{
	vomeWaitLoadSource (3000);

	VOLOGE (" 000 ");

	voCOMXAutoLock lock (&m_mutLoad);
	m_bPrepareAsync = true;

	if (m_hLoadThread == NULL)
	{
		if (m_nSourceType == 0)
		{
			NotifyOpenSource ();
		}
		else
		{
			if (m_nStatus >= 0)
			{
				Run ();
				m_bAutoStart = true;
			}
			else
			{
				NotifyOpenSource ();
			}
		}
	}
	
	return 0;
}

void CVOMEPlayer::NotifyOpenSource (void)
{
	VOA_NOTIFYEVENT event;
	event.msg = MEDIA_PREPARED;
	event.ext1 = 0;
	event.ext2 = 0;

	VOLOGE (" 000  ststus %d ", m_nStatus);

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
		return -1;

	if (m_nFile == -1 &&  m_hFile == NULL && strlen (m_szFile) <= 0)
		return -1;

	m_bSourceLoading = true;

	VOME_SOURCECONTENTTYPE Source;
	memset (&Source, 0, sizeof (VOME_SOURCECONTENTTYPE));
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
	OMX_VO_SOURCEDRM_CALLBACK		drmCallback;
	drmCallback.pUserData = this;
	drmCallback.fCallback = vomeSourceDrmProc;
	Source.pDrmCB = &drmCallback;
	Source.nFlag = 3; // Audio and Video

    OMX_U32 nRC = m_fAPI.Playback (m_hPlay, &Source);
    if (nRC != OMX_ErrorNone)
	{
		VOLOGE ("m_fAPI.Playback error \n");
		m_bSourceLoading = false;
		return -1;
	}

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
			//turn on for fixing eclair(2.1) version can not displaying well 
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		}

		m_dspArea.hView = NULL;
		m_dspArea.nWidth = m_nVideoWidth;
		m_dspArea.nHeight = m_nVideoHeight;
//		m_fAPI.SetDisplayArea (m_hPlay, &m_dspArea);

		m_bNotifyVideoSize = false;
		m_bSetVideoBuffer = false;
	}

	VOME_AUDIOFORMATTYPE audioFormat;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_AudioFormat, &audioFormat);
	if (nRC == OMX_ErrorNone)
	{
		VOLOGI ("Audio: S %d, C %d \n", audioFormat.nSampleRate, audioFormat.nChannels);

		if (m_pCallBack != NULL)
		{
			m_fmtAudio.nSampleRate = audioFormat.nSampleRate;
			m_fmtAudio.nChannels = audioFormat.nChannels;
			m_fmtAudio.nSampleBits = audioFormat.nSampleBits;
			m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
		}
	}

	m_nDuration = 0;
	GetDuration (&m_nDuration);

	if (m_nSourceType > 0)
		m_bSeeking = true;

	m_nStatus = 0;
	m_bSourceLoading = false;

	m_fComp.GetComponentByName (m_hPlay, "OMX.VisualOn.Audio.DoblyEffect.XXX", &m_pDM3);
	if (m_pDM3 != NULL)
	{
		int nEnable = 1;
		m_pDM3->SetConfig (m_pDM3, (OMX_INDEXTYPE)OMX_IndexConfigDAK_DEFAULT, &nEnable);
	}

	return 0;
}

int CVOMEPlayer::Run (void)
{
   	if (m_hPlay == NULL)
       	return -1;

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
		return -1;
}

int CVOMEPlayer::Pause (void)
{
    if (m_hPlay == NULL)
       	return -1;

    m_fAPI.Pause (m_hPlay);

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
	
	m_nStatus = 2;

	return 0;
}

int CVOMEPlayer::Stop (void)
{
    if (m_hPlay == NULL)
       	return -1;

	if (m_nStatus < 1)
		return 0;

    m_fAPI.Stop (m_hPlay);

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStop, NULL, NULL);

	m_nStatus = 3;
	m_nStartPos = 0;

	return 0;
}

bool CVOMEPlayer::IsPlaying (void)
{
    if (m_nStatus == 1)
    	return true;
    else
    	return false;
}

int CVOMEPlayer::SetPos (int nPos)
{
	if (m_nDuration == 0)
		return -1;

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

	if (m_nSourceType > 0)
	{
		m_bSeeking = true;
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
//	VOLOGI ("******   GetPos");

    if (m_hPlay == NULL)
    	return -1;

	if (m_nStartPos > 0)
	{
		if (m_nStatus != 1 && m_nStatus != 2)
		{
			*pPos = m_nStartPos;
			return 0;
		}
	}

    m_fAPI.GetCurPos (m_hPlay, (OMX_S32 *)pPos);

//	VOLOGI ("******   %d", *pPos);

	if (*pPos == 0)
	{

//		VOLOGI ("******  Source %d, Prep %d ", m_nSourceType, m_bPrepareAsync);

		if (m_nSourceType > 0 && m_bPrepareAsync)
		{
		//	if (m_nStatus == 3 || m_nStatus < 1)
				*pPos = 100;
		}
	}

//	VOLOGI ("******  000 %d", *pPos);

	return 0;
}

int CVOMEPlayer::GetDuration (int * pPos)
{
    if (m_hPlay == NULL)
    	return -1;

 	m_fAPI.GetDuration (m_hPlay, (OMX_S32 *) pPos);

	return 0;
}

int CVOMEPlayer::Invoke (const Parcel& request, Parcel *reply)
{
	return -1;
}

#ifdef _LINUX_ANDROID
int CVOMEPlayer::GetMetadata (const android::media::Metadata::Filter& ids, Parcel *records)
{
	using android::media::Metadata;
	
    if (m_hPlay == NULL)
    	return -1;

    if (ids.size() != 0) 
	{
        LOGW("Metadata filtering not implemented, ignoring.");
    }

    Metadata metadata((android::Parcel *)records);
    bool ok = true;

    // Right now, we only communicate info about the liveness of the
    // stream to enable/disable pause and seek in the UI.

	// Now I only make it hard code. it should get its value from our m_pPlayer

	int nSeekable = m_nDuration > 0 ? 1 : 0;

    ok = ok && metadata.appendBool(Metadata::kPauseAvailable, nSeekable == 1 ? true : false);
    ok = ok && metadata.appendBool(Metadata::kSeekBackwardAvailable, nSeekable == 1 ? true : false);
    ok = ok && metadata.appendBool(Metadata::kSeekForwardAvailable, nSeekable == 1 ? true : false);
    return ok ? 0 : -1;
}
#endif // _LINUX_ANDROID

int CVOMEPlayer::Reset (void)
{
	m_nLoop = 0;
	m_nStatus = -1;

	return 0;
}

int CVOMEPlayer::Uninit (void)
{
	if (m_hPlay == NULL)
		return 0;

	m_bUninit = true;

	voCOMXAutoLock lock (&m_mutLoad);
	if (m_nStatus >= 1)
		Stop ();

	if (m_hPlay != NULL)
	{
		VOLOGW ("m_hPlay != NULL");

		m_fAPI.Uninit (m_hPlay);
		m_hPlay = NULL;
	}

	if (m_pDrmEngine != NULL)
	{
		delete m_pDrmEngine;
		m_pDrmEngine = NULL;
	}

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
			Pause ();
			SetPos (0);
			Run ();
		}
		else
		{
			Stop ();

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

	voCOMXAutoLock lock (&m_mutLoad);
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
		if (m_bPrepareAsync && !m_bSourceLoading)
			NotifyOpenSource ();

		if (m_bAutoStart)
			Pause ();;

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

	return 0;
}

int CVOMEPlayer::RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer)
{
	if (m_bPrepareAsync)
	{
		m_mutAVRender.Lock ();
		NotifyOpenSource ();
		m_mutAVRender.Unlock ();
	}

	if(m_bSeeking)
	{
		m_mutAVRender.Lock ();
		m_bSeeking = false;
		VOA_NOTIFYEVENT event;
		memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
		event.msg = MEDIA_SEEK_COMPLETE;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		m_mutAVRender.Unlock ();
	}

	if (m_bAutoStart)
	{
		Pause ();;
		return 0;
	}

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
	else if (pVideoBuffer->Color == OMX_COLOR_FormatYCbYCr || pVideoBuffer->Color == OMX_COLOR_FormatYCrYCb ||
			 pVideoBuffer->Color == OMX_COLOR_FormatCbYCrY ||pVideoBuffer->Color == OMX_COLOR_FormatCrYCbY)
	{
		m_VideoBuff.nColorType = VOA_COLOR_YUV422;
	}
	else
	{
		m_VideoBuff.nColorType = VOA_COLOR_YUV420;
		// return OMX_ErrorNotImplemented;
	}

	if (!m_bNotifyVideoSize || pVideoBuffer->Width != m_nVideoWidth || pVideoBuffer->Height != m_nVideoHeight)
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
		m_bSetVideoBuffer = true;
		return OMX_ErrorNone;
	}

	int nRC = m_pCallBack (m_pUserData, VOAP_IDC_renderVideo, &m_VideoBuff, NULL);

	if (!m_bSetVideoBuffer && nRC >= 0)
	{
		OMX_VO_VIDEOBUFFERTYPE videoBuff;
		memset (&videoBuff, 0, sizeof (OMX_VO_VIDEOBUFFERTYPE));

		m_pCallBack (m_pUserData, VOAP_IDC_getVideoBuffer, &videoBuff.Buffer[0], NULL);
		videoBuff.Stride[0] = m_nVideoWidth * 2;

		// VOLOGI ("VOAP_IDC_getVideoBuffer: %d, %d", videoBuff.Buffer[0], videoBuff.Stride[0]);

		m_fAPI.SetParam (m_hPlay, VOME_PID_VideoDataBuffer, &videoBuff);
	}

	if (nRC >= 0)
	{
		m_bNotifyVideoSize = true;
		m_bSetVideoBuffer = true;

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

	if (pVOME->m_bPrepareAsync)
	{
		pVOME->m_mutAVRender.Lock ();
		pVOME->NotifyOpenSource ();
		pVOME->m_mutAVRender.Unlock ();
	}

	if(pVOME->m_bSeeking)
	{
		pVOME->m_mutAVRender.Lock ();
		pVOME->m_bSeeking = false;
		VOA_NOTIFYEVENT event;
		memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
		event.msg = MEDIA_SEEK_COMPLETE;
		pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		pVOME->m_mutAVRender.Unlock ();
	}

	if (pVOME->m_bAutoStart)
	{
		pVOME->Pause ();;
		return OMX_ErrorNone;
	}

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
		pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_setAudioFormat, &pVOME->m_fmtAudio, NULL);
	}

	pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_RenderAudio, 
									pBuffer->pBuffer + pBuffer->nOffset, &pBuffer->nFilledLen);

	return OMX_ErrorNone;
}

OMX_U32 CVOMEPlayer::vomeSourceDrmProc(OMX_PTR pUserData, OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	//	VOLOGW("@@@@@@  CVOMEPlayer vomeVideoRenderBufferProc 000");
	return ((CVOMEPlayer *)pUserData)->vomeOnSourceDrm (nFlag, pParam, nReserved);
	//return OMX_ErrorNone;
}

OMX_U32 CVOMEPlayer::vomeOnSourceDrm(OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
{
	if(OMX_VO_SOURCEDRM_FLAG_DRMINFO == nFlag)
	{
		OMX_VO_SOURCEDRM_INFO * pDrmInfo = (OMX_VO_SOURCEDRM_INFO *)pParam;
		if(OMX_VO_DRMTYPE_DIVX == pDrmInfo->nType)
		{
			//initialize DivX DRM module
			//pDrmInfo->pDrmInfo is OMX_VO_DIVXDRM_INFO*
			if(m_pDrmEngine)
				delete m_pDrmEngine;

			m_pDrmEngine = new CDrmEngine(VO_DRMTYPE_DIVX);
			if(!m_pDrmEngine)
				return VO_ERR_OUTOF_MEMORY;

			VO_U32 result = m_pDrmEngine->LoadLib(NULL);
			if(1 != result)
			{
				VOLOGE ("Load DRM Engine fail");
				return OMX_VO_ERR_DRM_MODULENOTFOUND;
			}

			result = m_pDrmEngine->Open();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Open fail: 0x%08X", result);
				return result;
			}

			result = m_pDrmEngine->SetDrmInfo(0, pDrmInfo->pDrmInfo);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->SetDrmInfo fail: 0x%08X", result);
				return result;
			}

			VO_DRM_FORMAT format;
			result = m_pDrmEngine->GetDrmFormat(&format);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->GetDrmFormat fail: 0x%08X", result);
				return result;
			}

			VOLOGI ("mode: %d, total: 0x%08X, used: 0x%08X", format.nMode, format.uInfo.iRentalCount.nTotal, format.uInfo.iRentalCount.nUsed);

			result = m_pDrmEngine->Commit();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Commit fail: 0x%08X", result);
				return result;
			}

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMTYPE_WindowsMedia == pDrmInfo->nType)
		{
			//initialize WMDRM module
			//pDrmInfo->pDrmInfo is OMX_VO_WMDRM_INFO*
			if(m_pDrmEngine)
				delete m_pDrmEngine;

			m_pDrmEngine = new CDrmEngine(VO_DRMTYPE_WindowsMedia);
			if(!m_pDrmEngine)
				return VO_ERR_OUTOF_MEMORY;

			VO_U32 result = m_pDrmEngine->LoadLib(NULL);
			if(1 != result)
			{
				VOLOGE ("Load DRM Engine fail");
				return OMX_VO_ERR_DRM_MODULENOTFOUND;
			}

			result = m_pDrmEngine->Open();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Open fail: 0x%08X", result);
				return result;
			}

			result = m_pDrmEngine->SetDrmInfo(0, pDrmInfo->pDrmInfo);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->SetDrmInfo fail: 0x%08X", result);
				return result;
			}

			VO_DRM_FORMAT format;
			result = m_pDrmEngine->GetDrmFormat(&format);
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->GetDrmFormat fail: 0x%08X", result);
				return result;
			}

			result = m_pDrmEngine->Commit();
			if(OMX_VO_ERR_DRM_OK != result)
			{
				VOLOGE ("m_pDrmEngine->Commit fail: 0x%08X", result);
				return result;
			}

			return OMX_VO_ERR_DRM_OK;
		}
		else
			return OMX_VO_ERR_DRM_MODULENOTFOUND;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_DRMDATA == nFlag)
	{
		OMX_VO_SOURCEDRM_DATA * pDrmData = (OMX_VO_SOURCEDRM_DATA *)pParam;
		if(OMX_VO_DRMDATATYPE_MEDIASAMPLE == pDrmData->sDataInfo.nDataType)
		{
			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_VIDEOSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
			if(OMX_VO_ERR_DRM_OK != result)
				return result;

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_AUDIOSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			//DivX DRM module process audio sample
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
			if(OMX_VO_ERR_DRM_OK != result)
				return result;

			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_PACKETDATA == pDrmData->sDataInfo.nDataType)
		{
			if(!m_pDrmEngine)
				return OMX_VO_ERR_DRM_ERRORSTATE;

			VO_U32 result = m_pDrmEngine->DecryptData((VO_DRM_DATAINFO*)&pDrmData->sDataInfo, pDrmData->pData, pDrmData->nSize);
			if(OMX_VO_ERR_DRM_OK != result)
				return result;

			return OMX_VO_ERR_DRM_OK;
		}
		else
			return OMX_VO_ERR_DRM_BADPARAMETER;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_FLUSH == nFlag)
	{
		//reserved
		return OMX_VO_ERR_DRM_OK;
	}
	else if(OMX_VO_SOURCEDRM_FLAG_ISSUPPORTED == nFlag)
	{
		//check if DRM module occur
		return OMX_VO_ERR_DRM_OK;
	}
	else
		return OMX_VO_ERR_DRM_BADPARAMETER;
}

int CVOMEPlayer::LoadModule (void)
{
	vostrcpy(m_szDllFile, _T("voOMXME"));
	vostrcpy(m_szAPIName, _T("voOMXGetEngineAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGI ("LoadLib fail");
		return -1;
	}

	VOOMXGetEngineAPI pAPI = (VOOMXGetEngineAPI) m_pAPIEntry;
	if (pAPI == NULL)
		return -1;
	pAPI (&m_fAPI, 0);

#ifdef _WIN32
#ifdef _WIN32_WCE
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) GetProcAddress (m_hDll, _T("voOMXGetComponentAPI"));
#else
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) GetProcAddress (m_hDll, "voOMXGetComponentAPI");
#endif // _WIN32_WCE
#elif defined _LINUX
	VOOMXGetComponentAPI pComp = (VOOMXGetComponentAPI) dlsym (m_hDll, "voOMXGetComponentAPI");
#endif // _WIN32

	if (pComp == NULL)
		return -1;

	pComp (&m_fComp, 0);

	return 0;
}
