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

#ifndef __VOFLOENGINE_H__
#define __VOFLOENGINE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
#include "voString.h"
#include "voMem.h"

#include "voType.h"
#include "voAudio.h"
#include "voVideo.h"



/**
* Parameter ID
* S - set; G - get; S/G - both set and get.
*/
#define	VO_PID_FLOENGINE_BASE			0x19800708							    /*!< the base param ID for FLOEngine modules */
#define	VO_PID_FLOENGINE_CALLBACK		(VO_PID_FLOENGINE_BASE | 0x0001)		/*!< <S> set callback functions */
#define	VO_PID_FLOENGINE_LIBPATH		(VO_PID_FLOENGINE_BASE | 0x0002)		/*!< <S> const char*, path of shared libraries */
#define	VO_PID_FLOENGINE_DATAPATH		(VO_PID_FLOENGINE_BASE | 0x0003)		/*!< <S> const char*, path of user data, such as log, dump data */
#define	VO_PID_FLOENGINE_DXDRM			(VO_PID_FLOENGINE_BASE | 0x0004)		/*!< <S> VO_S32, Discretix DRM, 0: Disable; 1: Enable */
#define	VO_PID_FLOENGINE_LOGLEVEL		(VO_PID_FLOENGINE_BASE | 0x0005)		/*!< <S> VO_S32, logging level, 0: Disable; 1, 2, 3... Enable */
#define	VO_PID_FLOENGINE_ERRORRETRY		(VO_PID_FLOENGINE_BASE | 0x0006)		/*!< <S> VO_S32, Socket error retry count */
#define	VO_PID_FLOENGINE_NETTIMEOUT		(VO_PID_FLOENGINE_BASE | 0x0007)		/*!< <S> VO_S32, Timeout for network operations */
#define VO_PID_FLOENGINE_CCMODE         (VO_PID_FLOENGINE_BASE | 0x0008)        /*!< <S> VO_S32, Channel change mode */
#define VO_PID_FLOENGINE_H264ERROR        (VO_PID_FLOENGINE_BASE | 0x0020)        /*!< <S> VO_S32, notify the Engine to increase one packet lost when h.264 error happens */
#define VO_PID_FLOENGINE_H264HEADDATA        (VO_PID_FLOENGINE_BASE | 0x0021)        /*!< <S> VO_CODECBUFFER*, Get the H264 head Data sps/pps */
#define VO_FLOENGINE_CCMODE_NORMAL 0
#define VO_FLOENGINE_CCMODE_OCC 1
#define VO_FLOENGINE_CCMODE_OCC_IT 2

/**
* Error code
*/
#define VO_ERR_FLOENGINE_OK				VO_ERR_NONE
#define VO_ERR_FLOENGINE_BASE			0x19810708
#define VO_ERR_FLOENGINE_FAIL			VO_ERR_FLOENGINE_BASE
#define VO_ERR_FLOENGINE_OUTOFMEMORY	(VO_ERR_FLOENGINE_BASE | 0x0001)		/*!< out of memory */
#define VO_ERR_FLOENGINE_NULLPOINTER	(VO_ERR_FLOENGINE_BASE | 0x0002)		/*!< null pointer */
#define VO_ERR_FLOENGINE_INVALIDARG		(VO_ERR_FLOENGINE_BASE | 0x0003)		/*!< invalid argument */
#define VO_ERR_FLOENGINE_NOTIMPLEMENT	(VO_ERR_FLOENGINE_BASE | 0x0004)		/*!< not implement */

/**
* Event Type
*/
/** removed by Jason, 7/3/2010, not used
#define VO_FLOENGINE_EVENT_BASE								0x19820700
#define	VO_FLOENGINE_EVENT_AccessGranted					(VO_FLOENGINE_EVENT_BASE | 0x0001)
#define	VO_FLOENGINE_EVENT_AccessDenied						(VO_FLOENGINE_EVENT_BASE | 0x0002)
#define	VO_FLOENGINE_EVENT_AccessDeniedBlackout				(VO_FLOENGINE_EVENT_BASE | 0x0003)
#define VO_FLOENGINE_EVENT_AccessDeniedNoPermission			(VO_FLOENGINE_EVENT_BASE | 0x0004)	 
#define VO_FLOENGINE_EVENT_NetworkStatusOK					(VO_FLOENGINE_EVENT_BASE | 0x0005)
#define VO_FLOENGINE_EVENT_NetworkAcquiring					(VO_FLOENGINE_EVENT_BASE | 0x0006)
#define VO_FLOENGINE_EVENT_NetworkOutOfCoverage				(VO_FLOENGINE_EVENT_BASE | 0x0007)
#define VO_FLOENGINE_EVENT_NetworkInterfaceDown				(VO_FLOENGINE_EVENT_BASE | 0x0008)
#define VO_FLOENGINE_EVENT_NetworkIncompatible				(VO_FLOENGINE_EVENT_BASE | 0x0009)
#define VO_FLOENGINE_EVENT_StreamStatusOK					(VO_FLOENGINE_EVENT_BASE | 0x000A)
#define VO_FLOENGINE_EVENT_StreamProgramActivationFailed	(VO_FLOENGINE_EVENT_BASE | 0x000B)
#define VO_FLOENGINE_EVENT_StreamProgramDeactivated			(VO_FLOENGINE_EVENT_BASE | 0x000C)
*/
#define FLO_RTSP_CONNECTION_INFO_DISCONNECTED 500

/**
 * FLOEngine Frame type
*/
#define VO_FLOENGINE_FRAMETYPE_AUDIO    0
#define VO_FLOENGINE_FRAMETYPE_VIDEO    1


/**
 * FLOEngine Callback functions structure
*/
typedef struct
{
	VO_PBYTE	pData;			/*!< the frame data pointer */
	VO_U32		nSize;			/*!< the frame data size */
	VO_U64		nStartTime;		/*!< the frame start time */
	VO_U32		nFrameType;		/*!< the frame type, audio, video */
} VO_FLOENGINE_FRAME;


/**
 * FLOEngine Callback functions structure
*/
typedef struct
{
	VO_S32 (* OnEvent)(int nEventType, unsigned int nParam, void * pData);

	VO_S32 (* OnFrame)(VO_FLOENGINE_FRAME * pFrame); 

} VO_FLOENGINE_CALLBACK;


/**
* FLO Engine function set
*/
typedef struct
{
	VO_U32 (VO_API * Create) (VO_HANDLE * phFLOEngine);

	VO_U32 (VO_API * Destroy) (VO_HANDLE hFLOEngine);

	VO_U32 (VO_API * Open) (VO_HANDLE hFLOEngine, const char * pDataSource);

	VO_U32 (VO_API * Close) (VO_HANDLE hFLOEngine);

	VO_U32 (VO_API * Start) (VO_HANDLE hFLOEngine);

	VO_U32 (VO_API * Stop) (VO_HANDLE hFLOEngine);

	VO_U32 (VO_API * GetParam) (VO_HANDLE hFLOEngine, VO_U32 nParamID, VO_S32 * pParam);

	VO_U32 (VO_API * SetParam) (VO_HANDLE hFLOEngine, VO_U32 nParamID, VO_PTR pParam);

} VO_FLOENGINE_API;


VO_S32 VO_API voGetFLOEngineAPI(VO_FLOENGINE_API * pFLOEngineAPI);

typedef VO_U32 (VO_API * PVOGETFLOENGINEAPI) (VO_FLOENGINE_API * pFLOEngineAPI);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VOFLOENGINE_H__
