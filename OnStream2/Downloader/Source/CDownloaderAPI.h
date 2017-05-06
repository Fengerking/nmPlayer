	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDownloaderAPI.h

	Contains:	CDownloaderAPI header file

	Written by:	

	Change History (most recent first):
	2013-10-14		Jim			Create file

*******************************************************************************/

#ifndef __CDownloaderAPI_H__
#define __CDownloaderAPI_H__

#include "CDllLoad.h"
#include "voStreamingDownloaderAPI.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CDownloaderAPI : public CDllLoad
{
public:
	CDownloaderAPI ();
	virtual ~CDownloaderAPI (void);
    
public:
    int Init (void** phHandle, DOWNLOADER_API_EVENTCALLBACK* pCallBack, DOWNLOADER_API_INITPARAM* pParam );
    int Uninit (void* hHandle);
    int Open (void* hHandle, void* pSource, unsigned int uFlag, void* pLocalDir);
    int Close (void* hHandle );
    int StartDownload (void* hHandle);
    int StopDownload (void* hHandle);
    int PauseDownload (void* hHandle);
    int ResumeDownload (void* hHandle);
    int GetParam (void* hHandle, unsigned int nParamID, void* pParam);
    int SetParam (void* hHandle, unsigned int nParamID, void* pParam);
    
    int GetVideoCount (void* pHandle);
    int GetAudioCount (void* pHandle);
    int GetSubtitleCount (void* pHandle);
    int SelectVideo (void* pHandle, int nIndex);
    int SelectAudio (void* pHandle, int nIndex);
    int SelectSubtitle (void* pHandle, int nIndex);
    bool IsVideoAvailable (void* pHandle, int nIndex);
    bool IsAudioAvailable (void* pHandle, int nIndex);
    bool IsSubtitleAvailable (void* pHandle, int nIndex);
    int CommitSelection (void* pHandle);
    int ClearSelection (void* pHandle);
    int GetVideoProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetAudioProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetSubtitleProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);
    int GetCurrPlayingTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);
    int GetCurrSelectedTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

public:
	virtual VO_U32		LoadLib (VO_HANDLE hInst);
    
private:
    ADAPTIVESTREAM_DOWNLOADER_API m_api;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CDownloaderAPI_H__
