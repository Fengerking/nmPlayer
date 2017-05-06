/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voStreamingDownloader_H__
#define __voStreamingDownloader_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voSource2.h"
#include "voOnStreamSource.h"

#define VO_FLAG_STREAMDOWNLOADER_ASYNCOPEN					0x00000001


#define VO_EVENTID_STREAMDOWNLOADER_BASE						0x10000000
#define VO_EVENTID_STREAMDOWNLOADER_OPENCOMPLETE			(VO_EVENTID_STREAMDOWNLOADER_BASE | 0x0001)				/*!< notify the open is complete.*/
#define VO_EVENTID_STREAMDOWNLOADER_MANIFEST_OK				(VO_EVENTID_STREAMDOWNLOADER_BASE | 0x0002)				/*!< notify the manifest has been generated successfully.carry the url of the local manifest.*/
#define VO_EVENTID_STREAMDOWNLOADER_END						(VO_EVENTID_STREAMDOWNLOADER_BASE | 0x0004)				/*!< notify the download is finished. */
#define VO_EVENTID_STREAMDOWNLOADER_PROGRAMINFO_CHANGE		(VO_EVENTID_STREAMDOWNLOADER_BASE | 0x0005)			/*!< notify the programinfo has been changed. */
#define VO_EVENTID_STREAMDOWNLOADER_MANIFEST_UPDATE			(VO_EVENTID_STREAMDOWNLOADER_BASE | 0x0006)				/*!< notify the manifest file has been updated. struct PROGRESS_INFO will be passed with this event.*/

#define VO_EVENTID_STREAM_ERR_BASE								0x90000000
#define VO_EVENTID_STREAM_ERR_DOWNLOADMANIFEST_FAIL			(VO_EVENTID_STREAM_ERR_BASE | 0x0001)				/*!< notify that it is fail to download the manifest.*/
#define VO_EVENTID_STREAM_ERR_WRITEMANIFEST_FAIL				(VO_EVENTID_STREAM_ERR_BASE | 0x0002)				/*!< notify that it is fail to write the manifest.*/
#define VO_EVENTID_STREAM_ERR_DOWNCHUNK_FAIL					(VO_EVENTID_STREAM_ERR_BASE | 0x0003)				/*!< notify that it is fail to download the chunk file.*/
#define VO_EVENTID_STREAM_ERR_WRITECHUNK_FAIL					(VO_EVENTID_STREAM_ERR_BASE | 0x0004)				/*!< notify that it is fail to write the chunk file.*/
#define VO_EVENTID_STREAM_ERR_SDCARD_FULL						(VO_EVENTID_STREAM_ERR_BASE | 0x0005)				/*!< notify that the sdcard is full.*/
#define VO_EVENTID_STREAM_ERR_GENERATE_MANIFEST_FAIL			(VO_EVENTID_STREAM_ERR_BASE | 0x0006)				/*!< notify that the generate manifest fail.*/
#define VO_EVENTID_STREAM_ERR_NOTSUPPORT_LIVE_FAIL			(VO_EVENTID_STREAM_ERR_BASE | 0x0007)				/*!< notify that this link is live.*/
#define VO_EVENTID_STREAM_ERR_NOTSUPPORT_LOCAL_FAIL			(VO_EVENTID_STREAM_ERR_BASE | 0x0008)				/*!< notify that this link is local.*/


typedef struct{
	VO_U32	CurrDuration;				//The length of the stream has been downloaded. Unit is ms.  
	VO_U32	TotalDuration;				//The duration of the program.Unit is ms.
}PROGRESS_INFO;

