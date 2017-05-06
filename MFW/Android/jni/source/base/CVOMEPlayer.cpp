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

#include "vojnilog.h"


#if defined __VOPRJ_BLUEST__
#include <AndroidRuntime.h>
#include "wdrm.h"
using namespace android;
static void*												g_pbluedrm = NULL;
#endif

#define VOINFO(format, ...) do { \
	LOGI("%s::%s()->%d: " format "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);

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
	, m_bSourceLoading (false)
	, m_pFilePipe (NULL)
	, m_bNotifyVideoSize (false)
	, m_bSetVideoBuffer (false)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_hLoadThread (NULL)
	, m_nLoadThreadID (0)
{
	JNILOGI ("construct");
	strcpy (m_szFile, "");
	memset(m_szworkingpath, 0, 128);
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
	// tmp VOLOGI ();
	VOINFO("gonna uninit...");
	Uninit ();
	VOINFO("uninit over...");
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

int CVOMEPlayer::Init(const char* path)
{
	strcpy(m_szworkingpath, path);

	LoadModule ();

	if (m_hModule == NULL)
		return -1;

	m_fAPI.Init (&m_hPlay, 0, vomeCallBack, this);
	if (m_hPlay == NULL)
		return -1;

	m_fAPI.SetParam (m_hPlay, VOME_PID_WorkingPath, (void*)path);
	m_fAPI.SetParam (m_hPlay, OMX_VO_IndexSourceDrm, &m_drmCB);
	m_fAPI.SetParam (m_hPlay, OMX_VO_IndexLibraryOperator, &m_libOP);

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
	JNILOGI2 ("Set Source: %s  ", pSource);

	if (m_hLoadThread != NULL)
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

		JNILOGI2 ("Count: %d", g_voAndroidStreamSource);

		if (g_voAndroidStreamSource > 1)
			return -1;
	}

#if defined __VOPRJ_BLUEST__
	wdrm_t pdrm = get_wdrm();
	if (pdrm) 
		g_pbluedrm = pdrm->alloc(pSource, pdrm->_store);
#endif

	JNILOGI2("setsource thread id: 0x%08x", pthread_self());

	OMX_ERRORTYPE et;
	if (m_pCallBack != NULL)
		et = voOMXThreadCreate (&m_hLoadThread, &m_nLoadThreadID, (voOMX_ThreadProc)vomeLoadSourceProc, this, 0);
	else
		JNILOGI("m_pCallBack is null");
      
       JNILOGI2("InsufficientResources : %d BadParameter: %d ErrorNone : %d" , OMX_ErrorInsufficientResources, OMX_ErrorBadParameter, OMX_ErrorNone);

	JNILOGI2("load thread return : %d id : %d p: 0x%08x", et, m_nLoadThreadID, m_hLoadThread);

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
			JNILOGE2 ("fopen error. ID %d,  %s", errno, strerror(errno));
		}
	}
	else
	{
		JNILOGE2 ("dup error. ID %d,  %s", errno, strerror(errno));
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
 	
 	JNILOGI("vomeLoadSourceProc ");
	return pPlayer->vomeLoadSource ();
}

OMX_U32 CVOMEPlayer::vomeLoadSource (void)
{
      JNILOGI("vomeLoadSource v2");
      
	int nRC = BuildGraph ();

      JNILOGD("after build graph");
      
	voCOMXAutoLock lock (&m_mutLoad);
	m_hLoadThread = NULL;

	if (m_bPrepareAsync)
	{
		if (m_nSourceType == 0)
		{
		      JNILOGD("before vomeLoadSource1");
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
			      JNILOGD("before vomeLoadSource2");
				NotifyOpenSource ();
			}
		}
	}

	JNILOGD("before return vomeLoadSource");

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
		//JNILOGV2("WAIT %d", voOMXOS_GetSysTime () - nStartTime > nWaitTime);
		if (voOMXOS_GetSysTime () - nStartTime > nWaitTime)
		{
//		      JNILOGE("waiting load source time out");
			break;
		}
	}

	return 0;
}

int CVOMEPlayer::SetLoop (int nLoop)
{
	m_nLoop = nLoop;

	return 0;
}

int CVOMEPlayer::PrepareAsync (void)
{
	JNILOGI("before WaitLoadSource");
	vomeWaitLoadSource (3000);
	JNILOGI("after WaitLoadSource");
	
		
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

	if (m_nStatus < 0)
	{
		JNILOGE("BuildGraph failed!.");

		event.msg = MEDIA_ERROR;
		event.ext1 = MEDIA_ERROR_UNKNOWN;
		event.ext2 = -1;
	}
	else
	{
		event.msg = MEDIA_PREPARED;
		JNILOGI ("BuildGraph successful!.");
	}

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

	m_bPrepareAsync = false;
}

