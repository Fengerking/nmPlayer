/*
 *  COMXALMediaPlayer.cpp
 *
 *  Created by Lin Jun on 10/09/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */

#include "CHLSServerWrap.h"
#include "voOSFunc.h"
#include <string.h>
#include "voLog.h"
#include "voOSTools.h"

#ifdef _IOS
#include "CAVNativePlayer.h"
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#define _CREATE_SERVER_

CHLSServerWrap::CHLSServerWrap(VONP_LIB_FUNC* pLibOP)
:CBaseWrap(pLibOP)
,m_pServer(NULL)
,m_pCurrUnavailableStream(NULL)
,m_bServerReady(false)
,m_pOldNativePlayer(NULL)
,m_pOldServer(NULL)
{
    //m_nMaxFrameBufDepth = IsHighPerformanceDevices()?5*1000:10*1000;
    
    m_nMaxFrameBufDepth = 5*1000;
    
    const char* pszVersion = GetOSVersion();
    
    if(pszVersion)
    {
#ifdef _IOS
        //adjust buffer depth for devices version less than 5.0
        if(!strncasecmp(pszVersion, "4.", strlen("4.") ) || !strncasecmp(pszVersion, "3.", strlen("3.")) )
        {
            m_nMaxFrameBufDepth = 10*1000;
        }
#endif
    }
}

CHLSServerWrap::~CHLSServerWrap(void)
{
	
}

