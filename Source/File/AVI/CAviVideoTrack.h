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
File:		CAviTrack.h

Contains:	The wrapper for avi video track  

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#pragma once
#include "CAviTrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef struct tagAviIndexBufferingEntry {
	VO_U64						pos_in_file;
	VO_U32						size;			//first bit is no key frame flag(0 - key frame, 1 - internal frame)
	VO_U64						dd_pos_in_file;
	VO_U32						dd_size;
	tagAviIndexBufferingEntry*	next;
} AviIndexBufferingEntry, *PAviIndexBufferingEntry;

class CAviVideoTrack :
	public CAviTrack
{
	friend class CAviReader;
public:
	CAviVideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviVideoTrack();

public:
	virtual VO_U32			GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual	VO_U32			GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetFirstFrame(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			GetNextKeyFrame(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat);
	virtual VO_U32			GetBitmapInfoHeader(VO_BITMAPINFOHEADER** ppBitmapInfoHeader);
	virtual VO_U32			GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32			GetFrameNum(VO_U32* pdwFrameNum);
	virtual VO_U32			GetFrameTime(VO_U32* pdwFrameTime);
	virtual VO_U32			GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime);

protected:
	virtual VO_U32			Prepare();
	virtual VO_U32			Unprepare();

	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32			GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			SetPosN(VO_S64* pPos);
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32			GetSampleK(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			SetPosK(VO_S64* pPos);
	virtual VO_U32 			GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo);
	virtual VO_U32			GetThumbnailSyncSampleInfo(VO_PTR pParam);

protected:
	virtual VO_BOOL			IndexBuffering_Fill();
	virtual VO_VOID			IndexBuffering_Remove(VO_U32 dwRemoveCount);
	virtual VO_BOOL			AddFrame(CGFileChunk* pFileChunk, VO_U32 dwLen);
	inline VO_BOOL			IsKeyFrame_MPEG4(VO_PBYTE pMpeg4Header);
	inline VO_BOOL			IsKeyFrame_DIVX(VO_PBYTE pDivxHeader);

	virtual VO_VOID			Flush();

	virtual VO_VOID			OnIdx1(VO_U32 dwCurrIndexNum, PAviOriginalIndexEntry pEntry);

protected:
	VO_U64						m_ullAvgTimePerFrame;

	VO_BOOL						m_bNoIndex;
	PAviIndexBufferingEntry		m_pList;
	PAviIndexBufferingEntry		m_pHead;
	PAviIndexBufferingEntry		m_pTail;
	VO_U32						m_dwCount;

	VO_PBYTE					m_pFrameHeader;
	VO_U32						m_dwCodec;

	VO_BOOL						m_bHasDdChunk;
	VO_BYTE						m_ddInfo[10];

	PBaseStreamMediaSampleIndexEntry	m_pTempEntry;

	VO_U32						m_dwExtData;
	VO_PBYTE					m_pExtData;
};

#ifdef _VONAMESPACE
}
#endif
