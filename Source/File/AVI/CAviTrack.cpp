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
#include "CAviReader.h"
#include "CAviTrack.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

#ifndef _FILE_IO_NOT_USE_BUFFER
CAviFileBuffer::CAviFileBuffer(VO_MEM_OPERATOR* _mem_opr)
	: CvoBaseMemOpr(_mem_opr)
	, buf_ptr(VO_NULL)
	, len(0)
	, file_pos(0)
{
	buf_ptr = (VO_PBYTE)MemAlloc(AVI_FILE_BUFFER_SIZE);
}

CAviFileBuffer::~CAviFileBuffer()
{
	MemFree(buf_ptr);
}

VO_BOOL CAviFileBuffer::read_from_buffer(VO_PBYTE* _buf_ptr_ptr, VO_U64 _file_pos, VO_U32 _len)
{
	if((_file_pos < file_pos) || ((_file_pos + _len) > (file_pos + len)))
		return VO_FALSE;

	*_buf_ptr_ptr = buf_ptr + (_file_pos - file_pos);
	return VO_TRUE;
}

VO_S32 CAviFileBuffer::write_from_file(CAviReader* _reader, VO_U64 _file_pos, VO_U32 _len)
{
	file_pos = _file_pos;
	len = _len;

	return _reader->ReadBuffer(buf_ptr, file_pos, len);
}
#endif	//_FILE_IO_NOT_USE_BUFFER

CAviTrack::CAviTrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CAviReader *pReader, VO_MEM_OPERATOR* pMemOp)
	: CBaseStreamFileTrack(nType, btStreamNum, dwDuration, pReader, pMemOp)
	, m_pReader(pReader)
	, m_wStreamTwocc((((btStreamNum & 0xF) + 0x30) << 8) + ((btStreamNum & 0xF0) + 0x30))
	, m_dwPropBufferSize(0)
	, m_pPropBuffer(VO_NULL)
	, m_dwMaxSampleSize(0)
	, m_pIndex(VO_NULL)
	, m_dwCurrIndex(0)
#ifndef _FILE_IO_NOT_USE_BUFFER
	, m_pAviFileBuffer(VO_NULL)
	, m_bForceMemCopy(VO_FALSE)
#endif	//_FILE_IO_NOT_USE_BUFFER
	, m_dwLastIndex(0)
{
}

CAviTrack::~CAviTrack()
{
	SAFE_DELETE(m_pIndex);
	SAFE_MEM_FREE(m_pPropBuffer);
#ifndef _FILE_IO_NOT_USE_BUFFER
	SAFE_DELETE(m_pAviFileBuffer);
#endif	//_FILE_IO_NOT_USE_BUFFER
}

VO_U32 CAviTrack::Prepare()
{
	if(m_pIndex)
	{
#ifndef _FILE_IO_NOT_USE_BUFFER
		if(m_pAviFileBuffer)
			delete m_pAviFileBuffer;

		m_pAviFileBuffer = new CAviFileBuffer(m_pMemOp);
		if(!m_pAviFileBuffer)
			return VO_FALSE;

		// we must use the buffer only once, otherwise the content maybe be modified by DivXDRM engine.
		// East 2010/08/24
		if(m_pReader->IsDivXDRM())
			m_bForceMemCopy = VO_TRUE;
#endif	//_FILE_IO_NOT_USE_BUFFER

		return CBaseTrack::Prepare();
	}
	else
		return CBaseStreamFileTrack::Prepare();
}

VO_U32 CAviTrack::Unprepare()
{
	CBaseStreamFileTrack::Unprepare();

	SAFE_DELETE(m_pIndex);
#ifndef _FILE_IO_NOT_USE_BUFFER
	SAFE_DELETE(m_pAviFileBuffer);
#endif	//_FILE_IO_NOT_USE_BUFFER

	return VO_ERR_SOURCE_OK;
}

VO_S32 CAviTrack::ReadFileContent(VO_PBYTE* ppBuffer, VO_U64 ullFilePos, VO_U32 dwSize)
{
	VO_S32 nRes = 0;
#ifdef _FILE_IO_NOT_USE_BUFFER
	nRes = m_pReader->ReadBuffer(m_pSampleData, ullFilePos, dwSize);
	*ppBuffer = m_pSampleData;
	return nRes;
#else	//_FILE_IO_NOT_USE_BUFFER
	if(dwSize >= AVI_FILE_BUFFER_SIZE)
	{
		nRes = m_pReader->ReadBuffer(m_pSampleData, ullFilePos, dwSize);
		*ppBuffer = m_pSampleData;
		return nRes;
	}

	//try to read the content from this track's buffering
	if(m_pAviFileBuffer->read_from_buffer(ppBuffer, ullFilePos, dwSize))
	{
		if(m_bForceMemCopy)
		{
			MemCopy(m_pSampleData, *ppBuffer, dwSize);
			*ppBuffer = m_pSampleData;
		}

		return 1;
	}

	//try to read the content from other track's buffering
	CAviTrack* pOtherTrack = (CAviTrack*)m_pReader->GetOtherTrackInUsed(this);
	if(pOtherTrack && pOtherTrack->ReadFromBufferByOtherTrack(ppBuffer, ullFilePos, dwSize))
	{
		//other track maybe flush the data when decoding
		//so we must do a memory copy
		MemCopy(m_pSampleData, *ppBuffer, dwSize);
		*ppBuffer = m_pSampleData;

		return 1;
	}

	//try to read the content from file and re-fill the buffering content
	VO_U32 dwRead = AVI_FILE_BUFFER_SIZE;
	nRes = m_pAviFileBuffer->write_from_file(m_pReader, ullFilePos, dwRead);
	if(1 != nRes)
		return nRes;

	if(m_pAviFileBuffer->read_from_buffer(ppBuffer, ullFilePos, dwSize))
	{
		if(m_bForceMemCopy)
		{
			MemCopy(m_pSampleData, *ppBuffer, dwSize);
			*ppBuffer = m_pSampleData;
		}

		return 1;
	}

	return 0;
#endif	//_FILE_IO_NOT_USE_BUFFER
}

#ifndef _FILE_IO_NOT_USE_BUFFER
VO_BOOL CAviTrack::ReadFromBufferByOtherTrack(VO_PBYTE* ppBuffer, VO_U64 ullFilePos, VO_U32 dwSize)
{
	return m_pAviFileBuffer->read_from_buffer(ppBuffer, ullFilePos, dwSize);
}
#endif	//_FILE_IO_NOT_USE_BUFFER

VO_VOID CAviTrack::OnIdx1(VO_U32 dwCurrIndexNum, PAviOriginalIndexEntry pEntry)
{
	if(pEntry->dwSize > m_dwMaxSampleSize)
	{
		m_dwMaxSampleSize = pEntry->dwSize;

		VO_VIDEO_FORMAT videoformat = {0};
		if (GetVideoFormat(&videoformat) == VO_ERR_NONE)
		{
			VO_U32 iExperienceSize = videoformat.Height * videoformat.Width * 3 / 2;
			if (m_dwMaxSampleSize > iExperienceSize)
				m_dwMaxSampleSize = iExperienceSize;
		}
	}
}