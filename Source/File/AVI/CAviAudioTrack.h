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
File:		CAviAudioTrack.h

Contains:	The wrapper for avi audio track 

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

class CAviAudioTrack :
	public CAviTrack
{
	friend class CAviReader;
public:
	CAviAudioTrack(VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviAudioTrack();

public:
	virtual	VO_U32			GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32			GetMaxSampleSize(VO_U32* pdwMaxSampleSize);
	virtual VO_U32			GetCodecCC(VO_U32* pCC);
	virtual VO_U32			GetFirstFrame(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32			GetWaveFormatEx(VO_WAVEFORMATEX** ppWaveFormatEx);
	virtual VO_U32			GetBitrate(VO_U32* pdwBitrate);
	virtual VO_U32          GetScale() {return m_nScale;}
	virtual VO_U32          GetRate() {return m_nRate;}

	virtual VO_U32			GetCodec(VO_U32* pCodec);

protected:
	//GetSample and SetPos implement of normal mode!!
	virtual VO_U32			GetSampleN(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32			SetPosN(VO_S64* pPos);
	//GetSample and SetPos implement of key frame mode!!
	virtual VO_U32			GetSampleK(VO_SOURCE_SAMPLE* pSample) {return VO_ERR_NOT_IMPLEMENT;}
	virtual VO_U32			SetPosK(VO_S64* pPos) {return VO_ERR_NOT_IMPLEMENT;}

	virtual VO_BOOL			AddFrame(CGFileChunk* pFileChunk, VO_U32 dwLen);

	virtual VO_VOID			OnIdx1(VO_U32 dwCurrIndexNum, PAviOriginalIndexEntry pEntry);

	VO_VOID					perpare_ogg_headerdata();
	
	VO_BOOL					PCM24216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign);
	VO_BOOL					PCM32216(VO_PBYTE pBuffer, VO_U32* pdwSize, VO_U16 nChannels, VO_U16 nBlockAlign);

protected:
	VO_U32				m_dwAvgBytesPerSec;
	VO_U64				m_ullTotalSize;
	VO_U64				m_ullAudioTotalSize;			//audio total size(no index case)
	VO_U64				m_ullAudioCurrTotalSize;
	double				m_dAudioVbrTimePerSample;		//<MS>
	VO_U32				m_dwIndexOffset;				//some VBR audio will put some padding frame on header

	VO_PBYTE			m_ptr_privatedata;
	VO_U32				m_privatedata_size;
	
	VO_BOOL				m_bPCM;
	VO_U16				m_wFileBitsPerSample;
	VO_U16				m_nFileBlockAlign;
	VO_U16				m_nChannels;
	VO_U32				m_nScale;
	VO_U32              m_nRate;
};

#ifdef _VONAMESPACE
}
#endif
