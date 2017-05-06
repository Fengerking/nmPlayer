	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		H264RawDataInterface.h

	Contains:	H264 raw data interface header file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2012-02-02		Rodney		Create file

*******************************************************************************/


#pragma once
#include "voType.h"

#if defined __cplusplus
extern "C" {
#endif

VO_U32 VO_API voFRH264Open(VO_PTR* ppHandle, VO_SOURCE_OPENPARAM* pParam);
VO_U32 VO_API voFRH264GetTrackInfo(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo);
VO_U32 VO_API voFRH264GetSample(VO_PTR pHandle, VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample);
VO_U32 VO_API voFRH264SetPos(VO_PTR pHandle, VO_U32 nTrack, VO_S64* pPos);
VO_U32 VO_API voFRH264SetFileParam(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
VO_U32 VO_API voFRH264SetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
VO_U32 VO_API voFRH264GetTrackParam(VO_PTR pHandle, VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);

#if defined __cplusplus
}
#endif
