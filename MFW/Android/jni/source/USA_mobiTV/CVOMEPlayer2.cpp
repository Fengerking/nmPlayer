  	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOMEPlayer2.cpp

	Contains:	CVOMEPlayer2 class file

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

#define LOG_TAG "CVOMEPlayer2"

#include "CVOMEPlayer2.h"
#include "voOMXOSFun.h"

#include "voLog.h"
#include "voSource.h"

#define VOINFO(format, ...) { \
	LOGI("[VO] %s()->%d: " format, __FUNCTION__, __LINE__, __VA_ARGS__); }

#define    MEDIA_NOP                   VOME_NOP		    
#define    MEDIA_PREPARED	      VOME_PREPARED	    
#define    MEDIA_PLAYBACK_COMPLETE     VOME_PLAYBACK_COMPLETE   
#define    MEDIA_BUFFERING_UPDATE      VOME_BUFFERING_UPDATE    
#define    MEDIA_SEEK_COMPLETE	      VOME_SEEK_COMPLETE	    
#define    MEDIA_SET_VIDEO_SIZE	      VOME_SET_VIDEO_SIZE	    
#define    MEDIA_ERROR		      VOME_ERROR		    
#define    MEDIA_INFO                  VOME_INFO                

#define    MEDIA_ERROR_UNKNOWN 1

typedef OMX_ERRORTYPE (* VOOMXGetEngineAPI) (VOOMX_ENGINEAPI * pEngine, OMX_U32 uFlag);

int CVOMEPlayer2::g_voAndroidStreamSource = 0;
#define DISABLE_SEEK 0
CVOMEPlayer2::CVOMEPlayer2(void)
	: m_nStatus (-1)
	, m_nLoop (0)
	, m_bUninit (false)
	, m_nVideoWidth (0)
	, m_nVideoHeight (0)
	, m_hModule (NULL)
	//, m_hPlay (NULL)
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
	,m_bNotified(false)
{
	VOLOGI ("construct CVOMEPlayer2");
	strcpy (m_szFile, "");
	memset (&m_VideoBuff, 0, sizeof (VOA_VIDEO_BUFFERTYPE));
	memset (&m_dspArea, 0, sizeof (OMX_VO_DISPLAYAREATYPE));
	memset (&m_fmtAudio, 0, sizeof (VOA_AUDIO_FORMAT));
	m_libOP.LoadLib = OMX_voLoadLib;
	m_libOP.GetAddress = OMX_voGetAddress;
	m_libOP.FreeLib = OMX_voFreeLib;
	m_libOP.pUserData = this;
}

CVOMEPlayer2::~CVOMEPlayer2()
{
	VOLOGI ("destruct CVOMEPlayer2");

	m_fAPI.SetMsgHandler(NULL, NULL);
	Uninit ();
}

int CVOMEPlayer2::SetCallBack (VOACallBack pCallBack, void * pUserData)
{
	m_pCallBack = pCallBack;
	m_pUserData = pUserData;

	m_fAPI.SetMsgHandler(m_pCallBack, m_pUserData);

	return 0;
}

int CVOMEPlayer2::SetFilePipe (void * pFilePipe)
{
	m_pFilePipe = (OMX_VO_FILE_OPERATOR *)pFilePipe;
	return 0;
}
#define CHECK_RET(ret) \
	if ((ret)!=OMX_ErrorNone)\
	{\
		VOLOGE("mAPI_error %X",ret);\
		return -1;\
	}
