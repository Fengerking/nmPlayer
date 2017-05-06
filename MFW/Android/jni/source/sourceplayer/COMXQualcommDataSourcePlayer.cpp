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
#if defined LINUX
#include <dlfcn.h>
#include "voOSFunc.h"
#endif
#include "stdlib.h"
#include "COMXQualcommDataSourcePlayer.h"
#include "voLog.h"



#include "voOMXThread.h"
#include "voOMXOSFun.h"
#include "vojnilog.h"

#pragma warning (disable : 4996)


#define LOG_TAG "COMXQualcommDataSourcePlayer"


#define VOME_CONFIG_CORE_FILE _T("vomeCore.cfg")
#define VOME_CONFIG_COMP_FILE _T("vomeComp.cfg")

COMXQualcommDataSourcePlayer * COMXQualcommDataSourcePlayer::g_pDataSourcePlayer = NULL;

COMXQualcommDataSourcePlayer::COMXQualcommDataSourcePlayer(void)
	: COMXEngine ()
	, m_pCompSrc (NULL)
	, m_pCompAD (NULL)
	, m_pCompVD (NULL)
	, m_pCompAR (NULL)
	, m_pCompVR (NULL)
	, m_pCompClock (NULL)
	
	, m_libFLOEngine(NULL)
  , m_hFLOEngine(NULL)
{
  	g_pDataSourcePlayer = this;
  	
  	m_libOP.pUserData = this;
		m_libOP.LoadLib = OMX_mmLoadLib;
		m_libOP.GetAddress = OMX_mmGetAddress;
		m_libOP.FreeLib = OMX_mmFreeLib;

		memset(&m_FLOEngineAPI, 0, sizeof(VO_FLOENGINE_API));
		memset(&m_FLOEngineCallBack, 0, sizeof(VO_FLOENGINE_CALLBACK));
	
		CreateFLOEngine();
		
		JNILOGI("Construct");
}

