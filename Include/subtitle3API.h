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

#ifndef __subtitle3API_H__
#define __subtitle3API_H__

#include "voType.h"
#include "voString.h"
#include "voIVCommon.h"
#include "subtitleAPI.h"
#include "voSubtitleType.h"

//To set current JNIEnv* to lib
#define ID_SET_JAVA_ENV	1000

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus



typedef struct
{
	//open the library
	int (SUBTITLE_APIENTRY* vostInit) (VO_PTR * phSubtitle);

	//close the library
	int (SUBTITLE_APIENTRY* vostUninit) (VO_PTR hSubtitle);

	/**
	 * Get Subtitle Object
	 * \param hSubtitle [in] object from vostOpen.
	 * \param nTimeCurrent [in] input current time in ms.
	 * \param subtitleInfo [out] the returned subtitle object.
	 * \param nTrackIndex [in] input current track index.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* vostGetSubtitleObject) (VO_PTR hSubtitle, long nTimeCurrent, voSubtitleInfo ** subtitleInfo, int nTrackIndex);

	/**
	 * set media file path
	 * \param hSubtitle [in] object from vostOpen.
	 * \param pPath [in] media file path.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* vostSetMediaFile) (VO_PTR hSubtitle, VO_TCHAR * pPath);

	/**
	 * get track count
	 * \param hSubtitle [in] object from vostOpen.
	 * \retval track count.
	 */
	int (SUBTITLE_APIENTRY* vostGetTrackCount) (VO_PTR hSubtitle);

	/**
	 * set parameter
	 * \param hSubtitle [in] object from vostOpen.
	 * \param nID [in] the ID of parameter.
	 * \param lValue [in] the value of parameter.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* vostSetParameter) (VO_PTR hSubtitle, long nID, long lValue);

	/**
	 * get parameter
	 * \param hSubtitle [in] object from vostOpen.
	 * \param nID [in] the ID of parameter.
	 * \param lValue [out] the output value of parameter.
	 * \retval VO_ERR_OK Succeeded.
	 */
	int (SUBTITLE_APIENTRY* vostGetParameter) (VO_PTR hSubtitle, long nID, long *plValue);

}VO_SUBTITLE_3_API;



/**
 * Get Subtitle API interface
 * \param pSubtitle [IN/OUT] Return the Engine API handle.
 * \param uFlag,reserved
 * \retval Subtitle_ErrorNone Succeeded.
 */
SUBTITLE_ERRORTYPE SUBTITLE_APIENTRY voGetSubtitle3API (VO_SUBTITLE_3_API * pSubtitleAPI, int uFlag);

typedef SUBTITLE_ERRORTYPE (SUBTITLE_APIENTRY * VOGETSUBTITLE3API) (VO_SUBTITLE_3_API * pFunc, int uFlag);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __subtitle3API_H__
