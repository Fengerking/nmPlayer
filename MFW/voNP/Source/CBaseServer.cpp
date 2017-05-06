//
//  CBaseServer.cpp
//  
//
//  Created by Jim Lin on 9/12/12.
//
//

#include "voLog.h"
#include "CBaseServer.h"
#include "CHLSM3u8Stream.h"
#include "CHttpRequest.h"
#include "voOSFunc.h"

#define _USE_LOCAL_FILE_SIMULATE_

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CBaseServer::CBaseServer()
:m_pM3u8Stream(NULL)
,m_aStreamList(NULL)
,m_nStreamCount(10)
,m_nRequestM3u8Count(0)
,m_nLastRequestM3u8Time(0)
,m_bInitStream(true)
,m_bEOS(false)
{
    memset(&m_NotifyServerEventInfo, 0, sizeof(VONP_LISTENERINFO));
    m_nLastRequestM3u8Time = voOS_GetSysTime();
}

CBaseServer::~CBaseServer(void)
{
	Uninit();
}

int CBaseServer::Init()
{
    int nRet = VONP_ERR_None;
    
    m_bInitStream = true;
    
    return nRet;
}

int CBaseServer::Uninit()
{
    Close();
    
    return VONP_ERR_None;
}

int CBaseServer::Open(void* pSource, int nFlag, char* pPlaybackURL)
{
    int nRet = VONP_ERR_None;
    
    Close();
    
    m_bInitStream = true;
    m_bEOS = false;
            
    m_aStreamList = new CHLSHttpStream *[m_nStreamCount];
	memset (m_aStreamList, 0, sizeof (CHttpBaseStream *) * m_nStreamCount);

    m_pM3u8Stream = new CHLSM3u8Stream(this);
    
    if(m_pM3u8Stream)
    {
        char szBasePath[1024];
        memset(szBasePath, 0, 1024);
        
#ifdef _USE_LOCAL_FILE_SIMULATE_
#if defined(_IOS) || defined(_MAC_OS)
        voOS_GetAppFolder(szBasePath, 1024);
        //strcat(szBasePath, "playback.m3u8");
#else
        //strcpy(szBasePath, "/data/local/playback.m3u8";
        strcpy(szBasePath, "/data/local/";
#endif
#else
        strcpy(szBasePath, "/");
#endif // _USE_LOCAL_FILE_SIMULATE_
               
        m_pM3u8Stream->Open(szBasePath, nFlag);
               
        //output the playback URL,just like http://127.0.0.1:8080/XXXXXXX.m3u8
        vostrcpy(pPlaybackURL, m_pM3u8Stream->GetM3u8URL());
    }
    
    PrepareStream();
    
    return nRet;
}

int CBaseServer::Close()
{
    int nRet = VONP_ERR_None;
    
    if(m_pM3u8Stream)
    {
        m_pM3u8Stream->Close();
        delete m_pM3u8Stream;
        m_pM3u8Stream = NULL;
    }
    
    if(m_aStreamList)
    {
        for (int i = 0; i < m_nStreamCount; i++)
        {
            if (m_aStreamList[i] != NULL)
                delete m_aStreamList[i];
        }
        
        delete []m_aStreamList;
        m_aStreamList = NULL;
    }
    
    m_bEOS = true;
    
    return nRet;
}

int CBaseServer::Run()
{
    int nRet = VONP_ERR_None;
    
    
    return nRet;
}

int CBaseServer::Pause()
{
    int nRet = VONP_ERR_None;
    
    
    return nRet;
}

int CBaseServer::Stop()
{
    int nRet = VONP_ERR_None;
    
    
    return nRet;
}


int CBaseServer::GetParam(int nParamID, void* pValue)
{
    return VONP_ERR_None;
}

int CBaseServer::SetParam(int nParamID, void* pValue)
{
	if(VONP_PID_LISTENER == nParamID)
	{
		VOLOGI("[NPW]Set event callback %x", (unsigned int)pValue);
		VONP_LISTENERINFO* pNotifyEvent = (VONP_LISTENERINFO*)pValue;
        m_NotifyServerEventInfo.pUserData = pNotifyEvent->pUserData;
        m_NotifyServerEventInfo.pListener = pNotifyEvent->pListener;
        return VONP_ERR_None;
	}
	else
	{
		VOLOGW("[NPW]++++++++++++++Set Param ID(%x) not implement!!!+++++++++++++++++++", nParamID);
	}
    
    return VONP_ERR_None;
}


int CBaseServer::SendEvent(int nEventID, void* pParam1, void*pParam2)
{
	int nRet = VONP_ERR_Pointer;
	
	if(m_NotifyServerEventInfo.pListener && m_NotifyServerEventInfo.pUserData)
		nRet = m_NotifyServerEventInfo.pListener(m_NotifyServerEventInfo.pUserData, nEventID, pParam1, pParam2);
	
	return nRet;
}

void CBaseServer::PrepareStream()
{
//    char szBasePath[1024];
//    memset(szBasePath, 0, 1024);
//    voOS_GetAppFolder(szBasePath, 1024);
    
    for (int n=0; n<m_nStreamCount; n++)
    {
        if(m_aStreamList[n] == NULL)
		{
			m_aStreamList[n] = new CHLSHttpStream(this);
            m_aStreamList[n]->SetID(n);
            
//            char szTmp[1024];
//            memset(szTmp, 0, 1024);
//            sprintf(szTmp, "%s%d.ts", TRUNK_FILE_NAME, n);
//            //VOLOGI("[NPW]%s", szTmp);
//            
//            m_aStreamList[n]->Open(szTmp, 0);
		}
    }
}

void CBaseServer::SetMaxPlaylistCount(int nCount)
{
    m_nStreamCount = nCount;
}


CHLSM3u8Stream* CBaseServer::GetM3u8Stream()
{
    return m_pM3u8Stream;
}


int CBaseServer::OnSocketEvent(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    if(!pUserData)
        return VONP_ERR_Pointer;
    
    CBaseServer* pServer = (CBaseServer*)pUserData;
    return pServer->doSocketEvent(nID, pParam1, pParam2);
}


int CBaseServer::doSocketEvent(int nID, void * pParam1, void * pParam2)
{
    return VONP_ERR_Implement;
}


CHttpBaseStream* CBaseServer::QueryStream(CHttpRequest* pRequest)
{
    voCAutoLock lock(&m_mtxStream);
    
    char szTmp[1024];
    char szURI[1024];
    memset(szTmp, 0, 1024);
    memset(szURI, 0, 1024);

    // check if m3u8 request
    if(m_pM3u8Stream)
    {
        strcpy(szTmp, m_pM3u8Stream->GetM3u8URL());
        char* finder = strrchr(szTmp, '/');
        
        if(finder)
            strcpy(szURI, finder);
        
        finder = strstr(pRequest->m_szTarget, szURI);
        
        if(finder)
        {
            //VOLOGI("[NPW]Find m3u8 stream: %s", szTmp);
            return m_pM3u8Stream;
        }
    }
    
    //check if video file request
    for (int n=0; n<m_nStreamCount; n++)
    {
        CHLSHttpStream* pStream = m_aStreamList[n];
        
        if(pStream)
		{
            //VOLOGI("[NPW] want stream %s", pRequest->m_szTarget);
            //VOLOGI("[NPW] curr stream %s", pStream->GetSourceURL());
            
            memset(szTmp, 0, 1024);
            memset(szURI, 0, 1024);
            strcpy(szTmp, pStream->GetSourceURL());

            if(0 == strlen(szTmp))
                continue;
            
            char* finder = strstr(pRequest->m_szTarget, szTmp);
            if(finder && pStream->IsAvailable())
            {
                //VOLOGI("[NPW]Found stream: %s, id %d, size %d", finder, pStream->GetID(), pStream->Size());
                return pStream;
            }
		}
    }
    
    return NULL;
}

CHttpBaseStream* CBaseServer::GetUnavailableStream()
{
    voCAutoLock lock(&m_mtxStream);
    
    if(!m_aStreamList)
        return NULL;
    
    for (int n=0; n<m_nStreamCount; n++)
    {
        CHttpSinkStream* pStream = (CHttpSinkStream*)m_aStreamList[n];
        
        if(pStream)
		{
            if(!pStream->IsAvailable())
            {
                //VOLOGI("[NPW]Find unavailable stream: %s", pStream->GetSourceURL());
                return pStream;
            }
		}
    }
    
    return NULL;
}

int CBaseServer::IsReady()
{    
    if(m_pM3u8Stream)
        return m_pM3u8Stream->IsReady();
    
    return false;
}
               
int CBaseServer::RemoveStream(CHttpBaseStream* pSteam)
{
    voCAutoLock lock(&m_mtxStream);
    
    if(m_pM3u8Stream)
    {
        m_pM3u8Stream->RemoveStream((CHLSHttpStream*)pSteam);
    }
    
    return VONP_ERR_None;                   
}

               
int CBaseServer::AddStream(CHttpBaseStream* pSteam)
{
    voCAutoLock lock(&m_mtxStream);
    
    if(m_pM3u8Stream)
    {
        m_pM3u8Stream->AddStream((CHLSHttpStream*)pSteam);
    }
    
    return VONP_ERR_None;
}
               
int CBaseServer::GetChunkDuration()
{
    if(m_pM3u8Stream)
        return m_pM3u8Stream->GetChunkDuration()*1000;

    return 0;
}

void CBaseServer::SetChunkDuration(int nDuration)
{
    if(m_pM3u8Stream)
        return m_pM3u8Stream->SetChunkDuration(nDuration);
}
               

bool CBaseServer::IsStreamFull()
{
    voCAutoLock lock(&m_mtxStream);
    
    if(!m_pM3u8Stream)
        return false;
    
    //int nCount = m_bInitStream?3:3;
    int nCount = m_bInitStream?m_pM3u8Stream->GetMaxChunkCount():m_pM3u8Stream->GetMaxChunkCount();
    
    bool bFull =  m_pM3u8Stream->GetAvailableStreamCount()>nCount;
    
    if(bFull)
        m_bInitStream = false;
    
    return bFull;
}
               
int CBaseServer::CreateM3U8()
{
   if(m_pM3u8Stream)
       m_pM3u8Stream->CreateM3u8();
    
    return VONP_ERR_None;
}


int CBaseServer::Flush()
{
   for (int n=0; n<m_nStreamCount; n++)
   {
       if(m_aStreamList && m_aStreamList[n])
       {
           m_aStreamList[n]->SetAvailable(false);
       }
   }

   if(m_pM3u8Stream)
       m_pM3u8Stream->Flush();
   
   return VONP_ERR_None;
}

void CBaseServer::SetEOS(bool bEOS)
{
    m_bEOS = bEOS;
    
    if(m_pM3u8Stream)
        m_pM3u8Stream->SetFlag(bEOS?STREAM_FLAG_EOS:STREAM_FLAG_UNKNOWN);
    
//    if(bEOS)
//    {
//        m_pM3u8Stream->DumpStreamListInfo(true);
//        m_pM3u8Stream->DumpStreamListInfo(false);
//    }
}
