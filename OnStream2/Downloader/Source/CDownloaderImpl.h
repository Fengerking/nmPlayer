	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDownloaderImpl.h

	Contains:	CDownloaderImpl header file

	Written by:	

	Change History (most recent first):
	2013-10-14		Jim			Create file

*******************************************************************************/

#ifndef __CDownloaderImpl_H__
#define __CDownloaderImpl_H__


#include "CDownloaderAPI.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CDownloaderImpl
{
public:
	CDownloaderImpl ();
	virtual ~CDownloaderImpl (void);
    
public:
    
    int Init (DOWNLOADER_API_EVENTCALLBACK* pCallBack, DOWNLOADER_API_INITPARAM* pParam );
    int Uninit ();
    int Open (void* pSource, unsigned int uFlag, void* pLocalDir);
    int Close ();
    int StartDownload ();
    int StopDownload ();
    int PauseDownload ();
    int ResumeDownload ();
    int GetParam (unsigned int nParamID, void* pParam);
    int SetParam (unsigned int nParamID, void* pParam);

    int GetVideoCount ();
    int GetAudioCount ();
    int GetSubtitleCount ();
    int SelectVideo (int nIndex);
    int SelectAudio (int nIndex);
    int SelectSubtitle (int nIndex);
    bool IsVideoAvailable (int nIndex);
    bool IsAudioAvailable (int nIndex);
    bool IsSubtitleAvailable (int nIndex);
    int CommitSelection ();
    int ClearSelection ();
    int GetVideoProperty (int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetAudioProperty (int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetSubtitleProperty (int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetCurrPlayingTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    int GetCurrSelectedTrackIndex (VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

private:
    static int DownloaderEventCallback (void* pUserData, unsigned int nID, void* pParam1, void* pParam2);
    int onDownloadEvent(unsigned int nID, void* pParam1, void* pParam2);

private:
    CDownloaderAPI*                 m_pAPI;
    void*                           m_hDownloader;
    DOWNLOADER_API_EVENTCALLBACK    m_evtCallback;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CDownloaderImpl_H__
