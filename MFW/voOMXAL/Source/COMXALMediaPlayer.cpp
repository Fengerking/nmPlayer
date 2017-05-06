/*
 *  COMXALMediaPlayer.cpp
 *
 *  Created by Lin Jun on 19/03/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "COMXALMediaPlayer.h"
#include "COMXALWrapper.h"
#include "voOSFunc.h"
#include <string.h>
#include "voLog.h"
#include "voSink.h"
#include "cmnMemory.h"
#include "fVideoHeadDataInfo.h"
#include "voHalInfo.h"

#include <jni.h>
#include "CJniEnvUtil.h"

//#define _VO_OMXAL_LOG_EVERY_FRAME

COMXALMediaPlayer::COMXALMediaPlayer(VONP_LIB_FUNC* pLibOP)
  :m_pWrap(NULL)
  ,m_pReadBufFunc(NULL)
  ,m_pMuxStream(NULL)
  ,m_hEventNotifyThread(NULL)
  ,m_hReadBufThread(NULL)
  ,m_bStopReadThread(true)
  ,m_bPauseReadBuf(false)
  ,m_nOpenFlag(VONP_FLAG_SOURCE_READBUFFER)
  ,m_pLibOP(pLibOP)
  ,m_nPlayerStatus(VONP_STATUS_INIT)
  ,m_bSeeking(false)
  ,m_bEnableAudioEffect(false)
  ,m_bWaitKeyFrame(true)
  ,m_nStatusEOS(0)
  ,m_pNotifyEvent(NULL)
  ,m_pAudioDecoder(NULL)
  ,m_pAudioEffect(NULL)
  ,m_nMaxPCMOutputBufSize(512*1024)
  ,m_pJavaEnv(NULL)
  ,m_pJavaSurface(NULL)
  ,m_pNativeWindow(NULL)
  ,m_bFormatChanged(false)
  ,m_bFirstRun(true)
  ,m_nLastReadVideoTime(0)
  ,m_nLastReadAudioTime(0)
  ,m_nPlayingTime(0)
  ,m_nPosOffsetTime(0)
  ,m_nNewFormatStartTime(0)
  ,m_nLastPosUpdateTime(0)
  ,m_bAudioOnly(false)
  ,m_bVideoOnly(false)
  ,m_bBuffering(true)
  ,m_bSeeked(false)
  ,m_bNeedFlush(false)
  ,m_bForceReadAudioFirst(true)
  ,m_pLogPrintCallback(NULL)
  ,m_bSupportAudioCodec(true)
  ,m_bSupportVideoCodec(true)
  ,m_bHasBeenAudioOnly(false)
  ,m_nSeekPos(0)
  ,m_bGotNewFormatBuf(false)
  ,m_bReadFlushBuffer(false)
  ,m_nTryReadBufCount(0)
  ,m_nLastBATime(-1)
  ,m_nLastBAVideoTime(-1)
  ,m_nLastAudioNewFormatTime(0)
  ,m_bViewChanged(false)
  ,m_bReadAudioNewFormat(false)
  ,m_bReadVideoNewFormat(false)
  ,m_nForceReadAudioNewFmt(0)
  ,m_nForceReadVideoNewFmt(0)
  ,m_bCancelSendRawFrame(false)
  ,m_nPlayingTimeViewChanged(0)
  ,m_bGetFirstTimeAfterViewChanged(false)
  ,m_bStopEventNotifyThread(false)
  ,m_bOpened(false)
  ,m_bUseOutsideRender(false)
  ,m_nAudioStepTime(200)
  ,m_nAudioStepSize(44100*4)
  ,m_pAudioOutBuff(NULL)
  ,m_nAudioOutSize(0)
  ,m_nAudioBufferSize(44100*4)
  ,m_pAudioPcmBuff(NULL)
  ,m_bSeekWhenPaused(false)
  ,m_bViewAvailable(true)
  ,m_pFrameTools(NULL)
  ,m_nAspectRatio(VONP_RATIO_00)
  ,m_bAudioStatusOK(false)
  ,m_bViewChangeing(false)
#ifdef _VODEBUG
  ,m_bPlayTsFile(false)
  ,m_nFileSuffix(0)
  ,m_hPlayTsFile(NULL)
#endif	// _VODEBUG
	,m_nPlayingTimeOffset(0)
	,m_bForceReadVideoNewFmt(false)
	,m_bForceReadAudioNewFmt(false)
	,m_bWaitFirstAudioFrame(true)
	,m_nLastVideoNewFormatTime(0)
	,m_bForcePeekBuf(true)
	,m_nAppStatus(APP_NORMAL)
	,m_nLastWrapReadDataTime(-1)
	,m_pVideoSplit(NULL)
	,lastRunTime(0)
	,m_bNeedFormatChange(false)
{	
  memset(m_szURL, 0, 1024);

  m_ReadBufQueryFromOMXAL.pUserData	= this;
  m_ReadBufQueryFromOMXAL.pRead		= ReadBufQueryFromOMXAL;

  m_EventCallbackFromOMXAL.pUserData			= this;
  m_EventCallbackFromOMXAL.pEvenetCallback	= EventCallbackFromOMXAL;

  memset(&m_AudioDecOutputBuf, 0, sizeof(VO_CODECBUFFER));

  m_AudioFormat.nSampleRate	= 44100;
  m_AudioFormat.nChannels		= 2;
  m_AudioFormat.nSampleBits	= 16;

  memset(&m_AudioTrackInfo, 0, sizeof(VONP_BUFFER_FORMAT));
  memset(&m_VideoTrackInfo, 0, sizeof(VONP_BUFFER_FORMAT));

  m_AudioTrackInfo.nStreamType	= VONP_SS_Audio;
  m_AudioTrackInfo.nCodec			= VONP_AUDIO_CodingAAC;
  m_VideoTrackInfo.nStreamType	= VONP_SS_Video;
  m_VideoTrackInfo.nCodec			= VONP_VIDEO_CodingH264;

  m_sCPUInfo.nCoreCount = 2;
  m_sCPUInfo.nCPUType       = 1;
  m_sCPUInfo.nFrequency = 1200000;
  m_sCPUInfo.llReserved = 200000;

  CheckCPUInfo();

  memset(&m_fmtVideo, 0, sizeof(VONP_VIDEO_FORMAT));

  m_MemOp.Alloc = cmnMemAlloc;
  m_MemOp.Free = cmnMemFree;
  m_MemOp.Set = cmnMemSet;
  m_MemOp.Copy = cmnMemCopy;
  m_MemOp.Check = cmnMemCheck;
  m_MemOp.Compare = cmnMemCompare;
  m_MemOp.Move = cmnMemMove;

  memset(m_pPlayerPath, 0, sizeof(VO_TCHAR) * 1024);
#ifdef _VODEBUG
  FILE * hCheck = fopen("/sdcard/voomxal/playomxalenqueue.txt", "rb");
  if(hCheck)
  {
    m_bPlayTsFile = true;
    fclose(hCheck);
  }
#endif	// _VODEBUG
    	
    memset(&m_peekBufInfo, 0 ,sizeof(PEEK_BUFFER_INFO));
    ResetPeekBuffer();
    m_peekBufInfo.bEnable = true;

	memset(&m_pSplitBuf, 0, sizeof(VONP_BUFFERTYPE));
	memset(m_sAndroidVersion, 0, 64);
	__system_property_get ("ro.build.version.release", m_sAndroidVersion);
    
	VOLOGI("[OMXAL]+");
}

COMXALMediaPlayer::~COMXALMediaPlayer(void)
{
  VOLOGI("[OMXAL]-");
  m_pLibOP = NULL;
  Uninit();
}

int COMXALMediaPlayer::Init()
{
  VOLOGI("[OMXAL]");

  Uninit();

  CreateFrameTools();

  m_pWrap = new COMXALWrapper(&m_EventCallbackFromOMXAL);

  if(!m_pWrap)
    return VONP_ERR_OutMemory;

  int nRet = m_pWrap->Init();

  InitVideoSplit();
//  nRet = CreateMemStream();

  if(m_bEnableAudioEffect)
  {
    CreateAudioDecoder(VO_AUDIO_CodingAAC, &m_AudioFormat);
    CreateAuidoEffect();
  }

  CreatePCMBuffer();

  return nRet;
}

void COMXALMediaPlayer::InitVideoSplit()
{
  if(strstr(m_sAndroidVersion, "4.3") == m_sAndroidVersion)
  {
  	if(m_pVideoSplit)
		delete m_pVideoSplit;
	m_pVideoSplit = NULL;
  	m_pVideoSplit = new CVideoFrameSplit;
  }
  else
	m_pVideoSplit = NULL;
}

int COMXALMediaPlayer::Uninit()
{
  VOLOGI("[OMXAL]+COMXALMediaPlayer::Uninit()");

  DestroyReadThread();
  DestroyOMXAL();
  releaseNativeWindow();
  DestroyMemStream();
  DestroyAudioDecoder();
  DestroyAudioEffect();
  DestroyFrameTools();

  if(m_VideoTrackInfo.pHeadData)
  {
    delete []((unsigned char*)m_VideoTrackInfo.pHeadData);
    m_VideoTrackInfo.pHeadData		= NULL;
    m_VideoTrackInfo.nHeadDataSize	= 0;
  }

  if(m_AudioTrackInfo.pHeadData)
  {
    delete []((unsigned char*)m_AudioTrackInfo.pHeadData);
    m_AudioTrackInfo.pHeadData		= NULL;
    m_AudioTrackInfo.nHeadDataSize	= 0;
  }

  if(m_pVideoSplit)
	  delete m_pVideoSplit;
  m_pVideoSplit = NULL;

  if(NULL != m_pSplitBuf.pBuffer)
	  delete []((unsigned char*)m_pSplitBuf.pBuffer);
  m_pSplitBuf.pBuffer = NULL;

  DestroyPCMBuffer();

  VOLOGI("[OMXAL]-COMXALMediaPlayer::Uninit()");

  return VONP_ERR_None;
}

int COMXALMediaPlayer::SetView(void * pView)
{
  VOLOGI("[OMXAL]new %x old %x", pView, m_pJavaSurface);

  if(!pView)
  {
    m_bViewAvailable = false;

    VOLOGW("[OMXAL]App maybe run on backgroud, playing time %d, sys %d", GetPlayingTime(), voOS_GetSysTime());
    return VONP_ERR_Pointer;
  }

  //if(pView != m_pJavaSurface || m_pNativeWindow == NULL)
  if(m_pJavaSurface == NULL || m_pNativeWindow == NULL)
  {
    m_pJavaSurface = pView;
    releaseNativeWindow();
    getNativeWindow();
    return VONP_ERR_None;
  }
  if( ProcessViewChanged(pView))
    return VONP_ERR_None;

  return VONP_ERR_Implement;
}

int COMXALMediaPlayer::Open(void* pSource, int nFlag)
{	
  VOLOGI("[OMXAL]");

  memset(&m_fmtVideo, 0, sizeof(VONP_VIDEO_FORMAT));

    m_nSeekPos              = 0;
    m_bSeeked               = false;
    m_bEOSed                = false;
    
    m_nLastPosUpdateTime    = 0;
    
    m_bForcePeekBuf         = true;

  m_nPlayerStatus	= VONP_STATUS_INIT;
  m_nOpenFlag		= nFlag;
  m_bAudioOnly	= false;
  m_bVideoOnly	= false;
  m_bGotNewFormatBuf = false;
  m_bReadFlushBuffer = false;
  m_nTryReadBufCount = 0;
  m_bViewChanged     = false;
  m_bReadAudioNewFormat = false;
  m_bReadVideoNewFormat = false;
  m_nForceReadAudioNewFmt = 0;
  m_nForceReadVideoNewFmt = 0;
  m_bCancelSendRawFrame = false;
  m_nPlayingTimeViewChanged = 0;
  m_bGetFirstTimeAfterViewChanged = false;
  m_bFirstRun					  = true;
  m_bOpened                       = true;
  m_bUseOutsideRender             = false;
  m_nPlayingTime                  = 0;
  m_bBuffering 					  = true;

  if(nFlag == VONP_FLAG_SOURCE_URL)
  {
    if(pSource)
    {
      memset(m_szURL, 0, 1024);
      strcpy(m_szURL, (char*)pSource);
      VOLOGI("URL -> %s", m_szURL);
    }
    else
    {
      VOLOGI("[OMXAL]URL is NULL");
    }
  }
  else if(nFlag == VONP_FLAG_SOURCE_READBUFFER)
  {
    m_pReadBufFunc	= (VONP_READBUFFER_FUNC*)pSource;
  }
  else if(nFlag == VONP_FLAG_SOURCE_SENDBUFFER)
  {
  }
  else 
  {
    return VONP_ERR_Unknown;
  }

  CreateEventNotifyThread();

  return VONP_ERR_None;
}

int COMXALMediaPlayer::Close()
{
  VOLOGI("[OMXAL]");

  if(!m_pWrap)
    return VONP_ERR_Pointer;

  Stop();

  int nRet = m_pWrap->Close();

  return nRet;	
}

int COMXALMediaPlayer::Run()
{
  VOLOGI("[OMXAL]Run, %d, build time:%s  %s", voOS_GetSysTime(), __TIME__,  __DATE__);

  int nRet            = VONP_ERR_None;

  if(IsRunning())
    //return VONP_ERR_Status;
    return VONP_ERR_None;

  voCAutoLock lock(&m_mtStatus);

  CreateEventNotifyThread();

  //to handle the case:call Stop,then call Run
  if(!m_pWrap)
  {
    VOLOGI("[OMXAL]create OMXAL");
    CreateOMXAL();
  }	

  if(!m_pMuxStream)
  {
    VOLOGI("[OMXAL]create mux stream");
    nRet = CreateMemStream();
    if(VONP_ERR_None != nRet)
      return nRet;
  }


  if(IsPause())
  {
    VOLOGI("[OMXAL]Run after pause");

    if(m_bSeeked)
    {
      VOLOGI("[OMXAL]+Flush mem stream, %d", m_bViewChanged?1:0);
      if(m_pMuxStream && !m_bViewChanged)
        m_pMuxStream->Flush();
      VOLOGI("[OMXAL]-Flush mem stream");

      //begin to read buffer
      m_nStatusEOS			= 0;
      m_bWaitKeyFrame			= true;
      m_nLastReadVideoTime	= 0;
      m_nLastReadAudioTime	= 0;
      m_nTryReadBufCount      = 0;
      m_bSeeking				= false;
      m_bForceReadAudioFirst	= true;
      m_nLastPosUpdateTime    = 0;
      NotifyBuffering(true);

      VOLOGI("[OMXAL]++++++Begin to read buffer...");
      m_bPauseReadBuf			= false;
      m_nPlayerStatus = VONP_STATUS_RUNNING;
    }
    else
    {
      VONP_BUFFERTYPE buf;

      if(GetPlayingTime() > 0)
      {
        bool bReadAudio = true;

        int nRead = PeekFlushBuffer(&buf, bReadAudio);
        VOLOGI("[OMXAL]Finish peek flush buffer, return %d", nRead);

        // get flush buffer
        if(nRead == 0 && buf.pBuffer && buf.nSize>0)
        {
          SendRawFrame(m_bAudioOnly?true:bReadAudio, &buf);
        }	               
      }

      if(m_bViewChanged)
      {
        SendAudioHeadData(buf.llTime);
        SendVideoHeadData(buf.llTime);
      }

      m_bViewChanged  = false;
      m_bPauseReadBuf	= false;
      m_nPlayerStatus = VONP_STATUS_RUNNING;
	  if(NULL != m_pWrap)
		  nRet = m_pWrap->Run();
    }
  }
  else
  {
    VOLOGI("[OMXAL]Run normally");

    m_bViewChanged       = false;
    m_bPauseReadBuf		 = false;
    m_nPosOffsetTime	 = 0;
    m_nNewFormatStartTime= 0;
    //m_nPlayingTime		 = 0;
    m_nLastReadVideoTime = 0;
    m_nLastReadAudioTime = 0;
    m_nLastPosUpdateTime = 0;
    m_bSeeking = false;
    m_bForceReadAudioFirst = true;
    m_nStatusEOS	     = 0;

    if(m_pMuxStream)
      nRet = m_pMuxStream->Run();

    if(m_nOpenFlag != VONP_FLAG_SOURCE_SENDBUFFER)
      CreateReadThread();

    if(m_pWrap)
      nRet = m_pWrap->Open(&m_ReadBufQueryFromOMXAL, m_pNativeWindow);

    if(nRet != VONP_ERR_None)
    {
      VOLOGE("[OMXAL]OMX AL open failed!!!");
      return nRet;
    }

    //must set VONP_STATUS_RUNNING before  m_pWrap->Run(),because run will read buffer from ts stream
    m_nPlayerStatus = VONP_STATUS_RUNNING;
    if(m_pWrap)
      nRet = m_pWrap->Run();		
  }

  m_bEOSed = false;

  VOLOGI("[OMXAL]Run ret %d", nRet);
  return nRet;
}

int COMXALMediaPlayer::Pause()
{
  VOLOGI("[OMXAL]+Pause %x", m_pWrap);

  voCAutoLock lock(&m_mtStatus);

  if(IsPause() || IsStop())
  {
    VOLOGI("[OMXAL]-Pause 01");
    return VONP_ERR_Status;
  }

  //pause before seek not finished,it's same as seek when pause
  m_bSeekWhenPaused   = m_bSeeked;

  m_bPauseReadBuf	= true;
  m_nPlayerStatus = VONP_STATUS_PAUSED;
  int nRet = VONP_ERR_None;

  if(m_pWrap)
    nRet = m_pWrap->Pause();

  VOLOGI("[OMXAL]-Pause 02");

  return nRet;	
}

int COMXALMediaPlayer::Stop()
{
  VOLOGI("[OMXAL]Stop, %d", voOS_GetSysTime());

  if(!m_pWrap)
    return VONP_ERR_Pointer;

  int nRet        = VONP_ERR_Unknown;
  m_bSeeking		= false;
  m_nPlayerStatus = VONP_STATUS_STOPPED;

  NotifyBuffering(false);

  ResetPeekBuffer();
  DestroyBAInfo();
  DestroyMemStream();
  DestroyReadThread();
  DestroyOMXAL();
  DestroyEventNotifyThread();

  m_nLastReadVideoTime    = 0;
  m_nLastReadAudioTime    = 0;
  m_nPlayingTime          = 0;
  m_nPosOffsetTime        = 0;
  m_nNewFormatStartTime   = 0;
  m_bAudioOnly            = false;
  m_bVideoOnly            = false;
  m_bSeeked               = false;
  m_nTryReadBufCount      = 0;
  m_bWaitKeyFrame         = true;
  m_bReadAudioNewFormat   = false;
  m_bReadVideoNewFormat   = false;
  m_nForceReadAudioNewFmt = 0;
  m_nForceReadVideoNewFmt = 0;
  m_bCancelSendRawFrame   = false;
  m_nAspectRatio          = VONP_RATIO_00;
  m_bAudioStatusOK 		  = false;
  m_bNeedFormatChange 	  = false;
  lastRunTime			  = 0;
  m_nLastWrapReadDataTime = 0;
  m_bFirstRun			  = true;

  memset(&m_fmtVideo, 0, sizeof(VONP_VIDEO_FORMAT));

  return nRet;
}

int COMXALMediaPlayer::GetPos()
{
  if(!m_pWrap)
    return -1;

  return GetPlayingTime();
}

int COMXALMediaPlayer::GetPlayingTime()
{
  // 	VOLOGI("[OMXAL]Get Playing Time seek flag %d pos %d, view flag %d pos %d, pos %d, offset %d", 
  // 		m_bSeeked ? 1 : 0, m_nSeekPos, m_bViewChanged ? 1 : 0, m_nPlayingTimeViewChanged, m_nPlayingTime, m_nPosOffsetTime);

  if(m_bSeeked)
    return m_nSeekPos;
  if(m_bViewChanged)
    return m_nPlayingTimeViewChanged;

  return m_nPlayingTime;
}

int COMXALMediaPlayer::SetPos(int nPos)
{
  VOLOGI("+[OMXAL]Set pos %d, %02d:%02d:%02d, systime %d", nPos, nPos/1000/3600, ((nPos/1000)%3600)/60, (((nPos/1000)%3600)%60), voOS_GetSysTime());

  if(false == m_bVideoOnly)
    m_bAudioStatusOK = false;

  m_nStatusEOS = 0;

  int nRet            = VONP_ERR_None;

  m_bSeekWhenPaused   = IsPause();
  bool bIsRunning     = IsRunning();
  m_bAudioOnly = false;
  m_bVideoOnly = false;

  m_bUseOutsideRender = false;
  m_nAudioOutSize = 0;
  m_nSeekPos		= nPos;
  m_bSeeked		= true;
  m_bNeedFlush	= true;

  //pause read buffer
  m_bPauseReadBuf	= true;
  m_bSeeking		= true;

  //wait pause read buffer thread
#ifdef _VOLOG_INFO
  int nTest = 0;
  nTest = voOS_GetSysTime();
#endif
  m_semSeek.Wait(5000);
#ifdef _VOLOG_INFO
  VOLOGI("[OMXAL]Seek pause read return..., %d", (int)(voOS_GetSysTime() - nTest));
#endif

  //
  voCAutoLock lock(&m_mtStatus);

  if(m_nStatusEOS&0x01 || m_nStatusEOS&0x02)
  {
    VOLOGW("[OMXAL]Play complete by source EOS");

    // to fix loop playback issue,it will continue to seek to 0 after EOS
    m_nPlayerStatus = VONP_STATUS_RUNNING;

    SendEOS();
    return VONP_ERR_None;
  }

  Flush();

  m_bGotNewFormatBuf	= false;
  m_nLastAudioNewFormatTime = 0;
  m_bReadAudioNewFormat = false;
  m_bReadVideoNewFormat = false;
  m_nForceReadAudioNewFmt = 0;
  m_nForceReadVideoNewFmt = 0;
  m_bNeedFormatChange = false;

  if(bIsRunning)
  {
    VOLOGI("[OMXAL]Restart playback");
    Run();
  }

  VOLOGI("-[OMXAL]Set pos nRet 0X%08X", nRet);
  return nRet;	
}

int COMXALMediaPlayer::SendEvent(int nEventID, void* pParam1, void*pParam2)
{
  int nRet = VONP_ERR_Pointer;

  if(m_pNotifyEvent && m_pNotifyEvent->pUserData)
    nRet = m_pNotifyEvent->pListener(m_pNotifyEvent->pUserData, nEventID, pParam1, pParam2);

  return nRet;
}

void COMXALMediaPlayer::Flush()
{
  voCAutoLock lock(&m_mtStatus);

  ResetPeekBuffer();
  DestroyBAInfo();

  if(m_pFrameTools)
	  m_pFrameTools->ResetFrameCount();
  if(m_pMuxStream)
    m_pMuxStream->Flush();
  if(m_pWrap)
    m_pWrap->Flush();
  if(m_pVideoSplit)
  	m_pVideoSplit->ResetContext();
}

int COMXALMediaPlayer::doSendDataBuf(int nSSType, VONP_BUFFERTYPE* pBuffer)
{
  //	VOLOGI("send data buffer, type %d, size %d, time %lld", nSSType, pBuffer->nSize, pBuffer->llTime);

  if(nSSType == VONP_SS_TS)
    return m_pMuxStream->SendTsStream(pBuffer);

  VO_SINK_SAMPLE sample;
  memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
  sample.Buffer	= pBuffer->pBuffer;
  sample.Size		= pBuffer->nSize;
  sample.nAV		= 0;
  sample.Time		= pBuffer->llTime;
  sample.DTS		= -1;

  if(nSSType == VONP_SS_Audio)
    sample.nAV		= 0;
  else if(nSSType == VONP_SS_Video)
    sample.nAV		= 1;

  int nRet = VONP_ERR_Retry;
  while (nRet == VONP_ERR_Retry)
  {
    if(IsStop())
      return VONP_ERR_None;

    if(m_pMuxStream)
      nRet = m_pMuxStream->SendRawFrame(&sample);

    if(nRet == VONP_ERR_Retry)
      voOS_Sleep(2);
  }

  return nRet;
}

int COMXALMediaPlayer::SendDataBuf(void* pUserData, int nSSType, VONP_BUFFERTYPE* pBuffer)
{
  COMXALMediaPlayer* pPlayer = (COMXALMediaPlayer*)pUserData;
  if(!pPlayer)
    return VONP_ERR_Pointer;

  return pPlayer->doSendDataBuf(nSSType, pBuffer);
}

int COMXALMediaPlayer::GetParam(int nParamID, void* pValue)
{
  if(VONP_PID_AUDIO_VOLUME == nParamID)
  {
  }
  else if(VONP_PID_STATUS == nParamID)
  {
    *(int*)pValue = m_nPlayerStatus;
    return VONP_ERR_None;
  }
  else if(VONP_PID_SENDBUFFERFUNC == nParamID)
  {
    VONP_SENDBUFFER_FUNC* pFunc = (VONP_SENDBUFFER_FUNC*)pValue; 
    pFunc->pUserData	= this;
    pFunc->SendData		= SendDataBuf;
    return VONP_ERR_None;
  }
  else if(VONP_PID_CPU_INFO == nParamID)
  {
    VONP_CPU_INFO *pCPUInfo = (VONP_CPU_INFO *)pValue;
    if(pCPUInfo)
    {
      pCPUInfo->nCoreCount	= m_sCPUInfo.nCoreCount;
      pCPUInfo->nCPUType		= m_sCPUInfo.nCPUType;
      pCPUInfo->nFrequency	= m_sCPUInfo.nFrequency;
      pCPUInfo->llReserved	= m_sCPUInfo.llReserved;
      return VONP_ERR_None;
    }
    else
    {
      return VONP_ERR_Pointer;
    }
  }
  else if(VONP_PID_AUDIO_BUFFER == nParamID)
  {
    return GetAudioBuffer((VONP_BUFFERTYPE**)pValue);
  }
  else if(VONP_PID_VIDEO_BUFFER == nParamID)
  {
    return GetVideoBuffer((VONP_BUFFERTYPE**)pValue);
  }
  else if(VONP_PID_AUDIO_FORMAT == nParamID)
  {
    if(!pValue)
      return VONP_ERR_Pointer;
    VOLOGI("[OMXAL]Output audio format:%d %d %d", m_AudioFormat.nSampleRate, m_AudioFormat.nChannels, m_AudioFormat.nSampleBits);
    VONP_AUDIO_FORMAT* pFmt  = (VONP_AUDIO_FORMAT*)pValue;
    pFmt->nSampleRate           = m_AudioFormat.nSampleRate;
    pFmt->nChannels             = m_AudioFormat.nChannels;
    pFmt->nSampleBits           = m_AudioFormat.nSampleBits;
    return VONP_ERR_None;
  }
  else
  {
    VOLOGW("[OMXAL]++++++++++++++Get Param ID(%d) not implement!!!+++++++++++++++++++", nParamID);
  }	

  return VONP_ERR_Implement;
}

int COMXALMediaPlayer::SetParam(int nParamID, void* pValue)
{
  if(VONP_PID_FUNC_LIB == nParamID)
  {
    m_pLibOP = (VONP_LIB_FUNC*)pValue;
    VOLOGI("[OMXAL]m_pLibOP %x", (int)m_pLibOP);
    return VONP_ERR_None;
  }
  else if(VONP_PID_LISTENER == nParamID)
  {
    VOLOGI("[OMXAL]Set event callback %x", (unsigned int)pValue);
    m_pNotifyEvent = (VONP_LISTENERINFO*)pValue;
    return VONP_ERR_None;
  }
  else if(VONP_PID_JAVAENV == nParamID)
  {
    VOLOGI("[OMXAL]Set Java Env %x", (unsigned int)pValue);
    m_pJavaEnv = pValue;
    return VONP_ERR_None;
  }
  else if(VONP_PID_AUDIO_VOLUME == nParamID)
  {
  }
  else if(VONP_PID_STATUS == nParamID)
  {
  }
  //else if(VO_PID_COMMON_LOGFUNC == nParamID)
  else if(VONP_PID_COMMON_LOGFUNC == nParamID)
  {
    m_pLogPrintCallback = (VO_LOG_PRINT_CB *)pValue;
    if(m_pLogPrintCallback)
    {
      VOLOGINIT (m_pPlayerPath);
      VOLOGI("[OMXAL]Set log func %x", (unsigned int)pValue);
    }
    return VONP_ERR_None;
  }
  else if(VONP_PID_VIEW_ACTIVE == nParamID || VONP_PID_APPLICATION_RESUME == nParamID)
  {
    VOLOGI("[OMXAL]View actived %x", pValue);
    if(m_pJavaSurface)// && m_pJavaSurface != pValue)
    {
      VOLOGW("[OMXAL]Surface view changed!!!old %x, new %x", m_pJavaSurface, pValue);
      ProcessViewChanged(pValue);
    }
    return VONP_ERR_None;
  }
  else if(VONP_PID_PLAYER_PATH == nParamID)
  {
    if(NULL != pValue)
    {
      strcpy(m_pPlayerPath, (VO_TCHAR*)pValue);
    }
    return VONP_ERR_None;
  }
  else
  {
    VOLOGW("[OMXAL]++++++++++++++Set Param ID(%x) not implement!!!+++++++++++++++++++", nParamID);
  }

  return VONP_ERR_Implement;
}

int COMXALMediaPlayer::doEventCallbackFromOMXAL(int nEventID, void* pPram)
{
  if(nEventID == OMXAL_EVENT_Seek_Complete)
  {
    VOLOGI("[OMXAL]OMX AL seek complete event");
  }
  else if(nEventID == OMXAL_EVENT_Current_Position)
  {
	//VOLOGI("[OMXAL]OMXAL_EVENT_Current_Position %d pPram %d Diff : %d", m_nPlayingTime, *((int *)pPram), *((int *)pPram) - m_nPlayingTime);
    m_nPlayingTime			= *((int *)pPram);
    m_nLastPosUpdateTime	= voOS_GetSysTime();
  }
  else if(OMXAL_EVENT_VideoRender_Start == nEventID)
  {
	  SendEvent(VONP_CB_VideoRenderStart, NULL, NULL);
  }

  return VONP_ERR_None;	
}

int	COMXALMediaPlayer::EventCallbackFromOMXAL(void* pUserData, int nEventID, void* pPram)
{	
  COMXALMediaPlayer* pPlayer = (COMXALMediaPlayer*)pUserData;

  if(pPlayer)
    return pPlayer->doEventCallbackFromOMXAL(nEventID, pPram);

  return VONP_ERR_Pointer;
}

int	COMXALMediaPlayer::doReadBufQueryFromOMXAL(char* pBuffer, int* pWantReadSize, int nReadType)
{
#ifdef _VODEBUG
  if(m_bPlayTsFile)
  {
    if(m_nFileSuffix == -1)
      return VONP_ERR_EOS;

    bool bStart = false;
    if(NULL == m_hPlayTsFile)
    {
      char szFile[256];
      sprintf(szFile, "/sdcard/voomxal/omxalenqueue_%d.ts", m_nFileSuffix);
      m_hPlayTsFile = fopen(szFile, "rb");

      if(NULL == m_hPlayTsFile)
      {
        VOLOGI("[OMXAL]create omxal enqueue ts file %d fail, so end", m_nFileSuffix);
        m_nFileSuffix = -1;

        return VONP_ERR_EOS;
      }

      bStart = true;
      VOLOGI("[OMXAL]play omxal enqueue ts file %d start...", m_nFileSuffix);
    }

    size_t nReaded = fread(pBuffer, 1, *pWantReadSize, m_hPlayTsFile);
    if(nReaded <= 0)
    {
      VOLOGI("[OMXAL]play omxal enqueue ts file %d finish!", m_nFileSuffix);
      m_nFileSuffix++;

      fclose(m_hPlayTsFile);
      m_hPlayTsFile = NULL;

      return VONP_ERR_FLush_Buffer;
    }

    *pWantReadSize = nReaded;
    return bStart ? VONP_ERR_Format_Changed : VONP_ERR_None;
  }
#endif	// _VODEBUG

  int nRet	= VONP_ERR_Retry;

  if(IsStop())
  {
    VOLOGI("[OMXAL]return read TS stream callback from OMX AL by stop %d,fmt changed %d", IsStop()?1:0, m_bFormatChanged?1:0);
    nRet = VONP_ERR_EOS;
    *pWantReadSize = 0;
    VOLOGI("[OMXAL]-doReadBufQueryFromOMXAL 02");
    return nRet;
  }

  if(!m_pMuxStream)
  {
    VOLOGW("[OMXAL]Mux stream instance is NULL");
    return VONP_ERR_Retry;
  }

  bool bInitRead = (nReadType == READ_TYPE_INIT);
  if(!bInitRead && m_bAudioOnly)
    *pWantReadSize = MPEG2_TS_PACKET_SIZE;

  int nWant	= *pWantReadSize;
  while (nRet == VONP_ERR_Retry)
  {
    if(bInitRead)
    {
      if(m_nStatusEOS&0x01 || m_nStatusEOS&0x02)
      {
        VOLOGW("[OMXAL]Play complete by read to end when init");
        m_bSeeked = false;
        SendEOS();
        nRet = VONP_ERR_EOS;
        *pWantReadSize = 0;
        VOLOGI("[OMXAL]-doReadBufQueryFromOMXAL 01");
        return nRet;
      }	
    }

    if(!IsSupportAudioCodec() || !IsSupportVideoCodec())
    {
      VOLOGE("[OMXAL]return read TS stream callback from OMX AL by codec not support");
      return VONP_ERR_Format_NotSupport;
    }		

    // remove CheckEOS,bug 15561, Local file-> Loop-> Play to end will hang
    //if(IsStop() || m_bFormatChanged)// || CheckEOS())
    if(IsStop())
    {
      VOLOGI("[OMXAL]return read TS stream callback from OMX AL by stop %d,fmt changed %d", IsStop()?1:0, m_bFormatChanged?1:0);
      nRet = VONP_ERR_EOS;
      //nRet = VONP_ERR_None;
      *pWantReadSize = 0;
      VOLOGI("[OMXAL]-doReadBufQueryFromOMXAL 00");
      return nRet;
    }

    if(IsPause())
    {
      voOS_Sleep(5);
      continue;
    }

    if(m_bNeedFlush)
    {
      VOLOGI("[OMXAL]need flush here");
      m_bNeedFlush = false;
      return VONP_ERR_FLush_Buffer;
    }

    nWant = *pWantReadSize;
    if(m_pMuxStream && IsRunning())
      nRet = m_pMuxStream->ReadMuxBuffer(pBuffer, &nWant);

	m_nLastWrapReadDataTime = voOS_GetSysTime();

    if(nRet == VONP_ERR_Retry || IsPause())
    {
      voOS_Sleep(5);
	  m_semBufQueue.Signal();
      if(m_bFormatChanged)
      {
        //VOLOGI("[OMXAL]Retry to read TS stream, nRet %x, readed size %d, sys %d", nRet, nWant, voOS_GetSysTime());
      }
    }
  }

#if 0
  //if(true == m_bViewChangeing && nRet == VONP_ERR_None)
  if(nRet == VONP_ERR_None)
  {
    static FILE * dump = fopen("/data/local/tmp/dump.ts", "w");
    if(NULL != dump)
    {
      fwrite(pBuffer, 1, nWant, dump);
    }
  }
#endif

  if(nRet==VONP_ERR_None && m_bFormatChanged)
  {
    if(m_bSeeked || m_bOpened)// || m_bAudioOnly || m_bVideoOnly)
    {
      m_bOpened = false;
      nRet = VONP_ERR_FLush_Buffer;
      VOLOGI("[OMXAL]Read TS stream return VONP_ERR_FLush_Buffer,system time %d", voOS_GetSysTime());
    }
    else
    {
	/*
		{ // work around for BA drop frames, wait AL output frames as many as possible before BA happens.
			int lastReadTime = m_nLastReadAudioTime >= m_nLastReadVideoTime ? m_nLastReadAudioTime : m_nLastReadVideoTime;
			int forwardTime = lastReadTime - GetPlayingTime();
			if(forwardTime > 50 && forwardTime < 2000)
			{
				VOLOGI("BA happens sleep %d ms", forwardTime);
				voOS_Sleep(forwardTime);
			}
		}
	 */

      nRet = VONP_ERR_Format_Changed;
      VOLOGI("[OMXAL]Read TS stream return VONP_ERR_Format_Changed,system time %d", voOS_GetSysTime());
    }

    m_bFormatChanged = false;
  }

  //VOLOGI("[OMXAL]-doReadBufQueryFromOMXAL, want %d, real %d, playing time %d, last audio %d, last video %d nRet 0X%08X", *pWantReadSize, nWant, GetPlayingTime(), m_nLastReadAudioTime, m_nLastReadVideoTime, nRet);

  *pWantReadSize = nWant;
  return nRet;
}

