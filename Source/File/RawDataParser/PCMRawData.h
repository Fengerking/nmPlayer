	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2011			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		PCMRawData.h

	Contains:	PCM raw data file parser class header file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2012-02-02		Rodney		Create file

*******************************************************************************/


#pragma once
#include "CBaseReader.h"
#include "voType.h"

#define STREAM_BUFFER_SIZE				0x1000
#define DURATION_PER_SAMPLE_BLOCKS		200			// 200 ms


class CPCMRawData :
	public CBaseReader
{
public:
	CPCMRawData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CPCMRawData(void);

public:
	virtual VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32 GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat);
	virtual VO_U32 GetCodecCC(VO_U32* pCC);
	virtual VO_U32 GetCodec(VO_U32* pCodec);
	virtual VO_U32 GetDuration() { return m_dwDuration; }
	virtual VO_U32 GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32 SetPos(VO_S64* pPos);
	virtual VO_U32 GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo);
	virtual VO_U32 SetTrackParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetTrackParameter(VO_U32 uID, VO_PTR pParam);

protected:
	// Big Endian To Little Endian
	virtual VO_VOID BETOLE(VO_PTR pBEBuffer, VO_U32 dwSize);
	virtual VO_U32 SetPCMFormat(VO_AUDIO_FORMAT* pPCMFormat);

protected:
	VO_U32				m_dwBytesPerSecond;	// bytes number of PCM data in a second.
	VO_U32				m_dwBytesPerSample;	// bytes number per sample.
	VO_U32				m_dwMaxSampleSize;	// maximum sample size.
	VO_U32				m_dwCurrIndex;	// current playing index of byte_stream_nal_unit.
	VO_U32				m_dwDuration;	// the playing duration of a PCM file, equal to m_dwNALCount or m_dwFrameCount.
	VO_S64				m_fileSize;		// file size of loaded file.
	VO_PBYTE			m_pStreamBuffer;// stream buffer used as sample buffer.
	VO_AUDIO_FORMAT		m_PCMFormat;	// receive format info from PD.
};