/*
typedef struct
{
	VO_SOURCE2_EVENTCALLBACK*	pCallBack;
	VO_PCHAR 					pLocalDir;
}DownloaderParam;
*/
typedef struct
{
/**
 * Initial a source session and Set the Source( url etc. ) into the module
 * For this function please only set the pInitParam, do not do anything else
 * This function should be called first for a session.
 * \pParam [in] notify call back function pointer.pParam->pInitParam should be pointed to struct VO_SOURCE2_EVENTCALLBACK.
 * \pParam [in] it's struct is VO_SOURCE2_INITPARAM.
 */
VO_U32 (VO_API * Init) (VO_HANDLE* phHandle, VO_SOURCE2_EVENTCALLBACK* pCallBack, VO_SOURCE2_INITPARAM * pParam );

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Uninit) (VO_HANDLE hHandle);

/**
 * Open the streaming, this function is asynchronous.
 * Please call this function after Init successfully.
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pSource [in] source file description, should be an url.
 * \param uFlag [in] source file description, should be an url.  
 * \param pLocalDir [in] all the file will be generated in this description.
 */
VO_U32 (VO_API * Open) ( VO_HANDLE hHandle, VO_PTR pSource, VO_U32 uFlag, VO_PTR pLocalDir);

/**
 * Close
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Close) ( VO_HANDLE hHandle );

/**
 * Start to download the file chunk,you'd better call this function after receiving the evnt "VO_EVENTID_STREAMDOWNLOADER_OPENCOMPLETE".
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * StartDownload) (VO_HANDLE hHandle);

/**
 * Stop download
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * StopDownload) (VO_HANDLE hHandle);

/**
 * Pause Download the file chunk
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * PauseDownload) (VO_HANDLE hHandle);

/**
 * Resume Download
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * ResumeDownload) (VO_HANDLE hHandle);


/**
 * GetProgramInfo
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pProgramInfo [out] return the info of the Stream, source will keep the info structure available until you call close
 */
VO_U32 (VO_API * GetProgramInfo) (VO_HANDLE hHandle, VO_SOURCE2_PROGRAM_INFO** ppProgramInfo);

/**
 * Select the Stream
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nStream [in] Stream ID
 */
VO_U32 (VO_API * SelectStream) (VO_HANDLE hHandle, VO_U32 nStream);

/**
 * Select the Track
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nTrack [in] nTrackID
 */
VO_U32 (VO_API * SelectTrack) (VO_HANDLE hHandle, VO_U32 nTrackID, VO_SOURCE2_TRACK_TYPE nType);

/**
 * Get the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

/**
 * Set the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

} VO_ADAPTIVESTREAM_DOWNLOADER_API;


typedef struct
{
/**
 * Initial a source session and Set the Source( url etc. ) into the module
 * For this function please only set the pInitParam, do not do anything else
 * This function should be called first for a session.
 * \pParam [in] notify call back function pointer.pParam->pInitParam should be pointed to struct VO_SOURCE2_EVENTCALLBACK.
 * \pParam [in] it's struct is VO_SOURCE2_INITPARAM.
 */
VO_U32 (VO_API * Init) (VO_HANDLE* phHandle, VO_SOURCE2_EVENTCALLBACK* pCallBack, VO_SOURCE2_INITPARAM * pParam );

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Uninit) (VO_HANDLE hHandle);

/**
 * Open the streaming, this function is asynchronous.
 * Please call this function after Init successfully.
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pSource [in] source file description, should be an url.
 * \param uFlag [in] source file description, should be an url.  
 * \param pLocalDir [in] all the file will be generated in this description.
 */
VO_U32 (VO_API * Open) ( VO_HANDLE hHandle, VO_PTR pSource, VO_U32 uFlag, VO_PTR pLocalDir);

/**
 * Close
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * Close) ( VO_HANDLE hHandle );

/**
 * Start to download the file chunk,you'd better call this function after receiving the evnt "VO_EVENTID_STREAMDOWNLOADER_OPENCOMPLETE".
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * StartDownload) (VO_HANDLE hHandle);

/**
 * Stop download
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * StopDownload) (VO_HANDLE hHandle);

/**
 * Pause Download the file chunk
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * PauseDownload) (VO_HANDLE hHandle);

/**
 * Resume Download
 * \param hHandle [in] The source handle. Opened by Init().
 */
VO_U32 (VO_API * ResumeDownload) (VO_HANDLE hHandle);