int	COMXALMediaPlayer::ReadBufQueryFromOMXAL(void * pUserData, char* pBuffer, int* pWantReadSize, int nReadType)
{
  COMXALMediaPlayer* pPlayer = (COMXALMediaPlayer*)pUserData;
  if(!pPlayer)
    return VONP_ERR_Retry;

  return pPlayer->doReadBufQueryFromOMXAL(pBuffer, pWantReadSize, nReadType);
}

int COMXALMediaPlayer::SendRawFrame(bool bAudio, VONP_BUFFERTYPE* pFrame)
{
  if(!IsSupportAudioCodec() || !IsSupportVideoCodec())
  {
    VOLOGW("[OMXAL]Cancel send raw frame,%s %d %d by not support codec", bAudio?"audio":"video", (int)pFrame->llTime, pFrame->nSize);
    return VONP_ERR_None;
  }

  if(!bAudio && NULL != m_pVideoSplit)
  {
	  int sptRet = -1;
	  VO_U32 sptSize = 0;
	  unsigned char * outPtr;

	  m_pVideoSplit->InputFrameInfo(pFrame->pBuffer, pFrame->nSize, pFrame->llTime);

	  sptRet = m_pVideoSplit->GetFrameInfo(&outPtr, &sptSize, (VO_U64*)&(m_pSplitBuf.llTime));
	  if(0 == sptRet)
	  {
	  	if(NULL == m_pSplitBuf.pBuffer)
		{
			m_pSplitBuf.nSize = sptSize * 2;
			m_pSplitBuf.pBuffer = new unsigned char[m_pSplitBuf.nSize];
		}
		else if((int)sptSize > m_pSplitBuf.nSize)
		{
			delete []((unsigned char*)m_pSplitBuf.pBuffer);
			m_pSplitBuf.nSize = sptSize * 2;
			m_pSplitBuf.pBuffer = new unsigned char[m_pSplitBuf.nSize];
		}

		memcpy(m_pSplitBuf.pBuffer, outPtr, sptSize);
		pFrame->nSize = sptSize;
		pFrame->llTime = m_pSplitBuf.llTime;
	  }
	  else
	  {
	  	VOLOGI("[OMXAL]Cannot get video frame from video spliter ");
	  	return VONP_ERR_None;
	  }
  }


  VO_SINK_SAMPLE sample;
  memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
  if(!bAudio && NULL != m_pVideoSplit)
	  sample.Buffer	= m_pSplitBuf.pBuffer;
  else
	  sample.Buffer	= pFrame->pBuffer;
  sample.Size		= pFrame->nSize;
  if(!bAudio && IsKeyFrame_H264(sample.Buffer, sample.Size, 0))
    sample.Size |= 0x80000000;
  sample.nAV		= bAudio?0:1;
  sample.Time		= pFrame->llTime;
  sample.DTS		= -1;

#if 0
  //if(true == m_bViewChangeing)
    DumpRawFrame(bAudio, sample.Buffer, sample.Size, sample.Time);
#endif

#if 0
	if(!AdjustTimestamp(&sample, pFrame))
	{  
		VOLOGW("[OMXAL]Drop frame %s %08lld", bAudio?"AUDIO":"VIDEO", pFrame->llTime);
		return VONP_ERR_None;
	}  
#endif

  int nRet = VONP_ERR_Retry;
  int nRetryTime      = m_bAudioOnly?1000*30:1000*15;
  int nRetryCount		= 0;
  int nRetrySleepTime	= 2;
  while (nRet == VONP_ERR_Retry)
  {
    nRetryCount++;

    if(IsStop() || IsSeeking() || m_bStopReadThread || m_bCancelSendRawFrame)
    {
      VOLOGW("[OMXAL]Cancel send raw frame,%s %d %d, stop %d, seeking %d, stop reading %d, cancel %d", bAudio?"audio":"video", (int)sample.Time, (int)sample.Size, IsStop()?1:0, IsSeeking()?1:0, m_bStopReadThread?1:0, m_bCancelSendRawFrame?1:0);
      break;
    }

    if(m_pMuxStream)
      nRet = m_pMuxStream->SendRawFrame(&sample);

    if(nRet == VONP_ERR_Retry)
    {
      if(nRetryCount*nRetrySleepTime > nRetryTime)
      {
        if(IsPause())
        {
          nRetryCount = 0;
          VOLOGW("[OMXAL]Retry to send raw frame because pause");
          continue;
        }

        VOLOGW("[OMXAL]++++Push %s buffer into mux timeout,Time %d, Size %d, Playing Time %d, mem size %d++++", bAudio?"audio":"video", (int)pFrame->llTime, pFrame->nSize, GetPlayingTime(), m_pMuxStream->GetMemStreamSize());
        break;
      }

      voOS_Sleep(nRetrySleepTime);
    }
  }

  return nRet;
}

#if 0
bool COMXALMediaPlayer::AdjustTimestamp(VO_SINK_SAMPLE* pMuxSample, VONP_BUFFERTYPE* pBuf)
{
	bool bAudio = pMuxSample->nAV==0?true:false;

	if(m_pFrameTools && bAudio)
	{  
		long long nTS = m_pFrameTools->GetAACFrameTime(pBuf);

		if(nTS >= 0)
		{
			pMuxSample->Time = nTS;
			return true;
		}
		else
		{
			if(m_bAudioOnly)
				return true;

			VOLOGI("[OMXAL][Seek] return");
			return false;
		}
	}
	else if(!bAudio)
	{
		long long nTS = m_pFrameTools->GetVideoFrameTime(pBuf);
		pMuxSample->Time = nTS;        
		return true;
	}  

	return false;
}
#endif


int COMXALMediaPlayer::CreateAudioDecoder(int nCodec, VONP_AUDIO_FORMAT* pFmt)
{	
  if(m_pAudioDecoder)
    return VONP_ERR_None;

  VOLOGI("[OMXAL]+Create audio decoder: %d %d %d m_pLibOP %p", (int)pFmt->nSampleRate, (int)pFmt->nChannels, (int)pFmt->nSampleBits, m_pLibOP);

  //	m_nMaxPCMOutputBufSize		= pFmt->nSampleRate*pFmt->nChannels*(pFmt->nSampleBits/2);	
  //	m_AudioDecOutputBuf.Buffer	= new unsigned char[m_nMaxPCMOutputBufSize];
  //	m_AudioDecOutputBuf.Length	= m_nMaxPCMOutputBufSize;	

  m_pAudioDecoder = new CADecoder(NULL, nCodec, &m_MemOp);
  m_pAudioDecoder->SetLib(m_pLibOP);
  m_pAudioDecoder->SetLibOperator((VO_LIB_OPERATOR *)m_pLibOP);

  VO_AUDIO_FORMAT fmt;
  fmt.Channels	= pFmt->nChannels;
  fmt.SampleRate	= pFmt->nSampleRate;
  fmt.SampleBits	= pFmt->nSampleBits;
  return m_pAudioDecoder->Init(NULL, 0, &fmt);
}

