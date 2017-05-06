/*
 *  CHLSServer.cpp
 *
 *  Created by Lin Jun on 10/09/12.
 *  Copyright 2011 VisualOn. All rights reserved.
 *
 */


#include "voOSFunc.h"
#include "voLog.h"
#include "CHLSServer.h"
#include "CHLSListenSocket.h"
#include "CHttpRequest.h"
#include "CHttpSocket.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CHLSServer::CHLSServer()
:m_pListenSocket(NULL)
,m_bListenSocketFail(false)
{
    memset(&m_NotifyServerEventInfo, 0, sizeof(VONP_LISTENERINFO));
    
    VOLOGI("[NPW][Seek] create server %x", (unsigned int)this);
}

CHLSServer::~CHLSServer(void)
{
	Uninit();
    
    VOLOGI("[NPW][Seek] destroy server %x", (unsigned int)this);
}

int CHLSServer::Init()
{
    int nRet = VONP_ERR_None;
    
    nRet = CBaseServer::Init();
    
    return nRet;
}

int CHLSServer::Uninit()
{
    Close();
    
    return VONP_ERR_None;
}

int CHLSServer::Open(void* pSource, int nFlag, char* pPlaybackURL)
{
    int nRet = VONP_ERR_Implement;
    
    m_bListenSocketFail = false;
    
    CBaseServer::Open(pSource, nFlag, pPlaybackURL);
    
    while (VONP_ERR_None != nRet)
    {
        if(m_pListenSocket)
        {
            m_pListenSocket->Close();
            delete m_pListenSocket;
            m_pListenSocket = NULL;
        }
        
        if(m_pM3u8Stream)
            m_pListenSocket = new CHLSListenSocket(this, m_pM3u8Stream->GetPortNumber());
        else
            m_pListenSocket = new CHLSListenSocket(this, 8080);
        
        if(m_pM3u8Stream)
            m_pListenSocket->SetStream(m_pM3u8Stream);
        
        //set socket event
        SOCKET_EVENT_CALLBACK_INFO info;
        info.pUserData  = this;
        info.pListener  = CBaseServer::OnSocketEvent;
        m_pListenSocket->SetEventCallback(&info);
        
        //open
        nRet = m_pListenSocket->Create();        
    }
    
    m_pListenSocket->Open(pSource, 0);
    
    if(m_pM3u8Stream)
        vostrcpy(pPlaybackURL, m_pM3u8Stream->GetM3u8URL());
    
    return nRet;
}

int CHLSServer::Close()
{
    int nRet = VONP_ERR_None;
    
    if(m_pListenSocket)
    {
        m_pListenSocket->Close();
        delete m_pListenSocket;
        m_pListenSocket = NULL;
    }
    
    CBaseServer::Close();
    
    return nRet;
}

int CHLSServer::Run()
{
    int nRet = VONP_ERR_None;
    
    if(m_pListenSocket)
        nRet = m_pListenSocket->Start();
    
    CBaseServer::Run();

    return nRet;
}

int CHLSServer::doSocketEvent(int nID, void * pParam1, void * pParam2)
{
    voCAutoLock lock(&m_mtxStream);
    
    int nRet = VONP_ERR_None;
    
    if(SOCKET_EVENT_HTTP_REQUEST == nID)
    {
        CHttpRequest* pRequest = (CHttpRequest*)pParam1;
        CHttpSocket* pSocket = (CHttpSocket*)pParam2;
        
        //VOLOGI("TEST %d", pSocket->GetSocket());
        
        if(pRequest)
        {
            CHLSHttpStream* pStream = (CHLSHttpStream*)QueryStream(pRequest);

            if(pStream && pSocket)
            {
                //pStream->SetCompressBuffer(pRequest->m_bGzip);
                
                if(pStream->IsM3u8Stream())
                {
                    int nCurrTime = voOS_GetSysTime();
                    
                    m_nRequestM3u8Count++;
                    //VOLOGI("[NPW]Client %x request M3u8: count %d, interval %d", (unsigned int)pSocket, m_nRequestM3u8Count, (nCurrTime-m_nLastRequestM3u8Time));
                    m_nLastRequestM3u8Time = nCurrTime;
                    
                    ((CHLSM3u8Stream*)pStream)->CreateM3u8();
                }
                else
                {
                    //VOLOGI("[NPW]Client %x request TS: %s", (unsigned int)pSocket, pRequest->m_szTarget);
                }

                // here seek to 0,actually it need check the request content range
                //pStream->Seek(0, 0);
                pStream->Seek(pRequest->m_lPosBegin, 0);
                pSocket->SetStream(pStream);
            }
        }
        else
        {
            VOLOGE("[NPW]Can't find request stream!!!");
        }
    }
    else if(SOCKET_EVENT_STREAM_EOS == nID)
    {
        //CHttpSocket* pRequest      = (CHttpSocket*)pParam1;
        CHLSHttpStream* pStream    = (CHLSHttpStream*)pParam2;
                
        if(pStream)
        {
            //VOLOGI("[NPW]Stream is EOS. %s", pStream->GetSourceURL());
            RemoveStream(pStream);
        }
    }
    else if(SOCKET_EVENT_SOCKET_ACCEPT_FAILED == nID)
    {
        if(m_bListenSocketFail)
            return nRet;
     
        VOLOGE("Socket accept failed!!!");
        m_bListenSocketFail = true;
        PostRunOnMainRequest(false, SOCKET_EVENT_SOCKET_ACCEPT_FAILED, NULL, NULL);
    }
    
    return nRet;
}

void CHLSServer::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    if(SOCKET_EVENT_SOCKET_ACCEPT_FAILED == nID)
    {
        SendEvent(SOCKET_EVENT_SOCKET_ACCEPT_FAILED, NULL, NULL);
    }
}

void CHLSServer::PrepareStream()
{
    //this will use stream as vidoe file
    return CBaseServer::PrepareStream();
  
    /*
    //this will use local file as video file,only for test
    char szBasePath[1024];
    memset(szBasePath, 0, 1024);
    voOS_GetAppFolder(szBasePath, 1024);
    
    for (int n=0; n<m_nStreamCount; n++)
    {
        if(m_aStreamList[n] == NULL)
		{
			m_aStreamList[n] = new CHttpFileStream(this);
            
            char szTmp[1024];
            memset(szTmp, 0, 1024);
            sprintf(szTmp, "%sfileSequence%d.ts", szBasePath, n);
            //VOLOGI("[NPW]%s", szTmp);
            
            m_aStreamList[n]->Open(szTmp, 0);
		}
    }
     */
}

int CHLSServer::Flush()
{
    int nRet = CBaseServer::Flush();
    
    if(m_pListenSocket)
    {
        nRet = m_pListenSocket->Flush();
    }
    
    return nRet;
}

int CHLSServer::Stop()
{
    int nRet = VONP_ERR_None;
    
    if(m_pListenSocket)
    {        
        m_pListenSocket->Stop();
    }
    
    return nRet;
}




