/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		CDownloaderImpl.cpp
 
 Contains:	CDownloaderImpl header file
 
 Written by:
 
 Change History (most recent first):
 2013-10-14		Jim			Create file
 
 *******************************************************************************/
#include "CDownloaderImpl.h"

#define LOG_TAG "CDownloaderImpl"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CDownloaderImpl::CDownloaderImpl ()
:m_pAPI(NULL)
,m_hDownloader(NULL)
{
    m_pAPI = new CDownloaderAPI;
    memset(&m_evtCallback, 0, sizeof(DOWNLOADER_API_EVENTCALLBACK));
}

CDownloaderImpl::~CDownloaderImpl ()
{
    Uninit ();
}


int CDownloaderImpl::Init (DOWNLOADER_API_EVENTCALLBACK* pCallBack, DOWNLOADER_API_INITPARAM* pParam )
{
    if(!m_pAPI)
        return -1;
    
    VOLOGI("[DLD]Init %x %x", (unsigned int)pCallBack, (unsigned int)pParam);
    
    if(pParam)
    {
        if(pParam->strWorkPath)
            m_pAPI->SetWorkPath((TCHAR*)pParam->strWorkPath);
        m_pAPI->LoadLib(NULL);
    }
    
    if(pCallBack)
    {
        m_evtCallback.pUserData = pCallBack->pUserData;
        m_evtCallback.SendEvent = pCallBack->SendEvent;
    }
    
    DOWNLOADER_API_EVENTCALLBACK callback;
    memset(&callback, 0, sizeof(DOWNLOADER_API_EVENTCALLBACK));
    callback.pUserData  = this;
    callback.SendEvent  = DownloaderEventCallback;
    m_pAPI->Init(&m_hDownloader, &callback, pParam);
    
    if(!m_hDownloader)
        return -1;
    
    return 0;
}

int CDownloaderImpl::Uninit ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Uninit");
    m_pAPI->Uninit(m_hDownloader);
    m_hDownloader = NULL;
    
    if(m_pAPI)
    {
        delete m_pAPI;
        m_pAPI = NULL;
    }

    return 0;
}

int CDownloaderImpl::Open (void* pSource, unsigned int uFlag, void* pLocalDir)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    if(!pSource || !pLocalDir)
        return -1;
    
    VOLOGI("[DLD]Open, flag %d, local %s, src %s", uFlag, (char*)pLocalDir, (char*)pSource);
    return m_pAPI->Open(m_hDownloader, pSource, uFlag, pLocalDir);
}

int CDownloaderImpl::Close ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Close");
    return m_pAPI->Close(m_hDownloader);
}

int CDownloaderImpl::StartDownload ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Start download");
    return m_pAPI->StartDownload(m_hDownloader);
}

int CDownloaderImpl::StopDownload ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Stop download");
    return m_pAPI->StopDownload(m_hDownloader);
}

int CDownloaderImpl::PauseDownload ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Pause download");
    return m_pAPI->PauseDownload(m_hDownloader);
}

int CDownloaderImpl::ResumeDownload ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;

    VOLOGI("[DLD]Resume download");
    return m_pAPI->ResumeDownload(m_hDownloader);
}

int CDownloaderImpl::GetParam (unsigned int nParamID, void* pParam)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Get param %d", nParamID);
    return m_pAPI->GetParam(m_hDownloader, nParamID, pParam);
}

int CDownloaderImpl::SetParam (unsigned int nParamID, void* pParam)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Set param %d", nParamID);
    return m_pAPI->SetParam(m_hDownloader, nParamID, pParam);
}

int  CDownloaderImpl::GetVideoCount ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    return m_pAPI->GetVideoCount(m_hDownloader);
}

int  CDownloaderImpl::GetAudioCount ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->GetAudioCount(m_hDownloader);
}

int  CDownloaderImpl::GetSubtitleCount ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->GetSubtitleCount(m_hDownloader);
}

int  CDownloaderImpl::SelectVideo (int nIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Select video %d", nIndex);
    return m_pAPI->SelectVideo(m_hDownloader, nIndex);
}