int COMXALMediaPlayer::DoAudioDecoder(VONP_BUFFERTYPE* pBufer)
{
  if(!m_pAudioDecoder)
    return VONP_ERR_Pointer;

  VO_CODECBUFFER input;
  memset(&input, 0, sizeof(VO_CODECBUFFER));

  input.Buffer	= pBufer->pBuffer;
  input.Length	= pBufer->nSize;
  input.Time		= pBufer->llTime;

  int nRet = m_pAudioDecoder->SetInputData(&input);

  VO_AUDIO_FORMAT fmt;
  memset(&fmt, 0, sizeof(VO_AUDIO_FORMAT));

  m_AudioDecOutputBuf.Buffer  = m_pAudioOutBuff + m_nAudioOutSize;
  m_AudioDecOutputBuf.Length	= m_nAudioBufferSize - m_nAudioOutSize;

  VO_BOOL bChanged = VO_FALSE;
  nRet = m_pAudioDecoder->GetOutputData(&m_AudioDecOutputBuf, &fmt, bChanged);
  //VOLOGI("[OMXAL]Audio decoder returns code %x", nRet);

  if(bChanged)
  {
    VOLOGI("[OMXAL]audio decoder return fromat changed");
    m_AudioFormat.nChannels		= fmt.Channels;
    m_AudioFormat.nSampleRate	= fmt.SampleRate;
    m_AudioFormat.nSampleBits	= fmt.SampleBits;

    nRet = VONP_ERR_Format_Changed;

    //if(m_pMuxStream)
    //m_pMuxStream->OnAudioFormatChanged(&m_AudioFormat);
  }

  if(nRet == VO_ERR_NONE)
  {
    pBufer->pBuffer	= m_AudioDecOutputBuf.Buffer;
    pBufer->nSize	= m_AudioDecOutputBuf.Length;

    //VOLOGI("%x %x %x %x %x %x %x %x", pBufer->pBuffer[0], pBufer->pBuffer[1], pBufer->pBuffer[2], pBufer->pBuffer[3],
    //pBufer->pBuffer[4], pBufer->pBuffer[5], pBufer->pBuffer[6], pBufer->pBuffer[7]);
    m_nAudioOutSize += m_AudioDecOutputBuf.Length;
    nRet = VONP_ERR_None;

#if 0
    static FILE* hFile = fopen("/data/local/tmp/pcm.pcm", "wb");

    if(!hFile)
      VOLOGI("[OMXAL]PCM dump file failed. %s", "/data/local/test/pcm.pcm");

    if(pBufer->nSize > 0 && hFile)
    {
      fwrite(pBufer->pBuffer, 1, pBufer->nSize, hFile);
    }

#endif

  }

  return nRet;
}

int COMXALMediaPlayer::DestroyAudioDecoder()
{
  if(m_pAudioDecoder)
  {
    delete m_pAudioDecoder;
    m_pAudioDecoder = NULL;
  }

  //	if(m_AudioDecOutputBuf.Buffer)
  //	{
  //		delete []m_AudioDecOutputBuf.Buffer;
  //		memset(&m_AudioDecOutputBuf, 0, sizeof(VO_CODECBUFFER));
  //	}	

  return VONP_ERR_None;
}

int COMXALMediaPlayer::CreateAuidoEffect()
{
  if(m_pAudioEffect)
    return VONP_ERR_None;

  m_pAudioEffect = new CAEffect(NULL, &m_MemOp);
  m_pAudioEffect->SetLib(m_pLibOP);

  VO_AUDIO_FORMAT fmt;
  fmt.Channels	= m_AudioFormat.nChannels;
  fmt.SampleRate	= m_AudioFormat.nSampleRate;
  fmt.SampleBits	= m_AudioFormat.nSampleBits;	
  int nRet = m_pAudioEffect->Init(&fmt);
  VOLOGI("[OMXAL]Audio effect init return code %x", nRet);

  m_pAudioEffect->Enable(m_bEnableAudioEffect?VO_TRUE:VO_FALSE);

  return nRet;
}

int COMXALMediaPlayer::DoAudioEffect(VONP_BUFFERTYPE* pBufer)
{
  if(!m_pAudioEffect)
    return VONP_ERR_Pointer;

  int nRet = VONP_ERR_None;

  VO_CODECBUFFER input;
  memset(&input, 0, sizeof(VO_CODECBUFFER));

  input.Buffer	= pBufer->pBuffer;
  input.Length	= pBufer->nSize;
  input.Time		= pBufer->llTime;

  nRet = m_pAudioEffect->SetInputData(&input);

  VO_AUDIO_FORMAT fmt;
  memset(&fmt, 0, sizeof(VO_AUDIO_FORMAT));
  m_AudioDecOutputBuf.Length	= m_nMaxPCMOutputBufSize;
  nRet = m_pAudioEffect->GetOutputData(&m_AudioDecOutputBuf, &fmt);
  VOLOGI("Audio effct returns code %x", nRet);

  if(nRet == 0)
  {
    pBufer->pBuffer	= m_AudioDecOutputBuf.Buffer;
    pBufer->nSize	= m_AudioDecOutputBuf.Length;
  }	

  return nRet;
}

int COMXALMediaPlayer::DestroyAudioEffect()
{
  if(m_pAudioEffect)
  {
    m_pAudioEffect->Uninit();
    delete m_pAudioEffect;
    m_pAudioEffect = NULL;
  }

  return VONP_ERR_None;
}

void COMXALMediaPlayer::OnFormatChanged()
{
  VOLOGI("+[OMXAL]OnFormatChanged: restruct mux stream, first %d", m_bFirstRun?1:0);

  if(m_pVideoSplit)
  	m_pVideoSplit->ResetContext();

  if(!m_bFirstRun)
  {
    //voCAutoLock lock(&m_mtStatus);

    //currently pure audio use render outside
    if(m_bAudioOnly)
    {
      VOLOGI("[OMXAL]Enable format change flag to true --> pure audio");
      if(m_pMuxStream)
        m_pMuxStream->ForceOutputWholeStream();

      WaitBufferEmpty();

      m_bFormatChanged = true;
    }
    else
    {
      if(m_pMuxStream)
        m_pMuxStream->ForceOutputWholeStream();

      WaitBufferEmpty();

      if(IsStop())
        return;

      //also use this flag to forbid to read ts stream,refer to doReadBufQueryFromOMXAL
      VOLOGI("[OMXAL]Enable format change flag to true");
      m_bFormatChanged = true;

      m_pMuxStream->Flush();
    }
  }
  else
    m_bOpened = false;

  if(m_pMuxStream && (m_bVideoOnly)) // for change program between pure audio and normal
  {
	  m_pMuxStream->Init();
  }

  if(m_bVideoOnly)
  {
    VOLOGW("[OMXAL]Set video only into mux, cache is 0...");
    m_pMuxStream->SetCacheDuration(0);
  }
  else if(m_bAudioOnly)
  {
     // m_bFormatChanged = false;
    //m_pMuxStream->SetCacheDuration(1000);        
  }
//  ResetPeekBuffer();
//  m_peekBufInfo.bPeeking = false;

  m_bFirstRun			= false;
  m_bWaitKeyFrame		= true;

  VOLOGI("-[OMXAL]OnFormatChanged: restruct mux stream, %d", m_bFormatChanged?1:0);
}

VO_U32 COMXALMediaPlayer::doReadBufThread()
{
    VONP_BUFFERTYPE buf;
	memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    
    bool    bForceFormatChanged = false;
	int     nMaxForceTryCount   = MAX_FORCE_READ_TRY_COUNT;
	int     nForceTryCount      = 0;
	bool	bShowAudioLog		= false;
	bool	bShowVideoLog		= false;
//	bool	bUseAudioOffsetTime	= false;
	int		nPosOffsetTime		= 0;
	int     nReadRet			= VONP_ERR_None;
    
    VOLOGI("[OMXAL]+Enter read thread");
	
	while (!m_bStopReadThread)
	{
		if(m_bPauseReadBuf || !m_pReadBufFunc || !m_pReadBufFunc->ReadAudio || !m_pReadBufFunc->ReadVideo || m_bUseOutsideRender)
		{
            //VOLOGI("[OMXAL]No reading data right now...");
            m_semSeek.Signal();
			voOS_Sleep(2);
			continue;
		}

		CheckWrapRunning();

		if(true == m_bNeedFormatChange)
		{
            VOLOGR("[OMXAL]Wait for finished format change...");
			voOS_Sleep(2);
			continue;
		}

		if(m_peekBufInfo.bPeeking && m_peekBufInfo.bEnable)
			PeekBuffer(&buf);

        //save memory
        if(!IsPeekBufAvailable())
            ResetPeekBuffer();
        
		voCAutoLock lock(&m_mtStatus);
    
        if( (m_bReadVideoNewFormat&&m_bReadAudioNewFormat) || (m_bReadVideoNewFormat&&m_bVideoOnly) || (m_bReadAudioNewFormat&&m_bAudioOnly)  || bForceFormatChanged)
        {
            if(bForceFormatChanged)
            {
                if(m_bReadAudioNewFormat)
                    nPosOffsetTime = m_nLastReadAudioTime;
                else if(m_bReadVideoNewFormat)
                    nPosOffsetTime = m_nLastReadVideoTime;
            }
            else
            {
                if(m_bReadAudioNewFormat)
                    nPosOffsetTime = m_nLastAudioNewFormatTime;//maybe from pure audio
                else if(m_bReadVideoNewFormat)
                    nPosOffsetTime = m_nLastReadVideoTime;
            }
            
            m_nLastBATime = nPosOffsetTime;
            
            VOLOGI("[OMXAL] -------->> BA happened, Time %d, Force %d, A %d, V %d, A-Only %d, V-Only %d, playing %d, %02d:%02d:%02d", nPosOffsetTime, bForceFormatChanged?1:0, m_bReadAudioNewFormat?1:0, m_bReadVideoNewFormat?1:0, m_bAudioOnly?1:0, m_bVideoOnly?1:0, GetPlayingTime(), nPosOffsetTime/1000/3600, ((nPosOffsetTime/1000)%3600)/60, (((nPosOffsetTime/1000)%3600)%60));

  
			m_bNeedFormatChange = true;
			voOS_Sleep(5);

            bForceFormatChanged     = false;
            m_bReadAudioNewFormat   = false;
            m_bReadVideoNewFormat   = false;
            
            if(m_bSeeked || IsAppResume())
            {
                m_nPlayingTimeOffset = nPosOffsetTime;
            }
            
            if(!m_bAudioOnly)
                SendAudioHeadData(nPosOffsetTime);
            
            if(m_bAudioOnly)
            {
                VOLOGW("[OMXAL]BA by audio only,use render outside");
                
				m_bUseOutsideRender = true;    
                //hold on in read thread
                continue;
            }
			//hold on in read thread and wait finish format change
			continue;
        }
        
        m_nTryReadBufCount++;
		
		//read audio
		if(((IsCanReadBuffer(true) && !m_bForceReadVideoNewFmt) || m_bForceReadAudioNewFmt) && !m_bReadAudioNewFormat)
		{
			memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
            nReadRet = ReadBuffer(true, &buf);
			if(bShowAudioLog || m_nTryReadBufCount<20 || m_bForceReadAudioNewFmt)
                VOLOGI(" [OMXAL]AUDIO, flag %x, %08lld, size %06d, diff %lld, buf %x, data %x, ret %d, audio %d,video %d, %lu", buf.nFlag, buf.llTime, buf.nSize, buf.llTime-m_nLastReadAudioTime, (int)buf.pBuffer, (int)buf.pData, nReadRet, m_bAudioOnly, m_bVideoOnly, voOS_GetSysTime());
            
			if(nReadRet == VONP_ERR_Retry)
			{
				voOS_Sleep(2);
			}
            else if(nReadRet == VONP_ERR_Audio_No_Now)
			{
				VOLOGW("[OMXAL]Read audio inavailable");

                //mainly process first read and seek this flag while begin playback
                if((!m_bVideoOnly||m_bSeeked) && !m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                {
                    m_peekBufInfo.bPeeking      = true;
                    m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_NOT_AVAILABLE;
                }

                m_bVideoOnly = true;
                //m_nTryReadBufCount = 0;
                m_bForceReadAudioNewFmt = false;
			}
            else if(nReadRet == VONP_ERR_EOS)
			{
				VOLOGW("[OMXAL]Reading audio buf reach EOS");
				m_nStatusEOS |= 0x01;
				
				if(m_bAudioOnly)
					m_nStatusEOS |= 0x02;
                
                m_bForceReadAudioNewFmt = false;
                
                if(m_nStatusEOS&0x01 && m_nStatusEOS&0x02)
                    OnReadBufEOS();
			}
            else if(nReadRet == VONP_ERR_None)
            {
				m_bAudioStatusOK = true;
                if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
                {
                    VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)buf.pData;
                    
                    if(buf.pData)
                    {
                        VOLOGI("[OMXAL]Read audio new format, codec %d, S %d, C %d, B %d, flag %d, time %lld, playing %d, last audio %d, try %d", pFmt->nCodec, pFmt->sFormat.audio.nSampleRate, pFmt->sFormat.audio.nChannels, pFmt->sFormat.audio.nSampleBits, buf.nFlag, buf.llTime, GetPlayingTime(), m_nLastReadAudioTime, nForceTryCount);
                 
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_NEW_FORMAT;
                            continue;
                        }

                        //m_AVFrameWriter.Write(true, (unsigned char*)pFmt->pHeadData, pFmt->nHeadDataSize, 0, false, true);
                        
                        if(!CheckCodecSupport(true, pFmt->nCodec))
                        {
                            VOLOGI("[OMXAL]+++++++++++Audio format NOT support. %d+++++++++++++++", pFmt->nCodec);
                            m_bSupportAudioCodec = false;
                            SendEvent(VONP_CB_Codec_NotSupport, NULL, NULL);
                            voOS_Sleep(2);
                            continue;
                        }
                        
                        if(!CheckValidBA(buf.llTime))
                        {
                            CopyTrackInfo(true, &m_AudioTrackInfo, pFmt);
                            continue;
                        }
                        
						m_bGotNewFormatBuf		= true;
                        m_bForceReadAudioNewFmt = false;
                        m_nLastReadAudioTime	= buf.llTime;
                        m_nLastAudioNewFormatTime = buf.llTime;
                        m_bSupportAudioCodec	= true;
                        CopyTrackInfo(true, &m_AudioTrackInfo, pFmt);
                        
                        m_bReadAudioNewFormat		= true;
                        nPosOffsetTime		= buf.llTime;
                        
                        if(buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
                            nPosOffsetTime = 0;
                        
                        m_bVideoOnly    = false;
                        
                        if(m_bReadVideoNewFormat)
                        {
                            VOLOGI("[OMXAL]continue to read audio");
                            continue;
                        }
                        else if(!m_bAudioOnly)
                        {
                            // continue to read video
                            VOLOGI("[OMXAL]go to read video");
                            nForceTryCount = 0;
                            m_bForceReadVideoNewFmt   = true;
                        }
                    }
                }
                else if(buf.pBuffer && buf.nSize>0 && m_bSupportAudioCodec)
                {
                    bool bFirstAudio = m_bWaitFirstAudioFrame?true:false;
                    if(m_bWaitFirstAudioFrame)
                    {
                        VOLOGI("[OMXAL]Got first audio frame, time %lld, size %d", buf.llTime, buf.nSize);
                        if(m_pFrameTools)
                            m_pFrameTools->UpdateDurationPerFrame(&buf);
                        m_bWaitFirstAudioFrame = false;
                    }
                    
                    if(abs(buf.llTime-m_nLastReadAudioTime) > MAX_DISCONTINUOS_TIME && !bFirstAudio)
                    {
                        VOLOGW("[OMXAL]-------->> Audio time discontinuos, curr %lld, last %d, diff %lld", buf.llTime, m_nLastReadAudioTime, buf.llTime-m_nLastReadAudioTime);
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_DISCONTINUOS;
                            continue;
                        }
                    }
                    
                    int nDiff = buf.llTime - m_nLastReadAudioTime;
                    //VOLOGI("[OMXAL]diff %f", (float)nDiff/(float)m_pFrameTools->GetDurationPerFrame());
                    if( ((float)nDiff/(float)m_pFrameTools->GetDurationPerFrame())>=MAX_AUDIO_INTERVAL_SCALE && nDiff<=500 && !m_bAudioOnly && !bFirstAudio)
                    {
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            VOLOGW("[OMXAL]-------->> Audio time interval is so large, curr %lld, last %d, diff %lld, frame duration %d", buf.llTime, m_nLastReadAudioTime, buf.llTime-m_nLastReadAudioTime, m_pFrameTools->GetDurationPerFrame());
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_LARGE_INTERVAL;
                            continue;
                        }
                    }

                    //CheckFlushBuffer(&buf);
                    //m_AVFrameWriter.Write(true, buf.pBuffer, buf.nSize, buf.llTime, false, false);
                    
                    if(CheckAudioDropFrame(&buf))
                    {
                        m_nLastReadAudioTime = buf.llTime;
                        continue;
                    }
                    
                    if(m_bReadAudioNewFormat)
                    {
                        // if both audio and video, set false when sending video head data
                       // bUseAudioOffsetTime = true;
                    }
                    
                    //if(CheckValidADTSFrame(&buf))
                        SendRawFrame(true, &buf);
                    
                    m_nLastReadAudioTime = buf.llTime;
                    
                    if(m_bForceReadAudioNewFmt && m_bReadVideoNewFormat && !m_peekBufInfo.bEnable)
                    {
                        if(buf.llTime >= m_nLastVideoNewFormatTime)
                        {
                            VOLOGW("[OMXAL]Force got audio new format: %lld", buf.llTime);
                            bForceFormatChanged         = true;
                            m_bForceReadAudioNewFmt     = false;
                            continue;
                        }
                    }
                }
            }
            else // read failed
            {
                if(!buf.pData && !buf.pBuffer)
                {
                    // read a NULL buffer after read audio new format and video inavailable
                    if(m_bReadAudioNewFormat && m_bAudioOnly)
                    {
                        VOLOGW("[OMXAL]Can't read available audio buffer after read audio new format and video inavailable");
                        voOS_Sleep(10);
                        continue;
                    }
                }
                
                VOLOGW("[OMXAL]read audio NULL - %d", nReadRet);
                voOS_Sleep(10);
            }
        }
		
		if(m_bPauseReadBuf || IsStop())
		{
			continue;
		}
        
        if(m_peekBufInfo.bPeeking)
        {
            continue;
        }
        
        if(m_bForceReadAudioNewFmt)
        {
            VOLOGW("Force to read audio new format, new format flag:A %d, V %d, try %02d", m_bReadAudioNewFormat?1:0, m_bReadVideoNewFormat?1:0, nForceTryCount);
            
            voOS_Sleep(20);
            nForceTryCount++;
            if(nForceTryCount < nMaxForceTryCount)
            {
                //continue;
            }
            else
            {
                bForceFormatChanged = true;
                m_bForceReadAudioNewFmt = false;
            }
            continue;
        }
        
        if(m_bForceReadVideoNewFmt)
        {
            VOLOGW("Force to read video new format, new format flag:A %d, V %d, try %02d", m_bReadAudioNewFormat?1:0, m_bReadVideoNewFormat?1:0, nForceTryCount);
            
            voOS_Sleep(20);
            nForceTryCount++;
            if(nForceTryCount < nMaxForceTryCount)
            {
            }
            else
            {
                bForceFormatChanged = true;
                m_bForceReadVideoNewFmt = false;
                continue;
            }
        }
        
		//read video
		if(IsCanReadBuffer(false) || m_bReadAudioNewFormat)
		{
			if(false == m_bAudioStatusOK && !m_bVideoOnly)
				continue;
			memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
            nReadRet = ReadBuffer(false, &buf);
			if(bShowVideoLog || m_nTryReadBufCount<20 || m_bForceReadVideoNewFmt)
                VOLOGI("[OMXAL]VIDEO, flag %x, %08lld, size %06d, diff %lld, buf %x, data %x, ret %d, audio %d,video %d, %lu", buf.nFlag, buf.llTime, buf.nSize, buf.llTime-m_nLastReadVideoTime, (int)buf.pBuffer, (int)buf.pData, nReadRet, m_bAudioOnly, m_bVideoOnly, voOS_GetSysTime());
            
			if(nReadRet == VONP_ERR_Retry)
			{
				voOS_Sleep(2);
			}
            else if(nReadRet == VONP_ERR_Video_No_Now)
			{
				VOLOGW("[OMXAL]Read video inavailable");
                
                //mainly process first read this flag while begin playback
                if((!m_bAudioOnly||m_bSeeked) && !m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                {
                    m_peekBufInfo.bPeeking      = true;
                    m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_VIDEO_NOT_AVAILABLE;
                }

				m_bAudioOnly = true;
                //m_nTryReadBufCount = 0;
                m_bForceReadVideoNewFmt = false;
			}
			else if(nReadRet == VONP_ERR_EOS)
			{
				VOLOGW("[OMXAL]Reading video buf reach EOS");
				m_nStatusEOS |= 0x02;
				
				if(m_bVideoOnly)
					m_nStatusEOS |= 0x01;
                
                m_bForceReadVideoNewFmt = false;
                
                if(m_nStatusEOS&0x01 && m_nStatusEOS&0x02)
                    OnReadBufEOS();
			}
            else if(nReadRet == VONP_ERR_None)
            {
                CheckVideoKeyFrame(&buf);
                
                if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
                {
                    VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)buf.pData;
                    if(buf.pData)
                    {
                        VOLOGI("[OMXAL]Read video new format, codec %d, width %d, height %d, flag %d, time %lld, try %d", pFmt->nCodec, pFmt->sFormat.video.nWidth, pFmt->sFormat.video.nHeight, buf.nFlag, buf.llTime, nForceTryCount);
                        
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_VIDEO_NEW_FORMAT;
                            continue;
                        }

                        //m_AVFrameWriter.Write(false, (unsigned char*)pFmt->pHeadData, pFmt->nHeadDataSize, 0, false, true);
                        //m_AVFrameReader.Read(false, (unsigned char*)pFmt->pHeadData, pFmt->nHeadDataSize, llTmp, bTmp, bTmp);
                        
                        if(!CheckCodecSupport(false, pFmt->nCodec))
                        {
                            m_bSupportVideoCodec = false;
                            VOLOGE("[OMXAL]+++++++++++Video format NOT support. %d+++++++++++", pFmt->nCodec);
                            SendEvent(VONP_CB_Codec_NotSupport, NULL, NULL);
                            voOS_Sleep(2);
                            continue;
                        }
                        
                        if(!CheckValidBA(buf.llTime))
                        {
                            CopyTrackInfo(true, &m_VideoTrackInfo, pFmt);
                            SendVideoHeadData(buf.llTime);
                            continue;
                        }
                        
                        m_bAudioOnly			= false;
                        m_bSupportVideoCodec	= true;
                        
                        if(pFmt->nCodec == VONP_VIDEO_CodingH264)
                            CheckH264Resolution(&buf);
                        
                        CopyTrackInfo(false, &m_VideoTrackInfo, pFmt);
                        CheckAspectRatio(&buf);

						m_bGotNewFormatBuf		= true;
                        m_bReadVideoNewFormat       = true;
                        m_bForceReadVideoNewFmt     = false;
                        m_nLastReadVideoTime        = buf.llTime;
                        m_nLastVideoNewFormatTime   = buf.llTime;
                        
                        if(!m_bReadAudioNewFormat && !m_bVideoOnly)
                        {
                            VOLOGI("[OMXAL]ready to read audio new format flag");
                            nForceTryCount = 0;
                            m_bForceReadAudioNewFmt = true;
                        }
                        
                        // continue to read audio
                        if(!m_bVideoOnly)
                        {
                            VOLOGI("[OMXAL]go to read audio");
                            continue;
                        }
                    }
                    else
                    {
                        VOLOGE("[OMXAL]No head data found...");
                    }
                }
                else
                {
                    if(m_bReadAudioNewFormat && !m_bReadVideoNewFormat)
                    {
                        //VOLOGI("[OMXAL]Can't read video format after read audio new format");
                    }
                }
                
                if(buf.pBuffer && buf.nSize>0 && m_bSupportVideoCodec)
                {
                    if(abs(buf.llTime-m_nLastReadVideoTime) > MAX_DISCONTINUOS_TIME)
                    {
                        VOLOGW("[OMXAL]-------->> Video time discontinuos, curr %lld, last %d, diff %lld", buf.llTime, m_nLastReadVideoTime, buf.llTime-m_nLastReadVideoTime);
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_VIDEO_DISCONTINUOS;
                            continue;
                        }
                    }
                    
                    //CheckFlushBuffer(&buf);
                    //m_AVFrameWriter.Write(false, buf.pBuffer, buf.nSize, buf.llTime, buf.nFlag&VONP_FLAG_BUFFER_KEYFRAME?true:false, false);
                    
                    if(CheckVideoDropFrame(&buf))
                    {
                        m_nLastReadVideoTime = buf.llTime;
                        continue;
                    }
                    
                    if(m_bWaitKeyFrame)
                    {
                        if(buf.nFlag & VONP_FLAG_BUFFER_KEYFRAME)
                        {
                            VOLOGI("[OMXAL]Got a key frame - %d...", (int)buf.llTime);
                            m_bWaitKeyFrame		= false;

                            if(m_bSeeked)
                                m_nLastVideoNewFormatTime = buf.llTime;

                            SendVideoHeadData(buf.llTime);
                            
                            if(m_nPosOffsetTime > buf.llTime)
                                VOLOGE("[OMXAL]+++++++++++Head data time large than key frame! head %d, key %d+++++++++++", m_nPosOffsetTime, (int)buf.llTime);
                        }
                        else
                        {
                            VOLOGW("[OMXAL]Waitting key frame..., %lld dropped", buf.llTime);
                            voOS_Sleep(1);
                            m_nLastReadVideoTime = buf.llTime;
                            continue;
                        }
                    }
                    
                  //  bUseAudioOffsetTime = false;
                    SendRawFrame(false, &buf);
                    m_nLastReadVideoTime = buf.llTime;
                    
                    if(m_bForceReadVideoNewFmt && m_bReadAudioNewFormat && !m_peekBufInfo.bEnable)
                    {
                        if(buf.llTime >= m_nLastAudioNewFormatTime)
                        {
                            VOLOGW("[OMXAL]Force got video new format: %lld", buf.llTime);
                            bForceFormatChanged         = true;
                            m_bForceReadVideoNewFmt     = false;
                            continue;
                        }
                    }
                }
            }
            else // read failed
            {
                //VOLOGW("[OMXAL]read video NULL - %d, %d", nReadRet, voOS_GetSysTime());
                voOS_Sleep(10);
            }
		}
        
		voOS_Sleep(2);
	}
	
	m_hReadBufThread = NULL;
	VOLOGI("[OMXAL]-Exit read thread!!!");
	
	return 0;
}

