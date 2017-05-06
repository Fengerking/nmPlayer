/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
/*******************************************************************************
	File:		voStreamingDownloaderAPI.cpp

	Contains:	voStreamingDownloader class file

	Written by:	Peter

	Change History (most recent first):
	2013-09-29		Peter			Create file

*******************************************************************************/
#include "voStreamingDownloader.h"
#include "voStreamingDownloaderAPI.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	VO_HANDLE phHandle;
	VO_ADAPTIVESTREAM_DOWNLOADER_API3 pAPIHandle;
} DOWNLOADER_API_HANDLE;

DOWNLOADER_API_HANDLE exAPI;

int ConvertReturnValue(unsigned int nRC)
{
	unsigned int osRC = nRC;

	switch(nRC)
	{
		case VO_RET_SOURCE2_FAIL:
			osRC = VOOSMP_ERR_Unknown;
			break;
		case VO_RET_SOURCE2_OPENFAIL:
			osRC = VOOSMP_SRC_ERR_OPEN_SRC_FAIL;
			break;
		case VO_RET_SOURCE2_NEEDRETRY:
			osRC = VOOSMP_ERR_Retry;
			break;
		case VO_RET_SOURCE2_END:
			osRC = VOOSMP_ERR_EOS;
			break;
		case VO_RET_SOURCE2_CONTENTENCRYPT:
			osRC = VOOSMP_SRC_ERR_CONTENT_ENCRYPT;
			break;
		case VO_RET_SOURCE2_CODECUNSUPPORT:
			osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
			break;
		case VO_RET_SOURCE2_PLAYMODEUNSUPPORT:
			osRC = VOOSMP_SRC_ERR_PLAYMODE_UNSUPPORT;
			break;
		case VO_RET_SOURCE2_ERRORDATA:
			osRC = VOOSMP_SRC_ERR_ERROR_DATA;
			break;
		case VO_RET_SOURCE2_SEEKFAIL:
			osRC = VOOSMP_SRC_ERR_SEEK_FAIL;
			break;
		case VO_RET_SOURCE2_FORMATUNSUPPORT:
			osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
			break;
		case VO_RET_SOURCE2_TRACKNOTFOUND:
			osRC = VOOSMP_SRC_ERR_TRACK_NOTFOUND;
			break;
		case VO_RET_SOURCE2_EMPTYPOINTOR:
			osRC = VOOSMP_ERR_Pointer;
			break;
		case VO_RET_SOURCE2_NOIMPLEMENT:
			osRC = VOOSMP_ERR_Implement;
			break;
		case VO_RET_SOURCE2_NODOWNLOADOP:
			osRC = VOOSMP_SRC_ERR_NO_DOWNLOAD_OP;
			break;
		case VO_RET_SOURCE2_INPUTDATASMALL:
			osRC = VOOSMP_ERR_SmallSize;
			break;
		case VO_RET_SOURCE2_OUTPUTNOTAVALIBLE:
			osRC = VOOSMP_SRC_ERR_FORMAT_UNSUPPORT;
			break;
		case VO_RET_SOURCE2_CHUNKSKIPPED:
			osRC = VOOSMP_SRC_ERR_CHUNK_SKIP;
			break;
	}

	return osRC;
}

int exInit (void** pHandle, DOWNLOADER_API_EVENTCALLBACK* pCallBack, DOWNLOADER_API_INITPARAM* pParam )
{
	memset( &exAPI, 0, sizeof (DOWNLOADER_API_HANDLE) );
	int nRet = voGetStreamingDownloaderAPI3( &(exAPI.pAPIHandle) );
	if (nRet != VO_RET_SOURCE2_OK)
	{
		return nRet;
	}

	unsigned int ulRet = exAPI.pAPIHandle.Init( &(exAPI.phHandle), (VO_SOURCE2_EVENTCALLBACK*)pCallBack, (VO_SOURCE2_INITPARAM*)pParam);
	*pHandle = (void*)(&exAPI);

	return ConvertReturnValue( ulRet );
}

int exUninit(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet = pExAPI->pAPIHandle.Uninit( pExAPI->phHandle);
	return ConvertReturnValue( nRet );
}

int exOpen(void* pHandle, void* pSource, unsigned int uFlag, void* pLocalDir)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet = pExAPI->pAPIHandle.Open( pExAPI->phHandle, pSource, uFlag, pLocalDir);
	return ConvertReturnValue( nRet );
}

int exClose(void* pHandle )
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.Close( pExAPI->phHandle );
	return ConvertReturnValue( nRet );
}

int exStartDownload(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.StartDownload( pExAPI->phHandle );
	return ConvertReturnValue( nRet );
}

int exStopDownload(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.StopDownload( pExAPI->phHandle );
	return ConvertReturnValue( nRet );
}

int exPauseDownload(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet = pExAPI->pAPIHandle.PauseDownload( pExAPI->phHandle );
	return ConvertReturnValue( nRet );
}

int exResumeDownload(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.ResumeDownload( pExAPI->phHandle );
	return ConvertReturnValue( nRet );
}

int exGetParam(void* pHandle, unsigned int nParamID, void* pParam)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet = pExAPI->pAPIHandle.GetParam( pExAPI->phHandle, nParamID, pParam);
	return ConvertReturnValue( nRet );
}