int CVOMEPlayer::BuildGraph (void)
{
      JNILOGI ("CVOMEPlayer BuildGraph"); 
	if (m_nStatus >= 0)
		return 0;

	if (m_hPlay == NULL)
		return -1;

	if (m_nFile == -1 &&  m_hFile == NULL && strlen (m_szFile) <= 0)
		return -1;

	m_bSourceLoading = true;

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

      JNILOGI ("Before API.Playback 2"); 
     
	OMX_U32 nRC = m_fAPI.Playback (m_hPlay, &Source);

	JNILOGI ("after Playback"); 

	if (nRC != OMX_ErrorNone)
	{
		JNILOGE ("m_fAPI.Playback error \n");
		m_bSourceLoading = false;
		return -1;
	}

	JNILOGD ("Before get param VideoSize"); 

	VOME_VIDEOSIZETYPE videoSize;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_VideoSize, &videoSize);
	if (nRC == OMX_ErrorNone)
	{
		m_nVideoWidth = videoSize.nWidth;
		m_nVideoHeight = videoSize.nHeight;
		JNILOGI2 ("Video: W %d, H %d \n", m_nVideoWidth, m_nVideoHeight);

		m_bNotifyVideoSize = false;
		
		if (m_pCallBack != NULL)
		{
			VOA_NOTIFYEVENT event;
			event.msg = MEDIA_SET_VIDEO_SIZE;
			event.ext1 = m_nVideoWidth;
			event.ext2 = m_nVideoHeight;
			JNILOGI ("before IDC_notifyEvent");
			
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
			
	            // added two line below temply
			m_pCallBack (m_pUserData, VOAP_IDC_setVideoSize, &m_nVideoWidth, &m_nVideoHeight);
			m_bNotifyVideoSize = true;
			JNILOGI ("after IDC_notifyEvent");
		}

		m_dspArea.hView = NULL;
		m_dspArea.nWidth = m_nVideoWidth;
		m_dspArea.nHeight = m_nVideoHeight;
//		m_fAPI.SetDisplayArea (m_hPlay, &m_dspArea);

 	
		m_bSetVideoBuffer = false;
	}


      JNILOGD ("Before get audio format param"); 

	VOME_AUDIOFORMATTYPE audioFormat;
	nRC = m_fAPI.GetParam (m_hPlay, VOME_PID_AudioFormat, &audioFormat);
	if (nRC == OMX_ErrorNone)
	{
		JNILOGI2 ("Audio: S %d, C %d B: %d \n", audioFormat.nSampleRate, audioFormat.nChannels, audioFormat.nSampleBits);

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

	return 0;
}

int CVOMEPlayer::Run (void)
{
       JNILOGI ("CVOMEPlayer Run"); 
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
		JNILOGI2 ("seekTo %d Failed!", nPos);
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
		JNILOGI2 ("seekTo %d COMPLETE 0.", nPos);
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

	int nStart = voOMXOS_GetSysTime ();

	m_bUninit = true;

	voCOMXAutoLock lock (&m_mutLoad);
		VOINFO("stat: %d", m_nStatus);
		Stop();
	if (m_nStatus >= 1) {
		VOINFO("stop...");
		Stop ();
	}
		VOINFO("stop...");

	if (m_hPlay != NULL)
	{
		VOINFO("gonna uninit engine...");
		m_fAPI.Uninit (m_hPlay);
		VOINFO("uninit cmd sent...");
		m_hPlay = NULL;
	}

#if defined __VOPRJ_BLUEST__
	wdrm_t pdrm = get_wdrm();
	if (pdrm) 
		pdrm->release(g_pbluedrm);
#endif

#ifdef LINUX
	if (m_hModule != NULL)
		dlclose(m_hModule);
	m_hModule = NULL;
#endif	//LINUX

		VOINFO("gonna close file...");
	if (m_hFile != NULL)
		fclose (m_hFile);
	m_hFile = NULL;
		VOINFO("file closed...");

	if (m_nSourceType > 0)
	{
		if (m_bStreamSourceCount)
		{
			g_voAndroidStreamSource--;
			m_bStreamSourceCount = false;
		}
		JNILOGI2 ("Stream Source Count: %d", g_voAndroidStreamSource);
	}

	JNILOGI2 ("VOME stop and release time is %d", voOMXOS_GetSysTime () - nStart);

	m_nStatus = -1;

	return 0;
}

