/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		CDownloaderAPI.cpp
 
 Contains:	CDownloaderAPI header file
 
 Written by:
 
 Change History (most recent first):
 2013-10-14		Jim			Create file
 
 *******************************************************************************/
#include "CDownloaderAPI.h"
#include "voStreamingDownloaderAPI.h"

#define LOG_TAG "CDownloaderAPI"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CDownloaderAPI::CDownloaderAPI ()
{
    memset(&m_api, 0, sizeof(ADAPTIVESTREAM_DOWNLOADER_API));
}

CDownloaderAPI::~CDownloaderAPI ()
{
}


int CDownloaderAPI::Init (void** phHandle, DOWNLOADER_API_EVENTCALLBACK* pCallBack, DOWNLOADER_API_INITPARAM* pParam )
{
    if(!pParam)
        return -1;
        
    if(!m_api.Init)
        return -1;
    
    return m_api.Init(phHandle, pCallBack, pParam);
}

int CDownloaderAPI::Uninit (void* hHandle)
{
    if(!m_api.Uninit)
        return -1;
    return m_api.Uninit(hHandle);
}

int CDownloaderAPI::Open (void* hHandle, void* pSource, unsigned int uFlag, void* pLocalDir)
{
    if(!m_api.Open)
        return -1;
    return m_api.Open(hHandle, pSource, uFlag, (wchar_t*)pLocalDir);
}

int CDownloaderAPI::Close (void* hHandle )
{
    if(!m_api.Close)
        return -1;
    return m_api.Close(hHandle);
}

int CDownloaderAPI::StartDownload (void* hHandle)
{
    if(!m_api.StartDownload)
        return -1;
    return m_api.StartDownload(hHandle);
}

int CDownloaderAPI::StopDownload (void* hHandle)
{
    if(!m_api.StopDownload)
        return -1;
    return m_api.StopDownload(hHandle);
}

int CDownloaderAPI::PauseDownload (void* hHandle)
{
    if(!m_api.PauseDownload)
        return -1;
    return m_api.PauseDownload(hHandle);
}

int CDownloaderAPI::ResumeDownload (void* hHandle)
{
    if(!m_api.ResumeDownload)
        return -1;
    return m_api.ResumeDownload(hHandle);
}

int CDownloaderAPI::GetParam (void* hHandle, unsigned int nParamID, void* pParam)
{
    if(!m_api.GetParam)
        return -1;
    return m_api.GetParam(hHandle, nParamID, pParam);
}

int CDownloaderAPI::SetParam (void* hHandle, unsigned int nParamID, void* pParam)
{
    if(!m_api.SetParam)
        return -1;
    return m_api.SetParam(hHandle, nParamID, pParam);
}

int CDownloaderAPI::GetVideoCount (void* pHandle)
{
    if(!m_api.GetVideoCount)
        return -1;
    return m_api.GetVideoCount(pHandle);
}

int CDownloaderAPI::GetAudioCount (void* pHandle)
{
    if(!m_api.GetAudioCount)
        return -1;
    return m_api.GetAudioCount(pHandle);
}

int CDownloaderAPI::GetSubtitleCount (void* pHandle)
{
    if(!m_api.GetSubtitleCount)
        return -1;
    return m_api.GetSubtitleCount(pHandle);
}

int CDownloaderAPI::SelectVideo (void* pHandle, int nIndex)
{
    if(!m_api.SelectVideo)
        return -1;
    return m_api.SelectVideo(pHandle, nIndex);
}

int CDownloaderAPI::SelectAudio (void* pHandle, int nIndex)
{
    if(!m_api.SelectAudio)
        return -1;
    return m_api.SelectAudio(pHandle, nIndex);
}

int CDownloaderAPI::SelectSubtitle (void* pHandle, int nIndex)
{
    if(!m_api.SelectSubtitle)
        return -1;
    return m_api.SelectSubtitle(pHandle, nIndex);
}

bool CDownloaderAPI::IsVideoAvailable (void* pHandle, int nIndex)
{
    if(!m_api.IsVideoAvailable)
        return -1;
    return m_api.IsVideoAvailable(pHandle, nIndex);
}

bool CDownloaderAPI::IsAudioAvailable (void* pHandle, int nIndex)
{
    if(!m_api.IsAudioAvailable)
        return -1;
    return m_api.IsAudioAvailable(pHandle, nIndex);
}

bool CDownloaderAPI::IsSubtitleAvailable (void* pHandle, int nIndex)
{
    if(!m_api.IsSubtitleAvailable)
        return -1;
    return m_api.IsSubtitleAvailable(pHandle, nIndex);
}

int CDownloaderAPI::CommitSelection (void* pHandle)
{
    if(!m_api.CommitSelection)
        return -1;
    return m_api.CommitSelection(pHandle);
}

int CDownloaderAPI::ClearSelection (void* pHandle)
{
    if(!m_api.ClearSelection)
        return -1;
    return m_api.ClearSelection(pHandle);
}

int CDownloaderAPI::GetVideoProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if(!m_api.GetVideoProperty)
        return -1;
    return m_api.GetVideoProperty(pHandle, nIndex, ppProperty);
}

int CDownloaderAPI::GetAudioProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if(!m_api.GetAudioProperty)
        return -1;
    return m_api.GetAudioProperty(pHandle, nIndex, ppProperty);
}

int CDownloaderAPI::GetSubtitleProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
    if(!m_api.GetSubtitleProperty)
        return -1;
    return m_api.GetSubtitleProperty(pHandle, nIndex, ppProperty);
}

int CDownloaderAPI::GetCurrPlayingTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
    if(!m_api.GetCurrPlayingTrackIndex)
        return -1;
    return m_api.GetCurrPlayingTrackIndex(pHandle, pCurrIndex);
}

int CDownloaderAPI::GetCurrSelectedTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
    if(!m_api.GetCurrSelectedTrackIndex)
        return -1;
    return m_api.GetCurrSelectedTrackIndex(pHandle, pCurrIndex);
}




VO_U32 CDownloaderAPI::LoadLib (VO_HANDLE hInst)
{
    vostrcpy(m_szDllFile, _T("voStreamingDownloader"));
    vostrcpy(m_szAPIName, _T("voGetDownloaderAPI"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, ".so");
#elif defined _MAC_OS
	vostrcat(m_szDllFile, ".dylib");
#endif

#ifdef _IOS
    m_pAPIEntry = (VO_PTR)voGetDownloaderAPI;
#else
	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGE( "load %s fail!", m_szDllFile);
		return 0;
	}
#endif // _IOS
    
    if(!m_pAPIEntry)
    {
        VOLOGE("voGetStreamingDownloaderAPI not found!!!");
        return 0;
    }
    
    ((VOGETDOWNLOADERAPI)m_pAPIEntry)(&m_api);

	return 1;
}
