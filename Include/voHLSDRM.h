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
#ifndef __voHLSDRM_H__
#define __voHLSDRM_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM2.h"


enum HLS_DRM_PROCESSING_TYPE
{
	DecryptProcess_BEGIN,
	DecryptProcess_PROCESSING,
	DecryptProcess_END,
	DecryptProcess_UNDEFINED
};

typedef struct
{
	VO_PCHAR pURL;
	VO_PCHAR pManifest;
}S_IrdetoDRM_INFO;

typedef struct
{
    VO_PCHAR   pManifestData;
    VO_U32     ulManifestLength;
}S_DISCRETIX_INFO;

typedef struct
{
	VO_DRM2_TYPE				eDrmType;
	HLS_DRM_PROCESSING_TYPE		eDrmProcessInfo;
	VO_VOID*					pInfo;
}S_HLS_DRM_PROCESS_INFO;


typedef struct
{
	VO_U32		ulSequenceID;
	VO_CHAR		strURL[1024];
	VO_U32		ulKeyLen;
	VO_BYTE		aKey[32];
	VO_VOID*	pReserved;
}S_HLS_DRM_ASSIST_INFO;


/**
 * Structure of DRM process parameter for AES128
 */	
struct VO_DRM2_HSL_PROCESS_INFO
{
	VO_CHAR	szCurURL[2048];
	VO_CHAR	szKeyString[2048];	//#EXT-X-KEY string
	VO_U32	uSequenceNum;
	VO_PTR	pReserved;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voHLSDRM_H__

