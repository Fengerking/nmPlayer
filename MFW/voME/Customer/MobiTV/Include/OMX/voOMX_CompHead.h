	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2009			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		OMX_VO_FileSource.h

	Contains:	memory operator function define header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

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