int	 CVOMEPlayer2::	SetParam(int id, void* param)
{
    VOLOGI("<<<VP2>>> m_fAPI.SetParam");
	return m_fAPI.SetParam (id, param);
}
int		CVOMEPlayer2::GetParam(int id, void* param)
{
    VOLOGI("<<<VP2>>> m_fAPI.GetParam");
	return m_fAPI.GetParam (id, param);
}
int CVOMEPlayer2::Init (void)
{
	
	LoadModule ();
	VOLOGI("<<<VP2>>> m_fAPI.Init");
	OMX_ERRORTYPE ret=m_fAPI.Init (vomeCallBack, this);
	CHECK_RET(ret);
//	ret=m_fAPI.SetParam(MOBITV_FUNCSET_ID,&gMKPlayerFunc);
	ret=m_fAPI.SetMsgHandler(m_pCallBack, m_pUserData);
	CHECK_RET(ret);
	m_dspArea.nWidth = 480;
	m_dspArea.nHeight = 320;
	//m_fAPI.SetDisplayArea (&m_dspArea);
	VOLOGI("<<<VP2>>> m_fAPI.SetParam");
	ret=m_fAPI.SetParam (OMX_VO_IndexLibraryOperator, &m_libOP);
	CHECK_RET(ret);
#if 1
	OMX_VO_CHECKRENDERBUFFERTYPE	audioRender;
	audioRender.pUserData = this;
	audioRender.pCallBack = vomeAudioRenderBufferProc;
	VOLOGI("<<<VP2>>> m_fAPI.SetParam");
	ret=m_fAPI.SetParam (VOME_PID_AudioRenderBuffer, &audioRender);
	CHECK_RET(ret);
	OMX_VO_CHECKRENDERBUFFERTYPE	videoRender;
	videoRender.pUserData = this;
	videoRender.pCallBack = vomeVideoRenderBufferProc;
	VOLOGI("<<<VP2>>> m_fAPI.SetParam");
	ret=m_fAPI.SetParam (VOME_PID_VideoRenderBuffer, &videoRender);
	CHECK_RET(ret);
#endif	
	
	VOLOGI("Init Done");
	return 0;
}

int CVOMEPlayer2::SetSource (const char* pSource)
{
	VOLOGI ("Source: %s", pSource);

	if (m_hLoadThread != NULL)
		return -1;

  m_nSourceType = 1;

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

	if (m_pCallBack != NULL)
		voOMXThreadCreate (&m_hLoadThread, &m_nLoadThreadID, (voOMX_ThreadProc)vomeLoadSourceProc, this, 0);

	return 0;
}

int CVOMEPlayer2::SetSource (int fd, int64_t offset, int64_t length)
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

OMX_U32 CVOMEPlayer2::vomeLoadSourceProc (OMX_PTR pParam)
{
	CVOMEPlayer2 * pPlayer = (CVOMEPlayer2 *)pParam;

	return pPlayer->vomeLoadSource ();
}

OMX_U32 CVOMEPlayer2::vomeLoadSource (void)
{
  m_bSourceLoading = true;
  m_bNotified = false;
  int nRC = BuildGraph ();
  m_bSourceLoading = false;
  if (nRC!=0)
    {
      VOLOGE ("failed to BuildGraph" );
      
      VOA_NOTIFYEVENT event;
      event.msg = MEDIA_ERROR;
      event.ext1 = 0xdead;
      event.ext2 = m_fAPI.lRes_open();
      VOINFO("event.msg: %d", event.msg);
      VOINFO("event.ext1: %08x", event.ext1);
      VOINFO("event.ext2: %08x", event.ext2);
      /*
      if (event.ext2==0x82000404) {
	event.msg = MEDIA_ERROR;
	event.ext1 = MEDIA_ERROR_UNKNOWN;
	//event.ext2 = -1;
	VOINFO("event.msg: %d", event.msg);
	VOINFO("event.ext1: %08x", event.ext1);
	VOINFO("event.ext2: %08x", event.ext2);
      }
      */
      m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

      return nRC;
    }
  VOLOGI ("before NotifyOpenSouce1" );
  voCOMXAutoLock lock (&m_mutLoad);
  m_hLoadThread = NULL;
   VOLOGI ("before NotifyOpenSouce2,isAsync=%d,uninit=%d,force NotifyOpenSource",m_bPrepareAsync,m_bUninit);
#if 1
   NotifyOpenSource ();
#else
  if (m_bPrepareAsync)
    {
      if (1)//m_nSourceType == 0)
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
#endif
  return 0;
}

OMX_U32 CVOMEPlayer2::vomeWaitLoadSource (int nWaitTime)
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

int CVOMEPlayer2::SetLoop (int nLoop)
{
	m_nLoop = nLoop;

	return 0;
}

int CVOMEPlayer2::PrepareAsync (void)
{
	VOLOGI("begin3");
	vomeWaitLoadSource (3000);

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
	VOLOGI("end3");
	return 0;
}

void CVOMEPlayer2::NotifyOpenSource (void)
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
	if(!m_bNotified)
	{
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		m_bNotified = true;
	}

	m_bPrepareAsync = false;
}