/**
 * Get video track count
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval count of video track
 */
VO_U32 (VO_API * GetVideoCount) (VO_HANDLE hHandle);

/**
 * Get audio track count
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval count of audio track
 */
VO_U32 (VO_API * GetAudioCount) (VO_HANDLE hHandle);

/**
 * Get subtitle track count
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval count of subtitle track
 */
VO_U32 (VO_API * GetSubtitleCount) (VO_HANDLE hHandle);

/**
 * Select video track
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval 0 Succeeded
 */
VO_U32 (VO_API * SelectVideo) (VO_HANDLE hHandle, VO_S32 nIndex);

/**
 * Select audio track
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval 0 Succeeded
 */
VO_U32 (VO_API * SelectAudio) (VO_HANDLE hHandle, VO_S32 nIndex);

/**
 * Select subtitle track
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval 0 Succeeded
 */
VO_U32 (VO_API * SelectSubtitle) (VO_HANDLE hHandle, VO_S32 nIndex);

/**
 * Check if video track is available
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval true Succeeded
 */
VO_BOOL (VO_API* IsVideoAvailable) (VO_HANDLE hHandle, VO_S32 nIndex);

/**
 * Check if audio track is available
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval true Succeeded
 */
VO_BOOL (VO_API* IsAudioAvailable) (VO_HANDLE hHandle, VO_S32 nIndex);

/**
 * Check if subtitle track is available
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval true Succeeded
 */
VO_BOOL (VO_API * IsSubtitleAvailable) (VO_HANDLE hHandle, VO_S32 nIndex);

/**
 * Commit all current selections
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval 0 Succeeded
 */
VO_U32 (VO_API * CommitSelection) (VO_HANDLE hHandle);

/**
 * Remove all current uncommitted selections
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval 0 Succeeded
 */
VO_U32 (VO_API * ClearSelection) (VO_HANDLE hHandle);
/**
 * Get the properties of specified video stream.
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \param pProperty [out] The properties of track
 * \retval VOOSMP_ERR_None Succeeded
 */
VO_U32 (VO_API * GetVideoProperty) (VO_HANDLE hHandle, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);

/**
 * Get the properties of specified audio stream.
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \param pProperty [out] The properties of track
 * \retval VOOSMP_ERR_None Succeeded
 */
VO_U32 (VO_API * GetAudioProperty) (VO_HANDLE hHandle, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);

/**
 * Get the properties of specified subtitle stream.
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \param pProperty [out] The properties of track
 * \retval VOOSMP_ERR_None Succeeded
 */
VO_U32 (VO_API * GetSubtitleProperty) (VO_HANDLE hHandle, VO_S32 nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);

/**
 * Retrieve the assets which is currently being playing
 * \param pHandle [in] The source handle. Opened by Init().
 * \param pCurrIndex [in/out] The current selected index
 * \retval VOOSMP_ERR_None Succeeded
 */
VO_U32 (VO_API * GetCurrPlayingTrackIndex) (VO_HANDLE hHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

/**
 * Retrieve the assets which is currently being selected or default assets before playing
 * \param pHandle [in] The source handle. Opened by Init().
 * \param pCurrIndex [in/out] The current selected index
 * \retval VOOSMP_ERR_None Succeeded
 */
VO_U32 (VO_API * GetCurrSelectedTrackIndex) (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);


/**
 * Get the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \
 */
VO_U32 (VO_API * GetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

/**
 * Set the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
VO_U32 (VO_API * SetParam) (VO_HANDLE hHandle, VO_U32 nParamID, VO_PTR pParam);

}VO_ADAPTIVESTREAM_DOWNLOADER_API3;


VO_S32 VO_API voGetStreamingDownloaderAPI3(VO_ADAPTIVESTREAM_DOWNLOADER_API3* pHandle);
VO_S32 VO_API voGetStreamingDownloaderAPI(VO_ADAPTIVESTREAM_DOWNLOADER_API* pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voStreamingDownloader_H__

