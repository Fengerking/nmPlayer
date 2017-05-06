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

#ifndef __voAAC2_H__
#define __voAAC2_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voSource2.h"


#define 	SUBHEAD_BASE						1000
#define 	SUBHEAD_ID3_PRIV				SUBHEAD_BASE
#define 	SUBHEAD_ID3_TXXX				(SUBHEAD_ID3_PRIV+1)	
#define 	SUBHEAD_ID3_APIC				(SUBHEAD_ID3_TXXX+1)	


#define VO_AAC2_PARAM_BASE					0X00010000				
#define VO_AAC2_PARAM_RESET				(VO_AAC2_PARAM_BASE + 1)

typedef struct{
	VO_BYTE		pFrameHead[4];
	VO_PBYTE	pFrameData;
	VO_U32		nDataLength;
	VO_U32		nSubHeadFlag;	//use to describe the data type define by user!	
	VO_PTR		pSubStruct;
}ID3Frame;

typedef struct{
	VO_BYTE		pTextEncoding[1];
	VO_PBYTE	pDescription;
	VO_U32		nDescriptionLenght;
	VO_PBYTE	pValue;
	VO_U32		nValueLength;
}TxxxStruct;

typedef struct{
	VO_PBYTE	pOwnerIdentifier;
	VO_U32		nOwnerIdentifierLenght;
	VO_PBYTE	pPrivateData;
	VO_U32		nPrivateDataLenght;
}PrivStruct;

typedef struct{
	VO_BYTE		pTextEncoding[1];
	VO_PBYTE	pMimeType;
	VO_U32		nMimeTypeLength;	
	VO_BYTE		pPictureType[1];
	VO_PBYTE	pDescription;
	VO_U32		nDescriptionLength;
	VO_PBYTE	pPictureData;
	VO_U32		nPictureDataLength;
}ApicStruct;


VO_S32 VO_API voGetSource2AACAPI(VO_SOURCE2_API* pHandle);
VO_S32 VO_API voGetPushAudioAPI(VO_SOURCE2_API* pHandle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voAAC2_H__

