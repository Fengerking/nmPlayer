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
#ifndef __voAndroidVR_h__
#define __voAndroidVR_h__

#include "vompType.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Initialize Android Video Render instance use default settings.
 * \param phRender [out] Return the Render handle.
 * \param hSurface [in] The surface handle
 * \retval 0 Succeeded.
 */
int voavrInit (void ** phRender, void * hSurface, int nFlag);
typedef int (* AVRINIT) (void ** phRender, void * hSurface, int nFlag);


/**
 * Lock the surface and get surface info
 * \param hRender [in] The handle.which was created by avrInit.
 * \param pVideoInfo [in/out] Video info
 * \retval 0 Succeeded.
 */
int voavrLock (void * hRender, VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize);
typedef int (* AVRLOCK) (void * hRender, VOMP_VIDEO_BUFFER * pVideoInfo);


/**
 * Un Lock the surface
 * \param hRender [in] The handle.which was created by avrInit.
 * \retval 0 Succeeded.
 */
int voavrUnLock (void * hRender);
typedef int (* AVRUNLOCK) (void * hRender);


/**
 * Get parameter from Render
 * \param hRender [in] The handle.which was created by avrInit.
 * \param nID [in] Parameter ID.
 * \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval 0 Succeeded.
 */
int voavrGetParam (void * hRender, int nID, void * pValue);
typedef int (* AVRGETPARAM) (void * hRender, int nID, void * pValue);


/**
 * Set parameter to Render
 * \param hRender [in] The handle.which was created by avrInit.
 * \param nID [in] Parameter ID.
 * \param pValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
 * \retval 0 Succeeded.
 */
int voavrSetParam (void * hRender, int nID, void * pValue);
typedef int (* AVRSETPARAM) (void * hRender, int nID, void * pValue);

/**
 * Uninitialize Render and free the resource.
 * \param hRender [in] The handle.which was created by avrInit.
 * \retval 0 Succeeded.
 */
int voavrUninit (void * hRender);
typedef int (* AVRUNINIT) (void * hRender);

/**
 * the Android video render function set
 */
typedef struct
{
	void * hRender;
	int (* Init) (void ** phRender, void * hSurface, int nFlag);
	int (* Lock) (void * hRender, VOMP_VIDEO_BUFFER * pVideoInfo, VOMP_RECT * pVideoSize);
	int (* UnLock) (void * hRender);
	int (* GetParam) (void * hRender, int nID, void * pValue);
	int (* SetParam) (void * hRender, int nID, void * pValue);
	int (* Uninit) (void * hRender);
	int (* SetAudioInfo) (void * pValue);
	int (* GetAudioBufTime) (void * pValue);
	int (* SetWorkPath) (void * pValue);
} VOAVR_FUNCSET;

/**
 * Get the android video render function set
 * \param pFuncSet [out] The function set pointer..
 * \retval 0 Succeeded.
 */
int voavrGetFuncSet (VOAVR_FUNCSET * pFuncSet);
typedef int (* VOAVRGETFUNCSET) (VOAVR_FUNCSET * pFuncSet);

/*
 *	Set audio info into android VR
 */
int voavrSetAudioInfo(void * pValue);

/*
 *	Get audio buffer time from android VR
 */

int voavrGetAudioBufTime (void * pValue);

/*
*	Set work path into android VR
*/
int voavrSetWorkPath (void * pValue);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif // __voAndroidVR_h__