VO_U32 COMXALMediaPlayer::ReadBufThreadProc(VO_PTR pParam)
{
  COMXALMediaPlayer* pPlayer = (COMXALMediaPlayer*)pParam;

  if(!pPlayer)
    return 0;

  return pPlayer->doReadBufThread();
}

int COMXALMediaPlayer::CreateReadThread()
{
  //DestroyReadThread();
  if(m_hReadBufThread)
    return VONP_ERR_None;

  m_bStopReadThread = false;

  VO_U32 nID = 0;
  voThreadCreate(&m_hReadBufThread, &nID, ReadBufThreadProc, this, 0);
  return VONP_ERR_None;
}

int COMXALMediaPlayer::DestroyReadThread()
{
  if(m_hReadBufThread)
  {
    VOLOGI("[OMXAL]set m_bStopReadThread true");
    m_bStopReadThread = true;

    int nCount = 0;
    while (m_hReadBufThread && nCount<500)
    {
      nCount++;
      voOS_Sleep(10);
    }
  }

  return VONP_ERR_None;
}

int COMXALMediaPlayer::CreateEventNotifyThread()
{
  if(m_hEventNotifyThread)
    return VONP_ERR_None;

  m_bStopEventNotifyThread = false;
  VO_U32 nID = 0;
  voThreadCreate(&m_hEventNotifyThread, &nID, EventNotifyThreadProc, this, 0);
  return VONP_ERR_None;
}

int COMXALMediaPlayer::DestroyEventNotifyThread()
{
  if(m_hEventNotifyThread)
  {
    m_bStopEventNotifyThread = true;

    int nCount = 0;
    while (m_hEventNotifyThread && nCount<500)
    {
      nCount++;
      voOS_Sleep(10);
    }
  }

  return VONP_ERR_None;
}

VO_U32 COMXALMediaPlayer::EventNotifyThreadProc(VO_PTR pParam)
{
  COMXALMediaPlayer* pPlayer = (COMXALMediaPlayer*)pParam;

  if(!pPlayer)
    return VONP_ERR_Pointer;

  return pPlayer->doEventNotifyThread();
}

VO_U32 COMXALMediaPlayer::doEventNotifyThread()
{
  VOLOGI("[OMXAL]Enter event notify thread");

  //while (!IsStop())
  while(!m_bStopEventNotifyThread)
  {
    //VOLOGI("[OMXAL]Check event notify thread");

    CheckEOS();

    CheckBuffering();

    CheckFormatChanged();

	CheckBAInfoChanged();

    CheckSeekMode();

    voOS_Sleep(20);
  }

  m_hEventNotifyThread = NULL;
  VOLOGI("[OMXAL]Exit event notify thread");

  return VONP_ERR_None;
}


bool COMXALMediaPlayer::CheckCodecSupport(bool bAudio, int nCodec)
{
  if(bAudio)
  {
    // OMXAL can support mp3, but we will do it in future, East 20121206
    if(VONP_AUDIO_CodingAAC == nCodec/* || VONP_AUDIO_CodingMP3 == nCodec*/)
      return true;
  }
  else
  {
    if(VONP_VIDEO_CodingH264 == nCodec)
      return true;
  }

  VOLOGE("[OMXAL]can't support such codec %d for %s", nCodec, bAudio ? "audio" : "video");
  return false;
}

bool COMXALMediaPlayer::IsCanReadBuffer(bool bAudio)
{
//  int nMaxAVOffsetTime	= 500;

  if(m_nLastReadAudioTime - GetPlayingTime() > 10*1000 && m_nPlayingTime>5*1000)
  {
    //VOLOGW("[OMXAL]Can't read buffer, last audio %d, play time %d, playing time %d, mem %d", m_nLastReadAudioTime, GetPlayingTime(), m_nPlayingTime, m_pMuxStream?0:m_pMuxStream->GetMemStreamSize());
    //voOS_Sleep(200);
    //return false;
  }

  //To workaround DASH,SS send audio/video inavailable flag before EOS
  if(m_bVideoOnly && m_bAudioOnly)
  {
    return true;
  }

  if(bAudio)
  {
    if(m_bVideoOnly)
      return false;
    if(m_bAudioOnly)
      return true;
    if(m_nStatusEOS & 0x01)
      return false;		
    // video EOS
    if(m_nStatusEOS & 0x02)
      return true;		

    //20120424
    if(m_nLastReadVideoTime==0 || m_nLastReadAudioTime==0 )//|| (m_nLastReadAudioTime-m_nLastReadVideoTime) <= nMaxAVOffsetTime)
    {
      //VOLOGI("[OMXAL]Can read audio frame A %d, V %d, B %d, D %d", m_nLastReadAudioTime, m_nLastReadVideoTime, m_pMuxStream?m_pMuxStream->GetMemStreamSize():0, (m_nLastReadAudioTime-m_nLastReadVideoTime));
      return true;
    }

    if(abs(m_nLastReadAudioTime-m_nLastReadVideoTime) > 20*1000)
      return true;

	if(m_nLastReadAudioTime <= m_nLastReadVideoTime)
		return true;
  }
  else // video
  {
    if(m_bAudioOnly)
      return false;
    if(m_bVideoOnly)
      return true;
    if(m_nStatusEOS & 0x02)
    {
      return false;
    }		
    // audio EOS
    if(m_nStatusEOS & 0x01)
      return true;

    //20120424
    if(m_nLastReadVideoTime==0 || m_nLastReadAudioTime==0 )//|| (m_nLastReadVideoTime-m_nLastReadAudioTime) <= nMaxAVOffsetTime)
    {
      //VOLOGI("[OMXAL]Can read video frame A %d, V %d, B %d, D %d", m_nLastReadAudioTime, m_nLastReadVideoTime, m_pMuxStream?m_pMuxStream->GetMemStreamSize():0, (m_nLastReadAudioTime-m_nLastReadVideoTime));
      return true;
    }

    if(abs(m_nLastReadAudioTime-m_nLastReadVideoTime) > 20*1000)
      return true;
	if(m_nLastReadVideoTime <= m_nLastReadAudioTime)
		return true;

  }

  return false;
}

