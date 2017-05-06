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

#ifndef __voWidevine_H__
#define __voWidevine_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
	VO_U32		nDecryptID;
	VO_U32		nHeaderLen;
	VO_PBYTE	pHeadData;

}VO_Widevine_INFO;

enum {
     Widevine_DRM_ERROR_BASE = -2000,
     Widevine_DRM_ERROR_UNKNOWN = Widevine_DRM_ERROR_BASE,
     Widevine_DRM_ERROR_LICENSE_EXPIRED = Widevine_DRM_ERROR_BASE - 1,
     Widevine_DRM_ERROR_SESSION_NOT_OPENED = Widevine_DRM_ERROR_BASE - 2,
     Widevine_DRM_ERROR_DECRYPT_UNIT_NOT_INITIALIZED = Widevine_DRM_ERROR_BASE - 3,
     Widevine_DRM_ERROR_DECRYPT = Widevine_DRM_ERROR_BASE - 4,
     Widevine_DRM_ERROR_CANNOT_HANDLE = Widevine_DRM_ERROR_BASE - 5,

     Widevine_DRM_NO_ERROR = 0
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voWidevine_H__
