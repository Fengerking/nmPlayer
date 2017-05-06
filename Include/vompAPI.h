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

#ifndef __vompAPI_h__
#define __vompAPI_h__

#include "vompType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Initialize voMP SDK instance use default settings.
 * \param phPlayer [out] Return the voMP handle.
 * \param hViewWnd [in] The parent window of video window
 * \retval VOMP_E_NOERROR Succeeded.
 * \retval VOMP_ERR_OutMemory Out of memory.
 */
int vompInit (void ** phPlayer, int nFlag, VOMPListener pListener, void * pUserData);
typedef int (* VOMPINIT) (void ** phPlayer, int nFlag, VOMPListener pListener, void * pUserData);

/**
 * Open a media file prepair to play. 
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param pSource [in] The data source pointer. The type depends on the flag.
 * \param nFlag[in] Indate the source type and load mode. 
 * \retval VOMP_E_NOERROR Succeeded.
 * \retval VOMP_ERR_File of unknow file format.
 */
int vompSetDataSource (void * hPlayer, void * pSource, int nFlag);
typedef int (* VOMPSETDATASOURCE) (void * hPlayer, void * pSource, int nFlag);

/**
 * Send the buffer into player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param pBuffer [in] It is audio or video stream buffer.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompSendBuffer (void * hPlayer, int nSSType, VOMP_BUFFERTYPE * pBuffer);
typedef int (* VOMPSENDBUFFER) (void * hPlayer, int nSSType, VOMP_BUFFERTYPE * pBuffer);

/**
 * Get the Video raw data from player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param ppBuffer [in] It is video buffer pointer
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetVideoBuffer (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer);
typedef int (* VOMPGETVIDEOBUFFER) (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer);

/**
 * Get the Audio raw data from player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param ppBuffer [in] It is video buffer pointer
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetAudioBuffer (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer);
typedef int (* VOMPGETAUDIOBUFFER) (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer);

/**
 * Run the player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param nSpeed [in] 1 normal playback, 2, 4, 8, 16, 32 fast forward. -2 -4 -8, -16, -32 back forward
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompRun (void * hPlayer);
typedef int (* VOMPRUN) (void * hPlayer);

/**
 * Pause the player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompPause (void * hPlayer);
typedef int (* VOMPPAUSE) (void * hPlayer);

/**
 * Stop the player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompStop (void * hPlayer);
typedef int (* VOMPSTOP) (void * hPlayer);

/**
 * Flush the player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompFlush (void * hPlayer);
typedef int (* VOMPFLUSH) (void * hPlayer);

/**
 * Get the status of player SDK
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param pStatus [out] The status of player.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetStatus (void * hPlayer, VOMP_STATUS * pStatus);
typedef int (* VOMPGETSTATUS) (void * hPlayer, VOMP_STATUS * pStatus);

/**
 * Get the duration of the file
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param pDuration [out] The duration of the media file (MS)
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetDuration (void * hPlayer, int * pDuration);
typedef int (* VOMPGETDURATION) (void * hPlayer, int * pDuration);

/**
 * Get current position when playback
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param pCurPos [out] The current position while playback (MS)
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetCurPos (void * hPlayer, int * pCurPos);
typedef int (* VOMPGETCURPOS) (void * hPlayer, int * pCurPos);

/**
 * Get current position when playback
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param nCurPos [in] The current position to set (MS)
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompSetCurPos (void * hPlayer, int nCurPos);
typedef int (* VOMPSETCURPOS) (void * hPlayer, int nCurPos);

/**
 * Get parameter from player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param nID [in] Parameter ID.
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetParam (void * hPlayer, int nID, void * pValue);
typedef int (* VOMPGETPARAM) (void * hPlayer, int nID, void * pValue);


/**
 * Set parameter to player
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \param nID [in] Parameter ID.
 * \param pValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompSetParam (void * hPlayer, int nID, void * pValue);
typedef int (* VOMPSETPARAM) (void * hPlayer, int nID, void * pValue);

/**
 * Uninitialize voMP SDK and free the resource.
 * \param hPlayer [in] The handle.which was created by vompInit.
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompUninit (void * hPlayer);
typedef int (* VOMPUNINIT) (void * hPlayer);


/**
 * the vomp function set
 */
typedef struct
{
	int (* Init) (void ** phPlayer, int nFlag, VOMPListener pListener, void * pUserData);
	int (* SetDataSource) (void * hPlayer, void * pSource, int nFlag);
	int (* SendBuffer) (void * hPlayer, int nSSType, VOMP_BUFFERTYPE * pBuffer);
	int (* GetVideoBuffer) (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer);
	int (* GetAudioBuffer) (void * hPlayer, VOMP_BUFFERTYPE ** ppBuffer);
	int (* Run) (void * hPlayer);
	int (* Pause) (void * hPlayer);
	int (* Stop) (void * hPlayer);
	int (* Flush) (void * hPlayer);
	int (* GetStatus) (void * hPlayer, VOMP_STATUS * pStatus);
	int (* GetDuration) (void * hPlayer, int * pDuration);
	int (* GetCurPos) (void * hPlayer, int * pCurPos);
	int (* SetCurPos) (void * hPlayer, int nCurPos);
	int (* GetParam) (void * hPlayer, int nID, void * pValue);
	int (* SetParam) (void * hPlayer, int nID, void * pValue);
	int (* Uninit) (void * hPlayer);
} VOMP_FUNCSET;

/**
 * Get the vomp function set
 * \param pFuncSet [out] The function set pointer..
 * \retval VOMP_E_NOERROR Succeeded.
 */
int vompGetFuncSet (VOMP_FUNCSET * pFuncSet);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __vompAPI_h__