bool COMXALMediaPlayer::CheckEOS()
{
  //int nAdjustOffset		= 1000;
#ifdef _VOLOG_INFO
  int nPlayingTime = 0;
  nPlayingTime = GetPlayingTime();
 #endif

  //VOLOGI("[OMXAL]Checking EOS...,playing %d, video %d, audio %d, EOS %d, pos update %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, m_nStatusEOS, voOS_GetSysTime()-m_nLastPosUpdateTime);
  //if(m_bVideoOnly)
//	  SendEOS();

  if(m_nStatusEOS&0x01 && m_nStatusEOS&0x02)
  {
    //VOLOGI("[OMXAL]Checking EOS...,playing %d, video %d, audio %d m_nLastPosUpdateTime %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, m_nLastPosUpdateTime);

    if(m_pMuxStream && m_pMuxStream->GetMemStreamSize()<=0 && IsRunning())
    {
      //if( (nPlayingTime+nAdjustOffset>=m_nLastReadAudioTime) /*|| (nPlayingTime+nAdjustOffset>=m_nLastReadVideoTime)*/ )
      if((voOS_GetSysTime()-m_nLastPosUpdateTime) > 500)
      {
        if(m_nLastPosUpdateTime == 0)
          return false;

        if(!m_bEOSed)
        {
          m_bEOSed = true;
          SendEOS();
#ifdef _VOLOG_INFO
          VOLOGI("[OMXAL]Play complete,playing %d, audio %d, video %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime);
#endif
        }
        return true;
      }
    }
  }

  return false;
}

int COMXALMediaPlayer::WaitBufferEmptyAfterGoForeground()
{
  int nWaitCount          = 0;
  int nLastPlayingTime	= GetPlayingTime();
  VOLOGI("[OMXAL] WaitBufferEmptyAfterGoForeground()..., playing %d, A %d, V %d", GetPlayingTime(), m_nLastReadAudioTime, m_nLastReadVideoTime);

  while(IsPause() || !IsSeeking())
  {
    if(IsStop())
      return VONP_ERR_None;

    nWaitCount++;
    int nPlayingTime = GetPlayingTime();

    int nOffsetTime = m_nLastReadAudioTime - nPlayingTime;
    if(nOffsetTime < 0)
      nOffsetTime = m_nLastReadVideoTime - nPlayingTime;

    if(m_pMuxStream->GetMemStreamSize() <= 0 || m_bUseOutsideRender)
    {
      //VOLOGI("[OMXAL]Buffer play completely 01,playing %d, audio %d, video %d, offset %d, last playing time %d, wait %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, nOffsetTime, nLastPlayingTime, nWaitCount);
      //break;
    }

    //check it each 500ms
    if(nWaitCount%100 == 0)
    {
      if(nLastPlayingTime == nPlayingTime)
      {
        VOLOGI("[OMXAL]%d Buffer play completely 02,playing %d, audio %d, video %d, offset %d, last playing time %d, wait %d", nWaitCount, nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, nOffsetTime, nLastPlayingTime, nWaitCount);
        break;
      }
      else
      {
        nLastPlayingTime	= GetPlayingTime();
      }
    }

    if(nWaitCount%100 == 0)
    {
      VOLOGI("[OMXAL]Waiting buffer play completely foreground,playing %d, last playing %d, A %d, V %d, mem %d", nPlayingTime, nLastPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, m_pMuxStream?m_pMuxStream->GetMemStreamSize():0);            
    }

    voOS_Sleep(5);
  }

  return VONP_ERR_None; 
}

int COMXALMediaPlayer::WaitBufferEmpty()
{
  // get last sample's time for TS stream
  int nLastAudioTime = -1, nLastVideoTime = -1;
  if(m_pMuxStream)
    m_pMuxStream->getLastSampleTime(&nLastAudioTime, &nLastVideoTime);

  int nPositionUpdatePeriod = m_pWrap ? m_pWrap->getPositionUpdatePeriod() : 100;

  VOLOGI("[OMXAL]WaitBufferEmpty, A %d, V %d, period %d, playing %d, LU %d", nLastAudioTime, nLastVideoTime, nPositionUpdatePeriod, GetPlayingTime(), m_nLastPosUpdateTime);

  int nWaitCount = 0;
  while((IsPause() || !IsSeeking()) && m_bAudioStatusOK)
  {
    // exit for external condition
    //if(IsStop() || m_bUseOutsideRender || m_bSeeked) // delete m_bUseOutsideRender for wait empty buffers which are held by OpenMax AL
    if(IsStop() || m_bSeeked)
    {
      VOLOGI("[OMXAL]buffer empty 1, stop %d, outside renderer %d, seek %d", IsStop(), m_bUseOutsideRender, m_bSeeked);
      break;
    }

    // no data input to OMXAL
    if((m_bAudioOnly && 0 == nLastAudioTime) || (m_bVideoOnly && 0 == nLastVideoTime) || (0 == nLastAudioTime && 0 == nLastVideoTime))
    {
      VOLOGI("[OMXAL]buffer empty 2, a only %d, v only %d", m_bAudioOnly, m_bVideoOnly);
      break;
    }

    int nPlayingTime = GetPlayingTime();
    int nOffsetTime = ((nLastAudioTime > nLastVideoTime) ? nLastAudioTime : nLastVideoTime) - nPlayingTime;
    if(nOffsetTime < 0)
    {
      VOLOGW("[OMXAL]buffer empty 3, playing %d", nPlayingTime);
      return VONP_ERR_Unknown;
    }

    if(m_pMuxStream->GetMemStreamSize() <= 0)	// memory of stream must be sent to OMXAL completely
    {
      /*
         If Android can fully support XA_ANDROID_ITEMKEY_FORMAT_CHANGE, we can call "break;" here directly;
         But unfortunately, most devices can not fully support it now;
         Just ASUS Transformer Primer 4.0.3 can support it well;
         So I do workaround here;
         I hope we can remove it in future, East 20121217;
       */
      if(nOffsetTime < 500)	// let's end if just 0.5s left since some device can't completely play if we don't send EOS
      {
        VOLOGI("[OMXAL]buffer empty 4, playing %d", nPlayingTime);
        break;
      }

      if(voOS_GetSysTime() > (VO_U32)(m_nLastPosUpdateTime + nPositionUpdatePeriod * 3))	// avoid timeout, won't be here normally
      {
        VOLOGW("[OMXAL]buffer empty 5, playing %d", nPlayingTime);
        break;
      }
    }

    if(!(nWaitCount & 0x7F))	// per 128 times
    {
      VOLOGI("[OMXAL]Waiting...%d, playing %d, mux memory %d", nWaitCount, nPlayingTime, m_pMuxStream ? m_pMuxStream->GetMemStreamSize() : 0);            
    }

    nWaitCount++;
    voOS_Sleep(5);
  }

  return VONP_ERR_None;
}

void COMXALMediaPlayer::DumpRawFrame(bool bAudio, unsigned char* pBuffer, int nSize, long long time)
{
  static int nWrite = 1;

  // check file exist
  if(nWrite == 0)
  {
    VO_S32 nCheckBackDoor = voOS_EnableDebugMode(1);

    if(nCheckBackDoor > 0)
    {
      FILE* hCheck = fopen("/data/local/tmp/rawdata.dat", "rb");
      if(hCheck)
      {
        nWrite = 1;
        fclose(hCheck);
        hCheck = NULL;
      }
      else
        nWrite = -1;
    }
    else
      nWrite = -1;
  }

  if(nWrite == 1)
  {
    FILE* hFile = NULL;

    if(bAudio)
    {
      //VOLOGI("[OMXAL]write audio frame %d", (int)nSize);
      static FILE* hAudio = fopen("/data/local/tmp/audio.aac", "w");

      if(!hAudio)
        VOLOGI("[OMXAL]audio frame dump file failed. %s", "/data/local/tmp/audio.aac");

      hFile = hAudio;
    }
    else
    {
      //VOLOGI("[OMXAL]write video frame %d", (int)nSize);
      static FILE* hVideo = fopen("/data/local/tmp/video.h264", "w");

      if(!hVideo)
        VOLOGI("[OMXAL]video frame dump file failed. %s", "/data/local/tmp/video.h264");

      hFile = hVideo;			
    }

    if(pBuffer && hFile)
    {
   //   fwrite(&nSize, 1, sizeof(nSize), hFile);
   //   fwrite(&time, 1, sizeof(time), hFile);
      nSize &= 0x7FFFFFFF;
      fwrite(pBuffer, 1, nSize, hFile);
    }				
  }	
}


void COMXALMediaPlayer::DumpHeadData(bool bAudio, unsigned char* pBuffer, int nSize)
{
#if 0
  if(!bAudio)
  {
    FILE* hFile = fopen("/data/local/tmp/headdata.h264", "wb");
    if(hFile)
    {
      VOLOGI("[OMXAL]Dump video head data %d", nSize);
      fwrite(pBuffer, 1, nSize, hFile);
      fclose(hFile);                
    }
  }
  else
  {
    FILE* hFile = fopen("/data/local/tmp/headdata.aac", "wb");
    if(hFile)
    {
      fwrite(pBuffer, 1, nSize, hFile);
      fclose(hFile);                
    }
  }
#endif
}


void COMXALMediaPlayer::CopyTrackInfo(bool bAudio, VONP_BUFFER_FORMAT* pDst, VONP_BUFFER_FORMAT* pSrc)
{
  if(bAudio)
  {
    pDst->sFormat.audio.nSampleRate	= pSrc->sFormat.audio.nSampleRate;
    pDst->sFormat.audio.nSampleBits	= pSrc->sFormat.audio.nSampleBits;
    pDst->sFormat.audio.nChannels	= pSrc->sFormat.audio.nChannels;
  }
  else
  {
    pDst->sFormat.video.nWidth	= pSrc->sFormat.video.nWidth;
    pDst->sFormat.video.nHeight	= pSrc->sFormat.video.nHeight;
    pDst->sFormat.video.nType	= pSrc->sFormat.video.nType;
  }

  pDst->nStreamType	= pSrc->nStreamType;
  pDst->nCodec		= pSrc->nCodec;
  pDst->nFourCC		= pSrc->nFourCC;

  if(pSrc->pHeadData && pSrc->nHeadDataSize>0)
  {
    if(pDst->pHeadData)
    {
      delete (unsigned char*)pDst->pHeadData;
      pDst->nHeadDataSize = 0;
    }

    pDst->nHeadDataSize	= pSrc->nHeadDataSize;		
    pDst->pHeadData		= new unsigned char[pSrc->nHeadDataSize];
    memcpy(pDst->pHeadData, pSrc->pHeadData, pSrc->nHeadDataSize);

    if(pSrc->nHeadDataSize >= 2)
      VOLOGI("[OMXAL]Head data: %x %x", ((VO_BYTE*)pSrc->pHeadData)[0], ((VO_BYTE*)pSrc->pHeadData)[1]);

    DumpHeadData(bAudio, (unsigned char*)pDst->pHeadData, pDst->nHeadDataSize);
  }
}


void COMXALMediaPlayer::NotifyBuffering(bool bEnable)
{
  if(bEnable)
  {
    if(m_bBuffering)
      return;

    m_bBuffering = true;
    VOLOGI("[OMXAL]Notify buffering, enable, playing time %d, sys time %d", GetPlayingTime(), voOS_GetSysTime());
    SendEvent(VONP_CB_StartBuff, NULL, NULL);
  }
  else
  {
    if(!m_bBuffering)
      return;

    m_bBuffering = false;
    VOLOGI("[OMXAL]Notify buffering, disable, playing time %d, sys time %d", GetPlayingTime(), voOS_GetSysTime());
    SendEvent(VONP_CB_StopBuff, NULL, NULL);
  }
}


void COMXALMediaPlayer::CheckWrapRunning(void)
{
  int		wrapReadTimeOffset = 0;
  wrapReadTimeOffset = voOS_GetSysTime() - m_nLastWrapReadDataTime;
  if(VONP_STATUS_RUNNING == m_nPlayerStatus && wrapReadTimeOffset >= 4000 && 50 > wrapReadTimeOffset % 1000)
  {
  	  int tsMuxerHoldSpe = 0 ;
	  if(m_VideoTrackInfo.sFormat.video.nHeight != 0 && m_VideoTrackInfo.sFormat.video.nWidth != 0)
		  tsMuxerHoldSpe = m_pMuxStream->GetMemStreamSize() / m_VideoTrackInfo.sFormat.video.nHeight / m_VideoTrackInfo.sFormat.video.nWidth;
	  if((tsMuxerHoldSpe >= 5 || true == m_bNeedFormatChange)&& 50 < voOS_GetSysTime() - lastRunTime)
	  {
		  VOLOGI("[OMXAL]Force set wrap to Run %d Sys %d", m_nLastWrapReadDataTime, voOS_GetSysTime());
		  {
			  Flush();
			  DestroyOMXAL();
			  CreateOMXAL();
			  m_pWrap->Open(&m_ReadBufQueryFromOMXAL, m_pNativeWindow);

			  if(m_pMuxStream)
			  {
				  m_pMuxStream->Flush();
				  m_pMuxStream->Run();
			  }
			  else
				  CreateMemStream();
			  m_bCancelSendRawFrame = false;
			  m_bWaitKeyFrame     = true;
			  m_pWrap->Run();
		  }
		  lastRunTime = voOS_GetSysTime();
	  }
  }
}

bool COMXALMediaPlayer::CheckBuffering()
{
  unsigned int nPeriodTime = 1200;

  if(IsPause())
    return false;
  if(m_nLastPosUpdateTime == 0)
    return false;

  if( (voOS_GetSysTime()-m_nLastPosUpdateTime) > nPeriodTime)
  {
    NotifyBuffering(true);
    return true;
  }
  else
  {
    if(m_bSeeked)
      return false;

    NotifyBuffering(false);
  }

  return false;
}

/*
bool COMXALMediaPlayer::CheckFormatChanged(bool bAudio, VONP_BUFFER_FORMAT* pNewFormat)
{
  bool m_bFormatChange = false;
  if(bAudio)
  {
    if( (m_AudioTrackInfo.sFormat.audio.nSampleBits==pNewFormat->sFormat.audio.nSampleBits) 
        && (m_AudioTrackInfo.sFormat.audio.nChannels==pNewFormat->sFormat.audio.nChannels)
        && (m_AudioTrackInfo.sFormat.audio.nSampleRate==pNewFormat->sFormat.audio.nSampleRate))
    {
      m_bFormatChange = false;
    }
  }
  else
  {
    if( (m_VideoTrackInfo.sFormat.video.nWidth==pNewFormat->sFormat.video.nWidth) 
        && (m_VideoTrackInfo.sFormat.video.nHeight==pNewFormat->sFormat.video.nHeight) )
    {
      m_bFormatChange = false;
    }
  }

  return m_bFormatChange;
}
*/

void COMXALMediaPlayer::CheckFormatChanged(void)
{
  if(true == m_bNeedFormatChange)
  {
	  if(!m_pMuxStream)
		  CreateMemStream();

	  if(m_pMuxStream)
	  {
		  m_pMuxStream->SetVideoOnly(m_bVideoOnly);
		  m_pMuxStream->SetAudioOnly(m_bAudioOnly);
	  }

	  OnFormatChanged();

	  m_semBufQueue.Wait(5000);

	  if(m_pFrameTools)
		  m_pFrameTools->ResetFrameCount();

	  if(!m_bAudioOnly)
		  m_bWaitKeyFrame = true;
	  m_bNeedFormatChange = false;
  }
}

bool COMXALMediaPlayer::CheckVideoDropFrame(VONP_BUFFERTYPE* pBuf)
{    
  if(pBuf->nFlag & VONP_FLAG_BUFFER_KEYFRAME)
    return false;

  if(m_bWaitKeyFrame)
    return false;

  //if(pBuf->llTime < (m_nLastReadAudioTime-1000))
  if(pBuf->llTime < m_nLastBAVideoTime)
  {
    VOLOGW("[OMXAL]video drop frame,time %lld, size %d, flag %d, audio %d, BA %d", pBuf->llTime, pBuf->nSize, pBuf->nFlag, m_nLastReadAudioTime, m_nLastBAVideoTime);
    return true;
  }

  return false;
}

bool COMXALMediaPlayer::CheckVideoKeyFrame(VONP_BUFFERTYPE* pBuf)
{
  if(pBuf->nFlag & VONP_FLAG_BUFFER_KEYFRAME)
  {
    return true;
  }
  else
  {
    if(pBuf->pBuffer && pBuf->nSize>0)
    {
      VO_BOOL bRet = VO_FALSE;

      // here need check codec type
      bRet = IsKeyFrame_H264(pBuf->pBuffer, pBuf->nSize, 0);

      if(bRet)
      {
        pBuf->nFlag |= VONP_FLAG_BUFFER_KEYFRAME;
        VOLOGW("[OMXAL]Found a key frame %lld, %d", pBuf->llTime, pBuf->nSize);
        return true;
      }
    }
  }

  return false;
}


bool COMXALMediaPlayer::SendVideoHeadData(int nTimeStamp)
{
  if(!m_VideoTrackInfo.pHeadData || m_VideoTrackInfo.nHeadDataSize<=0)
    return false;

#if 0
  VONP_BUFFERTYPE buf;
  memset(&buf, 0, sizeof(VONP_BUFFERTYPE));

  buf.llTime	= nTimeStamp;
  buf.pBuffer	= (unsigned char*)m_VideoTrackInfo.pHeadData;
  buf.nSize	= m_VideoTrackInfo.nHeadDataSize;

  VOLOGI("[OMXAL]Send video head data, time %d, size %d", nTimeStamp, m_VideoTrackInfo.nHeadDataSize);
  int nRet = SendRawFrame(false, &buf);
#else
  VO_SINK_SAMPLE sample;
  memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
  sample.Buffer = (unsigned char*)m_VideoTrackInfo.pHeadData;
  sample.Size   = m_VideoTrackInfo.nHeadDataSize;
  sample.nAV    = 1;
  sample.Time   = nTimeStamp;
  sample.DTS    = -1;

#if 0
LOGE("Eoollo_send ,%s %-5d %-5d : 1", "video", (int)sample.Time, (int)sample.Size);
//  if(true == m_bViewChangeing)
    DumpRawFrame(false, sample.Buffer, sample.Size, sample.Time);
#endif

  int nRet = VONP_ERR_Pointer;   

  if(m_pMuxStream)
    nRet = m_pMuxStream->SendHeadData(&sample);
#endif

  return nRet==VONP_ERR_None;
}

bool COMXALMediaPlayer::SendAudioHeadData(int nTimeStamp)
{
#if 1
  if(!m_AudioTrackInfo.pHeadData || m_AudioTrackInfo.nHeadDataSize<=0)
    return false;

  VO_SINK_SAMPLE sample;
  memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
  sample.Buffer = (unsigned char*)m_AudioTrackInfo.pHeadData;
  sample.Size   = m_AudioTrackInfo.nHeadDataSize;
  sample.nAV    = 0;
  sample.Time   = nTimeStamp;
  sample.DTS    = -1;

//  if(true == m_bViewChangeing)
//    DumpRawFrame(false, sample.Buffer, sample.Size, sample.Time);

  int nRet = VONP_ERR_Pointer;   

  if(m_pMuxStream)
    nRet = m_pMuxStream->SendHeadData(&sample);
#else
  if(!m_AudioTrackInfo.pHeadData || m_AudioTrackInfo.nHeadDataSize<=0)
    return false;

  VONP_BUFFERTYPE buf;
  memset(&buf, 0, sizeof(VONP_BUFFERTYPE));

  buf.llTime	= nTimeStamp;
  buf.pBuffer	= (unsigned char*)m_AudioTrackInfo.pHeadData;
  buf.nSize	= m_AudioTrackInfo.nHeadDataSize;

  VOLOGI("[OMXAL]Send audio head data, time %d, size %d", nTimeStamp, m_AudioTrackInfo.nHeadDataSize);
  int nRet = SendRawFrame(true, &buf);
#endif

  return nRet==VONP_ERR_None;	
}

bool COMXALMediaPlayer::CheckH264BFrame(VONP_BUFFERTYPE* pBuf)
{
  //	if(!pBuf->pBuffer || pBuf->nSize<=0)
  //		return false;
  //	
  //	VO_S32 GetFrameType (VO_U8* pInData, VO_S32 nInSize);
  //	
  //	VO_CODECBUFFER buf;
  //	memset(&buf, 0, sizeof(VO_CODECBUFFER));
  //	buf.Buffer	= pBuf->pBuffer;
  //	buf.Length	= pBuf->nSize;
  //	
  //	VO_VIDEO_PARAM_AVCTYPE output;
  //	memset(&output, 0, sizeof(VO_VIDEO_PARAM_AVCTYPE));
  //	getVIDEO_PARAM_AVCTYPE_H264(&buf, &output);
  //
  //	VOLOGI("nPFrames %d, nBFrames %d, profile %d, type %d", output.nPFrames, output.nBFrames, output.eProfile, GetFrameType(pBuf->pBuffer, pBuf->nSize));

  return false;
}

bool COMXALMediaPlayer::CheckSeekMode()
{
  //voCAutoLock lock(&m_mtStatus);

  if(!m_bSeeked || m_bPauseReadBuf || IsStop())
  {
    return false;
  }

  //Wait until read a buffer
  bool bTimeout = false;
  int nTryCount = 0;
  while(m_nLastReadAudioTime==0 || (m_nLastReadVideoTime==0 && !m_bAudioOnly))
  {
    nTryCount++;

    if(m_bGotNewFormatBuf)
      break;

    if(IsStop())
      return false;
    if(m_nStatusEOS&0x01 || m_nStatusEOS&0x02)
      break;

    //VOLOGW("[OMXAL]Waiting reading buffer, last audio %d, last video %d, new format %d...", m_nLastReadAudioTime, m_nLastReadVideoTime, m_bGotNewFormatBuf?1:0);
    //if(nTryCount >= 5000)
    if(nTryCount >= 100)
    {
      bTimeout = true;
      break;
    }

    voOS_Sleep(10);
  }

  if(bTimeout)
  {
    VOLOGW("[OMXAL]Reading buffer timeout after seek, %d", voOS_GetSysTime());
    return false;
  }

  if(m_nStatusEOS&0x01 || m_nStatusEOS&0x02)
  {
    VOLOGW("[OMXAL]Play complete by seek to end");
    m_bSeeked = false;
    SendEOS();
    m_bViewChanged = false;
    return false;
  }	

  if(m_bGotNewFormatBuf)
  {
    VOLOGW("[OMXAL]Seek with new format,last audio %d, last video %d", m_nLastReadAudioTime, m_nLastReadVideoTime);

    if(m_pWrap)
    {
      m_pWrap->SetPos(m_nSeekPos);

      if(m_bSeekWhenPaused)
      {
        m_pWrap->Run();
        m_bSeekWhenPaused = false;
      }
    }

    m_nPlayingTime  = m_nSeekPos;
    m_bViewChanged  = false;
    m_bSeeked       = false;

    SendEvent(VONP_CB_SeekComplete, NULL, NULL);
    return true;
  }
  else// seeking not using new format,for example local file playback
  {
    VOLOGW("[OMXAL]Seek without new format,last audio %d, last video %d, %d", m_nLastReadAudioTime, m_nLastReadVideoTime, m_bViewChanged?1:0);

    // flush OMX AL inner
    if(!m_bViewChanged)
    {
      m_pWrap->SetPos(m_nSeekPos);
    }

    if(m_bViewChanged)
    {
      VOLOGI("[OMXAL]Send video audio head data, %d %d", m_nLastReadAudioTime, m_nLastReadAudioTime);
      //SendAudioHeadData(m_nLastReadAudioTime);
      //SendVideoHeadData(m_nLastReadAudioTime);
    }

    if(m_pMuxStream)
      m_pMuxStream->Run();

    if(m_bSeekWhenPaused)
    {
      if(m_pWrap)
        m_pWrap->Run();
      m_bSeekWhenPaused = false;
    }

    if(m_bHasBeenAudioOnly)
      m_bUseOutsideRender = true;

    m_nPlayingTime = m_nSeekPos;
    m_bSeeked = false;

    SendEvent(VONP_CB_SeekComplete, NULL, NULL);
    m_bViewChanged = false;
    return false;
  }	

  return false;
}

void COMXALMediaPlayer::CheckCPUInfo()
{
  VO_CPU_Info info;
  get_cpu_info(&info);

  m_sCPUInfo.nCPUType     = 1;
  m_sCPUInfo.nCoreCount   = info.mCount;
  m_sCPUInfo.nFrequency   = info.mMaxCpuSpeed;
  m_sCPUInfo.llReserved   = info.mMinCpuSpeed;

  VOLOGI("[OMXAL]CPU info: core count %d, max %d, min %lld, neon %d", m_sCPUInfo.nCoreCount, m_sCPUInfo.nFrequency, m_sCPUInfo.llReserved, m_sCPUInfo.nCPUType);
}

bool COMXALMediaPlayer::CheckH264Resolution(VONP_BUFFERTYPE* pBuf)
{
    VO_VIDEO_HEADDATAINFO info;
    memset(&info, 0, sizeof(VO_VIDEO_HEADDATAINFO));
    
    VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)pBuf->pData;
    
    if(!pFmt)
        return false;
    
    VO_CODECBUFFER buf;
    memset(&buf, 0, sizeof(VO_CODECBUFFER));
    buf.Buffer	= (unsigned char*)pFmt->pHeadData;
    buf.Length	= pFmt->nHeadDataSize;
    
    int nRet = getResolution_H264(&buf, &info);
    
    if(0 == nRet)
    {
        VOLOGI("[OMXAL]Check video format: %lux%lu", info.Width, info.Height);
        
        if(m_fmtVideo.nWidth!=info.Width || m_fmtVideo.nHeight!=info.Height)
        {
            m_fmtVideo.nWidth   = info.Width;
            m_fmtVideo.nHeight  = info.Height;
            
            if(m_bFirstRun)
            {
                SendEvent(VONP_CB_VideoSizeChanged, &m_fmtVideo.nWidth, &m_fmtVideo.nHeight);
                return true;
            }
            
            BA_CHANGE_INFO* pInfo = new BA_CHANGE_INFO;
            memset(pInfo, 0, sizeof(BA_CHANGE_INFO));
            pInfo->bNeedVideoSizeUpdated    = true;
            pInfo->nVideoWidth              = m_fmtVideo.nWidth;
            pInfo->nVideoHeight             = m_fmtVideo.nHeight;
            pInfo->llUpdateTimestamp        = pBuf->llTime;
            
            m_sBAChangeInfoList.AddTail(pInfo);
        }
    }
	else
	{
		VOLOGE("[OMXAL]getResolution_H264 fail %d, buf 0x%08X, size %d", nRet, pBuf->pBuffer, pBuf->nSize);
		return VONP_ERR_Unknown;
	}

    return nRet==0?VONP_ERR_None:VONP_ERR_Unknown;
}

int COMXALMediaPlayer::PeekFlushBuffer(VONP_BUFFERTYPE* pBuffer, bool& bReadAudio)
{
  if(m_nStatusEOS&0x01 && m_nStatusEOS&0x02)
  {
    VOLOGW("[OMXAL]Reading buf reach EOS 01");
    return -1;
  }

  int nTryCount = 0;
  m_bReadFlushBuffer	= false;
  int nReadRet		= VONP_ERR_Retry;

  while( (nReadRet!=VONP_ERR_None) && (nReadRet!=VONP_ERR_FLush_Buffer) )
  {
    VOLOGW("[OMXAL]Peek %s flush buffer...", bReadAudio?"audio":"video");

    nTryCount++;
    memset(pBuffer, 0, sizeof(VONP_BUFFERTYPE));

    if(bReadAudio || m_bAudioOnly)
    {
      if(m_pReadBufFunc)
        nReadRet = m_pReadBufFunc->ReadAudio(m_pReadBufFunc->pUserData, pBuffer);
    }
    else
    {
      if(m_pReadBufFunc)
        nReadRet = m_pReadBufFunc->ReadVideo(m_pReadBufFunc->pUserData, pBuffer);
    }

    if( (nReadRet==VONP_ERR_None) || (nReadRet==VONP_ERR_FLush_Buffer) )
      break;

    if(nReadRet == VONP_ERR_EOS)
    {
      VOLOGW("[OMXAL]Reading buf reach EOS 00");
      m_nStatusEOS |= 0x01;
      m_nStatusEOS |= 0x02;
      return -1;
    }

    //if(nTryCount >= 1000*30)
    if(nTryCount >= 300)
      return -1;

    voOS_Sleep(2);
    bReadAudio = !bReadAudio;
  }

  if(nReadRet == VONP_ERR_FLush_Buffer)
  {
    VOLOGW("[OMXAL]++++++Got flush buffer, %x %d++++++", pBuffer->pBuffer, pBuffer->nSize);

    //
    if(m_pMuxStream)
    {
      // sometimes ts mux will crash if not call it when try it destroy it
      m_pMuxStream->ForceOutputWholeStream();
      DestroyMemStream();
    }

    int nRC = VONP_ERR_None;
    nRC = CreateMemStream();
    if(VONP_ERR_None != nRC)
      return nRC;
    m_pMuxStream->Run();

    //
    DestroyOMXAL();

    CreateOMXAL();

    m_pWrap->Open(&m_ReadBufQueryFromOMXAL, m_pNativeWindow);

    m_bReadFlushBuffer	= true;

    return 0;
  }
  else if(nReadRet == VONP_ERR_None)
  {
    VOLOGW("[OMXAL]++++++Not got flush buffer, flag %d++++++", pBuffer->nFlag);
    return 1;
  }

  return -1;
}

bool COMXALMediaPlayer::CreateOMXAL()
{
  m_pWrap = new COMXALWrapper(&m_EventCallbackFromOMXAL);
  if(!m_pWrap)
  {
    VOLOGE("[OMXAL]new COMXALWrapper fail!!");
    return false;
  }

  if(VONP_ERR_None != m_pWrap->Init())
  {
    VOLOGE("[OMXAL]m_pWrap init fail!!");
    return false;
  }

  return true;
}

void COMXALMediaPlayer::DestroyOMXAL()
{
  if(m_pWrap)
  {
    m_pWrap->Uninit();

    delete m_pWrap;
    m_pWrap = NULL;
  }
}

void COMXALMediaPlayer::CheckFlushBuffer(VONP_BUFFERTYPE* pBuf)
{
  if(m_bReadFlushBuffer)
  {
    SendAudioHeadData(pBuf->llTime);
    if(!m_bAudioOnly){
      SendVideoHeadData(pBuf->llTime);
    }

    m_bReadFlushBuffer      = false;
    m_nPlayingTime          = 0;
    m_nPosOffsetTime        = pBuf->llTime;
  }	
}

bool COMXALMediaPlayer::CheckAudioDropFrame(VONP_BUFFERTYPE* pBuf)
{
  if(pBuf->llTime < m_nLastAudioNewFormatTime)
  {
    VOLOGW("[OMXAL]Audio drop frame,time %lld, size %d, flag %d, last audio new format %d", pBuf->llTime, pBuf->nSize, pBuf->nFlag, m_nLastAudioNewFormatTime);
    return true;
  }
  else
  {
  }

  return false;
}

bool COMXALMediaPlayer::isLegalADTSFrame(unsigned char * pData, int nSize, int * pnActualSize)
{
  if(NULL == pData || nSize < 7)	// ADTS frame header is 7
  {
    VOLOGW("[OMXAL]illegal ADTS frame 0x%08X size %d", pData, nSize);
    return false;
  }

  if(0xFF != pData[0] || 0xF0 != (pData[1] & 0xF0))
  {
    VOLOGW("[OMXAL]illegal ADTS key flag 0x%02X 0x%02X", pData[0], pData[1]);
    return false;
  }

  if(pnActualSize)
    *pnActualSize = ((pData[3] & 0x3) << 11) + (pData[4] << 3) + (pData[5] >> 5);

  return true;
}

bool COMXALMediaPlayer::CheckValidADTSFrame(VONP_BUFFERTYPE* pBuf)
{
  int nFrameLen = 0;
  if(!isLegalADTSFrame(pBuf->pBuffer, pBuf->nSize, &nFrameLen))
  {
    VOLOGW("[OMXAL]illegal ADTS buffer 0x%08X size %d time %lld", pBuf->pBuffer, pBuf->nSize, pBuf->llTime);
    return false;
  }

  if(pBuf->nSize < nFrameLen)
  {
    VOLOGW("[OMXAL]ADTS buffer is too small %d %d", pBuf->nSize, nFrameLen);
    return false;
  }
  else if(pBuf->nSize > nFrameLen)
  {
    // check next ADTS frame, if illegal, drop all left buffer
    int nNextFrameLen = 0;
    if(!isLegalADTSFrame(pBuf->pBuffer + nFrameLen, pBuf->nSize - nFrameLen, &nNextFrameLen))
      pBuf->nSize = nFrameLen;
    else if(pBuf->nSize < (nFrameLen + nNextFrameLen))
      pBuf->nSize = nFrameLen;
  }

  return true;
}