int CVOMEPlayer2::BuildGraph (void)
{
  VOLOGI ("before BuildGraph");
  if (m_nStatus >= 0)
    return 0;
  
  //if (m_hPlay == NULL)
  //	return -1;
  
  //if (m_nFile == -1 &&  m_hFile == NULL && strlen (m_szFile) <= 0)
  //	return -1;

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
  
  VOINFO("%s", "gonna try to playback...");
  VOLOGI("<<<VP2>>> m_fAPI.Playback");
  OMX_U32 nRC = m_fAPI.Playback (&Source);
  if (nRC != OMX_ErrorNone)
    {
      VOLOGE ("m_fAPI.Playback error ");
      m_bSourceLoading = false;

      return -1;
    }
  
  m_nStatus = 0;
  VOME_VIDEOSIZETYPE videoSize;
  //nRC = m_fAPI.GetParam (VOME_PID_VideoSize, &videoSize);
  VOLOGI("<<<VP2>>> m_fAPI.GetVideoSize");
  nRC = m_fAPI.GetVideoSize (&videoSize);
  if (nRC == OMX_ErrorNone)
    {
      m_nVideoWidth = videoSize.nWidth;
      m_nVideoHeight = videoSize.nHeight;
      VOLOGI ("Video: W %d, H %d ", m_nVideoWidth, m_nVideoHeight);
      
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
      VOLOGI("<<<VP2>>> m_fAPI.SetDisplayArea");
      m_fAPI.SetDisplayArea (&m_dspArea);
      
      m_bNotifyVideoSize = false;
      m_bSetVideoBuffer = false;
    }
  else
    {
      VOLOGE("Video size is not passed yet");
    }
  VOME_AUDIOFORMATTYPE audioFormat;
  VOLOGI("<<<VP2>>> m_fAPI.GetAudioFormat");
  nRC = m_fAPI.GetAudioFormat (&audioFormat);
  if (nRC == OMX_ErrorNone)
    {
      VOLOGI ("Audio: S %d, C %d ", audioFormat.nSampleRate, audioFormat.nChannels);
      
      if (m_pCallBack != NULL)
	{
	  m_fmtAudio.nSampleRate = audioFormat.nSampleRate;
	  m_fmtAudio.nChannels = audioFormat.nChannels;
	  m_fmtAudio.nSampleBits = audioFormat.nSampleBits;
	  m_pCallBack (m_pUserData, VOAP_IDC_setAudioFormat, &m_fmtAudio, NULL);
	}
    }
  else
    {
      VOLOGE("Audio format is not passed yet");
    }
  
  m_nDuration = 0;
  GetDuration (&m_nDuration);
  
#if 0//!DISABLE_SEEK
  if (m_nSourceType > 0)
    m_bSeeking = true;
#endif//DISABLE_SEEK
  
  
  m_bSourceLoading = false;
  
  return 0;
}