int CHLSServerWrap::CreateNativePlayer()
{
    int nRet = VONP_ERR_None;
    
#ifdef _IOS
    m_pNativePlayer = new CAVNativePlayer();
#endif
    
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

int CHLSServerWrap::Init()
{
    int nRet = VONP_ERR_None;
    
    nRet = CBaseWrap::Init();

    return nRet;
}

int CHLSServerWrap::CreateServer()
{
    if (NULL != m_pServer) {
        DestroyServer();
    }
    
    int nRet = VONP_ERR_None;
    
#ifdef _CREATE_SERVER_
    m_pServer = new CHLSServer;
#endif
    
    if(m_pServer)
    {
        VONP_LISTENERINFO event;
        memset(&event, 0, sizeof(VONP_LISTENERINFO));
        event.pUserData = this;
        event.pListener = OnServerEvent;
        m_pServer->SetParam(VONP_PID_LISTENER, &event);

        nRet = m_pServer->Init();
    }
    
    
    m_bServerReady = false;
    
    return nRet;
}

void CHLSServerWrap::DestroyServer()
{
    if(m_pServer)
    {
        m_pServer->Close();
        delete m_pServer;
        m_pServer = NULL;
    }    
}


int CHLSServerWrap::Uninit()
{
    CBaseWrap::Uninit();
        
    DestroyServer();
    
    //maybe seek
    DestroyOldInstance();
    
    return VONP_ERR_None;
}

int CHLSServerWrap::Open(void* pSource, int nFlag)
{
    CreateServer();
    
    int nRet = VONP_ERR_None;

    if(m_pServer)
    {
        memset(m_szPlaybackURL, 0, sizeof(1024));
        nRet = m_pServer->Open(NULL, 0, m_szPlaybackURL);
    }
    else
    {
        //just test code
        strcpy(m_szPlaybackURL, "http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8");
    }

    nRet = CBaseWrap::Open(pSource, nFlag);
                
    return nRet;
}

int CHLSServerWrap::Close()
{
    int nRet = VONP_ERR_None;
    

    if(m_pServer)
        nRet = m_pServer->Close();

    nRet = CBaseWrap::Close();
    
    DestroyServer();
    
    //maybe seek
    DestroyOldInstance();
    
    return nRet;
}

int CHLSServerWrap::SetPos(int nPos)
{
    voCAutoLock lock(&m_mtSeek);
    
    VOLOGI("[NPW][Seek] +Seek %d", nPos);

    if(IsLoading())
        return VONP_ERR_None;

    int nRet = VONP_ERR_None;
        
    nRet = CBaseWrap::SetPos(nPos);
    
    m_bServerReady  = false;
    
    if(m_pServer)
    {
        m_pServer->Flush();
        m_pServer->SetEOS(false);
    }
    
    if(!m_pOldServer)
    {
        m_pOldServer = m_pServer;
        m_pServer = NULL;
        VOLOGI("[NPW][Seek] backup old server %x", (unsigned int)m_pOldServer);
    }
    else
    {
        if(m_pServer)
        {
            VOLOGI("[NPW][Seek] will destroy server %x", (unsigned int)m_pServer);
            m_pServer->Close();
            delete m_pServer;
            m_pServer = NULL;
        }
    }
    
    CreateServer();
    if(m_pServer)
    {
        memset(m_szPlaybackURL, 0, 1024);
        nRet = m_pServer->Open(NULL, 0, m_szPlaybackURL);
    }

    VOLOGI("[NPW] -Seek return %d", nRet);
    
    return nRet;
}

int CHLSServerWrap::Run()
{
    int nRet = VONP_ERR_None;
    
    VOLOGI("[NPW]Run, max buffer depth %d, sys time %lu, build time:%s  %s", m_nMaxFrameBufDepth, voOS_GetSysTime(), __TIME__,  __DATE__);
	
	voCAutoLock lock(&m_mtStatus);
    
	if(IsRunning())
		return VONP_ERR_Status;
	
	CreateEventNotifyThread();

    //m_nLastPosUpdateTime    = voOS_GetSysTime();
    
	if(IsPause())
	{
        VOLOGI("[NPW]Run after pause");

        m_bPauseReadBuf	= false;
        m_bSeeking      = false;
        
        if(m_bSeeked)
        {
            //m_bSeeking = false;
        }
        else
        {
            if(m_pNativePlayer && !IsAppResume())
                nRet = m_pNativePlayer->Run();            
        }
        
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
    
    //test code
    if(!m_pServer)
    {
        if(m_pNativePlayer)
            nRet = m_pNativePlayer->Run();
    }
    
    VOLOGI("[NPW]-Run ret %d", nRet);
    
    if(m_pServer)
        nRet = m_pServer->Run();
    
    return nRet;
}

int CHLSServerWrap::Pause()
{
    int nRet = VONP_ERR_None;
    
    nRet = CBaseWrap::Pause();
    
    if(m_pServer)
        nRet = m_pServer->Pause();

    return VONP_ERR_None;
}

int CHLSServerWrap::Stop()
{
    int nRet = VONP_ERR_None;
    
    if(IsStop())
        return nRet;
    
    VOLOGI("[NPW]CHLSServerWrap player Stop");
    
    if(m_pNativePlayer)
        nRet = m_pNativePlayer->Stop();
    
    nRet = CBaseWrap::Stop();
    
    if(m_pServer)
       nRet = m_pServer->Stop();

    return nRet;
}


bool CHLSServerWrap::IsCanReadBuffer(bool bAudio)
{
    return CBaseWrap::IsCanReadBuffer(bAudio);
}

bool CHLSServerWrap::CheckServerBufferFull(long long llCurrTime)
{
    int nRet            = VONP_ERR_Retry;
    int nRetryTime      = m_bAudioOnly?1000*30:1000*15;
	int nRetryCount		= 0;
	int nRetrySleepTime	= 2;
    
    CHttpBaseStream* pUnavailableStream  = NULL;
    
	while (nRet == VONP_ERR_Retry)
	{
		nRetryCount++;
		
		if(IsStop() || IsSeeking() || m_bStopReadThread || IsPause() || m_bCancelSendRawFrame)
		{
            //VOLOGI("[NPW]Break %d, %lld", m_bSeeked?1:0, llCurrTime);
			break;
		}
        
        pUnavailableStream = m_pServer?m_pServer->GetUnavailableStream():NULL;
        
		if(m_pServer)
			nRet = m_pServer->IsStreamFull()?VONP_ERR_Retry:VONP_ERR_None;
        else
            nRet = VONP_ERR_None;
        
        //if((llCurrTime - GetPlayingTime()) >= m_nMaxFrameBufDepth)
        if(m_nCurrPos>=1*1000 && m_pNativePlayer->GetBufferTime()>=4000)
        {
            //VOLOGI("[NPW] %08lld %08d %d", GetLastReadTime(), GetPlayingTime(), m_bSeeked?1:0);
            //VOLOGI("[NPW] %08lld %08d %d", llCurrTime, GetPlayingTime(), m_bSeeked?1:0);
            
            //if(m_nCurrPos>=1*1000 && m_pNativePlayer->GetBufferTime()>=2000)
            {
                nRet = VONP_ERR_Retry;
                //VOLOGI("[NPW]Buffer depth is large, curr depth %lld, max depth %d, curr pos %d, playing time %d, last frame %lld, np buf %d", (llCurrTime - GetPlayingTime()), m_nMaxFrameBufDepth, m_nCurrPos, GetPlayingTime(), llCurrTime, m_pNativePlayer->GetBufferTime());
            }
        }
        
        if(voOS_GetSysTime()-m_nLastPosUpdateTime > 150)
        {
            //VOLOGW("Mux frame immediatelly...");
            if(pUnavailableStream)
                break;
        }
        
        if(!pUnavailableStream)
            nRet = VONP_ERR_Retry;
        
		if(nRet == VONP_ERR_Retry)
		{
			if(nRetryCount*nRetrySleepTime > nRetryTime)
			{                
                VOLOGW("[NPW]Wait server buffer to write timeout...");
                SendEvent(VONP_CB_NativePlayerError, NULL, NULL);
				break;
			}
			
//            int nBufTime = 0;
//            if(m_pNativePlayer)
//                nBufTime = m_pNativePlayer->GetBufferTime();
//            
//            VOLOGI("[NPW]+++Wait server ready to push stream, last A %d, last V %d, playing %d, buf %d+++", m_nLastReadAudioTime, m_nLastReadVideoTime, GetPlayingTime(), nBufTime);
			voOS_Sleep(nRetrySleepTime);
		}
	}
    
    return true;
}

int CHLSServerWrap::SendRawFrame(bool bAudio, VONP_BUFFERTYPE* pFrame)
{
    CheckServerBufferFull(pFrame->llTime);
        
    //bool bEOS                   = (m_nStatusEOS&0x01 && m_nStatusEOS&0x02);
    int nRet                    = VONP_ERR_None;
    int nDuration               = 0;
    CHLSHttpStream* pStream     = NULL;
    
    if(m_pMuxStream)
        nDuration = m_pMuxStream->GetMemStreamDuration();
    
    if(m_pServer)
        pStream = (CHLSHttpStream*)m_pServer->GetUnavailableStream();
    else
        return CBaseWrap::SendRawFrame(bAudio, pFrame);
    
    int nChunkDurtion = m_pServer->GetChunkDuration();
    
    //mux stream is full
    if(nDuration >= nChunkDurtion)
    {
        //but no availble stream to write, it need retry
        if(!pStream)
        {
            VOLOGW("[NPW]No availble stream to write");
            return VONP_ERR_Retry;
        }
    }
    else
    {
        // mux stream is not full
        nRet = CBaseWrap::SendRawFrame(bAudio, pFrame);
        return nRet;
    }
    
    if(VONP_ERR_None == nRet)
    {
        if(m_pMuxStream)
        {            
            if(pStream)
            {
                //m_pMuxStream->ForceOutputWholeStream();
                //m_pMuxStream->ResetTimestamp();
                
                int nTotalSize = m_pMuxStream->GetMemStreamSize();
                
                if(nTotalSize > 0)
                {
                    int nWantSize = nTotalSize;
                    unsigned char* pBuf = pStream->AllocBuffer(nWantSize);
                    
                    if(pBuf)
                    {
                        m_pMuxStream->ReadMuxBuffer((char*)pBuf, &nWantSize);
                        m_pMuxStream->FlushMem();
                    }
                    else
                    {
                        VOLOGE("[NPW]Alloc buf failed!!!");
                    }
                    
                    pStream->SetDuration(nDuration);
                    pStream->UpdateStream(pBuf, nWantSize);
                    
                    if(m_bFormatChanged)
                    {
                        if(!m_bSeeked)
                        {
                            if(m_bBACasuedByVideoTimeNotDiscontinuo)
                            {
                                VOLOGI("[NPW]++++++++++Set STREAM_FLAG_DISCONTINUITY flag, last time %lld++++++++++", m_pMuxStream->GetLastTime());
                                m_bBACasuedByVideoTimeNotDiscontinuo = false;
                                pStream->SetFlag(STREAM_FLAG_DISCONTINUITY);
                            }
                        }
                        else
                        {
                            //VOLOGI("[NPW]++++++++++Set STREAM_FLAG_DISCONTINUITY falg seek, last tiem %lld++++++++++", m_pMuxStream->GetLastTime());
                        }
                            
                        m_bFormatChanged = false;
                    }
                    
                    m_pServer->AddStream(pStream);

                    VOLOGI("[NPW]TS chunk ready: id %d, size %d, duration %d, buf %d, %s", pStream->GetID(), nWantSize, nDuration, m_pNativePlayer->GetBufferTime(), pStream->GetSourceURL());

                    ((CTsMuxStream*)m_pMuxStream)->ForceWritePATPMT();
                    
                    if(nWantSize < nTotalSize)
                    {
                        VOLOGW("[NPW]Size not match %d %d", nWantSize, nTotalSize);
                    }
                    else
                    {
                        //VOLOGI("[NPW]Read buf from mux %d, duration %d", nWantSize, nDuration);
                    }
                }
                else
                {
                    VOLOGE("[NPW]----------Why TS not output----------");
                }
                
                m_pMuxStream->ResetTimestamp();
            }
        }
    }
    
    //continue to mux this frame
    nRet = CBaseWrap::SendRawFrame(bAudio, pFrame);
    
    return nRet;
}

int CHLSServerWrap::SetView(void* pView)
{    
    m_pView = pView;
    
    if(m_bSeeked || IsAppResume())
        return VONP_ERR_None;
    
    int nRet = VONP_ERR_None;
    
    if(m_pNativePlayer)
        nRet = m_pNativePlayer->SetView(pView);
    
    return VONP_ERR_None;
}

bool CHLSServerWrap::CheckBufferReady()
{
    voCAutoLock lock(&m_mtSeek);
    
    //VOLOGI("[NPW]Playing time %d, last time %d, total buffer %d", GetPlayingTime(), m_nLastReadAudioTime, m_nLastReadAudioTime-GetPlayingTime());
    if(m_bServerReady)
        return true;
    
    if(m_pServer && m_pServer->IsReady())
    {        
        VOLOGI("[NPW]Prepare TS chunk ready, use time %d", voOS_GetSysTime()-m_nReadyRunUseTime);
        
        if(m_bSeeked || IsAppResume())
        {            
            if(m_pNativePlayer)
            {
                if(!m_pOldNativePlayer)
                    m_pOldNativePlayer = m_pNativePlayer;
                else
                {
                    if(m_pNativePlayer)
                    {
                        m_pNativePlayer->Stop();
                        m_pNativePlayer->Close();
                        delete m_pNativePlayer;
                        m_pNativePlayer = NULL;
                    }
                }
                
                VOLOGI("[NPW][Seek] server ready");
                
                //it must destroy it before create new native player using same view,so that view can be re-used
                if(IsAppResume())
                    DestroyOldInstance();
                
                CreateNativePlayer();
                m_pNativePlayer->SetView(m_pView);
                m_pNativePlayer->Open((unsigned char*)m_szPlaybackURL, 0);
                m_pNativePlayer->Run();

                //m_pNativePlayer->ReplaceURL((unsigned char*)m_szPlaybackURL);
            }
            
            if(m_bSeeked)
                SendEvent(VONP_CB_SeekComplete, NULL, NULL);
            
            m_nAppStatus    = APP_NORMAL;
        }
        else
        {
            if(m_pNativePlayer)
                m_pNativePlayer->Run();
        }
        
        m_bServerReady = true;
    }
    
    return false;
}


int CHLSServerWrap::OnFormatChanged()
{
    if(m_bSeeked)
        return VONP_ERR_None;
    
    CheckServerBufferFull(0);
    
    int nRet                    = VONP_ERR_None;
    int nDuration               = 0;
    CHLSHttpStream* pStream     = NULL;
    
    if(m_pMuxStream)
        nDuration = m_pMuxStream->GetMemStreamDuration();
    
    if(m_pServer)
        pStream = (CHLSHttpStream*)m_pServer->GetUnavailableStream();
    
    //int nChunkDurtion = m_pServer->GetChunkDuration();
    
    if(VONP_ERR_None == nRet)
    {
        //if(m_pMuxStream)
        {
            if(pStream)
            {
                int nMemSize = m_pMuxStream->GetMemStreamSize();
            
                m_pMuxStream->ForceOutputWholeStream();
                int nTotalSize = m_pMuxStream->GetMemStreamSize();
                
                VOLOGI("Mem size, before: %d, after: %d, mem durarion: %d", nMemSize, nTotalSize, nDuration);
                
                if(nTotalSize > 0)
                {
                    int nWantSize = nTotalSize;
                    unsigned char* pBuf = pStream->AllocBuffer(nWantSize);
                    m_pMuxStream->ReadMuxBuffer((char*)pBuf, &nWantSize);
                    m_pMuxStream->FlushMem();
                    
                    pStream->SetDuration(nDuration);
                    pStream->UpdateStream(pBuf, nWantSize);
                    m_pServer->AddStream(pStream);
                    
                    VOLOGI("[NPW]Fill unavailable stream: id %d, size %d, duration %d, last time %lld, %s", pStream->GetID(), nWantSize, nDuration, m_pMuxStream->GetLastTime(), pStream->GetSourceURL());
                    
                    if(nWantSize < nTotalSize)
                    {
                        VOLOGW("[NPW]Size not match %d %d", nWantSize, nTotalSize);
                    }
                    else
                    {
                        //VOLOGI("[NPW]Read buf from mux %d, duration %d", nWantSize, nDuration);
                    }
                }
                else
                {
                    VOLOGE("[NPW]----------Why TS not output----------");
                }
                
                m_pMuxStream->OnFormatChanged();
                m_pMuxStream->ResetTimestamp();
            }
        }
    }
    
    VOLOGI("[NPW]OnFormatChanged");
    m_bFormatChanged = true;
    ((CTsMuxStream*)m_pMuxStream)->ForceWritePATPMT();
    
    return VONP_ERR_None;
}

void CHLSServerWrap::doProcessPlayerReadyRun()
{
    voCAutoLock lock(&m_mtSeek);
    
    VOLOGI("[NPW]Process player ready to run");
    
    if(!m_bServerReady)
    {
        VOLOGW("[NPW][Seek] Server not ready even NP ready to run!!!");
        return;
    }
    
    SendEvent(VONP_CB_PlayerReadyRun, NULL, NULL);
    VOLOGI("[NPW][Seek] VONP_CB_PlayerReadyRun, NP %x, Old NP %x, Server %x, Old Server %x", (unsigned int)m_pNativePlayer, (unsigned int)m_pOldNativePlayer, (unsigned int)m_pServer, (unsigned int)m_pOldServer);
    
    //reset chunk duration to 1 second,default is 2 second
    m_pServer->SetChunkDuration(1);
    
    DestroyOldInstance();
    
    CBaseWrap::doProcessPlayerReadyRun();
}

void CHLSServerWrap::DestroyOldInstance()
{
    if(m_pOldNativePlayer)
    {
        m_pOldNativePlayer->Stop();
        m_pOldNativePlayer->Close();
        delete m_pOldNativePlayer;
        m_pOldNativePlayer = NULL;
    }
    
    if(m_pOldServer)
    {
        m_pOldServer->Close();
        delete m_pOldServer;
        m_pOldServer = NULL;
    }    
}

int CHLSServerWrap::OnReadBufEOS()
{
    VOLOGI("[NPW]Read total buffer EOS 00");
    
    CBaseWrap::OnReadBufEOS();
    
    int nRet                    = VONP_ERR_None;
    int nDuration               = 0;
    
    if(!m_pServer)
        return nRet;
    
    CHLSHttpStream* pStream     = (CHLSHttpStream*)m_pServer->GetUnavailableStream();
    //int nChunkDurtion           = m_pServer->GetChunkDuration();
    
    if(VONP_ERR_None == nRet)
    {
        if(m_pMuxStream)
        {
            if(pStream)
            {
                m_pMuxStream->ForceOutputWholeStream();
                m_pMuxStream->ResetTimestamp();
                
                int nTotalSize = m_pMuxStream->GetMemStreamSize();
                
                if(nTotalSize > 0)
                {
                    int nWantSize = nTotalSize;
                    unsigned char* pBuf = pStream->AllocBuffer(nWantSize);
                    
                    if(pBuf)
                    {
                        m_pMuxStream->ReadMuxBuffer((char*)pBuf, &nWantSize);
                        m_pMuxStream->FlushMem();
                    }
                    else
                    {
                        VOLOGE("[NPW]Alloc buf failed!!!");
                    }
                    
                    //VOLOGI("[NPW]Fill unavailable stream: id %d, size %d, duration %d, %s", pStream->GetID(), nWantSize, nDuration, pStream->GetSourceURL());
                    
                    pStream->SetDuration(nDuration);
                    pStream->UpdateStream(pBuf, nWantSize);
                    m_pServer->AddStream(pStream);
                    
                    ((CTsMuxStream*)m_pMuxStream)->ForceWritePATPMT();
                }
                else
                {
                    VOLOGE("[NPW]00 ----------Why TS not output----------");
                }
            }
        }
    }
    
    VOLOGI("[NPW]Read total buffer EOS 01");
    m_pServer->SetEOS(true);
    
    return nRet;
}


void CHLSServerWrap::doProcessResume(void* hView)
{
    m_bServerReady  = false;
    
    if(m_pServer)
    {
        m_pServer->Flush();
        m_pServer->SetEOS(false);
    }
    
    if(!m_pOldServer)
    {
        m_pOldServer = m_pServer;
        m_pServer = NULL;
        VOLOGI("[NPW][Resume] backup old server %x", (unsigned int)m_pOldServer);
    }
    else
    {
        if(m_pServer)
        {
            VOLOGI("[NPW][Resume] will destroy server %x", (unsigned int)m_pServer);
            m_pServer->Close();
            delete m_pServer;
            m_pServer = NULL;
        }
    }
    
    CreateServer();
    
    if(m_pServer)
    {
        memset(m_szPlaybackURL, 0, 1024);
        m_pServer->Open(NULL, 0, m_szPlaybackURL);
    }
    
    CBaseWrap::doProcessResume(hView);
}

int CHLSServerWrap::OnServerEvent(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    CHLSServerWrap* pWrap = (CHLSServerWrap*)pUserData;
    
    if(pWrap)
    {
        return pWrap->doOnServerEvent(nID, pParam1, pParam2);
    }
    
    return VONP_ERR_Pointer;
}

int CHLSServerWrap::doOnServerEvent(int nID, void * pParam1, void * pParam2)
{
    if(SOCKET_EVENT_SOCKET_ACCEPT_FAILED == nID)
    {
        SendEvent(VONP_CB_NativePlayerError, NULL, NULL);
    }
    
    return VONP_ERR_None;
}


