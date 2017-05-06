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
#ifndef __voSource2_ProgramInfo_H__
#define __voSource2_ProgramInfo_H__

#include "voSource2.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VOS2_PROGINFO_BASE					0x87000000
// VOS2PRG_NotifyEvent ID
// The param uStatus is VOS2_PROGINFO_COMMAND, pValue is stream info
#define VOCB_PROGINFO_UPDATE_STREAM			(VOS2_PROGINFO_BASE | 0x87000001)
#define VOCB_PROGINFO_UPDATE_TRACK			(VOS2_PROGINFO_BASE | 0x87000005)

// The param uStatus is VOS2_PROGINFO_COMMAND, pValue is tracm info
#define VOCB_PROGINFO_UPDATE_RESET			(VOS2_PROGINFO_BASE | 0x87000010)
#define VOCB_PROGINFO_UPDATE_PROGTYPE		(VOS2_PROGINFO_BASE | 0x87000020)

// Param ID 
// the pValue should be VO_U32 *
// Stream parser module 0 - 0X1000
// Stream manager module 0X1000 - 0X2000
// OS Source module 0X2000 - 0X3000
#define VOID_PROGINFO_STREAM_ID				(VOS2_PROGINFO_BASE | 0x87000011)
#define VOID_PROGINFO_TRACK_ID				(VOS2_PROGINFO_BASE | 0x87000012)
// the pValue is VO_U32 *. The stream number
#define VOID_PROGINFO_STREAM_NUM			(VOS2_PROGINFO_BASE | 0x87000013)


#define VODEF_PROGINFO_FINDALL				0XFFFFFFFF
#define VODEF_PROGINFO_MUXID_STREAM			0
#define VODEF_PROGINFO_MUXID_INVALID		-1

enum VOS2_PROGINFO_COMMAND
{
	VOS2_PROGINFO_NEW				= 0,
	VOS2_PROGINFO_DELETE			= 1,
	VOS2_PROGINFO_SELECT			= 2,
	VOS2_PROGINFO_UPDATE			= 3,
	VOS2_PROGINFO_COMMAND_MAX		= VO_MAX_ENUM_VALUE
};

enum VOS2_PROGINFO_GETTYPE
{
	VOS2_PROGINFO_BYINDEX			= 0,
	VOS2_PROGINFO_BYID				= 1,
	VOS2_PROGINFO_BYSELECT			= 2,
	VOS2_PROGINFO_GETTYPE_MAX		= VO_MAX_ENUM_VALUE
};

typedef VO_S32 (VO_API * VOS2PRG_NotifyEvent) (VO_PTR pUserData, VO_U32 nID, VO_U32 uStatus, VO_PTR pValue);

typedef struct
{
	VO_PTR				pUserData;
	VOS2PRG_NotifyEvent	fNotify;
} VOS2_ProgInfo_Notify;

typedef struct
{
	VO_HANDLE	hHandle;

	VO_U32 (VO_API * SetProgType)	(VO_HANDLE hHandle, VO_SOURCE2_PROGRAM_TYPE nType);

	VO_U32 (VO_API * RegNotify)		(VO_HANDLE hHandle, VOS2_ProgInfo_Notify * pNotify, VO_BOOL bReg);

	VO_U32 (VO_API * SetStream)		(VO_HANDLE hHandle, VO_SOURCE2_STREAM_INFO * pStream, VOS2_PROGINFO_COMMAND nCmd);

	VO_U32 (VO_API * SetTrack)		(VO_HANDLE hHandle, VO_U32 uStrmID, VO_SOURCE2_TRACK_INFO * pTrack, VOS2_PROGINFO_COMMAND nCmd);

	VO_U32 (VO_API * GetStream)		(VO_HANDLE hHandle, VOS2_PROGINFO_GETTYPE nGetType, VO_U32 uValue, VO_SOURCE2_STREAM_INFO * pStream);

	VO_U32 (VO_API * GetTrack)		(VO_HANDLE hHandle, VOS2_PROGINFO_GETTYPE nGetType, VO_U32 uStrmID, VO_U32 uValue, VO_SOURCE2_TRACK_INFO * pTrack, VO_BOOL bCopyHead);

	VO_U32 (VO_API * Reset)			(VO_HANDLE hHandle);

	VO_U32 (VO_API * GetParam)		(VO_HANDLE hHandle, VO_U32 uID, VO_PTR pValue);

	VO_U32 (VO_API * DumpInfo)		(VO_HANDLE hHandle, VO_TCHAR * pFile);

} VOS2_ProgramInfo_Func;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	//__voSource2_ProgramInfo_H__