int CVOMEPlayer2::Run (void)
{
   //	if (m_hPlay == NULL)
     //  	return -1;
	while(m_bSourceLoading)
	{
		voOMXOS_Sleep (10);
		VOLOGI("wait for source loading done");
	}
	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStart, NULL, NULL);

	if (m_bAutoStart)
		m_bAutoStart = false;
	
	VOLOGI("<<<VP2>>> m_fAPI.Run");
    if (m_fAPI.Run ()== OMX_ErrorNone)
    {
		if (0)//m_nStartPos > 0)
		{
	VOLOGI("<<<VP2>>> m_fAPI.SetCurPos");
			m_fAPI.SetCurPos (m_nStartPos);
			m_nStartPos = 0;
		}

    	m_nStatus = 1;
    	return 0;
    }

	return -1;
}

int CVOMEPlayer2::Pause (void)
{
  

	VOLOGI("<<<VP2>>> m_fAPI.Pause");
    OMX_ERRORTYPE ret=m_fAPI.Pause ();
	CHECK_RET(ret);
	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
	
	m_nStatus = 2;

	return 0;
}
#include "voOSFunc.h"
int CVOMEPlayer2::Stop (void)
{
   // if (m_hPlay == NULL)
     //  	return -1;
	VOLOGI("status=%d",m_nStatus);
	if (m_nStatus < 0||m_nStatus==3)
		return 0;

	VOLOGI("<<<VP2>>> m_fAPI.Stop");
	OMX_ERRORTYPE ret=m_fAPI.Stop ();

	if (m_pCallBack != NULL)
		m_pCallBack (m_pUserData, VOAP_IDC_setAudioStop, NULL, NULL);

	m_nStatus = 3;
	m_nStartPos = 0;

	return 0;
}

bool CVOMEPlayer2::IsPlaying (void)
{
    if (m_nStatus == 1)
    	return true;
    else
    	return false;
}

int CVOMEPlayer2::SetPos (int nPos)
{


#if !DISABLE_SEEK
	if (nPos >= m_nDuration&&(int)m_nDuration<2000)
		return -1;
	if (nPos >= m_nDuration - 1500)
		nPos = m_nDuration - 1500;

	m_nStartPos = nPos;

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));
	event.msg = MEDIA_SEEK_COMPLETE;
	VOLOGI (" before send SEEK_COMPLETE");
	if ( m_nStatus <= 0)
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

	VOLOGI("<<<VP2>>> m_fAPI.SetCurPos");
	//m_fAPI.Pause();
    OMX_ERRORTYPE errType = m_fAPI.SetCurPos (nPos);
    if (errType != OMX_ErrorNone)
    {
        VOLOGI ("seekTo %d Failed!", nPos);
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
        return 0;
    }
#if 0
    if (m_nSourceType > 0)
    {
     	m_bSeeking = true;
		//if (m_pCallBack != NULL)
		//	m_pCallBack (m_pUserData, VOAP_IDC_setAudioPause, NULL, NULL);
    }
    else
#endif
    {
		if (m_pCallBack != NULL)
			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		VOLOGI ("seekTo %d COMPLETE 0.", nPos);
		//m_fAPI.Pause();
		//m_fAPI.Run();
	}
#endif//DISABLE_SEEK
	return 0;
}

