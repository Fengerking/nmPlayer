/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __voStreamingDownloaderAPI_H__
#define __voStreamingDownloaderAPI_H__

#include "voOnStreamSourceType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EVENTID_INFOR_STREAMDOWNLOADER_BASE						0x10000000
#define EVENTID_INFOR_STREAMDOWNLOADER_OPENCOMPLETE				(EVENTID_INFOR_STREAMDOWNLOADER_BASE | 0x0001)				/*!< notify the open is complete.*/
#define EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_OK				(EVENTID_INFOR_STREAMDOWNLOADER_BASE | 0x0002)				/*!< notify the manifest has been generated successfully.carry the url of the local manifest.*/
#define EVENTID_INFOR_STREAMDOWNLOADER_END							(EVENTID_INFOR_STREAMDOWNLOADER_BASE | 0x0004)				/*!< notify the download is finished. */
#define EVENTID_INFOR_STREAMDOWNLOADER_PROGRAMINFO_CHANGE		(EVENTID_INFOR_STREAMDOWNLOADER_BASE | 0x0005)				/*!< notify the programinfo has been changed. */
#define EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_UPDATE			(EVENTID_INFOR_STREAMDOWNLOADER_BASE | 0x0006)				/*!< notify the manifest file has been updated. struct PROGRESS_INFO will be passed with this event.*/

#define EVENTID_ERROR_STREAMDOWNLOADER_BASE						0x90000000
#define EVENTID_ERROR_STREAMDOWNLOADER_DOWNLOADMANIFEST_FAIL	(EVENTID_ERROR_STREAMDOWNLOADER_BASE | 0x0001)				/*!< notify that it is fail to download the manifest.*/
#define EVENTID_ERROR_STREAMDOWNLOADER_WRITEMANIFEST_FAIL		(EVENTID_ERROR_STREAMDOWNLOADER_BASE | 0x0002)				/*!< notify that it is fail to write the manifest.*/
#define EVENTID_ERROR_STREAMDOWNLOADER_DOWNCHUNK_FAIL			(EVENTID_ERROR_STREAMDOWNLOADER_BASE | 0x0003)				/*!< notify that it is fail to download the chunk file.*/
#define EVENTID_ERROR_STREAMDOWNLOADER_WRITECHUNK_FAIL			(EVENTID_ERROR_STREAMDOWNLOADER_BASE | 0x0004)				/*!< notify that it is fail to write the chunk file.*/
#define EVENTID_ERROR_STREAMDOWNLOADER_SDCARD_FULL				(EVENTID_ERROR_STREAMDOWNLOADER_BASE | 0x0005)				/*!< notify that the sdcard is full.*/
#define EVENTID_ERROR_STREAM_ERR_GENERATE_MANIFEST_FAIL			(EVENTID_ERROR_STREAMDOWNLOADER_BASE | 0x0006)				/*!< notify that the generate manifest fail.*/
/*
 * struct information for event 
 * EVENTID_INFOR_STREAMDOWNLOADER_MANIFEST_UPDATE
*/
typedef struct
{
	unsigned int uCurrDuration;
	unsigned int TotalDuration;
}DOWNLOADER_PROGRESS_INFO;

typedef struct
{
	void* pUserData;
	int (*SendEvent) (void* pUserData, unsigned int nID, void* nParam1, void* nParam2);
}DOWNLOADER_API_EVENTCALLBACK;

typedef struct  
{
	unsigned int uFlag;
	void* pInitParam;
	void* strWorkPath;     /*!< Load library work path */
}DOWNLOADER_API_INITPARAM;