int exSetParam(void* pHandle, unsigned int nParamID, void* pParam)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet = pExAPI->pAPIHandle.SetParam( pExAPI->phHandle, nParamID, pParam);
	return ConvertReturnValue( nRet );
}

int exGetVideoCount(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetVideoCount( pExAPI->phHandle);
	return ConvertReturnValue( nRet );
}

int exGetAudioCount(void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetAudioCount( pExAPI->phHandle);
	return ConvertReturnValue( nRet );
}

int exGetSubtitleCount (void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetSubtitleCount( pExAPI->phHandle);
	return ConvertReturnValue( nRet );
}

int exSelectVideo(void* pHandle, int nIndex)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.SelectVideo(pExAPI->phHandle, nIndex);
	return ConvertReturnValue( nRet );
}

int exSelectAudio(void* pHandle, int nIndex)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.SelectAudio(pExAPI->phHandle, nIndex);
	return ConvertReturnValue( nRet );
}

int exSelectSubtitle(void* pHandle, int nIndex)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.SelectSubtitle(pExAPI->phHandle, nIndex);
	return ConvertReturnValue( nRet );
}

bool exIsVideoAvailable(void* pHandle, int nIndex)
{
	if (pHandle == NULL) return false;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.IsVideoAvailable(pExAPI->phHandle, nIndex);
	return ConvertReturnValue( nRet );
}

bool exIsAudioAvailable(void* pHandle, int nIndex)
{
	if (pHandle == NULL) return false;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.IsAudioAvailable(pExAPI->phHandle, nIndex);
	return ConvertReturnValue( nRet );
}

bool exIsSubtitleAvailable (void* pHandle, int nIndex)
{
	if (pHandle == NULL) return false;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.IsSubtitleAvailable(pExAPI->phHandle, nIndex);
	return ConvertReturnValue( nRet );
}

int exCommitSelection (void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet = pExAPI->pAPIHandle.CommitSelection(pExAPI->phHandle);
	return ConvertReturnValue( nRet );
}

int exClearSelection (void* pHandle)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.ClearSelection(pExAPI->phHandle);
	return ConvertReturnValue( nRet );
}

int exGetVideoProperty (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetVideoProperty(pExAPI->phHandle, nIndex, ppProperty);
	return ConvertReturnValue( nRet );
}

int exGetAudioProperty(void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetAudioProperty(pExAPI->phHandle, nIndex, ppProperty);
	return ConvertReturnValue( nRet );
}

int exGetSubtitleProperty(void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetSubtitleProperty(pExAPI->phHandle, nIndex, ppProperty);
	return ConvertReturnValue( nRet );
}

int exGetCurrPlayingTrackIndex (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetCurrPlayingTrackIndex(pExAPI->phHandle, pCurrIndex);
	return ConvertReturnValue( nRet );
}

int exGetCurrSelectedTrackIndex(void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex)
{
	if (pHandle == NULL) return VOOSMP_ERR_Pointer;

	DOWNLOADER_API_HANDLE* pExAPI = (DOWNLOADER_API_HANDLE*) pHandle;
	int nRet =  pExAPI->pAPIHandle.GetCurrSelectedTrackIndex(pExAPI->phHandle, pCurrIndex);
	return ConvertReturnValue( nRet );
}

int voGetDownloaderAPI(ADAPTIVESTREAM_DOWNLOADER_API* pHandle)
{
	pHandle->Init				= exInit;
	pHandle->Uninit				= exUninit;
	pHandle->Open				= exOpen;
	pHandle->Close				= exClose;
	pHandle->StartDownload		= exStartDownload;
	pHandle->StopDownload		= exStopDownload;	
	pHandle->PauseDownload		= exPauseDownload;
	pHandle->ResumeDownload	= exResumeDownload;
	pHandle->GetParam			= exGetParam;
	pHandle->SetParam			= exSetParam;
	pHandle->GetVideoCount		= exGetVideoCount;
	pHandle->GetAudioCount		= exGetAudioCount;
	pHandle->GetSubtitleCount		= exGetSubtitleCount;
	pHandle->SelectVideo			= exSelectVideo;
	pHandle->SelectAudio			= exSelectAudio;
	pHandle->SelectSubtitle		= exSelectSubtitle;
	pHandle->IsVideoAvailable		= exIsVideoAvailable;
	pHandle->IsAudioAvailable		= exIsAudioAvailable;
	pHandle->IsSubtitleAvailable	= exIsSubtitleAvailable;
	pHandle->CommitSelection		= exCommitSelection;
	pHandle->ClearSelection		= exClearSelection;
	pHandle->GetVideoProperty	= exGetVideoProperty;
	pHandle->GetAudioProperty	= exGetAudioProperty;
	pHandle->GetSubtitleProperty	= exGetSubtitleProperty;
	pHandle->GetCurrPlayingTrackIndex = exGetCurrPlayingTrackIndex;
	pHandle->GetCurrSelectedTrackIndex = exGetCurrSelectedTrackIndex;

	return VOOSMP_ERR_None;
} 

#ifdef __cplusplus
}
#endif