int  CDownloaderImpl::SelectAudio (int nIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Select audio %d", nIndex);
    return m_pAPI->SelectAudio(m_hDownloader, nIndex);
}

int  CDownloaderImpl::SelectSubtitle (int nIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Select subtitle %d", nIndex);
    return m_pAPI->SelectSubtitle(m_hDownloader, nIndex);
}

bool  CDownloaderImpl::IsVideoAvailable (int nIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->IsVideoAvailable(m_hDownloader, nIndex);
}

bool  CDownloaderImpl::IsAudioAvailable (int nIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->IsAudioAvailable(m_hDownloader, nIndex);
}

bool  CDownloaderImpl::IsSubtitleAvailable (int nIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->IsSubtitleAvailable(m_hDownloader, nIndex);
}

int  CDownloaderImpl::CommitSelection ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Commit selection");
    return m_pAPI->CommitSelection(m_hDownloader);
}

int  CDownloaderImpl::ClearSelection ()
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Clear selection");
    return m_pAPI->ClearSelection(m_hDownloader);
}

int  CDownloaderImpl::GetVideoProperty (int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Get video property: %d", nIndex);
    return m_pAPI->GetVideoProperty(m_hDownloader, nIndex, ppProperty);
}

int  CDownloaderImpl::GetAudioProperty (int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Get audio property: %d", nIndex);
    return m_pAPI->GetAudioProperty(m_hDownloader, nIndex, ppProperty);
}

int  CDownloaderImpl::GetSubtitleProperty (int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    
    VOLOGI("[DLD]Get subtitle property: %d", nIndex);
    return m_pAPI->GetSubtitleProperty(m_hDownloader, nIndex, ppProperty);
}

int  CDownloaderImpl::GetCurrPlayingTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->GetCurrPlayingTrackIndex(m_hDownloader, pCurrIndex);
}

int  CDownloaderImpl::GetCurrSelectedTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
    if(!m_pAPI || !m_hDownloader)
        return -1;
    return m_pAPI->GetCurrSelectedTrackIndex(m_hDownloader, pCurrIndex);
}


int CDownloaderImpl::DownloaderEventCallback (void* pUserData, unsigned int nID, void* nParam1, void* nParam2)
{
    if(!pUserData)
        return -1;
    
    CDownloaderImpl* pDld = (CDownloaderImpl*)pUserData;
    return pDld->onDownloadEvent(nID, nParam1, nParam2);
}

int CDownloaderImpl::onDownloadEvent(unsigned int nID, void* pParam1, void* pParam2)
{
    int nRet = -1;
    
    VOLOGI("[DLD]+Recieve downloader event %x", nID);
    
    if(nID == EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_OK)
    {
        VOLOGI("[DLD]Recieve manifest ok event: ");
        
        if(pParam1)
        {
            VOLOGI("[DLD]New URL is %s", (unsigned char*)pParam1);
        }
    }
    else if(nID == EVENTID_INFOR_STREAMDOWNLOADER_OPENCOMPLETE)
    {
        VOLOGI("[DLD]Recieve open complete event");
    }
    else if(nID == EVENTID_INFOR_STREAMDOWNLOADER_END)
    {
        VOLOGI("[DLD]Recieve download end");
    }
    else if(nID == EVENTID_INFOR_STREAMDOWNLOADER_PROGRAMINFO_CHANGE)
    {
        VOLOGI("[DLD]Recieve program info changed");
    }
    else if(nID == EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_UPDATE)
    {
        VOLOGI("[DLD]Recieve manifest updated");
        
        if(pParam1)
        {
            DOWNLOADER_PROGRESS_INFO* info = (DOWNLOADER_PROGRESS_INFO*)pParam1;
            VOLOGI("[DLD]Total duration %d, downloaded duration %d", info->TotalDuration, info->uCurrDuration);
        }
    }

    if(m_evtCallback.pUserData && m_evtCallback.SendEvent)
    {
        nRet = m_evtCallback.SendEvent(m_evtCallback.pUserData, nID, pParam1, pParam2);
    }
    
    VOLOGI("[DLD]-Recieve downloader event %x", nID);
    
    return nRet;
}


