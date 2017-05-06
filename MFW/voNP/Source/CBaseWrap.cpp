/*
 *  CBaseWrap.cpp
 *
 *  Created by Lin Jun on 10/09/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CBaseWrap.h"
#include "voOSFunc.h"
#include <string.h>
#include "voLog.h"
#include "CAVNativePlayer.h"
#include "fVideoHeadDataInfo.h"
#include "voSink.h"
#include "voOSTools.h"

#ifdef _IOS
#include <mach/machine.h>
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CBaseWrap::CBaseWrap(VONP_LIB_FUNC* pLibOP)
:m_pLibOP(pLibOP)
,m_pView(NULL)
,m_pNativePlayer(NULL)
,m_pMuxStream(NULL)
,m_hEventNotifyThread(NULL)
,m_hReadBufThread(NULL)
,m_bStopReadThread(false)
,m_bPauseReadBuf(false)
,m_bForceReadAudioNewFmt(false)
,m_bForceReadVideoNewFmt(false)
,m_bReadAudioNewFormat(false)
,m_bReadVideoNewFormat(false)
,m_bAudioOnly(false)
,m_bVideoOnly(false)
,m_nPosOffsetTime(0)
,m_nLastReadAudioTime(0)
,m_nLastReadVideoTime(0)
,m_nLastAudioNewFormatTime(0)
,m_nLastVideoNewFormatTime(0)
,m_nLastBATime(-1)
,m_nTryReadBufCount(0)
,m_nStatusEOS(0)
,m_nPlayerStatus(VONP_STATUS_INIT)
,m_bSupportAudioCodec(true)
,m_bSupportVideoCodec(true)
,m_bWaitKeyFrame(true)
,m_bWaitFirstAudioFrame(true)
,m_bSeeking(false)
,m_bCancelSendRawFrame(false)
,m_nOpenFlag(0)
,m_bStopEventNotifyThread(false)
,m_bFormatChanged(false)
,m_bFirstRun(true)
,m_nSeekPos(0)
,m_bSeeked(false)
,m_nPlayingTimeOffset(0)
,m_nLastPosUpdateTime(0)
,m_nLastPlayingTime(0)
,m_nCurrPos(0)
,m_bEOSed(false)
,m_bBuffering(false)
,m_pFrameTools(NULL)
,m_nAspectRatio(VONP_RATIO_00)
,m_nReadyRunUseTime(0)
,m_nAppStatus(APP_NORMAL)
,m_bForcePeekBuf(true)
,m_bBACasuedByVideoTimeNotDiscontinuo(false)
,m_llPauseSysTime(0)
{
#if defined(_IOS) || defined(_MAC_OS)
    voMoreUNIXIgnoreSIGPIPE();
#endif
    
    memset(&m_NotifyEventInfo, 0, sizeof(VONP_LISTENERINFO));
    	
	memset(&m_AudioTrackInfo, 0, sizeof(VONP_BUFFER_FORMAT));
	memset(&m_VideoTrackInfo, 0, sizeof(VONP_BUFFER_FORMAT));
    memset(&m_fmtVideo, 0, sizeof(VONP_VIDEO_FORMAT));
	
	m_AudioTrackInfo.nStreamType	= VONP_SS_Audio;
	m_AudioTrackInfo.nCodec			= VONP_AUDIO_CodingAAC;
	m_VideoTrackInfo.nStreamType	= VONP_SS_Video;
	m_VideoTrackInfo.nCodec			= VONP_VIDEO_CodingH264;
    
    m_fmtAudio.nSampleRate	= 44100;
	m_fmtAudio.nChannels	= 2;
	m_fmtAudio.nSampleBits	= 16;
    
    memset(m_szPlaybackURL, 0, sizeof(1024));
    
    memset(&m_peekBufInfo, 0 ,sizeof(PEEK_BUFFER_INFO));
    ResetPeekBuffer();
    m_peekBufInfo.bEnable = true;
    
    memset(&m_sCPUInfo, 0, sizeof(VONP_CPU_INFO));
    GetCpuInfo(&m_sCPUInfo);
}

CBaseWrap::~CBaseWrap(void)
{
	m_pLibOP = NULL;
	Uninit();
    
#if defined(_IOS) || defined(_MAC_OS)
    voMoreUNIXRecoverySIGPIPE();
#endif
        
    VOLOGI("[NPW]Native player wrapper module destroyed!!!");
}

int CBaseWrap::Init()
{
    EnaleDumpRawFrame(true);
    
    int nRet = VONP_ERR_None;
    
    //CreateMemStream();
    CreateFrameTools();
    //CreateNativePlayer();

    m_bFirstRun     = true;
    m_nPlayerStatus = VONP_STATUS_INIT;
    m_bForcePeekBuf = true;
    
    return nRet;
}

int CBaseWrap::Uninit()
{
    DestroyReadThread();
    DestroyMemStream();
    DestroyHeadData();
    DestroyNativePlayer();
    DestroyFrameTools();
    ResetPeekBuffer();
    DestroyBAInfo();
    
    m_AVFrameWriter.Uninit();
    m_AVFrameReader.Uninit();
    
    VOLOGUNINIT();

    return VONP_ERR_None;
}

int CBaseWrap::SetView(void* pView)
{
    m_pView = pView;

    int nRet = VONP_ERR_None;
    
    if(m_pNativePlayer)
        nRet = m_pNativePlayer->SetView(pView);
   
    return VONP_ERR_None;
}

void CBaseWrap::Reset()
{
	m_bAudioOnly            = false;
	m_bVideoOnly            = false;
    m_bReadAudioNewFormat   = false;
    m_bReadVideoNewFormat   = false;
    
    m_nSeekPos              = 0;
    m_bSeeked               = false;
    m_bEOSed                = false;
    
    m_nLastPosUpdateTime    = 0;
    m_nLastPlayingTime      = 0;
    m_nCurrPos              = 0;
    
    m_bEOSed                = false;
    m_bBuffering            = false;
    m_bForcePeekBuf         = true;
    
    m_bBACasuedByVideoTimeNotDiscontinuo = false;
}

int CBaseWrap::Open(void* pSource, int nFlag)
{
    int nRet = VONP_ERR_None;
    
    m_bFirstRun         = true;
    m_nReadyRunUseTime  = voOS_GetSysTime();
    m_llPauseSysTime    = voOS_GetSysTime();
        
    Reset();
    
    NotifyBuffering(true);
    
    if(!m_pNativePlayer)
        CreateNativePlayer();
    
    m_nOpenFlag     = nFlag;
    m_nPlayerStatus = VONP_STATUS_LOADING;
    
    if(nFlag == VONP_FLAG_SOURCE_URL)
	{
		if(pSource)
		{
            strcpy(m_szPlaybackURL, (char*)pSource);
		}
		else
		{
			VOLOGI("[NPW]URL is NULL");
		}
	}
	else if(nFlag == VONP_FLAG_SOURCE_READBUFFER)
	{
		VONP_READBUFFER_FUNC* pReadBufFunc	= (VONP_READBUFFER_FUNC*)pSource;
        m_ReadBufFunc.pUserData = pReadBufFunc->pUserData;
        m_ReadBufFunc.ReadAudio = pReadBufFunc->ReadAudio;
        m_ReadBufFunc.ReadVideo = pReadBufFunc->ReadVideo;
	}
	else if(nFlag == VONP_FLAG_SOURCE_SENDBUFFER)
	{
	}
	else
	{
		return VONP_ERR_Unknown;
	}
	
    if(m_pNativePlayer)
    {
        nRet = m_pNativePlayer->Open(m_szPlaybackURL, nFlag);
    }
    
    CreateEventNotifyThread();

    return VONP_ERR_None;
}

int CBaseWrap::Close()
{
    int nRet = VONP_ERR_None;
    
    if(m_pNativePlayer)
        nRet = m_pNativePlayer->Close();
    
    DestroyReadThread();
    DestroyMemStream();
    DestroyHeadData();
    DestroyNativePlayer();
    ResetPeekBuffer();
    DestroyBAInfo();
    
    return VONP_ERR_None;
}

int CBaseWrap::Run()
{
    int nRet = VONP_ERR_None;
    
    VOLOGI("[NPW]Run, %lu, build time:%s  %s", voOS_GetSysTime(), __TIME__,  __DATE__);
	
	voCAutoLock lock(&m_mtStatus);
    
	if(IsRunning())
		return VONP_ERR_Status;
    
	CreateEventNotifyThread();
    
    //m_nLastPosUpdateTime    = voOS_GetSysTime();
    
	if(IsPause())
	{
        VOLOGI("[NPW]Run after pause, %d", m_nLastPosUpdateTime);
        
        m_bSeeking      = false;
        m_bPauseReadBuf	= false;
        m_nPlayerStatus = VONP_STATUS_RUNNING;
    }
	else
	{
        VOLOGI("[NPW]Run normally");
        
		m_bPauseReadBuf		 = false;
		m_nPosOffsetTime	 = 0;
		m_nLastReadVideoTime = 0;
		m_nLastReadAudioTime = 0;
		m_nStatusEOS	     = 0;
		
		if(m_pMuxStream)
			nRet = m_pMuxStream->Run();
        
		if(m_nOpenFlag != VONP_FLAG_SOURCE_SENDBUFFER)
        {
            //VOLOGI("NPW create read thread");
            CreateReadThread();
        }
				
		if(nRet != VONP_ERR_None)
		{
			VOLOGE("[NPW]Run failed!!!");
			return nRet;
		}
		
		m_nPlayerStatus = VONP_STATUS_RUNNING;
	}
    
    if(m_pNativePlayer && !IsAppResume())
        nRet = m_pNativePlayer->Run();
    
    VOLOGI("[NPW]-Run ret %d", nRet);
    
    return VONP_ERR_None;
}

int CBaseWrap::Pause()
{
    if(m_nPlayerStatus == VONP_STATUS_STOPPED)
        return VONP_ERR_None;

    int nRet = VONP_ERR_None;
    
    m_llPauseSysTime= voOS_GetSysTime();
    m_nPlayerStatus = VONP_STATUS_PAUSED;
    m_bPauseReadBuf = true;
    
    //wait pause read buffer thread
	int nTest = 0;
	nTest = voOS_GetSysTime();
	m_semSeek.Wait(5000);
	VOLOGI("[NPW]pause read return..., %d", (int)(voOS_GetSysTime() - nTest));

    
    if(m_pNativePlayer)
        nRet = m_pNativePlayer->Pause();

    return VONP_ERR_None;
}

int CBaseWrap::Stop()
{
    int nRet = VONP_ERR_None;
    
    if(IsStop())
        return nRet;
    
    m_nPlayerStatus = VONP_STATUS_STOPPED;
    m_nAspectRatio  = VONP_RATIO_00;
    
    DestroyReadThread();
    DestroyEventNotifyThread();
    
    if(m_pNativePlayer)
    {
        nRet = m_pNativePlayer->Stop();
        nRet = m_pNativePlayer->Close();
    }
    
    DumpRawFrame(true, NULL, 0);
    DumpRawFrame(false, NULL, 0);
    
    return VONP_ERR_None;
}

int CBaseWrap::GetPos()
{
    int nRet = GetPlayingTime();
    
    return nRet;
}

int CBaseWrap::SetPos(int nPos)
{
	VOLOGI("[NPW]Set pos %d, %02d:%02d:%02d, systime %lu, playing time %d", nPos, nPos/1000/3600, ((nPos/1000)%3600)/60, (((nPos/1000)%3600)%60), voOS_GetSysTime(), GetPlayingTime());
    
    //don't notify buffering while paused,but it will pause while do seeking
    if( (voOS_GetSysTime() - m_llPauseSysTime) < 500)
        NotifyBuffering(true);
    
    if(IsLoading())
        return VONP_ERR_None;
    
	int nRet            = VONP_ERR_None;
    
    m_nStatusEOS    = 0;
    m_bEOSed        = false;
	m_nSeekPos		= nPos;
	m_bSeeked		= true;
    m_nCurrPos      = 0;
    m_nPlayingTimeOffset = nPos;
    m_bForcePeekBuf = true;
    
	//pause read buffer
	m_bPauseReadBuf	= true;
	m_bSeeking		= true;
    m_bWaitKeyFrame = true;
    m_bWaitFirstAudioFrame = true;
    m_nReadyRunUseTime  = voOS_GetSysTime();
	
	//wait pause read buffer thread
	int nTest = 0;
	nTest = voOS_GetSysTime();
	m_semSeek.Wait(5000);
	VOLOGI("[NPW]Seek pause read return..., %d", (int)(voOS_GetSysTime() - nTest));
    
    //
    voCAutoLock lock(&m_mtStatus);
    
    Flush();
	
	//m_bGotNewFormatBuf	= false;
	m_nLastAudioNewFormatTime = 0;
    m_bReadAudioNewFormat = false;
    m_bReadVideoNewFormat = false;
    m_bForceReadAudioNewFmt = false;
    m_bForceReadVideoNewFmt = false;
    m_bBACasuedByVideoTimeNotDiscontinuo = false;
    
    m_nLastReadVideoTime = 0;
    m_nLastReadAudioTime = 0;
    
    m_nAspectRatio = VONP_RATIO_00;
    memset(&m_fmtVideo, 0, sizeof(VONP_VIDEO_FORMAT));
    
	return nRet;
}

void CBaseWrap::Flush()
{
    if(m_pMuxStream)
        m_pMuxStream->Flush();
    
    if(m_pFrameTools)
        m_pFrameTools->ResetFrameCount();
    
    ResetPeekBuffer();
    DestroyBAInfo();
}

int CBaseWrap::GetParam(int nParamID, void* pValue)
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
//		VONP_SENDBUFFER_FUNC* pFunc = (VONP_SENDBUFFER_FUNC*)pValue;
//		pFunc->pUserData	= this;
//		pFunc->SendData		= SendDataBuf;
//		return VONP_ERR_None;
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
        //return GetAudioBuffer((VONP_BUFFERTYPE**)pValue);
    }
    else if(VONP_PID_VIDEO_BUFFER == nParamID)
    {
        //return GetVideoBuffer((VONP_BUFFERTYPE**)pValue);
    }
    else if(VONP_PID_AUDIO_FORMAT == nParamID)
    {
        if(!pValue)
            return VONP_ERR_Pointer;
        VOLOGI("[NPW]Output audio format:%d %d %d", m_fmtAudio.nSampleRate, m_fmtAudio.nChannels, m_fmtAudio.nSampleBits);
        VONP_AUDIO_FORMAT* pFmt  = (VONP_AUDIO_FORMAT*)pValue;
        pFmt->nSampleRate           = m_fmtAudio.nSampleRate;
        pFmt->nChannels             = m_fmtAudio.nChannels;
        pFmt->nSampleBits           = m_fmtAudio.nSampleBits;
        return VONP_ERR_None;
    }
	else
	{
		VOLOGW("[NPW]++++++++++++++Get Param ID(%d) not implement!!!+++++++++++++++++++", nParamID);
	}
	
	return VONP_ERR_Implement;

}

int CBaseWrap::SetParam(int nParamID, void* pValue)
{
    if(VONP_PID_FUNC_LIB == nParamID)
	{
		m_pLibOP = (VONP_LIB_FUNC*)pValue;
		VOLOGI("[NPW]m_pLibOP %x", (int)m_pLibOP);
		return VONP_ERR_None;
	}
	else if(VONP_PID_LISTENER == nParamID)
	{
		VOLOGI("[NPW]Set event callback %x", (unsigned int)pValue);
		VONP_LISTENERINFO* pNotifyEvent = (VONP_LISTENERINFO*)pValue;
        m_NotifyEventInfo.pUserData = pNotifyEvent->pUserData;
        m_NotifyEventInfo.pListener = pNotifyEvent->pListener;
        return VONP_ERR_None;
	}
	else if(VONP_PID_AUDIO_VOLUME == nParamID)
	{
	}
	else if(VONP_PID_STATUS == nParamID)
	{
	}
	else if(VONP_PID_COMMON_LOGFUNC == nParamID)
	{
		VO_LOG_PRINT_CB* pLogPrintCallback = (VO_LOG_PRINT_CB *)pValue;
		if(pLogPrintCallback)
		{
			//vologInit (pLogPrintCallback->pUserData, pLogPrintCallback->fCallBack);
            VOLOGINIT("");
			VOLOGI("[NPW]Set log func %x", (unsigned int)pValue);
		}
        return VONP_ERR_None;
	}
    else if(VONP_PID_APPLICATION_SUSPEND == nParamID)
    {
        doProcessSuspend();
    }
    else if(VONP_PID_APPLICATION_RESUME == nParamID)
    {
        doProcessResume(pValue);
    }
    else if(VONP_PID_VIEW_ACTIVE == nParamID)
    {
    }
	else
	{
		VOLOGW("[NPW]++++++++++++++Set Param ID(%x) not implement!!!+++++++++++++++++++", nParamID);
	}

    return VONP_ERR_None;
}

void CBaseWrap::doProcessSuspend()
{
    if(IsAppSuspend())
        return;
    
    VOLOGI("[NPW]App suspend %lu", voOS_GetSysTime());
    
    m_bPauseReadBuf	= true;
	
	//wait pause read buffer thread
	int nTest = 0;
	nTest = voOS_GetSysTime();
	m_semSeek.Wait(5000);
	VOLOGI("[NPW]Pause read buffer return..., %d", (int)(voOS_GetSysTime() - nTest));

    
    m_nAppStatus = APP_ENTER_SUSPEND;
}

void CBaseWrap::doProcessResume(void* hView)
{
    if(IsAppResume())
        return;
    
    VOLOGI("[NPW]App resume %lu", voOS_GetSysTime());
    
    m_nAppStatus = APP_ENTER_RESUME;

    bool bRunning = IsRunning();
    
    if(bRunning)
        Pause();
    
    m_bWaitKeyFrame = true;
    m_bWaitFirstAudioFrame = true;
    m_nReadyRunUseTime  = voOS_GetSysTime();
    
    Flush();
	
	m_nLastAudioNewFormatTime = 0;
    m_bReadAudioNewFormat = false;
    m_bReadVideoNewFormat = false;
    m_bForceReadAudioNewFmt = false;
    m_bForceReadVideoNewFmt = false;
    
    m_nLastReadVideoTime = 0;
    m_nLastReadAudioTime = 0;
    
    m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_APP_RESUME;
    m_peekBufInfo.bPeeking      = true;
    
    if(bRunning)
        Run();
}


int CBaseWrap::GetAudioBuffer(VONP_BUFFERTYPE** ppBuffer)
{
    return VONP_ERR_None;
}

int CBaseWrap::GetVideoBuffer(VONP_BUFFERTYPE** ppBuffer)
{
    return VONP_ERR_None;
}

int CBaseWrap::SendEvent(int nEventID, void* pParam1, void*pParam2)
{
	int nRet = VONP_ERR_Pointer;
	
	if(m_NotifyEventInfo.pListener && m_NotifyEventInfo.pUserData)
		nRet = m_NotifyEventInfo.pListener(m_NotifyEventInfo.pUserData, nEventID, pParam1, pParam2);
	
	return nRet;
}

int CBaseWrap::GetPlayingTime()
{
    int nRet = -1;
    
    if(m_bSeeked)
        return m_nSeekPos;
    
//    if(m_pNativePlayer)
//        nRet = m_pNativePlayer->GetPos();
    
    nRet = m_nCurrPos;
    
    nRet += m_nPlayingTimeOffset;
    
    //VOLOGI("[NPW]Playing time %d, np %d, offset %d", nRet, m_nCurrPos, m_nPlayingTimeOffset);
    
    return nRet;
}

int CBaseWrap::OnNativePlayerEvent(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    CBaseWrap* pWrap = (CBaseWrap*)pUserData;
    if(pWrap)
        return pWrap->doOnNativePlayerEvent(nID, pParam1, pParam2);
    return VONP_ERR_None;
}

int CBaseWrap::doOnNativePlayerEvent(int nID, void * pParam1, void * pParam2)
{
    int nRet = VONP_ERR_None;
    
    if(nID == NATIVE_PLAYER_EVENT_POS_UPDATE)
    {
        if(!pParam1)
            return VONP_ERR_None;
        
        //AVPlayer maybe continue to call this while seek(m_nCurrPos will set to zero while seeking)
        if(m_nCurrPos==0 && *((int*)pParam1)>1000)
        {
            m_nPlayingTimeOffset -= *((int*)pParam1);
        }
            
        m_nCurrPos = *((int*)pParam1);
        
        int nCurrPos = GetPlayingTime();
        
        if(m_nLastPlayingTime != nCurrPos)
        {
            m_nLastPosUpdateTime    = voOS_GetSysTime();
            m_nLastPlayingTime      = nCurrPos;
        }

        return VONP_ERR_None;
    }
    else if(nID == NATIVE_PLAYER_EVENT_START_RUN)
    {
        VOLOGI("[NPW]Player start, ready run use time %lu, np buffer %d", voOS_GetSysTime() - m_nReadyRunUseTime, m_pNativePlayer?m_pNativePlayer->GetBufferTime():0);
        
        m_nLastPosUpdateTime    = voOS_GetSysTime();
        doProcessPlayerReadyRun();
    }
    else if(nID == NATIVE_PLAYER_EVENT_START_BUFFER)
    {
        if(!IsPause())
        {
            NotifyBuffering(true);
        }
    }
    else if(nID == NATIVE_PLAYER_EVENT_STOP_BUFFER)
    {
        NotifyBuffering(false);
    }
    
    return nRet;
}

void CBaseWrap::doProcessPlayerReadyRun()
{
    m_bSeeked       = false;
    
    if(!m_bAudioOnly)
    {
        VOLOGI("[NPW]Notify video render start, playing time %d, sys time %lu", GetPlayingTime(), voOS_GetSysTime());
        SendEvent(VONP_CB_VideoRenderStart, NULL, NULL);        
    }
}

VO_U32 CBaseWrap::ReadBufThreadProc(VO_PTR pParam)
{
	CBaseWrap* pPlayer = (CBaseWrap*)pParam;
	
	if(!pPlayer)
		return 0;
	
	return pPlayer->doReadBufThread();
}

int CBaseWrap::CreateReadThread()
{
	if(m_hReadBufThread)
		return VONP_ERR_None;
	
	m_bStopReadThread = false;
    
	VO_U32 nID = 0;
	voThreadCreate(&m_hReadBufThread, &nID, ReadBufThreadProc, this, 0);
	return VONP_ERR_None;
}

int CBaseWrap::DestroyReadThread()
{
	if(m_hReadBufThread)
	{
		VOLOGI("[NPW]set m_bStopReadThread true");
		m_bStopReadThread = true;
		
		int nCount = 0;
		while (m_hReadBufThread && nCount<2500)
		{
			nCount++;
			voOS_Sleep(2);
		}
	}
	
	return VONP_ERR_None;
}


VO_U32 CBaseWrap::doReadBufThread()
{
    VONP_BUFFERTYPE buf;
	memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    
    bool    bForceFormatChanged = false;
    int     nMaxForceTryCount   = MAX_FORCE_READ_TRY_COUNT;
	int     nForceTryCount      = 0;
	bool	bShowAudioLog		= false;
	bool	bShowVideoLog		= false;
	bool	bUseAudioOffsetTime	= false;
	int		nPosOffsetTime		= 0;
	int     nReadRet			= VONP_ERR_None;
    
    VOLOGI("[NPW]+Enter read thread");
	
	while (!m_bStopReadThread)
	{
		if(m_bPauseReadBuf || !m_ReadBufFunc.ReadAudio || !m_ReadBufFunc.ReadVideo)
		{
            //VOLOGI("[NPW]No reading data right now...");
            m_semSeek.Signal();
			voOS_Sleep(2);
			continue;
		}
        
        if(m_peekBufInfo.bPeeking && m_peekBufInfo.bEnable)
            PeekBuffer(&buf);

        //save memory
        if(!IsPeekBufAvailable())
            ResetPeekBuffer();
        
        //voCAutoLock lock(&m_mtStatus);
    
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
            
            VOLOGI("[NPW] -------->> BA happened, Time %d, Force %d, A %d, V %d, A-Only %d, V-Only %d, playing %d, %02d:%02d:%02d", nPosOffsetTime, bForceFormatChanged?1:0, m_bReadAudioNewFormat?1:0, m_bReadVideoNewFormat?1:0, m_bAudioOnly?1:0, m_bVideoOnly?1:0, GetPlayingTime(), nPosOffsetTime/1000/3600, ((nPosOffsetTime/1000)%3600)/60, (((nPosOffsetTime/1000)%3600)%60));

            if(!m_pMuxStream)
                CreateMemStream();

            if(m_pMuxStream)
            {
                m_pMuxStream->SetVideoOnly(m_bVideoOnly);
                m_pMuxStream->SetAudioOnly(m_bAudioOnly);
            }
            
            if(m_pFrameTools)
                m_pFrameTools->ResetFrameCount();
            
            if(!m_bAudioOnly)
                m_bWaitKeyFrame = true;
            
            if(!m_bFirstRun && !IsAppResume())
                OnFormatChanged();
            else
            {
                m_nPlayingTimeOffset = nPosOffsetTime;
                m_bFirstRun = false;
            }
            
            
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
                VOLOGW("[NPW]BA by audio only,use render outside");
                
                //hold on in read thread
                continue;
            }
        }
        
        m_nTryReadBufCount++;
		
		//read audio
		if((IsCanReadBuffer(true) && !m_bForceReadVideoNewFmt) || m_bForceReadAudioNewFmt)
		{
			memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
            nReadRet = ReadBuffer(true, &buf);
			if(bShowAudioLog || m_nTryReadBufCount<20 || m_bForceReadAudioNewFmt)
                VOLOGI(" [NPW]AUDIO, flag %x, %08lld, size %06d, diff %lld, buf %x, data %x, ret %d, audio %d,video %d, %lu", buf.nFlag, buf.llTime, buf.nSize, buf.llTime-m_nLastReadAudioTime, (int)buf.pBuffer, (int)buf.pData, nReadRet, m_bAudioOnly, m_bVideoOnly, voOS_GetSysTime());
            
			if(nReadRet == VONP_ERR_Retry)
			{
				//if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
				{
					//VOLOGW("[NPW]Read audio retry... %d", voOS_GetSysTime());
					voOS_Sleep(2);
					//continue;
				}
			}
            else if(nReadRet == VONP_ERR_Audio_No_Now)
			{
				VOLOGW("[NPW]Read audio inavailable");

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
				VOLOGW("[NPW]Reading audio buf reach EOS");
				m_nStatusEOS |= 0x01;
				
				if(m_bAudioOnly)
					m_nStatusEOS |= 0x02;
                
                m_bForceReadAudioNewFmt = false;
                
                if(m_nStatusEOS&0x01 && m_nStatusEOS&0x02)
                    OnReadBufEOS();
			}
            else if(nReadRet == VONP_ERR_None)
            {
                if(buf.nFlag & VONP_FLAG_BUFFER_NEW_FORMAT || buf.nFlag & VONP_FLAG_BUFFER_NEW_PROGRAM)
                {
                    VONP_BUFFER_FORMAT* pFmt = (VONP_BUFFER_FORMAT*)buf.pData;
                    
                    if(buf.pData)
                    {
                        VOLOGI("[NPW]Read audio new format, codec %d, S %d, C %d, B %d, flag %d, time %lld, playing %d, last audio %d, try %d", pFmt->nCodec, pFmt->sFormat.audio.nSampleRate, pFmt->sFormat.audio.nChannels, pFmt->sFormat.audio.nSampleBits, buf.nFlag, buf.llTime, GetPlayingTime(), m_nLastReadAudioTime, nForceTryCount);
                        
                        VOLOGI("A Ntive player buf: %d", m_pNativePlayer->GetBufferTime());
                 
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_NEW_FORMAT;
                            continue;
                        }

                        m_AVFrameWriter.Write(true, (unsigned char*)pFmt->pHeadData, pFmt->nHeadDataSize, 0, false, true);
                        
                        if(!CheckCodecSupport(true, pFmt->nCodec))
                        {
                            VOLOGI("[NPW]+++++++++++Audio format NOT support. %d+++++++++++++++", pFmt->nCodec);
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
                            VOLOGI("[NPW]continue to read audio");
                            continue;
                        }
                        else if(!m_bAudioOnly)
                        {
                            // continue to read video
                            VOLOGI("[NPW]go to read video");
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
                        VOLOGI("[NPW]Got first audio frame, time %lld, size %d", buf.llTime, buf.nSize);
                        if(m_pFrameTools)
                            m_pFrameTools->UpdateDurationPerFrame(&buf);
                        m_bWaitFirstAudioFrame = false;
                    }
                    
                    if(abs(buf.llTime-m_nLastReadAudioTime) > MAX_DISCONTINUOS_TIME && !bFirstAudio)
                    {
                        VOLOGW("[NPW]-------->> Audio time discontinuos, curr %lld, last %d, diff %lld", buf.llTime, m_nLastReadAudioTime, buf.llTime-m_nLastReadAudioTime);
//                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
//                        {
//                            m_peekBufInfo.bPeeking      = true;
//                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_DISCONTINUOS;
//                            continue;
//                        }
                    }
                    
                    int nDiff = buf.llTime - m_nLastReadAudioTime;
                    //VOLOGI("[NPW]diff %f", (float)nDiff/(float)m_pFrameTools->GetDurationPerFrame());
                    if( ((float)nDiff/(float)m_pFrameTools->GetDurationPerFrame())>=MAX_AUDIO_INTERVAL_SCALE && nDiff<=500 && !m_bAudioOnly && !bFirstAudio)
                    {
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            VOLOGW("[NPW]-------->> Audio time interval is so large, curr %lld, last %d, diff %lld, frame duration %d", buf.llTime, m_nLastReadAudioTime, buf.llTime-m_nLastReadAudioTime, m_pFrameTools->GetDurationPerFrame());
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_AUDIO_LARGE_INTERVAL;
                            continue;
                        }
                    }

                    //CheckFlushBuffer(&buf);
                    m_AVFrameWriter.Write(true, buf.pBuffer, buf.nSize, buf.llTime, false, false);
                    
                    if(CheckAudioDropFrame(&buf))
                    {
                        m_nLastReadAudioTime = buf.llTime;
                        continue;
                    }
                    
                    if(m_bReadAudioNewFormat)
                    {
                        // if both audio and video, set false when sending video head data
                        bUseAudioOffsetTime = true;
                    }
                    
                    //if(CheckValidADTSFrame(&buf))
                        SendRawFrame(true, &buf);
                    
                    m_nLastReadAudioTime = buf.llTime;
                    
                    if(m_bForceReadAudioNewFmt && m_bReadVideoNewFormat && !m_peekBufInfo.bEnable)
                    {
                        if(buf.llTime >= m_nLastVideoNewFormatTime)
                        {
                            VOLOGW("[NPW]Force got audio new format: %lld", buf.llTime);
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
                        VOLOGW("[NPW]Can't read available audio buffer after read audio new format and video inavailable");
                        voOS_Sleep(2);
                        continue;
                    }
                }
                
                VOLOGW("[NPW]read audio NULL - %d", nReadRet);
                voOS_Sleep(2);
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
            
            voOS_Sleep(2);
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
            
            voOS_Sleep(2);
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
			memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
            nReadRet = ReadBuffer(false, &buf);
			if(bShowVideoLog || m_nTryReadBufCount<20 || m_bForceReadVideoNewFmt)
                VOLOGI("[NPW]VIDEO, flag %x, %08lld, size %06d, diff %lld, buf %x, data %x, ret %d, audio %d,video %d, %lu", buf.nFlag, buf.llTime, buf.nSize, buf.llTime-m_nLastReadVideoTime, (int)buf.pBuffer, (int)buf.pData, nReadRet, m_bAudioOnly, m_bVideoOnly, voOS_GetSysTime());
            
            if(nReadRet == VONP_ERR_Video_No_Now)
			{
				VOLOGW("[NPW]Read video inavailable");
                
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
				VOLOGW("[NPW]Reading video buf reach EOS");
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
                        VOLOGI("[NPW]Read video new format, codec %d, width %d, height %d, flag %d, time %lld, try %d", pFmt->nCodec, pFmt->sFormat.video.nWidth, pFmt->sFormat.video.nHeight, buf.nFlag, buf.llTime, nForceTryCount);
                        VOLOGI("V Ntive player buf: %d", m_pNativePlayer->GetBufferTime());
                        
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_VIDEO_NEW_FORMAT;
                            continue;
                        }

                        m_AVFrameWriter.Write(false, (unsigned char*)pFmt->pHeadData, pFmt->nHeadDataSize, 0, false, true);
                        //m_AVFrameReader.Read(false, (unsigned char*)pFmt->pHeadData, pFmt->nHeadDataSize, llTmp, bTmp, bTmp);
                        
                        if(!CheckCodecSupport(false, pFmt->nCodec))
                        {
                            m_bSupportVideoCodec = false;
                            VOLOGE("[NPW]+++++++++++Video format NOT support. %d+++++++++++", pFmt->nCodec);
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
                        
                        m_bReadVideoNewFormat       = true;
                        m_bForceReadVideoNewFmt     = false;
                        m_nLastReadVideoTime        = buf.llTime;
                        m_nLastVideoNewFormatTime   = buf.llTime;
                        
                        if(!m_bReadAudioNewFormat && !m_bVideoOnly)
                        {
                            VOLOGI("[NPW]ready to read audio new format flag");
                            nForceTryCount = 0;
                            m_bForceReadAudioNewFmt = true;
                        }
                        
                        // continue to read audio
                        if(!m_bVideoOnly)
                        {
                            VOLOGI("[NPW]go to read audio");
                            continue;
                        }
                    }
                    else
                    {
                        VOLOGE("[NPW]No head data found...");
                    }
                }
                else
                {
                    if(m_bReadAudioNewFormat && !m_bReadVideoNewFormat)
                    {
                        //VOLOGI("[NPW]Can't read video format after read audio new format");
                    }
                }
                
                if(buf.pBuffer && buf.nSize>0 && m_bSupportVideoCodec)
                {
                    if(abs(buf.llTime-m_nLastReadVideoTime) > MAX_DISCONTINUOS_TIME)
                    {
                        VOLOGW("[NPW]-------->> Video time discontinuos, curr %lld, last %d, diff %lld", buf.llTime, m_nLastReadVideoTime, buf.llTime-m_nLastReadVideoTime);
                        if(!m_peekBufInfo.bPeeking && !IsPeekBufAvailable() && m_peekBufInfo.bEnable)
                        {
                            m_bBACasuedByVideoTimeNotDiscontinuo = true;
                            m_peekBufInfo.bPeeking      = true;
                            m_peekBufInfo.nPeekTrigger  = PEEK_TRIGGER_VIDEO_DISCONTINUOS;
                            continue;
                        }
                    }
                    
                    //CheckFlushBuffer(&buf);
                    m_AVFrameWriter.Write(false, buf.pBuffer, buf.nSize, buf.llTime, buf.nFlag&VONP_FLAG_BUFFER_KEYFRAME?true:false, false);
                    
                    if(CheckVideoDropFrame(&buf))
                    {
                        m_nLastReadVideoTime = buf.llTime;
                        continue;
                    }
                    
                    if(m_bWaitKeyFrame)
                    {
                        if(buf.nFlag & VONP_FLAG_BUFFER_KEYFRAME)
                        {
                            VOLOGI("[NPW]Got a key frame - %d...", (int)buf.llTime);
                            m_bWaitKeyFrame		= false;

                            if(m_bSeeked)
                                m_nLastVideoNewFormatTime = buf.llTime;

                            SendVideoHeadData(buf.llTime);
                            
                            if(m_nPosOffsetTime > buf.llTime)
                                VOLOGE("[NPW]+++++++++++Head data time large than key frame! head %d, key %d+++++++++++", m_nPosOffsetTime, (int)buf.llTime);
                        }
                        else
                        {
                            VOLOGW("[NPW]Waitting key frame..., %lld dropped", buf.llTime);
                            voOS_Sleep(1);
                            m_nLastReadVideoTime = buf.llTime;
                            continue;
                        }
                    }
                    
                    bUseAudioOffsetTime = false;
                    SendRawFrame(false, &buf);
                    m_nLastReadVideoTime = buf.llTime;
                    
                    if(m_bForceReadVideoNewFmt && m_bReadAudioNewFormat && !m_peekBufInfo.bEnable)
                    {
                        if(buf.llTime >= m_nLastAudioNewFormatTime)
                        {
                            VOLOGW("[NPW]Force got video new format: %lld", buf.llTime);
                            bForceFormatChanged         = true;
                            m_bForceReadVideoNewFmt     = false;
                            continue;
                        }
                    }
                }
            }
            else // read failed
            {
                //VOLOGW("[NPW]read video NULL - %d, %d", nReadRet, voOS_GetSysTime());
                voOS_Sleep(2);
            }
		}
        
		//voOS_Sleep(2);
	}
	
	m_hReadBufThread = NULL;
	VOLOGI("[NPW]-Exit read thread!!!");
	
	return 0;
}

bool CBaseWrap::SendVideoHeadData(int nTimeStamp)
{
    //VOLOGI("[NPW]00 Send video head data, time %d, size %d", nTimeStamp);
    
	if(!m_VideoTrackInfo.pHeadData || m_VideoTrackInfo.nHeadDataSize<=0)
		return false;
    
    VOLOGI("[NPW]Send video head data, time %d, size %d", nTimeStamp, m_VideoTrackInfo.nHeadDataSize);
	
#if 0
	VONP_BUFFERTYPE buf;
	memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
	
	buf.llTime	= nTimeStamp;
	buf.pBuffer	= (unsigned char*)m_VideoTrackInfo.pHeadData;
	buf.nSize	= m_VideoTrackInfo.nHeadDataSize;
     
    int nRet = SendRawFrame(false, &buf);
#else
    
    VO_SINK_SAMPLE sample;
	memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
	sample.Buffer	= (unsigned char*)m_VideoTrackInfo.pHeadData;
	sample.Size		= m_VideoTrackInfo.nHeadDataSize;
	sample.nAV		= 1;
	sample.Time		= nTimeStamp;
	sample.DTS		= -1;
    
    int nRet = VONP_ERR_Pointer;
    
    if(m_pMuxStream)
        nRet = m_pMuxStream->SendHeadData(&sample);
#endif
 	
	return nRet==VONP_ERR_None;
}

bool CBaseWrap::SendAudioHeadData(int nTimeStamp)
{
	if(!m_AudioTrackInfo.pHeadData || m_AudioTrackInfo.nHeadDataSize<=0)
		return false;
    
    //VOLOGW("[NPW]Currently we don't need mux audio head data,sometimes it will cause AV sync issue.");
    return false;
	
	VONP_BUFFERTYPE buf;
	memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
	
	buf.llTime	= nTimeStamp;
	buf.pBuffer	= (unsigned char*)m_AudioTrackInfo.pHeadData;
	buf.nSize	= m_AudioTrackInfo.nHeadDataSize;
	
	VOLOGI("[NPW]Send audio head data, time %d, size %d", nTimeStamp, m_AudioTrackInfo.nHeadDataSize);
	int nRet = SendRawFrame(true, &buf);
	
	return nRet==VONP_ERR_None;
}

bool CBaseWrap::AdjustTimestamp(VO_SINK_SAMPLE* pMuxSample, VONP_BUFFERTYPE* pBuf)
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
            
            VOLOGI("[NPW][Seek] return");
            return false;
        }
    }
    else if(!bAudio)
    {
        long long nTS = m_pFrameTools->GetVideoFrameTime(pBuf);
        
        if(nTS >= 0)
        {
            //VOLOGI("[NPW]YYY VIDEO %08lld, %08lld", sample.Time*90, sample.Time);
        }
        
        pMuxSample->Time = nTS;
        return true;
    }
    
    return false;
}

int CBaseWrap::SendRawFrame(bool bAudio, VONP_BUFFERTYPE* pFrame)
{
	int nRet = VONP_ERR_Retry;
    
	VO_SINK_SAMPLE sample;
	memset(&sample, 0, sizeof(VO_SINK_SAMPLE));
	sample.Buffer	= pFrame->pBuffer;
	sample.Size		= pFrame->nSize;
	sample.nAV		= bAudio?0:1;
	sample.Time		= pFrame->llTime;
	sample.DTS		= -1;
	
	if(!IsSupportAudioCodec() || !IsSupportVideoCodec())
	{
		VOLOGW("[NPW]Cancel send raw frame,%s %d %d by not support codec", bAudio?"audio":"video", (int)sample.Time, (int)sample.Size);
		return VONP_ERR_None;
	}
    
    if(m_pNativePlayer)
    {
        //VOLOGI("[NPW]Native player buffering duration %d, playing time %d, last audio %d, diff %d", m_pNativePlayer->GetBufferTime(), GetPlayingTime(), m_nLastReadAudioTime, m_nLastReadAudioTime-GetPlayingTime());
    }
	
#if 0
    static int nCount = 0;
	if(nCount%30==0)
    {
        //if(!bAudio)
        {
            VOLOGI("[NPW]+Send %s frame to mux: time %08lld, size %06d, diff %d, playing time %d", bAudio?"audio":"video", pFrame->llTime, pFrame->nSize,
                   (unsigned int)(pFrame->llTime-(bAudio?m_nLastReadAudioTime:m_nLastReadVideoTime)), GetPlayingTime());
        }
    }
    nCount++;
#endif
	
 	DumpRawFrame(bAudio, sample.Buffer, sample.Size);
    
    if(!AdjustTimestamp(&sample, pFrame))
    {
        VOLOGW("[NPW]Drop frame %s %08lld", bAudio?"AUDIO":"VIDEO", pFrame->llTime);
        return VONP_ERR_None;
    }
    
    if(pFrame->nFlag & VONP_FLAG_BUFFER_DECODER_ONLY)
    {
        VOLOGW("Meet decode only flag: %lld, size %d", pFrame->llTime, pFrame->nSize);
    }
    
    if(pFrame->nFlag & VONP_FLAG_BUFFER_KEYFRAME)
        sample.Size |= 0X80000000;
	
    int nRetryTime      = m_bAudioOnly?1000*30:1000*15;
	int nRetryCount		= 0;
	int nRetrySleepTime	= 2;
	while (nRet == VONP_ERR_Retry)
	{
		nRetryCount++;
		
		if(IsStop() || IsSeeking() || m_bStopReadThread || m_bCancelSendRawFrame)
		{
			VOLOGW("[NPW]Cancel send raw frame,%s %d %d, stop %d, seeking %d, stop reading %d, cancel %d", bAudio?"audio":"video", (int)sample.Time, (int)sample.Size, IsStop()?1:0, IsSeeking()?1:0, m_bStopReadThread?1:0, m_bCancelSendRawFrame?1:0);
			break;
		}
        
		if(m_pMuxStream)
			nRet = m_pMuxStream->SendRawFrame(&sample, pFrame->llTime);
        else
            nRet = VONP_ERR_None;
		
		if(nRet == VONP_ERR_Retry)
		{
			if(nRetryCount*nRetrySleepTime > nRetryTime)
			{
				if(IsPause())
				{
					nRetryCount = 0;
					VOLOGW("[NPW]Retry to send raw frame because pause");
					continue;
				}
                
				VOLOGW("[NPW]++++Push %s buffer into mux timeout,Time %d, Size %d, Playing Time %d, mem size %d++++", bAudio?"audio":"video", (int)sample.Time, (int)sample.Size, GetPlayingTime(), m_pMuxStream->GetMemStreamSize());
				break;
			}
			
			voOS_Sleep(nRetrySleepTime);
		}
	}
	
	//VOLOGI("[NPW]-Send %s frame to mux: %d, %d", bAudio?"audio":"video", (unsigned int)pFrame->llTime, pFrame->nSize);
	
	return nRet;
}


bool CBaseWrap::IsCanReadBuffer(bool bAudio)
{
	//int nMaxAVOffsetTime	= 200;
	
	if(m_nLastReadAudioTime - GetPlayingTime() > 10*1000 && GetPlayingTime()>5*1000)
	{
		//VOLOGW("[NPW]Can't read buffer, last audio %d, play time %d, playing time %d, mem %d", m_nLastReadAudioTime, GetPlayingTime(), m_nPlayingTime, m_pMuxStream?0:m_pMuxStream->GetMemStreamSize());
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
		if(m_bAudioOnly)
			return true;
		if(m_nStatusEOS & 0x01)
			return false;
		// video EOS
		if(m_nStatusEOS & 0x02)
			return true;
		
		//20120424
		if(m_nLastReadVideoTime==0 || m_nLastReadAudioTime==0)// || (m_nLastReadAudioTime-m_nLastReadVideoTime) <= nMaxAVOffsetTime)
		{            
			//VOLOGI("[NPW]Can read audio frame A %d, V %d, B %d, D %d", m_nLastReadAudioTime, m_nLastReadVideoTime, m_pMuxStream?m_pMuxStream->GetMemStreamSize():0, (m_nLastReadAudioTime-m_nLastReadVideoTime));

//            if(m_bVideoOnly)
//                return false;

			return true;
		}
		
		if(abs(m_nLastReadAudioTime-m_nLastReadVideoTime) > 20*1000)
			return true;
        
        if(m_nLastReadAudioTime <= m_nLastReadVideoTime)
            return true;
	}
	else // video
	{
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
		if(m_nLastReadVideoTime==0 || m_nLastReadAudioTime==0)// || (m_nLastReadVideoTime-m_nLastReadAudioTime) <= nMaxAVOffsetTime)
		{
			//VOLOGI("[NPW]Can read video frame A %d, V %d, B %d, D %d", m_nLastReadAudioTime, m_nLastReadVideoTime, m_pMuxStream?m_pMuxStream->GetMemStreamSize():0, (m_nLastReadAudioTime-m_nLastReadVideoTime));
            
//            if(m_bAudioOnly)
//                return false;

			return true;
		}
		
		if(abs(m_nLastReadAudioTime-m_nLastReadVideoTime) > 20*1000)
			return true;
        
        if(m_nLastReadVideoTime <= m_nLastReadAudioTime)
            return true;
	}
    
	return false;
}

bool CBaseWrap::CheckCodecSupport(bool bAudio, int nCodec)
{
	bool bSupport = false;
	
	if(bAudio)
	{
		if(VONP_AUDIO_CodingAAC == nCodec)
		{
			return true;
		}
	}
	else
	{
		if(VONP_VIDEO_CodingH264 == nCodec)
		{
			return true;
		}
	}
    
	return bSupport;
}

bool CBaseWrap::CheckValidBA(int nBATime)
{
    //temp disable this feature
    return true;
    
    if(m_nLastBATime == -1)
        return true;
    
    if(abs(nBATime-m_nLastBATime) < 2000)
    {
        VOLOGW("[NPW]++++++++++++++++++++++++++++Maybe invalid BA, new %d, old %d", nBATime, m_nLastBATime);
        return false;
    }
    
    return true;
}

void CBaseWrap::CopyTrackInfo(bool bAudio, VONP_BUFFER_FORMAT* pDst, VONP_BUFFER_FORMAT* pSrc)
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
            VOLOGI("[NPW]Head data: %x %x", ((VO_BYTE*)pSrc->pHeadData)[0], ((VO_BYTE*)pSrc->pHeadData)[1]);
        
        //DumpHeadData(bAudio, (unsigned char*)pDst->pHeadData, pDst->nHeadDataSize);
	}
}

void CBaseWrap::DumpRawFrame(bool bAudio, unsigned char* pBuffer, int nSize)
{
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
			FILE* hCheck = fopen("/data/local/OMXALDUMP/raw_data.dat", "rb");
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
        FILE* hFile = NULL;
        
#if defined(_IOS) || defined(_MAC_OS)
        char* pszDump = NULL;
        char szTmp[1024];
        memset(szTmp, 0, 1024);
        voOS_GetAppFolder(szTmp, 1024);
        strcat(szTmp, "voDebugFolder/");
        
        if(bAudio)
            strcat(szTmp, "raw_audio.aac");
        else
            strcat(szTmp, "raw_video.h264");
        
        pszDump = szTmp;
#else
        char* pszDump = NULL;
        
        if(bAudio)
            pszDump = "/data/local/tmp/raw_audio.aac";
        else
            pszDump = "/data/local/tmp/raw_video.h264";
#endif
        
		if(bAudio)
		{
			static FILE* hAudio = NULL;
			
			if(!hAudio)
            {
                hAudio = fopen(pszDump, "wb");
            }
            
            if(!hAudio)
				VOLOGI("[NPW]audio frame dump file failed. %s", pszDump);
            
            if(pBuffer==NULL && nSize==0)
            {
                fclose(hAudio);
                hAudio = NULL;
                return;
            }
			
			hFile = hAudio;
		}
		else
		{
			static FILE* hVideo = NULL;
            
            if(!hVideo)
            {
                hVideo = fopen(pszDump, "wb");
            }
			
			if(!hVideo)
				VOLOGI("[NPW]video frame dump file failed. %s", pszDump);
            
            if(pBuffer==NULL && nSize==0)
            {
                fclose(hVideo);
                hVideo = NULL;
                return;
            }
			
			hFile = hVideo;
		}
		
		if(pBuffer && (nSize>0) && hFile)
		{
			fwrite(pBuffer, 1, nSize, hFile);
		}
    }
}

bool CBaseWrap::CheckAudioDropFrame(VONP_BUFFERTYPE* pBuf)
{
	if(pBuf->llTime < m_nLastAudioNewFormatTime)
	{
		VOLOGW("[NPW]Audio drop frame,time %lld, size %d, flag %d, last audio new format %d", pBuf->llTime, pBuf->nSize, pBuf->nFlag, m_nLastAudioNewFormatTime);
		return true;
	}
	else
	{
	}
	
	return false;
}


bool CBaseWrap::CheckValidADTSFrame(VONP_BUFFERTYPE* pBuf)
{
    unsigned char* pData = pBuf->pBuffer;
    
    if(pData == NULL)
    {
        return false;
    }
    
    //7 is the adts header length
    if(pBuf->nSize < 7)
    {
        VOLOGW("[NPW]Find invalid ADTS by buffer size error");
        return false;
    }
    
    if( ((*pData)!=0xFF) || ((*(pData+1))&0xF0)!=0xF0 )
    {
        VOLOGW("[NPW]Find invalid ADTS by key flag");
        return false;
    }
    
    //Get the frame Length
    int nLen = ((pData[3] & 0x3) << 11) + (pData[4] << 3) + (pData[5] >> 5);
    
    if(nLen != pBuf->nSize)
    {
        VOLOGW("[NPW]Find invalid ADTS by buffer size is not match");
        return false;
    }
    
    return true;
}


bool CBaseWrap::CheckVideoKeyFrame(VONP_BUFFERTYPE* pBuf)
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
			//bRet = IsKeyFrame_H264(pBuf->pBuffer, pBuf->nSize, 0);
            bRet = m_pFrameTools->IsKeyFrame(pBuf)?VO_TRUE:VO_FALSE;
			
			if(bRet)
			{
				pBuf->nFlag |= VONP_FLAG_BUFFER_KEYFRAME;
				//VOLOGW("[NPW]Found a key frame %lld, %d", pBuf->llTime, pBuf->nSize);
				return true;
			}
		}
	}
	
	return false;
}


bool CBaseWrap::CheckH264Resolution(VONP_BUFFERTYPE* pBuf)
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
        VOLOGI("[NPW]Check video format: %lux%lu", info.Width, info.Height);
        
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
    
    
    return nRet==0?VONP_ERR_None:VONP_ERR_Unknown;
}

bool CBaseWrap::CheckVideoDropFrame(VONP_BUFFERTYPE* pBuf)
{
    if(pBuf->nFlag & VONP_FLAG_BUFFER_KEYFRAME)
        return false;
    
    if(m_bWaitKeyFrame)
        return false;
    
	//if(pBuf->llTime < (m_nLastReadAudioTime-1000))
    //if(pBuf->llTime < m_nLastBATime)
    if(pBuf->llTime < (m_nLastVideoNewFormatTime-1000))
	{
		VOLOGW("[NPW]Video drop frame,time %lld, size %d, flag %d, audio %d, BA %d", pBuf->llTime, pBuf->nSize, pBuf->nFlag, m_nLastReadAudioTime, m_nLastVideoNewFormatTime);
		return true;
	}
	else
	{
		//VOLOGI("%lld, %d, %d", pBuf->llTime, m_nLastReadAudioTime, m_nLastReadVideoTime);
	}
	
	return false;
}



bool CBaseWrap::CreateMemStream()
{    
    m_pMuxStream = new CTsMuxStream(m_pLibOP, &m_fmtAudio);
    
    m_pMuxStream->SetVideoOnly(m_bVideoOnly);
    m_pMuxStream->SetAudioOnly(m_bAudioOnly);
    
    m_pMuxStream->Init();
    m_pMuxStream->SetCacheDuration(0);
    m_pMuxStream->SetMaxMemStreamSize(0X7FFFFFFF);

    return true;
}

bool CBaseWrap::DestroyMemStream()
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


int CBaseWrap::CreateEventNotifyThread()
{
	if(m_hEventNotifyThread)
		return VONP_ERR_None;
	
    m_bStopEventNotifyThread = false;
	VO_U32 nID = 0;
	voThreadCreate(&m_hEventNotifyThread, &nID, EventNotifyThreadProc, this, 0);
	return VONP_ERR_None;
}

int CBaseWrap::DestroyEventNotifyThread()
{
	if(m_hEventNotifyThread)
	{
        m_bStopEventNotifyThread = true;
        
		int nCount = 0;
		while (m_hEventNotifyThread && nCount<2500)
		{
			nCount++;
			voOS_Sleep(2);
		}
	}
	
	return VONP_ERR_None;
}

VO_U32 CBaseWrap::EventNotifyThreadProc(VO_PTR pParam)
{
	CBaseWrap* pPlayer = (CBaseWrap*)pParam;
	
	if(!pPlayer)
		return VONP_ERR_Pointer;
	
	return pPlayer->doEventNotifyThread();
}

VO_U32 CBaseWrap::doEventNotifyThread()
{
    VOLOGI("[NPW]Enter event notify thread");
    
	//while (!IsStop())
    while(!m_bStopEventNotifyThread)
	{
		//VOLOGI("[NPW]Check event notify thread");
		
		CheckEOS();
		
		//CheckBuffering();
		
		//CheckSeekMode();
        
        CheckBufferReady();
        
        CheckBAInfoChanged();
        
		voOS_Sleep(2);
	}
	
	m_hEventNotifyThread = NULL;
	VOLOGI("[NPW]Exit event notify thread");
	
	return VONP_ERR_None;
}


void CBaseWrap::DestroyHeadData()
{
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
}

int CBaseWrap::CreateNativePlayer()
{
    int nRet = VONP_ERR_None;
    
    m_pNativePlayer = new CBaseNativePlayer();
    
    if(m_pNativePlayer)
    {
        nRet = m_pNativePlayer->Init();
        
        VONP_LISTENERINFO event;
        memset(&event, 0, sizeof(VONP_LISTENERINFO));
        event.pUserData = this;
        event.pListener = OnNativePlayerEvent;
        m_pNativePlayer->SetParam(VONP_PID_LISTENER, &event);
        
        if(m_pView)
            nRet = m_pNativePlayer->SetView(m_pView);
    }
    
    return nRet;
}

void CBaseWrap::DestroyNativePlayer()
{
    if(m_pNativePlayer)
    {
        m_pNativePlayer->Close();
        delete m_pNativePlayer;
        m_pNativePlayer = NULL;
    }
}


int CBaseWrap::OnFormatChanged()
{
    return VONP_ERR_Implement;
}

void CBaseWrap::NotifyBuffering(bool bEnable)
{
	if(bEnable)
	{
		if(m_bBuffering)
			return;
		
		m_bBuffering = true;
		VOLOGI("[NPW]Notify buffering, enable, playing time %d, sys time %lu", GetPlayingTime(), voOS_GetSysTime());
		SendEvent(VONP_CB_StartBuff, NULL, NULL);
	}
	else
	{
		if(!m_bBuffering)
			return;
		
		m_bBuffering = false;
		VOLOGI("[NPW]Notify buffering, disable, playing time %d, sys time %lu", GetPlayingTime(), voOS_GetSysTime());
		SendEvent(VONP_CB_StopBuff, NULL, NULL);
	}
}

//bool CBaseWrap::CheckBuffering()
//{
//    unsigned int nPeriodTime = 1200;
//    
//    if(IsPause())
//        return false;
//    if(m_nLastPosUpdateTime == 0)
//        return false;
//	
//	if( (voOS_GetSysTime()-m_nLastPosUpdateTime) > nPeriodTime)
//	{
//		//NotifyBuffering(true);
//		return true;
//	}
//	else
//	{
//        if(m_bSeeked)
//            return false;
//		
//		//NotifyBuffering(false);
//	}
//	
//	return false;
//}

void CBaseWrap::SendEOS()
{
    SendEvent(VONP_CB_PlayComplete, NULL, NULL);
    
    //reset EOS to support loop playback,it will call SetPos(0)
    m_nStatusEOS = 0;
}

bool CBaseWrap::CheckEOS()
{
	//int nAdjustOffset		= 1000;
	int nPlayingTime = 0;
	nPlayingTime = GetPlayingTime();
	
	//VOLOGI("[NPW]Checking EOS...,playing %d, video %d, audio %d, EOS %d, pos update %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, m_nStatusEOS, voOS_GetSysTime()-m_nLastPosUpdateTime);
	
	if(m_nStatusEOS&0x01 && m_nStatusEOS&0x02)
	{
		//VOLOGI("[NPW]Checking EOS...,playing %d, video %d, audio %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime);
		
		if(IsRunning())
		{
			if((voOS_GetSysTime()-m_nLastPosUpdateTime) > 1000)
			{
                //seek to EOS
                if(m_nLastPosUpdateTime==0)
                    return false;
                
                //fix bug:for short duration clip(maybe 1 s),it should not eos becasue playback does not begin even read EOS
                if(GetPlayingTime() <= 0)
                {
                    VOLOGI("[NPW]Checking EOS...,playing %d, last video %d, last audio %d", GetPlayingTime(), m_nLastReadAudioTime, m_nLastReadVideoTime);
                    return false;
                }
                
                if(!m_bEOSed)
                {
                    VOLOGI("[NPW]Play complete,playing %d, audio %d, video %d, delay %lu, pos update %d", nPlayingTime, m_nLastReadAudioTime, m_nLastReadVideoTime, voOS_GetSysTime()-m_nLastPosUpdateTime, m_nLastPosUpdateTime);
                    
                    m_bEOSed = true;
                    SendEOS();
                }
				
				return true;
			}
		}
	}
	
	return false;
}


int CBaseWrap::CheckAspectRatio(VONP_BUFFERTYPE* pBuf)
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
            VOLOGI("[NPW]Found new aspect ratio %d", nAspectRatio);
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
        VOLOGW("[NPW]Aspect ratio failed,%d", nAspectRatio);
    }
    
    return nAspectRatio;
}

void CBaseWrap::CreateFrameTools()
{
    if(m_pFrameTools)
        return;
    
    m_pFrameTools = new CAVFrameTools;
    m_pFrameTools->Init();
}

void CBaseWrap::DestroyFrameTools()
{
    if(m_pFrameTools)
    {
        m_pFrameTools->Uninit();
        delete m_pFrameTools;
        m_pFrameTools = NULL;
    }
}

void CBaseWrap::DestroyBAInfo()
{
    BA_CHANGE_INFO* pInfo = m_sBAChangeInfoList.RemoveHead();
    
    while (pInfo)
    {
        //VOLOGI("[NPW]Destroy BA info: %lld", pInfo->llUpdateTimestamp);
        
        delete pInfo;
        pInfo = m_sBAChangeInfoList.RemoveHead();
    }
}

bool CBaseWrap::CheckBAInfoChanged()
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
                VOLOGI("[NPW]Notify res change: %dx%d", pInfo->nVideoWidth, pInfo->nVideoHeight);
                pInfo->bNeedVideoSizeUpdated = false;
                SendEvent(VONP_CB_VideoSizeChanged, &pInfo->nVideoWidth, &pInfo->nVideoHeight);
                bRet = true;
            }

            if(pInfo->bNeedAspectRatioUpdated && GetPlayingTime()>=pInfo->llUpdateTimestamp)
            {
                VOLOGI("[NPW]Notify aspect ratio chaged: %d", pInfo->nAspectRatio);
                pInfo->bNeedAspectRatioUpdated = false;
                SendEvent(VONP_CB_VideoAspectRatio, &pInfo->nAspectRatio, NULL);
                bRet = true;
            }
        }
        
        pos = m_sBAChangeInfoList.Next(pos);
    }

    return bRet;
}

void CBaseWrap::EnaleDumpRawFrame(bool bEnable)
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
        m_AVFrameWriter.Init(szTmp);
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

long long CBaseWrap::GetLastReadTime()
{
    if(m_bAudioOnly)
        return m_nLastReadAudioTime;

    if(m_bVideoOnly)
        return m_nLastReadVideoTime;
    
    return m_nLastReadAudioTime;
}

int CBaseWrap::OnReadBufEOS()
{
    m_bAudioOnly = false;
    m_bVideoOnly = false;
    
    return VONP_ERR_None;
}

VONP_BUFFERTYPE* CBaseWrap::CloneBuffer(VONP_BUFFERTYPE* pBuf)
{
    VONP_BUFFERTYPE* pClone = new VONP_BUFFERTYPE;
    memset(pClone, 0, sizeof(VONP_BUFFERTYPE));
    
    pClone->nSize       = pBuf->nSize;
    pClone->llTime      = pBuf->llTime;
    pClone->nFlag       = pBuf->nFlag;
    
    // we use llReserve to save ReadBuffer return code,here has issue if llReserve is used
    if(pBuf->llReserve > 0)
        VOLOGE("[NPW]Reserve is used!!!");
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

VONP_BUFFERTYPE* CBaseWrap::CloneSilentBuffer(VONP_BUFFERTYPE* pBuf)
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


void CBaseWrap::ReleasePeekBuffer(VONP_BUFFERTYPE* pBuf)
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


void CBaseWrap::ReleasePeekBuffer(CObjectList<VONP_BUFFERTYPE>* pList)
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

void CBaseWrap::ResetPeekBuffer()
{
    //VOLOGI("[NPW]Reset peek buffer");
    
    m_peekBufInfo.sAudioBuf.bPeekBufAvailable  = false;
    m_peekBufInfo.sVideoBuf.bPeekBufAvailable  = false;
    
    //VOLOGI("[NPW]%d, %d, %d", m_peekBufInfo.sAudioBuf.sList.GetCount(), m_peekBufInfo.sVideoBuf.sList.GetCount(), m_peekBufInfo.sRecycleBuf.GetCount());
    //release audio peek buffer
    ReleasePeekBuffer(&m_peekBufInfo.sAudioBuf.sList);
    
    //release video peek buffer
    ReleasePeekBuffer(&m_peekBufInfo.sVideoBuf.sList);
    
    ReleasePeekBuffer(&m_peekBufInfo.sRecycleBuf);
}

bool CBaseWrap::PeekBuffer(VONP_BUFFERTYPE* pBuf)
{
    VOLOGI("[NPW]+Peek buffer: trigger %d, time %lld. last audio %d, last video %d\n",
           m_peekBufInfo.nPeekTrigger, pBuf->llTime, m_nLastReadAudioTime, m_nLastReadVideoTime);

    ResetPeekBuffer();
 
    VONP_BUFFERTYPE buf;
    memset(&buf, 0, sizeof(VONP_BUFFERTYPE));
    
    int nUseTime                = voOS_GetSysTime();
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
        
        VOLOGI("[NPW]00 -Peek buffer, BA: A %lld, V %lld, last audio %lld, count %d, last video %lld, count %d, use time %lu\n", pAudioNewFormat?pAudioNewFormat->llTime:-1, pVideoNewFormat?pVideoNewFormat->llTime:-1,
               llLastAudioTime, m_peekBufInfo.sAudioBuf.sList.GetCount(), llLastVideoTime, m_peekBufInfo.sVideoBuf.sList.GetCount(), voOS_GetSysTime()-nUseTime);
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

        VOLOGI("[NPW]01 -Peek buffer, BA: A %lld, V %lld, last audio %lld, count %d, last video %lld, count %d, use time %lu\n", pAudioNewFormat?pAudioNewFormat->llTime:-1, pVideoNewFormat?pVideoNewFormat->llTime:-1,
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
        VOLOGW("[NPW]Peek buffer wrong status! trigger %d", m_peekBufInfo.nPeekTrigger);
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
                VOLOGI("[NPW]Need exit peek buffer...");
                m_peekBufInfo.bPeeking = false;
                ResetPeekBuffer();
                m_semSeek.Signal();
                break;
            }
            else
            {
                //VOLOGI("[NPW]Pause status,no reading data right now......");
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
                    VOLOGI("[NPW]Peek video: got new format, time %llu", buf.llTime);
                    
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
                
                VOLOGI("[NPW]Peek video buf: %d, %lld", buf.nSize, buf.llTime);
                llLastVideoTime = buf.llTime;
            }
            else if(nRet == VONP_ERR_Retry)
			{
                voOS_Sleep(2);
			}
            else if(nRet == VONP_ERR_Video_No_Now)
			{
				VOLOGW("[NPW]Peek video: Read video inavailable");
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
				VOLOGW("[NPW]Peek video: Reading video buf reach EOS");
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
                    VOLOGI("[NPW]Peek audio: got new format, time %lld", buf.llTime);
                    
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
                
                VOLOGI("[NPW]Peek audio buf: %d, %lld", buf.nSize, buf.llTime);
                
                llAudioDiffTime = buf.llTime - llLastAudioTime;
                llLastAudioTime = buf.llTime;
            }
            else if(nRet == VONP_ERR_Retry)
			{
                voOS_Sleep(2);
			}
            else if(nRet == VONP_ERR_Audio_No_Now)
			{
				VOLOGW("[NPW]Peek audio: Read audio inavailable");
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
				VOLOGW("[NPW]Peek audio: Reading audio buf reach EOS");
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
            VOLOGW("[NPW]Peek buffer wrong status!");
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
                    VOLOGW("[NPW]Audio/Video BA time not same: A %lld, V %lld, diff %lld, audio count %d, video count %d",
                           pAudioNewFormat->llTime, pVideoNewFormat->llTime, pAudioNewFormat->llTime-pVideoNewFormat->llTime,
                           m_peekBufInfo.sAudioBuf.sList.GetCount(), m_peekBufInfo.sVideoBuf.sList.GetCount());
                    
                    bool bOk = false;
                    
                    if(pAudioNewFormat->llTime < pVideoNewFormat->llTime)
                    {
                        VONP_BUFFERTYPE* pTail = m_peekBufInfo.sAudioBuf.sList.GetTail();
                        
//                      if(m_peekBufInfo.sAudioBuf.sList.GetCount() > 1)
                        {
                            VOLOGW("[NPW]Remove new format time %lld", pAudioNewFormat->llTime);
                            m_peekBufInfo.sAudioBuf.sList.Remove(pAudioNewFormat);
                        }

                        //adjust aduio new format time
                        long long llBackupTime = pAudioNewFormat->llTime;
                        pAudioNewFormat->llTime = pVideoNewFormat->llTime;

                        if(InsertPeekBuffer(&m_peekBufInfo.sAudioBuf.sList, pAudioNewFormat))
                        {
                            bOk = true;
                            
                            VOLOGW("[NPW]Re-insert new foramt to %lld", pAudioNewFormat->llTime);
                        }
                        else
                        {
                            pAudioNewFormat->llTime = llBackupTime;
                            
                            //VONP_BUFFERTYPE* pTail = m_peekBufInfo.sAudioBuf.sList.GetTail();
                            if(pTail)
                            {
                                VOLOGW("[NPW]Audio last time %lld", pTail->llTime);
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
//                                VOLOGW("[NPW]Finish drop audio frame to sync up video time");
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
                VOLOGI("[NPW]Can't peek audio new format, EOS %s, try count %d, buf count %d", bAudioEOS?"yes":"no", nTryReadBufCount, m_peekBufInfo.sVideoBuf.sList.GetCount());
                
                if(m_bForcePeekBuf && !bAudioEOS)
                {
                    VOLOGW("[NPW]Try to read audio new format because we don't read any audio EOS or unavailble!");
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
                        VOLOGW("[NPW]Insert new aduio format failed!!! first audio buf time %lld, video new format %lld",
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
                VOLOGE("[NPW]Can't peek video new format, EOS %s, try count %d, buf count %d", bVideoEOS?"yes":"no", nTryReadBufCount, m_peekBufInfo.sVideoBuf.sList.GetCount());
                
                if(pAudioNewFormat && !bVideoEOS)
                {
                    if(!m_VideoTrackInfo.pHeadData)
                    {
                        VOLOGW("[NPW]Why not read video new format because we don't read any video EOS or video unavailble!");
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
                        VOLOGW("[NPW]Insert new video format failed!!! first video buf time %lld, audio new format %lld",
                               pFirst?pFirst->llTime:-1, pAudioNewFormat->llTime);
                        
                        // last video time large than last audio time before peek buffers,so only can insert video new format before the first key frame
                        pNewBuf->llTime = pFirst?pFirst->llTime:0;
//                        m_peekBufInfo.sVideoBuf.sList.AddHead(pNewBuf);
                        
                        POSITION pKey = FindKeyFrame(&m_peekBufInfo.sVideoBuf.sList, pNewBuf->llTime);
                        if(pKey)
                        {
                            VOLOGI("[NPW]Re-insert video new format %lld before key frame %lld", pNewBuf->llTime, ((VONP_BUFFERTYPE*)m_peekBufInfo.sVideoBuf.sList.Get(pKey))->llTime);
                            
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
                            
                            VOLOGW("[NPW]Can't find key frame,add video new format to tail %lld", pNewBuf->llTime);
#else
                            
                            VOLOGW("[NPW]Can't find key frame,add video new format to tail %lld,ignore audio new format %lld", pNewBuf->llTime, pAudioNewFormat->llTime);
                            
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
                            VOLOGI("[NPW]Got key frame %lld, audio count %d, last %lld, video count %d, last %lld",
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
                                VONP_BUFFERTYPE* pFirst = m_peekBufInfo.sAudioBuf.sList.GetHead();
                                VOLOGW("[NPW]01 Insert new aduio format failed!!! first audio buf time %lld, audio new format %lld",
                                       pFirst?pFirst->llTime:-1, pNewBuf->llTime);
                                
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
            if(pVideoNewFormat || bVideoEOS)
            {
                if(pVideoNewFormat)
                {
                    VOLOGI("[NPW]Got video new format, %lld", pVideoNewFormat->llTime);
                }
                else
                {
                    VOLOGI("[NPW]Got video EOS");
                }
                
                bForceReadVideo         = false;
                m_peekBufInfo.bPeeking  = false;
            }
            else if(nTryReadBufCount >= nMaxTryCount)
            {
                VOLOGW("[NPW]Audio not available trigger try count %d", nTryReadBufCount);
                nTryReadBufCount = 0;
                continue;
            }
        }
        else if(m_peekBufInfo.nPeekTrigger == PEEK_TRIGGER_VIDEO_NOT_AVAILABLE)
        {
            if(pAudioNewFormat || bAudioEOS)
            {
                if(pAudioNewFormat)
                {
                    VOLOGI("[NPW]Got audio new format, %lld", pAudioNewFormat->llTime);
                }
                else
                {
                    VOLOGI("[NPW]Got audio EOS");
                }
                
                bForceReadAudio         = false;
                m_peekBufInfo.bPeeking  = false;
            }
            else if(nTryReadBufCount >= nMaxTryCount)
            {
                VOLOGW("[NPW]Video not available trigger try count %d", nTryReadBufCount);
                nTryReadBufCount = 0;
                continue;
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
                
                VOLOGI("[NPW]+Peek mock audio finished,process it. last %lld, first %lld, scale %f, count %d,%d",
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
                        VOLOGI("[NPW]Insert mock audio-00: %lld", pInsert->llTime);
                        sTmp.AddTail(pInsert);
                        
                        //last audio
                        if(nTotalCount == 1)
                        {
                            VOLOGI("[NPW]Insert mock audio-03: %lld", pExist->llTime);
                            sTmp.AddTail(pExist);
                            pExist = m_peekBufInfo.sAudioBuf.sList.RemoveHead();
                        }
                    }
                    else
                    {
                        VOLOGI("[NPW]Insert mock audio-01: %lld, %lld", llTime, pExist->llTime);
                    
                        pExist->llTime = llTime;
                        sTmp.AddTail(pExist);
                        
                        //not last audio
                        if(nTotalCount > 1)
                        {
                            llTime += nDurationPerFrame;
                            nTotalCount--;
                            pInsert->llTime = llTime;
                            sTmp.AddTail(pInsert);
                            VOLOGI("[NPW]Insert mock audio-02: %lld", llTime);                            
                        }
                        else
                        {
                            VOLOGI("[NPW]Don't insert this audio,%lld", pInsert->llTime);
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
                
                VOLOGI("[NPW]-Peek mock audio finished,process it. last %lld, first %lld, scale %f, count %d,%d, audio EOS %d",
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
    
    VOLOGI("[NPW]02 -Peek buffer, BA: A %lld, V %lld, last audio %lld, count %d, last video %lld, count %d, use time %lu\n", pAudioNewFormat?pAudioNewFormat->llTime:-1, pVideoNewFormat?pVideoNewFormat->llTime:-1,
           llLastAudioTime, m_peekBufInfo.sAudioBuf.sList.GetCount(), llLastVideoTime, m_peekBufInfo.sVideoBuf.sList.GetCount(), voOS_GetSysTime()-nUseTime);
    
    return true;
}


POSITION CBaseWrap::FindKeyFrame(CObjectList<VONP_BUFFERTYPE>* pList, long long llTimeBeforeKeyFrame)
{
    POSITION pos = pList->GetHeadPosition();
    
    long long llLastTime = 0XFFFFFFFF;
    
    while(pos)
    {
        VONP_BUFFERTYPE* pBuf = pList->Get(pos);
        
        if(pBuf)
        {
            //VOLOGI("[NPW]Check peek buf, time %lld", pBuf->llTime);
            
            //if(llTimeBeforeKeyFrame<pBuf->llTime && llTimeBeforeKeyFrame>llLastTime && pBuf->nFlag&VONP_FLAG_BUFFER_KEYFRAME)
            if(llTimeBeforeKeyFrame<=pBuf->llTime && pBuf->nFlag&VONP_FLAG_BUFFER_KEYFRAME)
            {
                VOLOGW("[NPW]Find key frame, time %lld before %lld", llTimeBeforeKeyFrame, pBuf->llTime);
                return pos;
            }
            
            llLastTime = pBuf->llTime;
        }
        
        pos = pList->Next(pos);
    }
    
    return NULL;
}

bool CBaseWrap::InsertPeekBuffer(CObjectList<VONP_BUFFERTYPE>* pList, VONP_BUFFERTYPE* pNewBuf)
{
    POSITION pos = pList->GetHeadPosition();
    
    long long llLastTime = 0XFFFFFFFF;
    
    while(pos)
    {
        VONP_BUFFERTYPE* pBuf = pList->Get(pos);
        
        if(pBuf)
        {
            //VOLOGI("[NPW]Check peek buf, time %lld", pBuf->llTime);
            
            //if(pNewBuf->llTime<pBuf->llTime && pNewBuf->llTime>=llLastTime)
            if(pNewBuf->llTime<=pBuf->llTime)
            {
                VOLOGW("[NPW]Insert peek buf, time %lld before %lld", pNewBuf->llTime, pBuf->llTime);
                pList->AddBefore(pos, pNewBuf);
                return true;
            }
            
            llLastTime = pBuf->llTime;
        }
        
        pos = pList->Next(pos);
    }
    
    return false;
}

bool CBaseWrap::IsPeekBufAvailable()
{
    if(m_peekBufInfo.sAudioBuf.bPeekBufAvailable || m_peekBufInfo.sVideoBuf.bPeekBufAvailable)
        return true;
    
    return false;
}

int CBaseWrap::ForceReadBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf)
{
    //disable it currentlly
    return VONP_ERR_Pointer;
    
    int nRet = VONP_ERR_Retry;
    
    if(m_ReadBufFunc.ReadAudio && m_ReadBufFunc.ReadVideo && m_ReadBufFunc.pUserData)
    {
        while (nRet == VONP_ERR_Retry)
        {
            if(bAudio)
                nRet = m_ReadBufFunc.ReadAudio(m_ReadBufFunc.pUserData, pBuf);
            else
                nRet = m_ReadBufFunc.ReadVideo(m_ReadBufFunc.pUserData, pBuf);
            
            if(m_bStopReadThread)
            {
                VOLOGW("[NPW]Exit force read buffer");
                break;
            }
        }
        
        voOS_Sleep(1);
    }
    
    VOLOGI("[NPW]Force read %s return, time %lld, size %d, ret %d", bAudio?"AUDIO":"VIDEO", pBuf->llTime, pBuf->nSize, nRet);
    
    return nRet;
}

int CBaseWrap::ReadBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf)
{
    int nRet = VONP_ERR_Retry;
    
    if(m_ReadBufFunc.ReadAudio && m_ReadBufFunc.ReadVideo && m_ReadBufFunc.pUserData)
    {
        if(bAudio)
        {
            if(m_peekBufInfo.sAudioBuf.bPeekBufAvailable)
            {
                nRet = ReadPeekBuffer(true, pBuf);
                
                if(nRet == -1)
                {
                    VOLOGI("[NPW]Read peek audio buffer FINISH");
                    
                    nRet = VONP_ERR_Retry;
                    m_peekBufInfo.sAudioBuf.bPeekBufAvailable = false;
                    
                    if(!m_peekBufInfo.sVideoBuf.bPeekBufAvailable)
                    {
                        VOLOGI("[NPW]Read peek buffer FINISH 00");
                    }
                }
                else
                {
                    VOLOGR("[NPW]Read peek audio: Time %08lld, Flag %d, Size %d, Ret %d", pBuf->llTime, pBuf->nFlag, pBuf->nSize, nRet);
                }
            }
            else
                nRet = m_ReadBufFunc.ReadAudio(m_ReadBufFunc.pUserData, pBuf);
        }
        else
        {
            if(m_peekBufInfo.sVideoBuf.bPeekBufAvailable)
            {
                nRet = ReadPeekBuffer(false, pBuf);
                
                if(nRet == -1)
                {
                    VOLOGI("[NPW]Read peek video buffer FINISH");
                    
                    nRet = VONP_ERR_Retry;
                    m_peekBufInfo.sVideoBuf.bPeekBufAvailable = false;
                    
                    if(!m_peekBufInfo.sAudioBuf.bPeekBufAvailable)
                    {
                        VOLOGI("[NPW]Read peek buffer FINISH 01");
                    }
                }
                else
                {
                    VOLOGR("[NPW]Read peek video: Time %08lld, Flag %d, Size %d, Ret %d", pBuf->llTime, pBuf->nFlag, pBuf->nSize, nRet);
                }
            }
            else
                nRet = m_ReadBufFunc.ReadVideo(m_ReadBufFunc.pUserData, pBuf);
        }
    }
    
    return nRet;
}

int CBaseWrap::ReadPeekBuffer(bool bAudio, VONP_BUFFERTYPE* pBuf)
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