bool COMXALMediaPlayer::getNativeWindow()
{
  if(!m_pJavaEnv || !m_pJavaSurface)
    return false;

  CJniEnvUtil	env((JavaVM *)m_pJavaEnv);
  if(!env.getEnv())
    return false;

  // obtain a native window from a Java surface
  m_pNativeWindow = ANativeWindow_fromSurface(env.getEnv(), (jobject)m_pJavaSurface);

  VOLOGI("[OMXAL]Native window %x", (unsigned int)m_pNativeWindow);
  return m_pNativeWindow ? true : false;
}

void COMXALMediaPlayer::releaseNativeWindow()
{
  if(m_pNativeWindow)
  {
    ANativeWindow_release(m_pNativeWindow);
    m_pNativeWindow = NULL;
  }
}

bool COMXALMediaPlayer::ProcessViewChanged(void * pNewView)
{    
  if(m_bViewAvailable)
  {
    VOLOGW("[OMXAL]Wrong status");
    return false;
  }
  m_bViewChangeing = true;

  m_bViewAvailable = true;

  releaseNativeWindow();
  m_pJavaSurface = pNewView;
  getNativeWindow();

  //if(m_bUseOutsideRender)
  //return true;

  VOLOGW("[OMXAL]+Process view changed, playing time %d, sys %d", GetPlayingTime(), voOS_GetSysTime());

  //m_bPauseReadBuf	= true;
  //WaitBufferEmptyAfterGoForeground();

  m_nPlayingTimeViewChanged = GetPlayingTime();
  m_bViewChanged  = true;

  bool bRunning = IsRunning();

  if(bRunning)
    Pause();

  //to exit doReadBufferFromOMXAL
  m_nPlayerStatus = VONP_STATUS_STOPPED;

  m_bGetFirstTimeAfterViewChanged = true;
  m_bCancelSendRawFrame   = true;

  m_bReadAudioNewFormat = false;
  m_bReadVideoNewFormat = false;
  m_nForceReadAudioNewFmt = 0;
  m_nForceReadVideoNewFmt = 0;
  m_bAudioStatusOK = false;

  //
  DestroyOMXAL();

  CreateOMXAL();
  m_pWrap->Open(&m_ReadBufQueryFromOMXAL, m_pNativeWindow);

  //		
  //voOS_Sleep(50);
  if(m_pMuxStream)
  {
    m_pMuxStream->Flush();
  }

  //    DestroyMemStream();
  //    CreateMemStream();

  m_pMuxStream->Run();

  m_bCancelSendRawFrame = false;

  m_bWaitKeyFrame     = true;

  m_nPlayingTime = m_nPlayingTimeViewChanged;

  if(bRunning)
    Run();

  VOLOGW("[OMXAL]-Process view changed, last A %d, last video %d, %d", m_nLastReadAudioTime, m_nLastReadVideoTime, m_nPosOffsetTime);

  return true;
}

bool COMXALMediaPlayer::CheckFirstTimeStampViewChanged(int nTimeStamp)
{
  if(m_bGetFirstTimeAfterViewChanged)
  {
    m_nPlayingTime = 0;
    m_nPosOffsetTime = nTimeStamp;
    m_bGetFirstTimeAfterViewChanged = false;
    return true;
  }

  return false;
}

int COMXALMediaPlayer::CreateMemStream()
{
  m_pMuxStream = new CMuxStream(m_pLibOP, &m_AudioFormat, m_pPlayerPath);

  m_pMuxStream->SetMaxMemStreamSize(0x100000);	// 1M

  if(m_bEnableAudioEffect)
    m_pMuxStream->OnCodecTypeChanged(true, VONP_AUDIO_CodingPCM);

  if(m_bVideoOnly)
  {
    VOLOGW("[OMXAL]Set video only into mux");
    m_pMuxStream->SetCacheDuration(0);
  }
  else
  {
    //m_pMuxStream->SetCacheDuration(1000);        
  }

  int nRC = m_pMuxStream->Init();
  m_pMuxStream->SetMaxMemStreamSize(0X1E00000);     

  return nRC;
}

bool COMXALMediaPlayer::DestroyMemStream()
{
  if(m_pMuxStream)
  {
    m_pMuxStream->Stop();
    m_pMuxStream->Uninit();
    delete m_pMuxStream;
    m_pMuxStream = NULL;        
  }

  return true;
}

int COMXALMediaPlayer::GetAudioBuffer(VONP_BUFFERTYPE** ppBuffer)
{
  //VOLOGI("[OMXAL]GetAudioBuffer %d", voOS_GetSysTime());

  int nReadRet = VONP_ERR_Retry;

  if(!m_bUseOutsideRender || !IsCanReadBuffer(true) || !m_pReadBufFunc || IsPause() || IsStop() || m_bNeedFormatChange)
  {
    voOS_Sleep(5);
    return VONP_ERR_Retry;
  }

  voCAutoLock lock(&m_mtStatus);

  VONP_BUFFERTYPE* pBuffer = *ppBuffer;

  if (pBuffer != NULL && pBuffer->pBuffer != NULL)
  {
    if (pBuffer->nSize < (int)m_nAudioStepSize)
      return VONP_ERR_SmallSize;

    m_pAudioOutBuff = pBuffer->pBuffer;
    m_nAudioBufferSize = pBuffer->nSize;
  }
  else
  {
    m_pAudioOutBuff = m_pAudioPcmBuff;
  }

  //create audio decoder
  CreateAudioDecoder(VO_AUDIO_CodingAAC, &m_AudioFormat);

  //read audio buffer
  VONP_BUFFERTYPE buf;
  memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
  nReadRet = m_pReadBufFunc->ReadAudio(m_pReadBufFunc->pUserData, &buf);

  //VOLOGI("READ AUDIO, flag %x, %08lld, size %06d, buf %x, data %x, ret %d, audio %d,video %d", buf.nFlag, buf.llTime, buf.nSize, (int)buf.pBuffer, (int)buf.pData, nReadRet, m_bAudioOnly, m_bVideoOnly);

  if(nReadRet == VONP_ERR_Retry)
  {
    //if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
    {
      VOLOGW("[OMXAL]Read audio retry...");
      voOS_Sleep(2);
    }
    return VONP_ERR_Retry;
  }
  else if(nReadRet == VONP_ERR_Audio_No_Now)
  {
    VOLOGW("[OMXAL]Read audio inavailable");
  }
  else if(nReadRet == VONP_ERR_EOS)
  {
    VOLOGW("[OMXAL]Reading audio buf reach EOS");
    m_nStatusEOS |= 0x01;

    if(m_bAudioOnly || m_bHasBeenAudioOnly)
      m_nStatusEOS |= 0x02;

    m_nForceReadAudioNewFmt = 0;
  }						
  else if(nReadRet == VONP_ERR_None)
  {
    if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
    {
      VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)buf.pData;

      if(buf.pData)
      {
        VOLOGI("[OMXAL]Read audio new format, codec %d, S %d, C %d, B %d, flag %d, time %lld, playing %d, last audio %d", pFmt->nCodec, pFmt->sFormat.audio.nSampleRate, pFmt->sFormat.audio.nChannels, pFmt->sFormat.audio.nSampleBits, buf.nFlag, buf.llTime, GetPlayingTime(), m_nLastReadAudioTime);

		ResetPeekBuffer();
		//if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
		{
			m_peekBufInfo.bPeeking      = true;
			m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_NEW_FORMAT;
		}
		PeekBuffer(&buf);
		m_peekBufInfo.bPeeking = false;

        if(!CheckCodecSupport(true, pFmt->nCodec))
        {
          VOLOGI("[OMXAL]+++++++++++Audio format NOT support. %d+++++++++++++++", pFmt->nCodec);
          m_bSupportAudioCodec = false;
          SendEvent(VONP_CB_Codec_NotSupport, NULL, NULL);
          voOS_Sleep(2);
          return VONP_ERR_Retry;
        }

        m_nForceReadAudioNewFmt = 0;
        m_nLastAudioNewFormatTime = buf.llTime;
        m_bGotNewFormatBuf		= true;
        m_bSupportAudioCodec	= true;

        CopyTrackInfo(true, &m_AudioTrackInfo, pFmt);

        m_bReadAudioNewFormat		= true;

        //if(buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
        //nPosOffsetTime = 0;

        if(m_bAudioOnly)
          m_bHasBeenAudioOnly = true;
        m_bAudioOnly = false;
        m_bVideoOnly = false;

        VOLOGI("[OMXAL]ready to read video new format flag");
        m_nForceReadVideoNewFmt = 60;

        //playback left PCM buffer
        if(m_nAudioOutSize > 0)
        {
          VOLOGW("[OMXAL]Render left PCM buffer");
          m_nPlayingTime = buf.llTime;

          if(pBuffer != NULL && pBuffer->pBuffer != NULL)
          {
            pBuffer->nSize     = m_nAudioOutSize;
            pBuffer->llTime    = buf.llTime;
          }
          else if(pBuffer != NULL)
          {
            pBuffer->nSize      = m_nAudioOutSize;
            pBuffer->llTime     = buf.llTime;
            pBuffer->pBuffer    = m_pAudioOutBuff;
          }

          m_nAudioOutSize = 0;
        }
		m_bUseOutsideRender         = false;
      }
    }
    else if(buf.pBuffer && buf.nSize>0 && m_bSupportAudioCodec)
    {		
      //CheckFlushBuffer(&buf);
      m_nLastReadAudioTime = buf.llTime;

      if(CheckAudioDropFrame(&buf))
      {
        return VONP_ERR_Retry;
      }				

      if(m_bReadAudioNewFormat)
      {
        // if both audio and video, set false when sending video head data
        //bUseAudioOffsetTime = true;
      }

      nReadRet = DoAudioDecoder(&buf);

      if(VONP_ERR_None == nReadRet)
      {
        m_nLastPosUpdateTime	= voOS_GetSysTime();

        if (m_nAudioOutSize < m_nAudioStepSize)
        {
          //VOLOGI("[OMXAL]PCM output buffer size less than step size, %d %d", m_nAudioOutSize, m_nAudioStepSize);
          return VONP_ERR_Retry;
        }
        else
        {
          //VOLOGI("[OMXAL]Output PCM,size %d, time %d", m_nAudioOutSize, buf.llTime);
          //VOLOGI("%x %x %x %x %x %x %x %x", m_pAudioOutBuff[0], m_pAudioOutBuff[1], m_pAudioOutBuff[2], m_pAudioOutBuff[3], m_pAudioOutBuff[4], m_pAudioOutBuff[5], m_pAudioOutBuff[6], m_pAudioOutBuff[7]);

          m_nPlayingTime = buf.llTime;

          if (pBuffer != NULL && pBuffer->pBuffer != NULL)
          {
            pBuffer->nSize     = m_nAudioOutSize;
            pBuffer->llTime    = buf.llTime;
          }
          else if(pBuffer != NULL)
          {
            pBuffer->nSize      = m_nAudioOutSize;
            pBuffer->llTime     = buf.llTime;
            pBuffer->pBuffer    = m_pAudioOutBuff;
          }

          //reset output buffer size
          m_nAudioOutSize = 0;
        }
      }
      else if(nReadRet == VONP_ERR_Format_Changed)
      {
        VOLOGW("[OMXAL]Update PCM buffer");
        CreatePCMBuffer();
      }
    }
  }
  else // read failed
  {
    //VOLOGW("[OMXAL]read audio NULL - %d", nReadRet);
    //voOS_Sleep(10);
  }

  return nReadRet;
}


int COMXALMediaPlayer::GetVideoBuffer(VONP_BUFFERTYPE** ppBuffer)
{
  voCAutoLock lock(&m_mtStatus);

  VOLOGR("[OMXAL]GetVideoBuffer %x", ppBuffer);

  voOS_Sleep(5);
  return VONP_ERR_Retry;
}


int COMXALMediaPlayer::CreatePCMBuffer()
{
  DestroyPCMBuffer();

  m_nAudioBufferSize  = m_AudioFormat.nSampleRate * m_AudioFormat.nChannels * m_AudioFormat.nSampleBits / 8;
  m_nAudioStepSize    = m_nAudioBufferSize  * m_nAudioStepTime / 1000;
  if(m_pAudioPcmBuff == NULL)
    m_pAudioPcmBuff = new unsigned char[m_nAudioBufferSize];

  m_pAudioOutBuff             = m_pAudioPcmBuff;
  m_nMaxPCMOutputBufSize		= m_nAudioBufferSize;	
  m_AudioDecOutputBuf.Buffer	= m_pAudioOutBuff;
  m_AudioDecOutputBuf.Length	= m_nAudioBufferSize;	
  m_nAudioOutSize             = 0;

  return 0;
}


int COMXALMediaPlayer::DestroyPCMBuffer()
{
  if(m_pAudioPcmBuff)
  {
    delete []m_pAudioPcmBuff;
    m_pAudioPcmBuff = NULL;
  }

  m_nAudioOutSize = 0;
  m_pAudioOutBuff = NULL;
  memset(&m_AudioDecOutputBuf, 0, sizeof(VO_CODECBUFFER));

  return VONP_ERR_None;
}

void COMXALMediaPlayer::SendEOS()
{
  SendEvent(VONP_CB_PlayComplete, NULL, NULL);

  //reset EOS to support loop playback,it will call SetPos(0)
  m_nStatusEOS = 0;
}


void COMXALMediaPlayer::CreateFrameTools()
{
  if(m_pFrameTools)
    return;

  m_pFrameTools = new CAVFrameTools;
  m_pFrameTools->Init();
}

void COMXALMediaPlayer::DestroyFrameTools()
{
  if(m_pFrameTools)
  {
    m_pFrameTools->Uninit();
    delete m_pFrameTools;
    m_pFrameTools = NULL;
  }
}

int COMXALMediaPlayer::CheckAspectRatio(VONP_BUFFERTYPE* pBuf)
{
    int nRet            = VONP_ERR_Unknown;
    int nAspectRatio    = -1;
    
    VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)pBuf->pData;
    
    if(!pFmt)
        return false;
    
    VONP_BUFFERTYPE buf;
    memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    buf.pBuffer     = (unsigned char*)pFmt->pHeadData;
    buf.nSize       = pFmt->nHeadDataSize;
    
    if(m_pFrameTools)
        nRet = m_pFrameTools->GetAspectRatio(&buf, &nAspectRatio);;
    
    if(nRet == VONP_ERR_None && nAspectRatio != -1)
    {
        if(m_nAspectRatio != nAspectRatio)
        {
            VOLOGI("[OMXAL]Found new aspect ratio %d", nAspectRatio);
            m_nAspectRatio = nAspectRatio;
            
            if(m_bFirstRun)
            {
                SendEvent(VONP_CB_VideoAspectRatio, &nAspectRatio, NULL);
                return nAspectRatio;
            }
            
            BA_CHANGE_INFO* pInfo = new BA_CHANGE_INFO;
            memset(pInfo, 0, sizeof(BA_CHANGE_INFO));
            pInfo->bNeedAspectRatioUpdated   = true;
            pInfo->nAspectRatio              = nAspectRatio;
            pInfo->llUpdateTimestamp         = pBuf->llTime;
            
            m_sBAChangeInfoList.AddTail(pInfo);
        }
    }
    else
    {
        VOLOGW("[OMXAL]Aspect ratio failed,%d", nAspectRatio);
    }
    
    return nAspectRatio;
}

bool COMXALMediaPlayer::CheckValidBA(int nBATime)
{
  //temp disable this feature
  return true;

  if(m_nLastBATime == -1)
    return true;

  if(abs(nBATime-m_nLastBATime) < 2000)
  {
    VOLOGW("[OMXAL]++++++++++++++++++++++++++++Maybe invalid BA, new %d, old %d", nBATime, m_nLastBATime);
    return false;
  }

  return true;
}

void COMXALMediaPlayer::DestroyBAInfo()
{
    BA_CHANGE_INFO* pInfo = m_sBAChangeInfoList.RemoveHead();
    
    while (pInfo)
    {
        //VOLOGI("[OMXAL]Destroy BA info: %lld", pInfo->llUpdateTimestamp);
        
        delete pInfo;
        pInfo = m_sBAChangeInfoList.RemoveHead();
    }
}

bool COMXALMediaPlayer::CheckBAInfoChanged()
{
    bool bRet = false;
    
    POSITION pos = m_sBAChangeInfoList.GetHeadPosition();
    
    while(pos)
    {
        BA_CHANGE_INFO* pInfo = m_sBAChangeInfoList.Get(pos);
        
        if(pInfo)
        {
            if(pInfo->bNeedVideoSizeUpdated && GetPlayingTime()>=pInfo->llUpdateTimestamp)
            {
                VOLOGI("[OMXAL]Notify res change: %dx%d", pInfo->nVideoWidth, pInfo->nVideoHeight);
                pInfo->bNeedVideoSizeUpdated = false;
                SendEvent(VONP_CB_VideoSizeChanged, &pInfo->nVideoWidth, &pInfo->nVideoHeight);
                bRet = true;
            }

            if(pInfo->bNeedAspectRatioUpdated && pInfo->llUpdateTimestamp>=GetPlayingTime())
            {
                VOLOGI("[OMXAL]Notify aspect ratio chaged: %d", pInfo->nAspectRatio);
                pInfo->bNeedAspectRatioUpdated = false;
                SendEvent(VONP_CB_VideoAspectRatio, &pInfo->nAspectRatio, NULL);
                bRet = true;
            }
        }
        
        pos = m_sBAChangeInfoList.Next(pos);
    }

    return bRet;
}

void COMXALMediaPlayer::EnaleDumpRawFrame(bool bEnable)
{
    if(!bEnable)
        return;
    
    static int nWrite = 0;
	
	// check file exist
	if(nWrite == 0)
	{
		VO_S32 nCheckBackDoor = voOS_EnableDebugMode(1);
		
		if(nCheckBackDoor > 0)
		{
#if defined(_IOS) || defined(_MAC_OS)
            char szDir[1024];
            voOS_GetAppFolder(szDir, 1024);
            strcat(szDir, "voDebugFolder/");
            strcat(szDir, "raw_data.dat");
            FILE* hCheck = fopen(szDir, "rb");
#else
			FILE* hCheck = fopen("/data/local/tmp/raw_data.dat", "rb");
#endif
            
			if(hCheck)
			{
				nWrite = 1;
				fclose(hCheck);
				hCheck = NULL;
			}
			else
				nWrite = -1;
		}
		else
		{
			nWrite = -1;
		}
	}

    if(nWrite == 1)
    {
        //This will dump H264/AAC frame
        char szTmp[1024];
        memset(szTmp, 0, 1024);
        voOS_GetAppFolder(szTmp, 1024);
        strcat(szTmp, "/voDebugFolder/raw_frame_debug");
        //m_AVFrameWriter.Init(szTmp);
    }
    
    
    
#if 0 //This will verify if dump frame is correct
    char szTmp[1024];
    memset(szTmp, 0, 1024);
    voOS_GetAppFolder(szTmp, 1024);
    strcat(szTmp, "/voDebugFolder/raw_frame_debug");
    m_AVFrameReader.Init(szTmp);
    
    unsigned char szBuf[512*1024];
    int nSize           = 0;
    long long llTime    = 0;
    bool bKeyFrame      = false;
    bool bHeadData      = false;
    bool bAudioEOS      = false;
    bool bVideoEOS      = false;
    
    while (!bAudioEOS || !bVideoEOS)
    {
        //read audio
        if(!bAudioEOS)
            m_AVFrameReader.Read(true, szBuf, nSize, llTime, bKeyFrame, bHeadData);
        
        if(nSize <= 0)
            bAudioEOS = true;
        
        //read video
        if(!bVideoEOS)
            m_AVFrameReader.Read(false, szBuf, nSize, llTime, bKeyFrame, bHeadData);
        
        if(nSize <= 0)
            bVideoEOS = true;
    }
#endif
}

long long COMXALMediaPlayer::GetLastReadTime()
{
    if(m_bAudioOnly)
        return m_nLastReadAudioTime;

    if(m_bVideoOnly)
        return m_nLastReadVideoTime;
    
    return m_nLastReadAudioTime;
}

int COMXALMediaPlayer::OnReadBufEOS()
{
    m_bAudioOnly = false;
    m_bVideoOnly = false;
    
    return VONP_ERR_None;
}

VONP_BUFFERTYPE* COMXALMediaPlayer::CloneBuffer(VONP_BUFFERTYPE* pBuf)
{
    VONP_BUFFERTYPE* pClone = new VONP_BUFFERTYPE;
    memset(pClone, 0, sizeof(VONP_BUFFERTYPE));
    
    pClone->nSize       = pBuf->nSize;
    pClone->llTime      = pBuf->llTime;
    pClone->nFlag       = pBuf->nFlag;
    
    // we use llReserve to save ReadBuffer return code,here has issue if llReserve is used
    if(pBuf->llReserve > 0)
        VOLOGE("[OMXAL]Reserve is used!!!");
    //pClone->llReserve   = pBuf->llReserve;
    pClone->llReserve   = VONP_ERR_None;

    if(pBuf->nSize>0 && pBuf->pBuffer)
    {
        pClone->pBuffer = new unsigned char[pBuf->nSize];
        memcpy(pClone->pBuffer, pBuf->pBuffer, pBuf->nSize);
    }
    
    // note,here not clone pData
    pClone->pData = NULL;
    
    return pClone;
}

