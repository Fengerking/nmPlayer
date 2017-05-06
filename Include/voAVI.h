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

#ifndef __voAVI_H__
#define __voAVI_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voFile.h>
#include "voSource2.h"

/**
 * Get File Read API interface
 * \param pReadHandle [out] Return the AVI file read handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetAVIReadAPI (VO_SOURCE_READAPI * pReadHandle, VO_U32 uFlag);

VO_S32 VO_API voGetAVI2ReadAPI(VO_SOURCE2_API* pReadHandle, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAVI_H__
