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

#ifndef __OMX_VO_FileSource_H__
#define __OMX_VO_FileSource_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "OMX_Types.h"

/**
 * Get the file source component handle.
 * \param uID [in/out] the handle to fill.
 * \return value is OMX_ErrorNone for successful
 */
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetFileSourceHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetAudioDecHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetAudioSinkHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetClockTimeHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetAudioEQHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetVideoDecHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
OMX_API OMX_ERRORTYPE OMX_APIENTRY voGetVideoSinkHandle (OMX_INOUT OMX_COMPONENTTYPE * pHandle);
/**
 * function pointer definition
 */
typedef OMX_ERRORTYPE (OMX_APIENTRY * VOGETFILESOURCEHANDLE) (OMX_INOUT OMX_COMPONENTTYPE * pHandle);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __OMX_VO_FileSource_H__