VONP_BUFFERTYPE* COMXALMediaPlayer::CloneSilentBuffer(VONP_BUFFERTYPE* pBuf)
{
    VONP_BUFFERTYPE* pClone = CloneBuffer(pBuf);
 
//#if 0
//    int nSilentSize = 1024*2*m_pFrameTools->GetChannelCount(pBuf) + ADTS_HEADER_LEN;
//    if(pClone->nSize < nSilentSize)
//    {
//        delete []pClone->pBuffer;
//        pClone->pBuffer = new unsigned char[nSilentSize];
//        // must copy ADTS head data
//        memcpy(pClone->pBuffer, pBuf->pBuffer, ADTS_HEADER_LEN);
//        
//        unsigned char* pHead = pClone->pBuffer+5;
//        *pHead = (nSilentSize>>3)&0xff;
//        pHead++;
//        *pHead = ((nSilentSize&0x07)<<5|0x1f);
//    }
//    pClone->nSize = nSilentSize;
//#endif
    
    //memset(pClone->pBuffer+ADTS_HEADER_LEN, 0, pClone->nSize-ADTS_HEADER_LEN);

    return pClone;
}


void COMXALMediaPlayer::ReleasePeekBuffer(VONP_BUFFERTYPE* pBuf)
{
    if (pBuf)
    {
        VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)pBuf->pData;
        
        if(pFmt)
        {
            if(pFmt->pHeadData)
            {
                delete [](unsigned char*)pFmt->pHeadData;
                pFmt->pHeadData = NULL;
            }
            
            delete pFmt;
            pFmt = NULL;
            
            pBuf->pData = NULL;
        }
        
        if(pBuf->pBuffer)
        {
            delete []pBuf->pBuffer;
            pBuf->pBuffer = NULL;
        }
        
        delete pBuf;
    }
}


void COMXALMediaPlayer::ReleasePeekBuffer(CObjectList<VONP_BUFFERTYPE>* pList)
{
    if(!pList)
        return;
    
    VONP_BUFFERTYPE* pBuf = (VONP_BUFFERTYPE*)pList->RemoveHead();
    
    while (pBuf)
    {
        ReleasePeekBuffer(pBuf);        
        pBuf = (VONP_BUFFERTYPE*)pList->RemoveHead();
    }
    
    //pList->RemoveAll();
}

void COMXALMediaPlayer::ResetPeekBuffer()
{
    //VOLOGI("[OMXAL]Reset peek buffer");
    
    m_peekBufInfo.sAudioBuf.bPeekBufAvailable  = false;
    m_peekBufInfo.sVideoBuf.bPeekBufAvailable  = false;
    
    //VOLOGI("[OMXAL]%d, %d, %d", m_peekBufInfo.sAudioBuf.sList.GetCount(), m_peekBufInfo.sVideoBuf.sList.GetCount(), m_peekBufInfo.sRecycleBuf.GetCount());
    //release audio peek buffer
    ReleasePeekBuffer(&m_peekBufInfo.sAudioBuf.sList);
    
    //release video peek buffer
    ReleasePeekBuffer(&m_peekBufInfo.sVideoBuf.sList);
    
    ReleasePeekBuffer(&m_peekBufInfo.sRecycleBuf);
}

bool COMXALMediaPlayer::PeekBuffer(VONP_BUFFERTYPE* pBuf)
{
    VOLOGI("[OMXAL]+Peek buffer: trigger %d, time %lld. last audio %d, last video %d\n",
           m_peekBufInfo.nPeekTrigger, pBuf->llTime, m_nLastReadAudioTime, m_nLastReadVideoTime);

    ResetPeekBuffer();
 
    VONP_BUFFERTYPE buf;
    memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    
#ifdef _VOLOG_INFO
    int nUseTime                = voOS_GetSysTime();
#endif
    int nRet                    = VONP_ERR_Retry;
    int nMaxTryCount            = MAX_FORCE_READ_TRY_COUNT;
    int nMockAudioMaxTime       = 500;
    int nTryReadBufCount        = 0;
    bool bAudioEOS              = false;
    bool bVideoEOS              = false;
    bool bForceReadAudio        = false;
    bool bForceReadVideo        = false;
    bool bGotKeyFrame           = false;
    long long llLastAudioTime   = 0;
    long long llLastVideoTime   = 0;
    long long llAudioDiffTime   = 0;
    VONP_BUFFERTYPE* pAudioNewFormat = NULL;
    VONP_BUFFERTYPE* pVideoNewFormat = NULL;
    VONP_BUFFERTYPE* pFirstAudioBuf  = NULL;
    //VONP_BUFFERTYPE* pFirstVideoBuf  = NULL;

    if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_NEW_FORMAT)
    {
        // audio new format cause
        if(pBuf->pData)
        {
            VONP_BUFFERTYPE* pNewBuf = CloneBuffer(pBuf);
            
            VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
            memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
            CopyTrackInfo(true, pFmt, (VONP_BUFFER_FORMAT*)pBuf->pData);
            
            pNewBuf->pData = pFmt;
            
            bForceReadVideo                 = true;
            pAudioNewFormat                 = pNewBuf;
            m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
            
            //fix time is not set after seek
            if(pBuf->llTime <= 0)
            {
                llLastAudioTime = pBuf->llTime;
                
                if(VONP_ERR_None == ForceReadBuffer(true, pBuf))
                {
                    pNewBuf = CloneBuffer(pBuf);
                    m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
                    pAudioNewFormat->llTime = pNewBuf->llTime;
                }
            }
        }
        
        llLastAudioTime = pBuf->llTime;
    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_VIDEO_NEW_FORMAT)
    {
        // video new format cause
        if(pBuf->pData)
        {
            VONP_BUFFERTYPE* pNewBuf = CloneBuffer(pBuf);
            
            VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
            memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
            CopyTrackInfo(false, pFmt, (VONP_BUFFER_FORMAT*)pBuf->pData);
            
            pNewBuf->pData = pFmt;
            
            bForceReadAudio                 = true;
            pVideoNewFormat                 = pNewBuf;
            m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
            
            //fix time is not set after seek
            if(pBuf->llTime <= 0)
            {
                llLastVideoTime = pBuf->llTime;
                
                if(VONP_ERR_None == ForceReadBuffer(false, pBuf))
                {
                    pNewBuf = CloneBuffer(pBuf);
                    m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
                    pVideoNewFormat->llTime = pNewBuf->llTime;
                }
            }
        }
        
        llLastVideoTime = pBuf->llTime;
    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_DISCONTINUOS)
    {
        // AUDIO time discontinuous cause
        
        // insert new format
        VONP_BUFFERTYPE* pNewBuf = new VONP_BUFFERTYPE;
        memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
        
        VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
        memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
        CopyTrackInfo(true, pFmt, &m_AudioTrackInfo);
        
        pNewBuf->pBuffer    = NULL;
        pNewBuf->nSize      = 0;
        pNewBuf->llTime     = pBuf->llTime;
        pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
        pNewBuf->llReserve  = VONP_ERR_None;
        pNewBuf->pData      = pFmt;
        
        bForceReadVideo                 = true;
        pAudioNewFormat                 = pNewBuf;
        m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
        
        //copy discontinuos buffer
        pNewBuf = CloneBuffer(pBuf);
        m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
        
        if(!m_bAudioOnly)
        {
            // video new format
            pNewBuf = new VONP_BUFFERTYPE;
            memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
            
            pFmt = new VONP_BUFFER_FORMAT;
            memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
            CopyTrackInfo(true, pFmt, &m_VideoTrackInfo);
            
            pNewBuf->pBuffer    = NULL;
            pNewBuf->nSize      = 0;
            pNewBuf->llTime     = pBuf->llTime;
            pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
            pNewBuf->llReserve  = VONP_ERR_None;
            pNewBuf->pData      = pFmt;
            
            //bForceReadVideo                 = true;
            pVideoNewFormat                 = pNewBuf;
            m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);            
        }
        
        m_peekBufInfo.bPeeking = false;
        m_peekBufInfo.sAudioBuf.bPeekBufAvailable = true;
        m_peekBufInfo.sVideoBuf.bPeekBufAvailable = true;
        
#ifdef _VOLOG_INFO
        VOLOGI("[OMXAL]00 -Peek buffer, BA: A %lld, V %lld, last audio %lld, count %d, last video %lld, count %d, use time %lu\n", pAudioNewFormat?pAudioNewFormat->llTime:-1, pVideoNewFormat?pVideoNewFormat->llTime:-1,
               llLastAudioTime, m_peekBufInfo.sAudioBuf.sList.GetCount(), llLastVideoTime, m_peekBufInfo.sVideoBuf.sList.GetCount(), voOS_GetSysTime()-nUseTime);
