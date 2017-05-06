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

Contains:	The wrapper for avi track  

Written by:	East

Reference:	OpenDML AVI File Format Extensions

Change History (most recent first):
2006-09-20		East			Create file

*******************************************************************************/
#pragma once
#include "CBaseStreamFileTrack.h"
#include "CAviIndex.h"
#include "fCodec.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CAviReader;
#ifndef _FILE_IO_NOT_USE_BUFFER
#define AVI_FILE_BUFFER_SIZE		0x20000	//128K
class CAviFileBuffer
	: public CvoBaseObject
	, public CvoBaseMemOpr
{
public:
	CAviFileBuffer(VO_MEM_OPERATOR* _mem_opr);
	virtual ~CAviFileBuffer();

public:
	VO_BOOL read_from_buffer(VO_PBYTE* _buf_ptr_ptr, VO_U64 _file_pos, VO_U32 _len);
	VO_S32	write_from_file(CAviReader* _reader, VO_U64 _file_pos, VO_U32 _len);

protected:
	VO_PBYTE			buf_ptr;
	VO_U32				len;
	VO_U64				file_pos;
};
#endif	//_FILE_IO_NOT_USE_BUFFER

class CAviTrack :
	public CBaseStreamFileTrack
{
	friend class CAviReader;
public:
	CAviTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp);
	virtual ~CAviTrack();

public:
	virtual VO_VOID			OnIdx1(VO_U32 dwCurrIndexNum, PAviOriginalIndexEntry pEntry);

protected:
	virtual VO_U32			Prepare();
	virtual VO_U32			Unprepare();

	VO_BOOL					IsCodecDivX() {return ((VOTT_VIDEO == m_nType) && fCodecIsDivX(((VO_BITMAPINFOHEADER*)m_pPropBuffer)->biCompression)) ? VO_TRUE : VO_FALSE;}
	VO_BOOL					IsCodecXviD() {return ((VOTT_VIDEO == m_nType) && fCodecIsXviD(((VO_BITMAPINFOHEADER*)m_pPropBuffer)->biCompression)) ? VO_TRUE : VO_FALSE;}

	//return value: 0 - fail; 1 - success; 2 - need retry
	VO_S32					ReadFileContent(VO_PBYTE* ppBuffer, VO_U64 ullFilePos, VO_U32 dwSize);
#ifndef _FILE_IO_NOT_USE_BUFFER
	//other track read from buffering
	virtual VO_BOOL			ReadFromBufferByOtherTrack(VO_PBYTE* ppBuffer, VO_U64 ullFilePos, VO_U32 dwSize);
#endif	//_FILE_IO_NOT_USE_BUFFER

	virtual VO_BOOL			AddFrame(CGFileChunk* pFileChunk, VO_U32 dwLen) = 0;

protected:
	CAviReader*			m_pReader;

	VO_U16				m_wStreamTwocc;
	//property buffer, audio-WAVEFORMATEX, video-BITMAPINFOHEADER
	VO_U32				m_dwPropBufferSize;
	VO_PBYTE			m_pPropBuffer;

	VO_U32				m_dwMaxSampleSize;

	//Index File
	//Media Sample Index Entries
	CAviBaseIndex*		m_pIndex;
	VO_U32				m_dwCurrIndex;

#ifndef _FILE_IO_NOT_USE_BUFFER
	CAviFileBuffer*		m_pAviFileBuffer;
	VO_BOOL				m_bForceMemCopy;
#endif	//_FILE_IO_NOT_USE_BUFFER

	VO_U32				m_dwLastIndex;
};

#ifdef _VONAMESPACE
}
#endif
