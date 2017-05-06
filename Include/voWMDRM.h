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

#ifndef __voWMDRM_H__
#define __voWMDRM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM.h"

//The object buffer not include id(GUID)
typedef struct
{
	VO_PBYTE			pContentEncryptionObject;
	VO_U32				nContentEncryptionObject;
	VO_PBYTE			pExtendedContentEncryptionObject;
	VO_U32				nExtendedContentEncryptionObject;
} VO_WMDRM_INFO;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voWMDRM_H__
