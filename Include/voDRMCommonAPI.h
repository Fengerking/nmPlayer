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
#ifndef __VODRM_COMMONAPI_H__
#define __VODRM_COMMONAPI_H__
#include "VOOSMPDRM.h"


typedef enum {

	VO_DRM_COMMONAPI_CUSTOMER_ENTRYPOINTF	= 0X00000001, /** Set the entry-point function of customer engine*/

	VO_DRM_COMMONAPI_MAX					= 0xFFFFFFFF
} VO_DRM_COMMONAPI_PID;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	int voCreateDRM(VOOSMPDRM** ppDRM);

	int voDestroyDRM(VOOSMPDRM* pDRM);

#if defined __cplusplus
}
#endif

#endif //__VODRM_COMMONAPI_H__