int CVOMEPlayer2::GetPos (int * pPos)
{
//	VOLOGI ("******   GetPos");

  

	if (m_nStartPos > 0)
	{
		if (m_nStatus != 1 && m_nStatus != 2)
		{
			*pPos = m_nStartPos;
			return 0;
		}
	}

	VOLOGI("<<<VP2>>> m_fAPI.GetCurPos");
    m_fAPI.GetCurPos ((OMX_S32 *)pPos);

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

int CVOMEPlayer2::GetDuration (int * pPos)
{
  
	VOLOGI("<<<VP2>>> m_fAPI.GetDuration");
 	m_fAPI.GetDuration ((OMX_S32 *) pPos);

	return 0;
}

int CVOMEPlayer2::Reset (void)
{
	m_nLoop = 0;
	m_nStatus = -1;

	return 0;
}

int CVOMEPlayer2::Uninit (void)
{
	
	m_bUninit = true;
	VOLOGI ("");
	voCOMXAutoLock lock (&m_mutLoad);
	if (m_nSourceType > 0)
	{
		if (m_bStreamSourceCount)
		{
			g_voAndroidStreamSource--;
			m_bStreamSourceCount = false;
		}
		VOLOGI ("Stream Source Count: %d", g_voAndroidStreamSource);
	}
	if (m_nStatus >= 1)
		Stop ();
	VOLOGI("<<<VP2>>> m_fAPI.Uninit");
	OMX_ERRORTYPE ret=m_fAPI.Uninit ();
	

#ifdef LINUX
	if (m_hModule != NULL)
		dlclose(m_hModule);
	m_hModule = NULL;
#endif	//LINUX

	if (m_hFile != NULL)
		fclose (m_hFile);
	m_hFile = NULL;
	


	m_nStatus = -1;
	CHECK_RET(ret)
	return 0;
}

int CVOMEPlayer2::CallBackFunc (int nID, void * pParam1, void * pParam2)
{
	if (m_pCallBack == NULL || m_bUninit)
		return 0;

	VOA_NOTIFYEVENT event;
	memset (&event, 0, sizeof (VOA_NOTIFYEVENT));

	if (nID == VOME_CID_PLAY_FINISHED)
	{
		VOLOGI("Play complete from VOME");
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
			bool hasNetError = m_fAPI.IsNetError();
			if(!hasNetError)
				m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
			VOLOGI("Play complete! error?=%d",hasNetError);
		}

		return 0;
	}
	else if (nID == VOME_CID_PLAY_ERROR)
	{
        VOLOGI ("VOME Unknown Error!");

		event.msg = MEDIA_ERROR;
		event.ext1 = MEDIA_ERROR_UNKNOWN;
		m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

		return 0;
	}	
	else if (nID == VOME_CID_PLAY_BUFFERING)
	{
		if (!m_fAPI.IsEndOfStream())
		{
			
			event.msg = MEDIA_BUFFERING_UPDATE;
			event.ext1 = *(int *)pParam1;
			if(event.ext1==0)
			{
				VOLOGI ("VOME_CID_PLAY_BUFFER_BEGINE");
				//m_pCallBack (m_pUserData, MOBITV_BUFFERING_BEGIN, NULL, NULL);

			}

			if(event.ext1==100)
			{
				VOLOGI ("VOME_CID_PLAY_BUFFER_END");
				//m_pCallBack (m_pUserData, MOBITV_BUFFERING_END, NULL, NULL);
			}

			m_pCallBack (m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
			
		}
	}
	

	return 0;
}

int CVOMEPlayer2::RenderVideo (OMX_VO_VIDEOBUFFERTYPE * pVideoBuffer)
{
	if (m_bPrepareAsync)
	{
		m_mutAVRender.Lock ();
		//VOLOGI("");
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
		VOLOGI (" seek complete3");
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

	if (!m_bNotifyVideoSize)
	{
		//VOLOGI ("Video Color %d W %d, H %d, Disp W %d, H %d", pVideoBuffer->Color, pVideoBuffer->Width, pVideoBuffer->Height, m_dspArea.nWidth, m_dspArea.nHeight);

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

	VOLOGI("<<<VP2>>> m_fAPI.SetParam");
		m_fAPI.SetParam ( VOME_PID_VideoDataBuffer, &videoBuff);
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

OMX_S32 CVOMEPlayer2::vomeCallBack (OMX_PTR pUserData, OMX_S32 nID, OMX_PTR pParam1, OMX_PTR pParam2)
{
	return ((CVOMEPlayer2*) pUserData)->CallBackFunc (nID, pParam1, pParam2);
}

OMX_ERRORTYPE CVOMEPlayer2::vomeVideoRenderBufferProc (OMX_HANDLETYPE hComponent,
										   OMX_PTR pAppData,
										   OMX_BUFFERHEADERTYPE* pBuffer,
										   OMX_IN OMX_INDEXTYPE nType,
										   OMX_IN OMX_PTR pFormat)
{
//	VOLOGW("@@@@@@  CVOMEPlayer2 vomeVideoRenderBufferProc 000");

	if (nType != (OMX_INDEXTYPE)OMX_VO_IndexVideoBufferType)
		return OMX_ErrorNotImplemented;

	return (OMX_ERRORTYPE) ((CVOMEPlayer2 *)pAppData)->RenderVideo ((OMX_VO_VIDEOBUFFERTYPE *)pFormat);
	//return OMX_ErrorNone;
}

OMX_ERRORTYPE CVOMEPlayer2::vomeAudioRenderBufferProc (OMX_HANDLETYPE hComponent,
													   OMX_PTR pAppData,
													   OMX_BUFFERHEADERTYPE* pBuffer,
													   OMX_IN OMX_INDEXTYPE nType,
													   OMX_IN OMX_PTR pFormat)
{
//	LOGW("@@@@@@  CVOMEPlayer2 vomeAudioRenderBufferProc ");

	CVOMEPlayer2 *					pVOME = (CVOMEPlayer2 *)pAppData;
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
		VOLOGI (" seek complete4");
		pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
		pVOME->m_mutAVRender.Unlock ();
	}

	if (0)//pVOME->m_bAutoStart)
	{
		pVOME->Pause ();
		return OMX_ErrorNone;
	}

	if (pVOME->m_pCallBack == NULL)
		return OMX_ErrorNone;

	//VOLOGI (" Audio Format %d, %d, %d", pAudioFormat->nSamplingRate,  pAudioFormat->nChannels,  pAudioFormat->nBitPerSample);
	//VOLOGI (" 11 Audio Format %d, %d, %d", pVOME->m_fmtAudio.nSampleRate,  pVOME->m_fmtAudio.nChannels,  pVOME->m_fmtAudio.nSampleBits);

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

OMX_U32 CVOMEPlayer2::vomeSourceDrmProc(OMX_IN OMX_U32 nDrmType, OMX_IN OMX_PTR pDrmInfo, 
									   OMX_IN OMX_PTR pUserData, OMX_IN OMX_U32 nReserved)
{
/*
	CVOMEPlayer2 * pVOME = (CVOMEPlayer2 *)pUserData;
	if(VO_SOURCE_DRM_DIVX == nDrmType)
	{
		VO_SOURCE_DRMINFO* pDrmInfoCont = (VO_SOURCE_DRMINFO *)pDrmInfo;
		if(pDrmInfoCont->Flags & VO_SOURCE_DRMINFO_FLAG_NOAUTH)
		{
			VOA_NOTIFYEVENT event;
			event.msg = MEDIA_ERROR;
			event.ext1 = MEDIA_ERROR_DRM_NOAUTH;
			event.ext2 = 0;

			if (pVOME->m_pCallBack != NULL)
				pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

			return 1;
		}

		if(pDrmInfoCont->Flags & VO_SOURCE_DRMINFO_FLAG_RENTAL)
		{
			//rental mode
			VOA_NOTIFYEVENT event;
			event.ext2 = (pDrmInfoCont->Param1 << 8) | pDrmInfoCont->Param2;
			if(pDrmInfoCont->Param1 < pDrmInfoCont->Param2)
			{
				event.msg = MEDIA_INFO;
				event.ext1 = MEDIA_INFO_DRM_RENTAL;

				if (pVOME->m_pCallBack != NULL)
					pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);
			}
			else	//rental expired
			{
				event.msg = MEDIA_ERROR;
				event.ext1 = MEDIA_ERROR_DRM_EXPIRED;

				if (pVOME->m_pCallBack != NULL)
					pVOME->m_pCallBack (pVOME->m_pUserData, VOAP_IDC_notifyEvent, &event, NULL);

				return 1;
			}
		}
	}
*/
	return 0;
}

#if defined __VOPRJ_BLUEST__

OMX_U32 CVOMEPlayer2::vomeWDrmPacketProc(OMX_PTR pPacketData, OMX_U32 dwPacketSize, OMX_PTR pUserData, OMX_U32 nReserved) {

	CVOMEPlayer2* pp = (CVOMEPlayer2*)pUserData;
	OMX_U32 ret = 0;
	if (pp != NULL)
		ret = pp->m_pCallBack(pp->m_pUserData, VOAP_IDC_DecryptWDrmPacket, pPacketData, (void*)dwPacketSize);

	return 0;
}

#endif

int CVOMEPlayer2::LoadModule (void)
{
#if 0//def LINUX
	char tmpPath[256];
	//strcpy(tmpPath,VO_APPLICATION_LIB_LOCATION);strcat(tmpPath,"libMKPlayer.so");
	strcpy(tmpPath,vo_application_lib_location);strcat(tmpPath,"libMKPlayer.so");
	m_hModule = dlopen (tmpPath, RTLD_NOW);
	int pathIndex=0;
	
	if (m_hModule == NULL)
	{
		VOLOGE ("@@@@@@ Load Module %s Error %s ", tmpPath,dlerror ());
		strcpy(tmpPath,"/system/lib/libMediaKit.so");
		m_hModule = dlopen (tmpPath, RTLD_NOW);
		pathIndex=1;
	}
	if (m_hModule == NULL)
	{
		VOLOGE ("@@@@@@ Load Module %s Error %s ", tmpPath,dlerror ());
		return -1;
	}
	else
	{
		VOLOGI ("load %s ,path=%d", tmpPath,pathIndex);
	}
#endif // LINUX
	return 0;
}

int CVOMEPlayer2::IsSeekable()
{
	return 1;
}

OMX_PTR CVOMEPlayer2::OMX_voLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag)
{
	OMX_PTR	hModule = NULL;
	char	szDll[256];
#ifdef LINUX
		int pathIdx=0;
	if (hModule == NULL) {
	  //strcpy (szDll, VO_APPLICATION_LIB_LOCATION);
	  strcpy (szDll, vo_application_lib_location);
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
		if (hModule == NULL) {
		  //strcpy (szDll, VO_APPLICATION_LIB_LOCATION);
		  strcpy (szDll, vo_application_lib_location);
			strcat(szDll, "lib");
			strcat (szDll, pLibName);
			hModule = dlopen (szDll, nFlag);
			pathIdx=3;
		}
	}
	if (hModule == NULL)
	{
		strcpy (szDll, "/data/local/voOMXPlayer/lib/");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
		pathIdx=4;
	}
	if (hModule == NULL)
	{
		strcpy (szDll, "lib");
		strcat (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
		pathIdx=5;
	}
	if (hModule == NULL)
	{
		strcpy (szDll, pLibName);
		hModule = dlopen (szDll, nFlag);
		pathIdx=6;
	}
	VOLOGI("lib=%s,pathIdx=%d",pLibName,pathIdx);
#endif // LINUX

	//	VOLOGE (" Name: %s, Result: %d ******************************", szDll, (int)hModule);

	return hModule;
}

OMX_PTR CVOMEPlayer2::OMX_voGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return NULL;

	OMX_PTR pFunc = NULL;

#ifdef LINUX
	pFunc = dlsym(hLib, pFuncName);
#endif // LINUX

	return pFunc;
}

OMX_S32 CVOMEPlayer2::OMX_voFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return -1;

#ifdef LINUX
	dlclose (hLib);
#endif // LINUX

	//	VOLOGE (" Handle: %d ******************************",  (int)hLib);

	return 0;
}

OMX_U32 CVOMEPlayer2::OMX_voSourceDrmCallBack (OMX_PTR pUserData, OMX_U32 nFlag, OMX_PTR pParam, OMX_U32 nReserved)
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
			VOINFO("decrypt wdrm buf: %p, size: %d", pDrmData->pData, (int)pDrmData->nSize);
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