COMXQualcommDataSourcePlayer::~COMXQualcommDataSourcePlayer ()
{
	 g_pDataSourcePlayer = NULL;
   DestroyFLOEngine();
}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::SetDisplayArea (OMX_VO_DISPLAYAREATYPE * pDisplay)
{
    VOLOGE ("<<<DaSoPl>>> enter");
    
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	memcpy (&m_dspArea, pDisplay, sizeof (OMX_VO_DISPLAYAREATYPE));

    OMX_ERRORTYPE lRes = m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
    VOLOGE ("<<<DaSoPl>>> leave");
    return lRes;
//	return m_OMXEng.SetDisplayArea (m_hEngine, pDisplay);
}
#if 0//defined(_WIN32_XP)
#define VOLOGI2 VOLOGI
#else
#define VOLOGI2(...)
#endif
static char newlink[1024];
static char* GetTheFileFromCFG(int param,char* filename=NULL)
{
	if (1)//param)
	{
		newlink[0]='\0';
		if (filename==NULL)
#ifdef _WIN32
			filename = "c:/mobitv.txt";
#else
			filename = "/data/local/mobitv.txt";
#endif
		FILE* pSDPFile = fopen(filename, "rb");
		if(pSDPFile)
		{
			fseek( pSDPFile, 0L, SEEK_END );
			int fileSize = ftell(pSDPFile);
			if(fileSize<0)
				fileSize = 1024*10;
			char* pSDPData = new char[fileSize+1];
			fseek(pSDPFile,0,SEEK_SET);
			int sdpDataSize = fread(pSDPData, 1, fileSize, pSDPFile);
			pSDPData[sdpDataSize] = '\0';
			char * pRTSPLink = strstr(pSDPData, "file://");
			if(pRTSPLink)
			{
				sscanf(pRTSPLink, "%[^\r\n]", newlink);
				VOLOGI("the link is %s\n",newlink);
			}
			else
			{
				VOLOGE("no valid file\n");
			}
			delete[] pSDPData;
			fclose(pSDPFile);
		}
		else
		{
			VOLOGE("can not open file,%s\n",filename);
		}
		return newlink;
	}
	else
	{
		//VOLOGE("can not open file,%s\n",filename);
		return "file:///sdcard/video/BeeMovie_1.3gp";
	}

}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::Playback (VOME_SOURCECONTENTTYPE * pSource)
{	
	VOLOGF ();
	
	JNILOGE("Playback");

/*
	if (!m_bInit)
		Init (NULL);
	if (m_pChain == NULL)
		return OMX_ErrorResourcesLost;

	if (m_pChain->m_pLibOP == NULL && m_pWorkPath != NULL)
		SetParam (OMX_VO_IndexLibraryOperator, &m_libOP);
*/
	OMX_ERRORTYPE errType = OMX_ErrorNone;

 JNILOGI("before stop");
	errType = Stop ();
	
	 JNILOGI("After stop");
	if (errType != OMX_ErrorNone)
	{
		JNILOGE2 ("The result is 0X%08X from Stop!", errType);
		return errType;
	}

/*
	//voCOMXAutoLock lock (&m_tmState);

	//errType = m_pChain->ReleaseBoxes ();
	if (errType != OMX_ErrorNone)
	{
		VOLOGE ("The result is 0X%08X from m_pChain->ReleaseBoxes!", errType);
		return errType;
	}
  */
  
 JNILOGI("before add component");
	errType = AddComponent ("OMX.VisualOn.DataSource", &m_pCompSrc);
	if (errType != OMX_ErrorNone)
		return errType;
		
		/*
	OMX_AUDIO_PARAM_PCMMODETYPE tpAudioFormat;
	memset (&tpAudioFormat, 0, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	tpAudioFormat.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("GetParameter Audio Foramt,%X",errType);
		return errType;
	}
	*/
	
	
	
	
	
	// ==============================

	OMX_PARAM_PORTDEFINITIONTYPE portType;
	memset (&portType, 0, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	portType.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;
	portType.format.audio.eEncoding = OMX_AUDIO_CodingAAC;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;

	// set the Audio format
	OMX_AUDIO_PARAM_PCMMODETYPE tpAudioFormat;
	memset (&tpAudioFormat, 0, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	tpAudioFormat.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	tpAudioFormat.nSamplingRate = 44100;
	tpAudioFormat.nChannels = 2;
	tpAudioFormat.nBitPerSample = 16;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	// set the video codec and size
	memset (&portType, 0, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	portType.nPortIndex = 1;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;
	portType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	portType.format.video.nFrameWidth = 320;
	portType.format.video.nFrameHeight = 240;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;

	// ==============================
	
	

 JNILOGI("before add clock");
	errType = AddComponent ("OMX.VisualOn.Clock.Time", &m_pCompClock);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Audio.Sink", &m_pCompAR);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = AddComponent ("OMX.VisualOn.Video.Sink", &m_pCompVR);
	if (errType != OMX_ErrorNone)
		return errType;
	
	m_pCompVD->SetParameter (m_pCompVD, (OMX_INDEXTYPE)OMX_VO_IndexTimeStampOwner, NULL);


	errType = ConnectPorts (m_pCompSrc, 0, m_pCompAD, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompSrc, 1, m_pCompVD, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompClock, 0, m_pCompSrc, 2, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);
	if (errType != OMX_ErrorNone)
		return errType;

	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
//	clkRef.eClock = OMX_TIME_RefClockVideo;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);

	//m_nVideoWidth = 320;
//	m_nVideoHeight = 240;

   JNILOGI("Before check floengine");

	if(m_hFLOEngine == NULL)
	{
		JNILOGE("m_hFLOEngine == NULL");
		return OMX_ErrorUndefined;
  }
#ifdef _WIN32
	WideCharToMultiByte (CP_ACP, 0, (LPWSTR) pSource->pSource, -1, m_szSource, 1024, NULL, NULL);
	strcpy(m_szSource, "rtsp://10.2.64.104/wtcc.pcap");
	if(_strnicmp(m_szSource, "rtsp://", 7) != 0)
		return OMX_ErrorNotImplemented;
#elif defined(_LINUX)
	strcpy(m_szSource, (OMX_STRING)pSource->pSource); 
//	strcpy(m_szSource, "rtsp://10.2.64.104/wtcc.pcap");
	if(strncasecmp(m_szSource, "rtsp://", 7) != 0)
		return OMX_ErrorNotImplemented;
#endif // _WIN32

	JNILOGI("Before set FLOEngine param");

	m_FLOEngineCallBack.OnEvent = COMXQualcommDataSourcePlayer::OnEvent;
	m_FLOEngineCallBack.OnFrame = COMXQualcommDataSourcePlayer::OnFrame;
	m_FLOEngineAPI.SetParam(m_hFLOEngine, VO_PID_FLOENGINE_CALLBACK, &m_FLOEngineCallBack);

	JNILOGI("Before flo engine open");
	VO_U32 ret = m_FLOEngineAPI.Open(m_hFLOEngine, m_szSource);
	if(ret != VO_ERR_FLOENGINE_OK)
	{
		JNILOGE2("FLOEngine Open Failure %d", ret);
		return OMX_ErrorUndefined;
	}

  JNILOGI("Before flo engine start");
	ret = m_FLOEngineAPI.Start(m_hFLOEngine);
	if(ret != VO_ERR_FLOENGINE_OK)
	{
		VOLOGE("FLOEngine Start Failure");
		return OMX_ErrorUndefined;
	}

	//m_sStatus = VOME_STATUS_Loaded;

	return OMX_ErrorNone;
	/*
    VOLOGE ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL)
	{
		VOLOGE("the engine is not inited yet");
		return OMX_ErrorInvalidState;
	}
	//return m_OMXEng.Playback (m_hEngine, pSource);
	
	
	OMX_ERRORTYPE	errType = OMX_ErrorNone;
#ifndef VORTSPIMP
	VOLOGI("source: %s", pSource->pSource);
#ifdef TEST_GET_FILE_FROM_CFG
	 char* url=GetTheFileFromCFG(0);
#else//GET_FILE_FROM_CFG
	char *url = (char *)(pSource->pSource);
#endif//GET_FILE_FROM_CFG
#else//VORTSPIMP
	char* url=GetTheLinkFromCFG(1);
#endif//VORTSPIMP	
	if (strlen(url)<7)
	{
		VOLOGE("Invalid URL");
		return OMX_ErrorUndefined;
	}
	VOLOGI("gonna open url....%s",url);
//tmp	errType=(OMX_ERRORTYPE)OpenURL(url);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("Open %s failed.", url);
		return errType;
	}
	
	//VOLOGI("%s", "url opened....");
	errType = AddComponent ("OMX.VisualOn.DataSource", &m_pCompSrc);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("AddComponent OMX.VisualOn.DataSource failed,%X",errType);
		return errType;
	}
	// set the Audio format
	OMX_AUDIO_PARAM_PCMMODETYPE tpAudioFormat;
	memset (&tpAudioFormat, 0, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	tpAudioFormat.nPortIndex = 0;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("GetParameter Audio Foramt,%X",errType);
		return errType;
	}

	tpAudioFormat.nSamplingRate = m_sampleRate;
	tpAudioFormat.nChannels = m_channelNum;
	tpAudioFormat.nBitPerSample = 16;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamAudioPcm, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("SetParameter Audio Foramt,%X",errType);
		return errType;
	}

	// set the video codec and size
	OMX_PARAM_PORTDEFINITIONTYPE portType;
	memset (&portType, 0, sizeof (OMX_PARAM_PORTDEFINITIONTYPE));
	portType.nPortIndex = 1;
	errType = m_pCompSrc->GetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("GetParameter Video Foramt,%X",errType);
		return errType;
	}
	portType.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
	portType.format.video.nFrameWidth = m_width;
	portType.format.video.nFrameHeight = m_height;
	errType = m_pCompSrc->SetParameter (m_pCompSrc, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
	{
		VOLOGE("SetParameter video Foramt,%X",errType);
		return errType;
	}
	errType = AddComponent ("OMX.VisualOn.Clock.Time", &m_pCompClock);

	errType = AddComponent ("OMX.VisualOn.Audio.Decoder.XXX", &m_pCompAD);
	errType = AddComponent ("OMX.VisualOn.Audio.Sink", &m_pCompAR);

	errType = AddComponent ("OMX.VisualOn.Video.Decoder.XXX", &m_pCompVD);
	errType = AddComponent ("OMX.VisualOn.Video.Sink", &m_pCompVR);
	
	VOLOGI("Before Audio Chain");
	
	errType = ConnectPorts (m_pCompSrc, 0, m_pCompAD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompAD, 1, m_pCompAR, 0, OMX_TRUE);

	VOLOGI("Before Video Chain");

	errType = ConnectPorts (m_pCompSrc, 1, m_pCompVD, 0, OMX_TRUE);
	errType = ConnectPorts (m_pCompVD, 1, m_pCompVR, 0, OMX_TRUE);

	VOLOGI("Before Clock Chain");
	errType = ConnectPorts (m_pCompClock, 0, m_pCompSrc, 2, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 1, m_pCompVR, 1, OMX_TRUE);
	errType = ConnectPorts (m_pCompClock, 2, m_pCompAR, 1, OMX_TRUE);
	VOLOGI("Before OMX_IndexConfigTimeActiveRefClock");
	
	OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE clkRef;
	clkRef.eClock = OMX_TIME_RefClockAudio;
//	clkRef.eClock = OMX_TIME_RefClockVideo;
	m_pCompClock->SetConfig (m_pCompClock, OMX_IndexConfigTimeActiveRefClock, &clkRef);
	// SetDisplayArea (&m_dspArea);
	m_dspArea.nHeight = m_height;
	m_dspArea.nWidth  = m_width;
	VOLOGI("Before OMX_VO_IndexConfigDisplayArea(%d,%d)",m_dspArea.nWidth, m_dspArea.nHeight);
	
	if (m_pCompVR != NULL)
		m_pCompVR->SetConfig (m_pCompVR, (OMX_INDEXTYPE)OMX_VO_IndexConfigDisplayArea, &m_dspArea);
	
	//errType = Run ();
	VO_MK_Player_Play((MK_Player *)lPlayer);
    VOLOGE ("<<<HGL>>> called VO_MK_Player_Play");
	int 	dwID = 0;
	HANDLE tmpHandle=0;
	if (m_pCompAD != NULL)
	{
		m_bAudioStop = false;
		if (m_hThreadAudio==NULL)
		{
			m_hThreadAudio = (HANDLE)1;
			voOMXThreadCreate(&tmpHandle, (OMX_U32*)&dwID, (voOMX_ThreadProc) ReadAudioDataProc, this, 0);
			if (errType!=OMX_ErrorNone)
			{
				m_hThreadAudio = NULL;
				VOLOGE("fail to create audio thread");
				return errType;
			}
		
		}
		else
		{
			VOLOGE("The audio thread is alive,%X",m_hThreadAudio);
			return OMX_ErrorInsufficientResources;
		}
	}
	
	if (m_pCompVD != NULL)
	{
		m_bVideoStop = false;
		//TBD:the voOMXThreadCreate has one bug, do no rely on the thread handle
		if (m_hThreadVideo == NULL)
		{
			m_hThreadVideo = (HANDLE)2;
			errType=voOMXThreadCreate (&tmpHandle, (OMX_U32*)&dwID, (voOMX_ThreadProc) ReadVideoDataProc, this, 0);
			if (errType!=OMX_ErrorNone)
			{
				m_hThreadVideo = NULL;
				VOLOGE("fail to create video thread");
				return errType;
			}
		
			
		}
		else
		{
			VOLOGE("The video thread is alive,%X",m_hThreadVideo);
			return OMX_ErrorInsufficientResources;
		}
	}
	
	if (m_hThreadTrigger==NULL)
	{
		m_hThreadTrigger = (HANDLE)3;		
		errType = 	voOMXThreadCreate (&tmpHandle, (OMX_U32*)&dwID, (voOMX_ThreadProc) TriggerProc, this, 0);
		if (errType!=OMX_ErrorNone)
		{
			m_hThreadTrigger = NULL;
			VOLOGE("fail to create trigger thread");
			return errType;
		}
	} 
	else
	{
		VOLOGE("The trigger thread is alive,%X",m_hThreadTrigger);
		return OMX_ErrorInsufficientResources;
	}
		

    VOLOGE ("<<<DaSoPl>>> leave");
    
	return errType;
	*/
	
}
 OMX_U32		COMXQualcommDataSourcePlayer::Release (void)
 {
    JNILOGI ("Release in source player");
	  Stop();
	 COMXEngine::Release();
       JNILOGI ("Release finished");
	 return 0;
 }
OMX_ERRORTYPE COMXQualcommDataSourcePlayer::Close (void)
{
    VOLOGE ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL)
		return OMX_ErrorInvalidState;

	Stop ();

    OMX_ERRORTYPE lRes = m_OMXEng.Close (m_hEngine);
    VOLOGE ("<<<DaSoPl>>> leave");
    return lRes;
//	return m_OMXEng.Close (m_hEngine);
}



OMX_ERRORTYPE COMXQualcommDataSourcePlayer::Run (void)
{
	JNILOGE("Qualcomm run");
	OMX_ERRORTYPE errType = m_OMXEng.Run (m_hEngine);

	return errType;

	
}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::Pause (void)
{

    OMX_ERRORTYPE lRes = m_OMXEng.Pause (m_hEngine);

    return lRes;

}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::Stop (void)
{  
	
		if(m_hFLOEngine != NULL)
	{
		m_FLOEngineAPI.Stop(m_hFLOEngine);
	}


	
	  if (m_hEngine == NULL)
	  	 return OMX_ErrorNone;
	  	 
    OMX_ERRORTYPE lRes = m_OMXEng.Stop (m_hEngine);

    return lRes;

}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::GetDuration (OMX_S32 * pDuration)
{
	/*
    VOLOGE ("<<<DaSoPl>>> enter");
	if (m_hEngine == NULL||lPlayer==NULL)
		return OMX_ErrorInvalidState;
	VOLOGI("duration=%d",m_duration);
	*pDuration=m_duration;
		*/
	return OMX_ErrorNone;


	
}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::GetCurPos (OMX_S32 * pCurPos)
{

    OMX_ERRORTYPE lRes = m_OMXEng.GetCurPos (m_hEngine, pCurPos);
   
    return lRes;

}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::SetCurPos (OMX_S32 nCurPos)
{   
    OMX_ERRORTYPE lRes = m_OMXEng.SetCurPos (m_hEngine, nCurPos);

    return lRes;

}

// ============================================================

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::CreateFLOEngine()
{
#ifdef _WIN32
	m_libFLOEngine = (HMODULE)m_libOP.LoadLib ( m_libOP.pUserData, "voFLOEngine.Dll", 0);
#else 
	m_libFLOEngine = (OMX_PTR)m_libOP.LoadLib ( m_libOP.pUserData, "libvoFLOEngine.so", RTLD_NOW);
#endif // _WIN32

	if(m_libFLOEngine == NULL)
	{
		JNILOGE ("lib voFLOEngine Load failure");
		return OMX_ErrorUndefined;
	}

	PVOGETFLOENGINEAPI pAPI = (PVOGETFLOENGINEAPI) m_libOP.GetAddress(m_libOP.pUserData, m_libFLOEngine, ("voGetFLOEngineAPI"), 0);
	
	JNILOGE ("before load voFLOEngine API");
	if (pAPI == NULL)
		return OMX_ErrorUndefined;

	pAPI (&m_FLOEngineAPI);

	m_FLOEngineAPI.Create(&m_hFLOEngine);

	JNILOGE ("Create voFLOEngine Success");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE COMXQualcommDataSourcePlayer::DestroyFLOEngine()
{
	if(m_hFLOEngine != NULL)
	{
		m_FLOEngineAPI.Destroy(m_hFLOEngine);
		m_hFLOEngine = NULL;
	}

	if(m_libFLOEngine != NULL)
	{
		m_libOP.FreeLib(m_libOP.pUserData, m_libFLOEngine, 0);
		m_libFLOEngine = NULL;
	}

	return OMX_ErrorNone;
}

VO_S32 COMXQualcommDataSourcePlayer::OnEvent(int nEventType, unsigned int nParam, void * pData)
{
	JNILOGE("OnEvent");
	if(g_pDataSourcePlayer == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	return 0;
}

VO_S32 COMXQualcommDataSourcePlayer::OnFrame(VO_FLOENGINE_FRAME * pFrame)
{
	//JNILOGD("OnFrame");
	if(g_pDataSourcePlayer == NULL)
		return VO_ERR_FLOENGINE_NULLPOINTER;

	OMX_BUFFERHEADERTYPE bufHead;
	memset (&bufHead, 0, sizeof (OMX_BUFFERHEADERTYPE));
	bufHead.nSize = sizeof (OMX_BUFFERHEADERTYPE);
	bufHead.nAllocLen = pFrame->nSize;

	if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_AUDIO)
		bufHead.nOutputPortIndex = 0; // 0 Audio  1 Video
	else if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_VIDEO)
		bufHead.nOutputPortIndex = 1; // 0 Audio  1 Video

	bufHead.nTickCount = 1;

	bufHead.nFilledLen = pFrame->nSize;
	bufHead.pBuffer = pFrame->pData;
	bufHead.nTimeStamp = pFrame->nStartTime;

	OMX_ERRORTYPE errType = OMX_ErrorUndefined;
	while(errType != OMX_ErrorNone)
	{
		errType = g_pDataSourcePlayer->m_pCompSrc->SetParameter(g_pDataSourcePlayer->m_pCompSrc, (OMX_INDEXTYPE)OMX_VO_IndexSendBuffer, &bufHead);
		
		if(errType != OMX_ErrorNone)
		{
			JNILOGE2("ERROR %d", errType);
			voOMXOS_Sleep(1000);
    }
   
   /*
   
   if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_AUDIO)
		{
			JNILOGD2("Audio: StartTime = %llu    Size = %d \n", pFrame->nStartTime, pFrame->nSize);
		}
		else if(pFrame->nFrameType == VO_FLOENGINE_FRAMETYPE_VIDEO)
		{
			JNILOGD2("Video: StartTime = %llu    Size = %d  \n", pFrame->nStartTime, pFrame->nSize);
		}
		*/
	}

	return VO_ERR_FLOENGINE_OK;
}


OMX_PTR COMXQualcommDataSourcePlayer::OMX_mmLoadLib (OMX_PTR	pUserData, OMX_STRING pLibName, OMX_S32 nFlag)
{
	VOLOGI ("%s", pLibName);

	COMXQualcommDataSourcePlayer *	pGraph = (COMXQualcommDataSourcePlayer *)pUserData;
//	OMX_STRING			pWorkPath = pGraph->GetWorkPath ();;
//OMX_STRING			pWorkPath = "/data/data/com.visualon.vome/lib/";
OMX_STRING			pWorkPath = "/data/data/com.qualcomm.mflo.flotestplayer/lib/";
	OMX_PTR				hDll = NULL;

#ifdef _WIN32
	VO_TCHAR	szDll[MAX_PATH];
	VO_TCHAR	szPath[MAX_PATH];
	TCHAR		szLibName[256];

	memset (szLibName, 0, 256);
	::MultiByteToWideChar (CP_ACP, 0, pLibName, -1, szLibName, sizeof (szLibName));

	if (pWorkPath != NULL)
	{
		vostrcpy (szDll, (VO_TCHAR *)pWorkPath);
		vostrcat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
	{
		GetModuleFileName ((HMODULE)pGraph->m_hInstance, szPath, sizeof (szPath));

		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			vostrcpy (szDll, szPath);
			vostrcat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		::GetModuleFileName (NULL, szPath, sizeof (szPath));
		VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
		if(pPos != NULL)
		{
			*(pPos + 1) = 0;

			vostrcpy (szDll, szPath);
			vostrcat (szDll, szLibName);
			hDll = LoadLibrary (szDll);
		}
	}

	if (hDll == NULL)
	{
		vostrcpy (szDll, szPath);
		vostrcat (szDll, _T("Dll\\"));
		vostrcat (szDll, szLibName);
		hDll = LoadLibrary (szDll);
	}

	if (hDll == NULL)
		hDll = LoadLibrary (szLibName);

#elif defined _LINUX_X86
	VO_TCHAR libname[256];
	VO_TCHAR szPath[256];

	vostrcpy(libname, pLibName);
	voOS_GetAppFolder(szPath, sizeof(szPath));
	vostrcat(szPath, libname);
	hDll = dlopen (szPath, RTLD_NOW);
	if (hDll == NULL)
	{
		voOS_GetAppFolder(szPath, sizeof(szPath));
		vostrcat(szPath, "lib/");
		vostrcat(szPath, libname);
		hDll = dlopen (szPath, RTLD_NOW);
	}

#elif defined _LINUX_ANDROID
	VO_TCHAR libname[256];
	VO_TCHAR szLibPath[256];

	vostrcpy(libname, pLibName);

	vostrcpy(szLibPath, "/data/local/voOMXPlayer/lib/");
	vostrcat(szLibPath, libname);
	hDll = dlopen (szLibPath, RTLD_NOW);
	if (hDll == NULL)
	{
		vostrcpy(szLibPath, "/data/local/voOMXPlayer/lib/lib");
		vostrcat(szLibPath, libname);
		hDll = dlopen (szLibPath, RTLD_NOW);
	}

	if (hDll == NULL && pWorkPath != NULL)
	{
		vostrcpy (szLibPath, pWorkPath);
		vostrcat (szLibPath, libname);
		hDll = dlopen (szLibPath, RTLD_NOW);

		if (hDll == NULL)
		{
			vostrcpy (szLibPath, pWorkPath);
			vostrcat(szLibPath, "lib");
			vostrcat (szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_NOW);
		}

		if (hDll == NULL)
		{
			vostrcpy (szLibPath, pWorkPath);
			vostrcat (szLibPath, libname);
			vostrcat(szLibPath, "lib/");
			hDll = dlopen (szLibPath, RTLD_NOW);
		}

		if (hDll == NULL)
		{
			vostrcpy (szLibPath, pWorkPath);
			vostrcat(szLibPath, "lib/lib");
			vostrcat (szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_NOW);
		}
	}

	if (hDll == NULL)
	{
		vostrcpy (szLibPath, libname);
		hDll = dlopen (szLibPath, RTLD_NOW);
		if (hDll == NULL)
		{
			vostrcpy(szLibPath, "lib");
			vostrcat(szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_NOW);
		}
	}
	if (hDll == NULL)
	{
		vostrcpy(szLibPath, "/data/data/com.visualon.vome/lib/");
		vostrcat(szLibPath, libname);
		hDll = dlopen (szLibPath, RTLD_NOW);
		if (hDll == NULL)
		{
			vostrcpy(szLibPath, "/data/data/com.visualon.vome/lib/lib");
			vostrcat(szLibPath, libname);
			hDll = dlopen (szLibPath, RTLD_NOW);
		}
	}

#endif // _WIN32

	return hDll;
}

OMX_PTR COMXQualcommDataSourcePlayer::OMX_mmGetAddress (OMX_PTR	pUserData, OMX_PTR hLib, OMX_STRING pFuncName, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return NULL;

	VOLOGI ("%s", pFuncName);

	OMX_PTR pFunc = NULL;

#ifdef _LINUX
	pFunc = dlsym (hLib, pFuncName);
#endif // _LINUX

#ifdef _WIN32
#ifdef _WIN32_WCE
	TCHAR szFuncName[128];
	memset (szFuncName, 0, 256);
	::MultiByteToWideChar (CP_ACP, 0, pFuncName, -1, szFuncName, sizeof (szFuncName));

	pFunc = GetProcAddress ((HMODULE)hLib, szFuncName);
#else
	pFunc = GetProcAddress ((HMODULE)hLib, pFuncName);
#endif //_WIN32_WCE
#endif // _WIN32

	return pFunc;
}

OMX_S32 COMXQualcommDataSourcePlayer::OMX_mmFreeLib (OMX_PTR	pUserData, OMX_PTR hLib, OMX_S32 nFlag)
{
	if (hLib == NULL)
		return -1;

#ifdef _LINUX
	dlclose (hLib);
#endif // _LINUX

#ifdef _WIN32
	FreeLibrary ((HMODULE)hLib);
#endif // _WIN32

	return 0;
}