typedef struct
{
/**
 * Initial a source session and Set the Source( url etc. ) into the module
 * For this function please only set the pInitParam, do not do anything else
 * This function should be called first for a session.
 * \pParam [in] notify call back function pointer.pParam->pInitParam should be pointed to struct DOWNLOADER_API_EVENTCALLBACK.
 * \pParam [in] it's struct is DOWNLOADER_API_INITPARAM.
 */
int (* Init) (void** pHandle, DOWNLOADER_API_EVENTCALLBACK* pCallBack, DOWNLOADER_API_INITPARAM* pParam );

/**
 * Uninitial a source session
 * \param hHandle [in] The source handle. Opened by Init().
 */
int (* Uninit) (void* pHandle);

/**
 * Open the streaming, this function is asynchronous.
 * Please call this function after Init successfully.
 * \param hHandle [in] The source handle. Opened by Init().
 * \param pSource [in] source file description, should be an url.  
 * \param pLocalDir [in] all the file will be generated in this description.
 */
int (* Open) (void* pHandle, void* pSource, unsigned int uFlag, void* pLocalDir);

/**
 * Close
 * \param hHandle [in] The source handle. Opened by Init().
 */
int (* Close) (void* pHandle );

/**
 * Start to download the file chunk,you'd better call this function after receiving the evnt "EVENTID_INFOR_STREAMDOWNLOADER_OPENCOMPLETE".
 * \param hHandle [in] The source handle. Opened by Init().
 */
int (* StartDownload) (void* pHandle);

/**
 * Stop download
 * \param hHandle [in] The source handle. Opened by Init().
 */
int (* StopDownload) (void* pHandle);

/**
 * Pause Download the file chunk
 * \param hHandle [in] The source handle. Opened by Init().
 */
int (* PauseDownload) (void* pHandle);

/**
 * Resume Download
 * \param hHandle [in] The source handle. Opened by Init().
 */
int (* ResumeDownload) (void* pHandle);


/**
 * Get the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \
 */
int (* GetParam) (void* pHandle, unsigned int nParamID, void* pParam);

/**
 * Set the special value from param ID
 * \param hHandle [in] The source handle. Opened by Init().
 * \param nParamID [in] The param ID
 * \param pParam [in] The set value depend on the param ID.
 */
int (* SetParam) (void* pHandle, unsigned int nParamID, void* pParam);
    
/**
 * Get video track count
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval count of video track
 */
int (* GetVideoCount) (void* pHandle);

/**
 * Get audio track count
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval count of audio track
 */
int (* GetAudioCount) (void* pHandle);

/**
 * Get subtitle track count
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval count of subtitle track
 */
int (* GetSubtitleCount) (void* pHandle);

/**
 * Select video track
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval 0 Succeeded
 */
int (* SelectVideo) (void* pHandle, int nIndex);

/**
 * Select audio track
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval 0 Succeeded
 */
int (* SelectAudio) (void* pHandle, int nIndex);

/**
 * Select subtitle track
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval 0 Succeeded
 */
int (* SelectSubtitle) (void* pHandle, int nIndex);

/**
 * Check if video track is available
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval true Succeeded
 */
bool (* IsVideoAvailable) (void* pHandle, int nIndex);

/**
 * Check if audio track is available
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval true Succeeded
 */
bool (* IsAudioAvailable) (void* pHandle, int nIndex);

/**
 * Check if subtitle track is available
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \retval true Succeeded
 */
bool (* IsSubtitleAvailable) (void* pHandle, int nIndex);

/**
 * Commit all current selections
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval 0 Succeeded
 */
int (* CommitSelection) (void* pHandle);

/**
 * Remove all current uncommitted selections
 * \param pHandle [in] The source handle. Opened by Init().
 * \retval 0 Succeeded
 */
int (* ClearSelection) (void* pHandle);
/**
 * Get the properties of specified video stream.
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \param pProperty [out] The properties of track
 * \retval VOOSMP_ERR_None Succeeded
 */
int (* GetVideoProperty) (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);

/**
 * Get the properties of specified audio stream.
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \param pProperty [out] The properties of track
 * \retval VOOSMP_ERR_None Succeeded
 */
int (* GetAudioProperty) (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);

/**
 * Get the properties of specified subtitle stream.
 * \param pHandle [in] The source handle. Opened by Init().
 * \param nIndex [in] The index of track
 * \param pProperty [out] The properties of track
 * \retval VOOSMP_ERR_None Succeeded
 */
int (* GetSubtitleProperty) (void* pHandle, int nIndex, VOOSMP_SRC_TRACK_PROPERTY** ppProperty);

/**
 * Retrieve the assets which is currently being playing
 * \param pHandle [in] The source handle. Opened by Init().
 * \param pCurrIndex [in/out] The current selected index
 * \retval VOOSMP_ERR_None Succeeded
 */
int (* GetCurrPlayingTrackIndex) (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

/**
 * Retrieve the assets which is currently being selected or default assets before playing
 * \param pHandle [in] The source handle. Opened by Init().
 * \param pCurrIndex [in/out] The current selected index
 * \retval VOOSMP_ERR_None Succeeded
 */
int (* GetCurrSelectedTrackIndex) (void* pHandle, VOOSMP_SRC_CURR_TRACK_INDEX* pCurrIndex);

} ADAPTIVESTREAM_DOWNLOADER_API;

int voGetDownloaderAPI(ADAPTIVESTREAM_DOWNLOADER_API* pHandle);
typedef int (* VOGETDOWNLOADERAPI)(ADAPTIVESTREAM_DOWNLOADER_API* pVersion);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef _VONAMESPACE
}
#endif

#endif /* __voStreamingDownloaderAPI_H__ */

