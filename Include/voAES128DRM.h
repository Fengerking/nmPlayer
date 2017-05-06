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

#ifndef __voAES128DRM_H__
#define __voAES128DRM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM.h"

typedef enum
{
	SUCCESS=1,
	KEY_ACQUIRE_FAIL,
	SOURCE_TYPE_NOT_SUPPORTED,
	PROT_TYPE_NOT_SUPPORTED,  
	DECRYPTION_FAIL,
	PROXIMITY_ERROR,
	LICENSE_EXPIRED,
	INSUFFICIENT_RIGHTS,
	RIGHTS_ACQUIRE_FAIL,
	INTERNAL_DRM_ERROR,
	IPRM_CLOCK_SKEW_ERROR,
	FATAL_ERROR,
}DRM_ERRORS;

typedef enum  {MOTOHLS= 1} DataSourceType;

typedef enum  {NONE=1,AES128,MEDIOS,IPRM} ProtectionType;

typedef struct
{
	ProtectionType type;
	VO_CHAR *URL;
	VO_BYTE *IV;
	VO_BOOL bLastblock;
} VO_AES128DRM_DecryptINFO;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAES128DRM_H__
