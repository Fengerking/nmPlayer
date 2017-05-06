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

/*******************************************************************************
File:		CAviMpeg2VideoTrack.h

Contains:	The wrapper for avi mpeg2 video 

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#pragma once
#include "CAviVideoTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAviMpeg2VideoTrack :
	public CAviVideoTrack
{
	friend class CAviReader;
public:
	CAviMpeg2VideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviMpeg2VideoTrack(void);

	virtual VO_U32			GetParameter(VO_U32 uID, VO_PTR pParam);

	virtual VO_U32			GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			SetPosN(VO_S64* pPos);
	virtual VO_U32			GetSampleB(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			GetSampleMpeg2(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			IsKeyFrame_Mpeg2(VO_U64 ullPos, VO_U32 uiSize);

private:
	VO_PBYTE					m_pBufSample;
	VO_U32						m_dwUsedLenofCurrentIndex; // for mpeg2 Split

	VO_BOOL						m_bNeedComposeMpeg2;
	VO_BOOL						m_bNeedDropBFrameAfterSeek;

	VO_BOOL						m_bIsGetSampleFirstTime;

	VO_S64						m_llPos;
};

#ifdef _VONAMESPACE
}
#endif
