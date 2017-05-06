	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		H264RawData.h

	Contains:	H264 raw data file parser class header file.

	Written by:	Rodney Zhang

	Change History (most recent first):
	2011-10-25		Rodney		Create file

*******************************************************************************/


#pragma once
#include "CBaseReader.h"
#include "voType.h"
#include "H265RawDataFileDataStruct.h"
#include "voList.h"
#include "CFileFormatCheck.h"

class CH265RawData :
	public CBaseReader
{
public:
	CH265RawData(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CH265RawData(void);

public:
	virtual VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	virtual VO_U32 GetCodecCC(VO_U32* pCC);
	virtual VO_U32 GetCodec(VO_U32* pCodec);
	virtual VO_U32 GetDuration() { return m_dwDuration; }
	virtual VO_U32 GetSample(VO_SOURCE_SAMPLE* pSample);
	virtual VO_U32 GetTrackInfo(VO_SOURCE_TRACKINFO* pTrackInfo);
	virtual VO_U32 SetTrackParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 GetTrackParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32 SetPos(VO_S64* pPos);
	virtual	VO_U32	GetHeadData(VO_CODECBUFFER* pHeadData);
	virtual VO_U32 InitTracks(VO_U32 nSourceOpenFlags);
protected:
	VO_U32 RequestABlock();

	// next_bits( 24 ) == 0x000001 or next_bits( 32 ) == 0x00000001 ?
	VO_BOOL next_bits(VO_BYTE bitsNoInBytes, VO_U8 bytes[4]);

	// Is  bytes[3] the boundary 3 zero bytes? true, yes; false, no.
	VO_BOOL IsBoundary3ZeroBytes(VO_U8 bytes[3]);

	// Get frame type(I, P, B).
	VO_VIDEO_FRAMETYPE GetH265FrameType(unsigned char * buffer , int size);

	// Get frame period.
	VO_U32 GetFramePeriod();

	// Parse H265 raw data file and find all nal unit in a H264 raw data file.
	// Return the total number of nal units in the file, or an error code if some error occurs.
	
	VO_U32 ParseH265RawDataFile();
	//Get the H264 head data
	VO_S32 Get265HeadData(char *p_input, VO_S32 input_len);

	//Get a frame 
	VO_S32 get_nalu(char *p_input, VO_S32 input_len, VO_S32 *nal_tyle);

protected:
	VO_BYTE				aStreamBlock[H265_STREAM_BUFFER_SIZE];	// a block of stream data.
	VO_U32				m_dwNALsPerFrame;	// number of NALs per frame
	VO_U32				m_dwDurationPerFrame;	// duration time per frame
	VO_U32				m_dwBlockIndex;		// block index in file.
	VO_U32				m_dwMaxSampleSize;	// maximum sample size.
	VO_U32				m_dwNALCount;	// number of NAL units in a raw data file.
	VO_U32				m_dwCurrIndex;	// current playing index of byte_stream_nal_unit.
	VO_U32				m_dwDuration;	// the playing duration of a H264 raw data file, equal to m_dwNALCount or m_dwFrameCount.
	VO_S64				m_fileSize;		// file size of loaded file.
	VO_BYTE				m_NALHead[4];	// the first 4 bytes in a byte_stream_nal_unit.
	VO_PBYTE			m_pH265HeadBuffer;// to contain the head data.
	VO_U32              m_headdatalen;   //head data length
	VO_PBYTE			m_pStreamBuffer;// stream buffer used as sample buffer.
	VO_U32              m_bytespeed;    //ms,byte speed;
	VO_BOOL             m_bIsRepeat;    //shall repeat to play?
	voList				*m_pNALList;	// nal unit list pointer.

	VO_U32				m_nSysTime;
};
