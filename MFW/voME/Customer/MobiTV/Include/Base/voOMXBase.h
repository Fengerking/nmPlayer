	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXBase.h

	Contains:	voOMXBase header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voOMXBase_H__
#define __voOMXBase_H__

#include <OMX_Component.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NULL
#define NULL 0
#endif // NULL

/** Defines the major version of the OMX */
#define OMX_VERSION_MAJOR		1
/** Defines the minor version of the OMX */
#define OMX_VERSION_MINOR		1
/** Defines the revision of the OMX */
#define OMX_VERSION_REVISION	0
/** Defines the step version of the OMX */
#define OMX_VERSION_STEP		0

/** Defines the major version of the core */
#define SPEC_VERSION_MAJOR  1
/** Defines the minor version of the core */
#define SPEC_VERSION_MINOR  0
/** Defines the revision of the core */
#define SPEC_REVISION      0
/** Defines the step version of the core */
#define SPEC_STEP          0

OMX_ERRORTYPE voOMXBase_CheckHeader (OMX_PTR pHeader, OMX_U32 nSize);

OMX_ERRORTYPE voOMXBase_SetHeader (OMX_PTR pHeader, OMX_U32 nSize, OMX_VERSIONTYPE * pVer = NULL);

OMX_ERRORTYPE voOMXBase_CopyBufferHeader (OMX_BUFFERHEADERTYPE * pDestHeader, OMX_BUFFERHEADERTYPE * pSourHeader);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voOMXBase_H__
