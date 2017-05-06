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

#ifndef __voPlayReady_H__
#define __voPlayReady_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voDRM.h"

typedef struct
{
	VO_PBYTE			pDrmHeader;
	VO_U32				nDrmHeader;
} VO_PLAYREADY_INFO;

typedef struct
{
	VO_U8				btInitializationVector[8];
	VO_U32				nBlockOffset;
	VO_U8				btByteOffset;
} VO_PLAYREADY_AESCTR_INFO;

typedef struct
{
	VO_PBYTE	pSampleEncryptionBox;        // Entire Sample encryption box (containing IVs and DRM data)
	VO_U32		uSampleEncryptionBoxSize;
	VO_U32		uSampleIndex;               // Index of the sample data within this movie fragment.
	VO_U32		uTrackId;
}VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voPlayReady_H__