int CVOMEPlayer::CallBackFunc (int nID, void * pParam1, void * pParam2)
{
	if (m_pCallBack == NULL || m_bUninit)
		return 0;

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));

	if (nID >= 100 && nID <= 400) {
		event.msg = MEDIA_INFO;
		event.ext1 = nID;
		event.ext2 = *(int*)pParam1;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		return 0;
	}

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

	        JNILOGI ("seekTo  COMPLETE 1");
		}
	}
	else if (nID == VOME_CID_PLAY_DOWNLOADPOS)
	{
		event.msg = MEDIA_BUFFERING_UPDATE;
		event.ext1 = *(int *)pParam1;
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
	else
	{
		m_VideoBuff.nColorType = VOA_COLOR_YUV420;
		// return OMX_ErrorNotImplemented;
	}

	if (!m_bNotifyVideoSize || (m_nVideoWidth != pVideoBuffer->Width || m_nVideoHeight != pVideoBuffer->Height))
	{
		 JNILOGI2 ("Size Changed : Video Color %d W %d, H %d, Disp W %d, H %d", pVideoBuffer->Color, pVideoBuffer->Width, pVideoBuffer->Height, m_dspArea.nWidth, m_dspArea.nHeight);

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
	     // JNILOGI("NOTIFY VIDEOSIZE TRUE");
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
		//JNILOGI("NOTIFY VIDEOSIZE TRUE");
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
//	JNILOGW("@@@@@@  CVOMEPlayer vomeVideoRenderBufferProc 000");

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
//	JNILOGW("@@@@@@  CVOMEPlayer vomeAudioRenderBufferProc \n");

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

		JNILOGI2 ("Audio: SamplingRate %d, Channels %d BitPerSample: %d \n", pAudioFormat->nSamplingRate, pAudioFormat->nChannels, pAudioFormat->nBitPerSample);

		pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_setAudioFormat, &pVOME->m_fmtAudio, NULL);
	}


	pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_RenderAudio, 
									pBuffer->pBuffer + pBuffer->nOffset, &pBuffer->nFilledLen);

	return OMX_ErrorNone;
}


int CVOMEPlayer::LoadModule (void)
{
#ifdef LINUX
	char libfile[256];
	sprintf(libfile, "%s/lib/%s", m_szworkingpath, "libvoOMXME.so");
	m_hModule = dlopen (libfile, RTLD_NOW);
	if (m_hModule == NULL)
		m_hModule = dlopen ("/data/data/com.orange.ocs/lib/libvoOMXME.so", RTLD_NOW);
	else
		JNILOGV("LOAD /data/data/com.visualon.vome/lib/libvoOMXME.so");

	if (m_hModule == NULL)
		m_hModule = dlopen ("/data/data/com.visualon.vome/lib/libvoOMXME.so", RTLD_NOW);
		
	if (m_hModule == NULL)
		m_hModule = dlopen ("/data/local/voOMXPlayer/lib/libvoOMXME.so", RTLD_NOW);
	else
		JNILOGV("/data/data/com.visualon.vome/lib/libvoOMXME.so");
	
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

	JNILOGD("api loaded");
	pAPI (&m_fAPI, 0);
#endif // LINUX
	return 0;
}



OMX_PTR CVOMEPlayer::OMX_voLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag)
{
	OMX_PTR	hModule = NULL;
	char	szDll[256];

#ifdef LINUX
	CVOMEPlayer *pm = (CVOMEPlayer*)pUserData;
	if (pm)
	{
		sprintf(szDll, "%s/lib/lib%s", pm->getWorkingPath(), pLibName);

		hModule = dlopen (szDll, nFlag);

		if (hModule == NULL) 
			sprintf(szDll, "%s/lib/%s", pm->getWorkingPath(), pLibName);

		hModule = dlopen (szDll, nFlag);
	}

	if (hModule == NULL) {
		strcpy (szDll, "/data/data/com.orange.ocs/lib/");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
	}

	if (hModule == NULL)
	{
		strcpy (szDll, "/data/data/com.visualon.vome/lib/");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);

		if (hModule == NULL)
		{
			strcpy (szDll, "/data/data/com.visualon.vome/lib/lib");
			strcat (szDll, pLibName);
			hModule = dlopen (szDll, nFlag);
		}
	}

	if (hModule == NULL)
	{
		strcpy (szDll, "/data/local/voOMXPlayer/lib/");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
		if (hModule == NULL)
		{
			strcpy (szDll, "/data/local/voOMXPlayer/lib/lib");
			strcat (szDll, pLibName);
			hModule = dlopen (szDll, nFlag);
		}
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
	if(OMX_VO_SOURCEDRM_FLAG_DRMINFO == nFlag)
	{
		OMX_VO_SOURCEDRM_INFO * pDrmInfo = (OMX_VO_SOURCEDRM_INFO *)pParam;
		if (OMX_VO_DRMTYPE_DIVX == pDrmInfo->nType)
		{
			//initialize DivX DRM module
			//pDrmInfo->pDrmInfo is OMX_VO_DIVXDRM_INFO*
			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMTYPE_WindowsMedia == pDrmInfo->nType)
		{
			//initialize WMDRM module
			//pDrmInfo->pDrmInfo is OMX_VO_WMDRM_INFO*
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
			//VisualOn WM DRM module can process
			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_VIDEOSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			//DivX DRM module process video sample
			//Only for DivX DRM, pDrmData->sDataInfo.pInfo is DD frame, 10 Bytes buffer
			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_AUDIOSAMPLE == pDrmData->sDataInfo.nDataType)
		{
			//DivX DRM module process audio sample
			return OMX_VO_ERR_DRM_OK;
		}
		else if(OMX_VO_DRMDATATYPE_PACKETDATA == pDrmData->sDataInfo.nDataType)
		{

#if defined __VOPRJ_BLUEST__
			wdrm_t pdrm = get_wdrm();
			if (pdrm) 
				pdrm->decrypt_packet(g_pbluedrm, (unsigned char*)pDrmData->pData, (int)pDrmData->nSize);
#endif
			//BlueStreak WM DRM module can process
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
	return 0;
}