#endif
        return true;

    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_VIDEO_DISCONTINUOS)
    {
        // VIDEO time discontinuous cause
        // insert new format
        VONP_BUFFERTYPE* pNewBuf = new VONP_BUFFERTYPE;
        memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
        
        VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
        memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
        CopyTrackInfo(true, pFmt, &m_VideoTrackInfo);
        
        pNewBuf->pBuffer    = NULL;
        pNewBuf->nSize      = 0;
        pNewBuf->llTime     = pBuf->llTime;
        pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
        pNewBuf->llReserve  = VONP_ERR_None;
        pNewBuf->pData      = pFmt;
        
        //bForceReadAudio                 = true;
        pVideoNewFormat                 = pNewBuf;
        m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
        
        //copy discontinuos buffer
        pNewBuf = CloneBuffer(pBuf);
        m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
        llLastVideoTime     = pBuf->llTime;
        
        if(!m_bVideoOnly)
        {
            // audio new format
            pNewBuf = new VONP_BUFFERTYPE;
            memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
            
            pFmt = new VONP_BUFFER_FORMAT;
            memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
            CopyTrackInfo(true, pFmt, &m_AudioTrackInfo);
            
            pNewBuf->pBuffer    = NULL;
            pNewBuf->nSize      = 0;
            pNewBuf->llTime     = pBuf->llTime;
            pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
            pNewBuf->llReserve  = VONP_ERR_None;
            pNewBuf->pData      = pFmt;
            
            bForceReadVideo                 = true;
            pAudioNewFormat                 = pNewBuf;
            m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);            
        }
        
        m_peekBufInfo.bPeeking = false;
        m_peekBufInfo.sAudioBuf.bPeekBufAvailable = true;
        m_peekBufInfo.sVideoBuf.bPeekBufAvailable = true;

        VOLOGI("[OMXAL]01 -Peek buffer, BA: A %lld, V %lld, last audio %lld, count %d, last video %lld, count %d, use time %lu\n", pAudioNewFormat?pAudioNewFormat->llTime:-1, pVideoNewFormat?pVideoNewFormat->llTime:-1,
               llLastAudioTime, m_peekBufInfo.sAudioBuf.sList.GetCount(), llLastVideoTime, m_peekBufInfo.sVideoBuf.sList.GetCount(), voOS_GetSysTime()-nUseTime);
        return true;
    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_APP_RESUME)
    {
        bGotKeyFrame    = false;
        bForceReadVideo = true;
    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_NOT_AVAILABLE)
    {
        nMaxTryCount    = nMaxTryCount/2;
        bForceReadVideo = true;
    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_VIDEO_NOT_AVAILABLE)
    {
        nMaxTryCount    = nMaxTryCount/2;
        bForceReadAudio = true;
    }
    else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_LARGE_INTERVAL)
    {
        bForceReadVideo = true;
        llLastAudioTime = pBuf->llTime;
        llAudioDiffTime = pBuf->llTime - m_nLastReadAudioTime;
        pFirstAudioBuf  = CloneBuffer(pBuf);
        m_peekBufInfo.sAudioBuf.sList.AddTail(pFirstAudioBuf);
    }
    else
    {
        VOLOGW("[OMXAL]Peek buffer wrong status! trigger %d", m_peekBufInfo.nPeekTrigger);
        return false;
    }
    
    
    //begin peek buffer
    memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    
    while (m_peekBufInfo.bPeeking && !m_bStopReadThread)
    {
        if(m_bPauseReadBuf)
		{
            if(m_bSeeked)
            {
                VOLOGI("[OMXAL]Need exit peek buffer...");
                m_peekBufInfo.bPeeking = false;
                ResetPeekBuffer();
                m_semSeek.Signal();
                break;
            }
            else
            {
                //VOLOGI("[OMXAL]Pause status,no reading data right now......");
                m_semSeek.Signal();
                voOS_Sleep(2);
                continue;
            }
		}

        nTryReadBufCount++;
        memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    
        if(bForceReadVideo)
        {
            // video
            nRet = ReadBuffer(false, &buf);
            
            if(nRet == VONP_ERR_None)
            {
                VONP_BUFFERTYPE* pNewBuf = CloneBuffer(&buf);

                if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
                {
                    VOLOGI("[OMXAL]Peek video: got new format, time %llu", buf.llTime);
                    
                    VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
                    memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
                    CopyTrackInfo(false, pFmt, (VONP_BUFFER_FORMAT*)buf.pData);
                    
                    bForceReadVideo                 = false;
                    pVideoNewFormat                 = pNewBuf;
                    pNewBuf->pData                  = pFmt;
                    
                    //fix time is not set after seek
                    if(pNewBuf->llTime <= 0)
                    {
                        llLastVideoTime = pNewBuf->llTime;
                        m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
                        
                        if(VONP_ERR_None == ForceReadBuffer(false, &buf))
                        {
                            pNewBuf = CloneBuffer(&buf);
                            m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
                            pVideoNewFormat->llTime = pNewBuf->llTime;
                        }
                    }
                    else
                        m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
                }
                else
                    m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
                
                VOLOGI("[OMXAL]Peek video buf: %d, %lld", buf.nSize, buf.llTime);
                llLastVideoTime = buf.llTime;
            }
            else if(nRet == VONP_ERR_Retry)
			{
                voOS_Sleep(10);
			}
            else if(nRet == VONP_ERR_Video_No_Now)
			{
				VOLOGW("[OMXAL]Peek video: Read video inavailable");
                if(bForceReadVideo)
                    nTryReadBufCount = nMaxTryCount;
                bVideoEOS                   = true;
                bForceReadVideo             = false;
                VONP_BUFFERTYPE* pNewBuf    = CloneBuffer(&buf);
                pNewBuf->llReserve          = VONP_ERR_Video_No_Now;
                m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
			}
            else if(nRet == VONP_ERR_EOS)
			{
				VOLOGW("[OMXAL]Peek video: Reading video buf reach EOS");
                if(bForceReadVideo)
                    nTryReadBufCount = nMaxTryCount;
                bVideoEOS                   = true;
                bForceReadVideo             = false;
                VONP_BUFFERTYPE* pNewBuf    = CloneBuffer(&buf);
                pNewBuf->llReserve          = VONP_ERR_EOS;
                m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
			}
        }
        else if(bForceReadAudio)
        {
            // audio
            nRet = ReadBuffer(true, &buf);
            
            if(nRet == VONP_ERR_None)
            {
                VONP_BUFFERTYPE* pNewBuf = CloneBuffer(&buf);
                
                if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
                {
                    VOLOGI("[OMXAL]Peek audio: got new format, time %lld", buf.llTime);
                    
                    VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
                    memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
                    CopyTrackInfo(false, pFmt, (VONP_BUFFER_FORMAT*)buf.pData);
                    
                    bForceReadAudio                 = false;
                    pAudioNewFormat                 = pNewBuf;
                    pNewBuf->pData                  = pFmt;
                    
                    //fix time is not set after seek,workaround
                    if(pNewBuf->llTime <= 0)
                    {
                        llLastAudioTime = pNewBuf->llTime;
                        m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
                        
                        if(VONP_ERR_None == ForceReadBuffer(true, &buf))
                        {
                            pNewBuf = CloneBuffer(&buf);
                            m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
                            pAudioNewFormat->llTime = pNewBuf->llTime;
                        }
                    }
                    else
                        m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
                }
                else
                    m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
                
                VOLOGI("[OMXAL]Peek audio buf: %d, %lld", buf.nSize, buf.llTime);
                
                llAudioDiffTime = buf.llTime - llLastAudioTime;
                llLastAudioTime = buf.llTime;
            }
            else if(nRet == VONP_ERR_Retry)
			{
                voOS_Sleep(10);
			}
            else if(nRet == VONP_ERR_Audio_No_Now)
			{
				VOLOGW("[OMXAL]Peek audio: Read audio inavailable");
                if(bForceReadAudio)
                    nTryReadBufCount = nMaxTryCount;
                bAudioEOS                   = true;
                bForceReadAudio             = false;
                VONP_BUFFERTYPE* pNewBuf    = CloneBuffer(&buf);
                pNewBuf->llReserve          = VONP_ERR_Audio_No_Now;
                m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
			}
            else if(nRet == VONP_ERR_EOS)
			{
				VOLOGW("[OMXAL]Peek audio: Reading audio buf reach EOS");
                if(bForceReadAudio)
                    nTryReadBufCount = nMaxTryCount;
                bAudioEOS                   = true;
                bForceReadAudio             = false;
                VONP_BUFFERTYPE* pNewBuf    = CloneBuffer(&buf);
                pNewBuf->llReserve          = VONP_ERR_EOS;
                m_peekBufInfo.sAudioBuf.sList.AddTail(pNewBuf);
			}
        }
        else
        {
            VOLOGW("[OMXAL]Peek buffer wrong status!");
            voOS_Sleep(2);
        }
        
        // check if can cancel peek and adjust the buffers
        
        if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_DISCONTINUOS)
        {
            // time discontinuos
        }
        else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_NEW_FORMAT || m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_VIDEO_NEW_FORMAT)
        {
            //if(llLastAudioTime > 10000)
            // got both audio and video format
            if(pAudioNewFormat && pVideoNewFormat)
            {
                if(abs(pAudioNewFormat->llTime-pVideoNewFormat->llTime) > 60)
                {
                    VOLOGW("[OMXAL]Audio/Video BA time not same: A %lld, V %lld, diff %lld, audio count %d, video count %d",
                           pAudioNewFormat->llTime, pVideoNewFormat->llTime, pAudioNewFormat->llTime-pVideoNewFormat->llTime,
                           m_peekBufInfo.sAudioBuf.sList.GetCount(), m_peekBufInfo.sVideoBuf.sList.GetCount());
                    
                    bool bOk = false;
                    
                    if(pAudioNewFormat->llTime < pVideoNewFormat->llTime)
                    {
                        VONP_BUFFERTYPE* pTail = m_peekBufInfo.sAudioBuf.sList.GetTail();
                        
//                      if(m_peekBufInfo.sAudioBuf.sList.GetCount() > 1)
                        {
                            VOLOGW("[OMXAL]Remove new format time %lld", pAudioNewFormat->llTime);
                            m_peekBufInfo.sAudioBuf.sList.Remove(pAudioNewFormat);
                        }

                        //adjust aduio new format time
                        long long llBackupTime = pAudioNewFormat->llTime;
                        pAudioNewFormat->llTime = pVideoNewFormat->llTime;

                        if(InsertPeekBuffer(&m_peekBufInfo.sAudioBuf.sList, pAudioNewFormat))
                        {
                            bOk = true;
                            
                            VOLOGW("[OMXAL]Re-insert new foramt to %lld", pAudioNewFormat->llTime);
                        }
                        else
                        {
                            pAudioNewFormat->llTime = llBackupTime;
                            
                            //VONP_BUFFERTYPE* pTail = m_peekBufInfo.sAudioBuf.sList.GetTail();
                            if(pTail)
                            {
                                VOLOGW("[OMXAL]Audio last time %lld", pTail->llTime);
                            }
                        }
                        
                        //continue read audio until insert the audio frame successfully
                        if(!bOk)
                        {
                            bForceReadAudio     = true;
                            nTryReadBufCount    = 0;
                            continue;
                        }
                    }
                    
                    //workaround,it need dropped audio frames while seeking,not for BA
//                    if(m_bSeeked && !m_bAudioOnly)
//                    {
//                        VONP_BUFFERTYPE* pRemove = m_peekBufInfo.sAudioBuf.sList.RemoveHead();
//                        
//                        while (pRemove)
//                        {
//                            if(pRemove->nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || pRemove->nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
//                            {
//                                VOLOGW("[OMXAL]Finish drop audio frame to sync up video time");
//                                m_peekBufInfo.sAudioBuf.sList.AddHead(pRemove);
//                                break;
//                            }
//                            else
//                                m_peekBufInfo.sRecycleBuf.AddTail(pRemove);
//                            
//                            pRemove = m_peekBufInfo.sAudioBuf.sList.RemoveHead();
//                        }
//                    }
                }
                
                m_bForcePeekBuf        = false;
                m_peekBufInfo.bPeeking = false;
            }
            
            //this change to handle the case:When BA happen,audio/video time is not align,source will not output audio/video any more,
            //so we need to try to read video/audio
            if(m_peekBufInfo.bPeeking)
            {
//                if( (!pAudioNewFormat&&!bAudioEOS) || (!pVideoNewFormat&&!bVideoEOS) )
//                {
//                    bForceReadVideo    = !bForceReadVideo;
//                    bForceReadAudio    = !bForceReadAudio;
//                }
                
                if( (!pAudioNewFormat&&!bAudioEOS) && pVideoNewFormat)
                {
                    //try to read audio again
                    bForceReadVideo    = false;
                    bForceReadAudio    = true;
                }
                else if( (!pVideoNewFormat&&!bVideoEOS) && pAudioNewFormat)
                {
                    //try to read video again
                    bForceReadVideo    = true;
                    bForceReadAudio    = false;
                }
            }
            //end
            
            // can't get audio new format flag
            if(m_peekBufInfo.bPeeking && nTryReadBufCount>=nMaxTryCount && !pAudioNewFormat)
            {
                VOLOGI("[OMXAL]Can't peek audio new format, EOS %s, try count %d, buf count %d", bAudioEOS?"yes":"no", nTryReadBufCount, m_peekBufInfo.sVideoBuf.sList.GetCount());
                
                if(m_bForcePeekBuf && !bAudioEOS)
                {
                    VOLOGW("[OMXAL]Try to read audio new format because we don't read any audio EOS or unavailble!");
                    bForceReadAudio  = true;
                    nTryReadBufCount = 0;
                    continue;
                }
                
                if(pVideoNewFormat && !bAudioEOS)
                {
                    VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
                    memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
                    CopyTrackInfo(true, pFmt, &m_AudioTrackInfo);
                    
                    VONP_BUFFERTYPE* pNewBuf = new VONP_BUFFERTYPE;
                    memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
                    
                    //important
                    pNewBuf->pBuffer    = NULL;
                    pNewBuf->nSize      = 0;
                    pNewBuf->llTime     = pVideoNewFormat->llTime;
                    pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
                    pNewBuf->llReserve  = VONP_ERR_None;
                    pNewBuf->pData      = pFmt;

                    if(!InsertPeekBuffer(&m_peekBufInfo.sAudioBuf.sList, pNewBuf))
                    {
                        VONP_BUFFERTYPE* pFirst = m_peekBufInfo.sAudioBuf.sList.GetHead();
                        VOLOGW("[OMXAL]Insert new aduio format failed!!! first audio buf time %lld, video new format %lld",
                               pFirst?pFirst->llTime:-1, pVideoNewFormat->llTime);
                        
                        // last audio time large than last video time before peek buffers,so only can insert audio new format at head
                        pNewBuf->llTime = pFirst?pFirst->llTime:0;
                        
                        m_peekBufInfo.sAudioBuf.sList.AddHead(pNewBuf);
                    }
                }
                
                m_bForcePeekBuf        = false;
                m_peekBufInfo.bPeeking = false;
            }
            
            // can't get video new format flag
            if(m_peekBufInfo.bPeeking && nTryReadBufCount>=nMaxTryCount && !pVideoNewFormat)
            {
                // But this maybe has issue,because head data maybe wrong
                VOLOGE("[OMXAL]Can't peek video new format, EOS %s, try count %d, buf count %d", bVideoEOS?"yes":"no", nTryReadBufCount, m_peekBufInfo.sVideoBuf.sList.GetCount());
                
                if(pAudioNewFormat && !bVideoEOS)
                {
                    if(!m_VideoTrackInfo.pHeadData)
                    {
                        VOLOGW("[OMXAL]Why not read video new format because we don't read any video EOS or video unavailble!");
                        bForceReadVideo  = true;
                        nTryReadBufCount = 0;
                        continue;
                    }
                    
                    VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
                    memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
                    CopyTrackInfo(true, pFmt, &m_VideoTrackInfo);
                    
                    VONP_BUFFERTYPE* pNewBuf = new VONP_BUFFERTYPE;
                    memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
                    
                    //important
                    pNewBuf->pBuffer    = NULL;
                    pNewBuf->nSize      = 0;
                    pNewBuf->llTime     = pAudioNewFormat->llTime;
                    pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
                    pNewBuf->llReserve  = VONP_ERR_None;
                    pNewBuf->pData      = pFmt;
                    
                    if(!InsertPeekBuffer(&m_peekBufInfo.sVideoBuf.sList, pNewBuf))
                    {
                        VONP_BUFFERTYPE* pFirst = m_peekBufInfo.sVideoBuf.sList.GetHead();
                        VOLOGW("[OMXAL]Insert new video format failed!!! first video buf time %lld, audio new format %lld",
                               pFirst?pFirst->llTime:-1, pAudioNewFormat->llTime);
                        
                        // last video time large than last audio time before peek buffers,so only can insert video new format before the first key frame
                        pNewBuf->llTime = pFirst?pFirst->llTime:0;
//                        m_peekBufInfo.sVideoBuf.sList.AddHead(pNewBuf);
                        
                        POSITION pKey = FindKeyFrame(&m_peekBufInfo.sVideoBuf.sList, pNewBuf->llTime);
                        if(pKey)
                        {
                            VOLOGI("[OMXAL]Re-insert video new format %lld before key frame %lld", pNewBuf->llTime, ((VONP_BUFFERTYPE*)m_peekBufInfo.sVideoBuf.sList.Get(pKey))->llTime);
                            
                            pNewBuf->llTime = ((VONP_BUFFERTYPE*)m_peekBufInfo.sVideoBuf.sList.Get(pKey))->llTime;
                            m_peekBufInfo.sVideoBuf.sList.AddBefore(pKey, pNewBuf);
                        }
                        else
                        {
#if 1
                            VONP_BUFFERTYPE* pTail = m_peekBufInfo.sVideoBuf.sList.GetTail();
                            if(pTail)
                                pNewBuf->llTime = pTail->llTime;
                            
                            m_peekBufInfo.sVideoBuf.sList.AddTail(pNewBuf);
                            
                            VOLOGW("[OMXAL]Can't find key frame,add video new format to tail %lld", pNewBuf->llTime);
#else
                            
                            VOLOGW("[OMXAL]Can't find key frame,add video new format to tail %lld,ignore audio new format %lld", pNewBuf->llTime, pAudioNewFormat->llTime);
                            
                            //ignore audio new format
                            ReleasePeekBuffer(&m_peekBufInfo.sAudioBuf.sList);
                            ReleasePeekBuffer(pNewBuf);
#endif
                        }
                    }
                }
                
                m_bForcePeekBuf        = false;
                m_peekBufInfo.bPeeking = false;
            }
        }
        else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_APP_RESUME)
        {
            if(nRet == VONP_ERR_None)
            {
                if(bForceReadVideo)
                {
                    if(buf.pBuffer && buf.nSize>0)
                    {
                        bGotKeyFrame = CheckVideoKeyFrame(&buf);
                        
                        if(bGotKeyFrame)
                        {
                            VOLOGI("[OMXAL]Got key frame %lld, audio count %d, last %lld, video count %d, last %lld",
                                   buf.llTime, m_peekBufInfo.sAudioBuf.sList.GetCount(), llLastAudioTime, m_peekBufInfo.sVideoBuf.sList.GetCount(), llLastVideoTime);
                            
                            //insert audio new format
                            VONP_BUFFER_FORMAT* pFmt = new VONP_BUFFER_FORMAT;
                            memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
                            CopyTrackInfo(true, pFmt, &m_AudioTrackInfo);
                            
                            VONP_BUFFERTYPE* pNewBuf = new VONP_BUFFERTYPE;
                            memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
                            
                            //important
                            pNewBuf->pBuffer    = NULL;
                            pNewBuf->nSize      = 0;
                            pNewBuf->llTime     = buf.llTime;
                            pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
                            pNewBuf->llReserve  = VONP_ERR_None;
                            pNewBuf->pData      = pFmt;
                            
                            if(!InsertPeekBuffer(&m_peekBufInfo.sAudioBuf.sList, pNewBuf))
                            {
#ifdef _VOLOG_WARNING
                                VONP_BUFFERTYPE* pFirst = m_peekBufInfo.sAudioBuf.sList.GetHead();
                                VOLOGW("[OMXAL]01 Insert new aduio format failed!!! first audio buf time %lld, audio new format %lld",
                                       pFirst?pFirst->llTime:-1, pNewBuf->llTime);
#endif
                                
                                // add head to drop the audio frame
                                m_peekBufInfo.sAudioBuf.sList.AddHead(pNewBuf);
                            }
                            
                            //insert video new format
                            pFmt = new VONP_BUFFER_FORMAT;
                            memset(pFmt, 0, sizeof(VONP_BUFFER_FORMAT));
                            CopyTrackInfo(true, pFmt, &m_VideoTrackInfo);
                            
                            pNewBuf = new VONP_BUFFERTYPE;
                            memset(pNewBuf, 0, sizeof(VONP_BUFFERTYPE));
                            
                            //important
                            pNewBuf->pBuffer    = NULL;
                            pNewBuf->nSize      = 0;
                            pNewBuf->llTime     = buf.llTime-1;// it need insert ahead the key frame
                            pNewBuf->nFlag      = VONP_FLAG_BUFFER_NEW_FORMAT;
                            pNewBuf->llReserve  = VONP_ERR_None;
                            pNewBuf->pData      = pFmt;
                            
                            if(!InsertPeekBuffer(&m_peekBufInfo.sVideoBuf.sList, pNewBuf))
                            {
                                VOLOGW("Insert video new format failed!");
                                m_peekBufInfo.sVideoBuf.sList.AddHead(pNewBuf);
                            }
                            else
                            {
                                VOLOGW("Insert video new format success!");
                            }
                            
                            m_bForcePeekBuf        = false;
                            m_peekBufInfo.bPeeking = false;
                        }
                    }
                }
                
                bForceReadVideo = llLastAudioTime >= llLastVideoTime;
                bForceReadAudio = llLastVideoTime > llLastAudioTime;
                
                if(m_bAudioOnly)
                {
                    bForceReadVideo = false;
                    bForceReadAudio = true;
                }
                if(m_bVideoOnly)
                {
                    bForceReadVideo = true;
                    bForceReadAudio = false;
                }
            }
        }
        else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_NOT_AVAILABLE)
        {
            if(pVideoNewFormat || nTryReadBufCount>=nMaxTryCount)
            {
                VOLOGI("[OMXAL]Audio not available trigger try count %d", nTryReadBufCount);
                bForceReadVideo         = false;
                m_peekBufInfo.bPeeking  = false;
            }
        }
        else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_VIDEO_NOT_AVAILABLE)
        {
            if(pAudioNewFormat || nTryReadBufCount>=nMaxTryCount)
            {
                VOLOGI("[OMXAL]Video not available trigger try count %d", nTryReadBufCount);
                bForceReadAudio         = false;
                m_peekBufInfo.bPeeking  = false;
            }
        }
        else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_AUDIO_LARGE_INTERVAL)
        {
            bForceReadVideo = llLastAudioTime >= llLastVideoTime;
            bForceReadAudio = llLastVideoTime > llLastAudioTime;
            
            if(m_bAudioOnly)
            {
                bForceReadVideo = false;
                bForceReadAudio = true;
            }
            if(m_bVideoOnly)
            {
                bForceReadVideo = true;
                bForceReadAudio = false;
            }
            
            float fScale    = (float)llAudioDiffTime/(float)m_pFrameTools->GetDurationPerFrame();
            
            if(llLastAudioTime-pFirstAudioBuf->llTime >= nMockAudioMaxTime || fScale<MAX_AUDIO_INTERVAL_SCALE || bAudioEOS || bVideoEOS)
            {
                int nDurationPerFrame = m_pFrameTools->GetDurationPerFrame();
                //int nReadCount  = m_peekBufInfo.sAudioBuf.sList.GetCount();
                int nTotalCount = (buf.llTime-m_nLastReadAudioTime) / nDurationPerFrame;
                
                VOLOGI("[OMXAL]+Peek mock audio finished,process it. last %lld, first %lld, scale %f, count %d,%d",
                       llLastAudioTime, pFirstAudioBuf->llTime, fScale, m_peekBufInfo.sAudioBuf.sList.GetCount(), nTotalCount);
                
                long long llTime = m_nLastReadAudioTime + nDurationPerFrame;
                VONP_BUFFERTYPE* pExist = m_peekBufInfo.sAudioBuf.sList.RemoveHead();
                CObjectList<VONP_BUFFERTYPE>     sTmp;
                
                while (nTotalCount > 0 && pExist)
                {
                    //VONP_BUFFERTYPE* pInsert = CloneBuffer(pExist);
                    VONP_BUFFERTYPE* pInsert = CloneSilentBuffer(pExist);
                    pInsert->llTime = llTime;
                    
                    if(pInsert->llTime <= pExist->llTime)
                    {
                        VOLOGI("[OMXAL]Insert mock audio-00: %lld", pInsert->llTime);
                        sTmp.AddTail(pInsert);
                        
                        //last audio
                        if(nTotalCount == 1)
                        {
                            VOLOGI("[OMXAL]Insert mock audio-03: %lld", pExist->llTime);
                            sTmp.AddTail(pExist);
                            pExist = m_peekBufInfo.sAudioBuf.sList.RemoveHead();
                        }
                    }
                    else
                    {
                        VOLOGI("[OMXAL]Insert mock audio-01: %lld, %lld", llTime, pExist->llTime);
                    
                        pExist->llTime = llTime;
                        sTmp.AddTail(pExist);
                        
                        //not last audio
                        if(nTotalCount > 1)
                        {
                            llTime += nDurationPerFrame;
                            nTotalCount--;
                            pInsert->llTime = llTime;
                            sTmp.AddTail(pInsert);
                            VOLOGI("[OMXAL]Insert mock audio-02: %lld", llTime);                            
                        }
                        else
                        {
                            VOLOGI("[OMXAL]Don't insert this audio,%lld", pInsert->llTime);
                            ReleasePeekBuffer(pInsert);
                        }
                        
                        pExist = m_peekBufInfo.sAudioBuf.sList.RemoveHead();
                    }
                    
                    llTime += nDurationPerFrame;
                    nTotalCount--;
                }
                
                if(bAudioEOS && pExist)
                {
                    VONP_BUFFERTYPE* pEOS = CloneBuffer(pExist);
                    pEOS->llReserve = VONP_ERR_EOS;
                    sTmp.AddTail(pEOS);
                }
                
                ReleasePeekBuffer(pExist);
                
                ReleasePeekBuffer(&m_peekBufInfo.sAudioBuf.sList);
                m_peekBufInfo.sAudioBuf.sList.RemoveAll();
                m_peekBufInfo.sAudioBuf.sList.AddTail(&sTmp);
                sTmp.RemoveAll();
                
                VOLOGI("[OMXAL]-Peek mock audio finished,process it. last %lld, first %lld, scale %f, count %d,%d, audio EOS %d",
                       llLastAudioTime, pFirstAudioBuf->llTime, fScale, m_peekBufInfo.sAudioBuf.sList.GetCount(), nTotalCount, bAudioEOS?1:0);
                
                m_bForcePeekBuf        = false;
                m_peekBufInfo.bPeeking = false;
            }
        }
        
        if(!m_peekBufInfo.bPeeking)
        {
            m_peekBufInfo.sAudioBuf.bPeekBufAvailable = true;
            m_peekBufInfo.sVideoBuf.bPeekBufAvailable = true;
        }
    }
    
    VOLOGI("[OMXAL]02 -Peek buffer, BA: A %lld, V %lld, last audio %lld, count %d, last video %lld, count %d, use time %lu\n", pAudioNewFormat?pAudioNewFormat->llTime:-1, pVideoNewFormat?pVideoNewFormat->llTime:-1,
           llLastAudioTime, m_peekBufInfo.sAudioBuf.sList.GetCount(), llLastVideoTime, m_peekBufInfo.sVideoBuf.sList.GetCount(), voOS_GetSysTime()-nUseTime);
    
    return true;
}


POSITION COMXALMediaPlayer::FindKeyFrame(CObjectList<VONP_BUFFERTYPE>* pList, long long llTimeBeforeKeyFrame)
{
    POSITION pos = pList->GetHeadPosition();
    
//    long long llLastTime = 0XFFFFFFFF;
    
    while(pos)
    {
        VONP_BUFFERTYPE* pBuf = pList->Get(pos);
        
        if(pBuf)
        {
            //VOLOGI("[OMXAL]Check peek buf, time %lld", pBuf->llTime);
            
            //if(llTimeBeforeKeyFrame<pBuf->llTime && llTimeBeforeKeyFrame>llLastTime && pBuf->nFlag&VONP_FLAG_BUFFER_KEYFRAME)
            if(llTimeBeforeKeyFrame<=pBuf->llTime && pBuf->nFlag&VONP_FLAG_BUFFER_KEYFRAME)
            {
                VOLOGW("[OMXAL]Find key frame, time %lld before %lld", llTimeBeforeKeyFrame, pBuf->llTime);
                return pos;
            }
            
//            llLastTime = pBuf->llTime;
        }
        
        pos = pList->Next(pos);
    }
    
    return NULL;
}

bool COMXALMediaPlayer::InsertPeekBuffer(CObjectList<VONP_BUFFERTYPE>* pList, VONP_BUFFERTYPE* pNewBuf)
{
    POSITION pos = pList->GetHeadPosition();
    
//    long long llLastTime = 0XFFFFFFFF;
    
    while(pos)
    {
        VONP_BUFFERTYPE* pBuf = pList->Get(pos);
        
        if(pBuf)
        {
            //VOLOGI("[OMXAL]Check peek buf, time %lld", pBuf->llTime);
            
            //if(pNewBuf->llTime<pBuf->llTime && pNewBuf->llTime>=llLastTime)
            if(pNewBuf->llTime<=pBuf->llTime)
            {
                VOLOGW("[OMXAL]Insert peek buf, time %lld before %lld", pNewBuf->llTime, pBuf->llTime);
                pList->AddBefore(pos, pNewBuf);
                return true;
            }
            
 //           llLastTime = pBuf->llTime;
        }
        
        pos = pList->Next(pos);
    }
    
    return false;
}

bool COMXALMediaPlayer::IsPeekBufAvailable()
{
    if(m_peekBufInfo.sAudioBuf.bPeekBufAvailable || m_peekBufInfo.sVideoBuf.bPeekBufAvailable)
        return true;
    
    return false;
}

int COMXALMediaPlayer::ForceReadBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf)
{
    //disable it currentlly
    return VONP_ERR_Pointer;
    
    int nRet = VONP_ERR_Retry;
    
    if(m_pReadBufFunc->ReadAudio && m_pReadBufFunc->ReadVideo && m_pReadBufFunc->pUserData)
    {
        while (nRet == VONP_ERR_Retry)
        {
            if(bAudio)
                nRet = m_pReadBufFunc->ReadAudio(m_pReadBufFunc->pUserData, pBuf);
            else
                nRet = m_pReadBufFunc->ReadVideo(m_pReadBufFunc->pUserData, pBuf);
            
            if(m_bStopReadThread)
            {
                VOLOGW("[OMXAL]Exit force read buffer");
                break;
            }
        }
        
        voOS_Sleep(1);
    }
    
    VOLOGI("[OMXAL]Force read %s return, time %lld, size %d, ret %d", bAudio?"AUDIO":"VIDEO", pBuf->llTime, pBuf->nSize, nRet);
    
    return nRet;
}

int COMXALMediaPlayer::ReadBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf)
{
    int nRet = VONP_ERR_Retry;
    
    if(m_pReadBufFunc->ReadAudio && m_pReadBufFunc->ReadVideo && m_pReadBufFunc->pUserData)
    {
        if(bAudio)
        {
            if(m_peekBufInfo.sAudioBuf.bPeekBufAvailable)
            {
                nRet = ReadPeekBuffer(true, pBuf);
				VOLOGI("[OMXAL]Read peek audio buffer nRet 0X%08X", nRet);
                
                if(nRet == -1)
                {
                    VOLOGI("[OMXAL]Read peek audio buffer FINISH");
                    
                    nRet = VONP_ERR_Retry;
                    m_peekBufInfo.sAudioBuf.bPeekBufAvailable = false;
                    
                    if(!m_peekBufInfo.sVideoBuf.bPeekBufAvailable)
                    {
                        VOLOGI("[OMXAL]Read peek buffer FINISH 00");
                    }
                }
                else
                {
                    VOLOGR("[OMXAL]Read peek audio: Time %08lld, Flag %d, Size %d, Ret %d", pBuf->llTime, pBuf->nFlag, pBuf->nSize, nRet);
                }
            }
            else
			{
                nRet = m_pReadBufFunc->ReadAudio(m_pReadBufFunc->pUserData, pBuf);
				if(NULL != pBuf)
					VOLOGI("[OMXAL]___ReadAudio RETURN 0X%08X Size %d Time %lld Flag 0X%08X", nRet, pBuf->nSize, pBuf->llTime, pBuf->nFlag);
			}
        }
        else
        {
            if(m_peekBufInfo.sVideoBuf.bPeekBufAvailable)
            {
                nRet = ReadPeekBuffer(false, pBuf);
				VOLOGI("[OMXAL]Read peek video buffer nRet 0X%08X", nRet);
                
                if(nRet == -1)
                {
                    VOLOGI("[OMXAL]Read peek video buffer FINISH");
                    
                    nRet = VONP_ERR_Retry;
                    m_peekBufInfo.sVideoBuf.bPeekBufAvailable = false;
                    
                    if(!m_peekBufInfo.sAudioBuf.bPeekBufAvailable)
                    {
                        VOLOGI("[OMXAL]Read peek buffer FINISH 01");
                    }
                }
                else
                {
                    VOLOGR("[OMXAL]Read peek video: Time %08lld, Flag %d, Size %d, Ret %d", pBuf->llTime, pBuf->nFlag, pBuf->nSize, nRet);
                }
            }
            else
			{
                nRet = m_pReadBufFunc->ReadVideo(m_pReadBufFunc->pUserData, pBuf);
				if(NULL != pBuf)
					VOLOGI("[OMXAL]___ReadVideo RETURN 0X%08X Size %d Time %lld Flag 0X%08X", nRet, pBuf->nSize, pBuf->llTime, pBuf->nFlag);
			}
        }
    }
    
    return nRet;
}

int COMXALMediaPlayer::ReadPeekBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf)
{
    int nRet = VONP_ERR_Retry;
    
    CObjectList<VONP_BUFFERTYPE>* pList = bAudio?&m_peekBufInfo.sAudioBuf.sList:&m_peekBufInfo.sVideoBuf.sList;
    
    VONP_BUFFERTYPE* pPeek = (VONP_BUFFERTYPE*)pList->RemoveHead();
    
    if(pPeek)
    {
        pBuf->nSize       = pPeek->nSize;
        pBuf->pBuffer     = pPeek->pBuffer;
        pBuf->llTime      = pPeek->llTime;
        pBuf->nFlag       = pPeek->nFlag;
        pBuf->llReserve   = pPeek->llReserve;
        
        pBuf->pData       = pPeek->pData;

        // save to recycle to release
        m_peekBufInfo.sRecycleBuf.AddTail(pPeek);
        
        return pBuf->llReserve;
    }
    else
        nRet = -1;
    
    return nRet;
}
