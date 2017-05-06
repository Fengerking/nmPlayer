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


#ifndef __voDataStream_H__
#define __voDataStream_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <voIndex.h>
#include <voString.h>

#define VO_DATA_TYPE_AUDIO		0
#define VO_DATA_TYPE_VIDEO		1
#define VO_DATA_TYPE_STREM_TS	2

/**
 * General input buffer
 */
typedef struct {
	VO_PBYTE	pBuffer;		/*!< Buffer pointer */
	VO_U32		nSize;			/*!< Buffer size in byte */
	VO_S64		llTime;			/*!< The time of the buffer */
	VO_U32		nFlag;			/*!< The flag of the buffer */
} VO_DATA_BUFFER;

/**
 * Data source function set
 */
typedef struct
{
	VO_U32		nVersion;	// It should be 0X1000XXXX.

	VO_PTR		hUserData;

	VO_U32 (VO_API * Read) (VO_PTR hUserData, int nType, VO_DATA_BUFFER * pBuffer);

	VO_U32 (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	VO_U32 (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_DATA_SOURCE_API;


/**
 * Data stream function set
 */
typedef struct
{
	VO_U32			nVersion;	// It should be 0X1000XXXX.

	VO_PTR		hHandle;

	VO_PTR		hUserData;

	VO_U32 (VO_API * Init) (VO_PTR * ppHandle, VO_PTR pUserData, VO_PTR pParam, int nFlag);

	VO_U32 (VO_API * Uninit) (VO_PTR pHandle);

	VO_U32 (VO_API * Start) (VO_PTR pHandle);

	VO_U32 (VO_API * Stop) (VO_PTR pHandle);

	VO_U32 (VO_API * Open) (VO_PTR pHandle, VO_PTR pSource, int nFlag);

	VO_U32 (VO_API * Close) (VO_PTR pHandle);

	VO_U32 (VO_API * Read) (VO_PTR pHandle, VO_PBYTE * pBuffer, VO_S32 lSize);

	VO_U32 (VO_API * Seek) (VO_PTR pHandle, VO_S64 llPos, VO_S32 lFlag);

	VO_U32 (VO_API * Size) (VO_PTR pHandle, VO_S64 * pllSize);

	VO_U32 (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	VO_U32 (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_DATA_STREAM_API;

VO_S32 VO_API voGetDataStreamAPI (VO_DATA_STREAM_API* pStreamAPI, VO_U32 uFlag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voDataStream_H__
