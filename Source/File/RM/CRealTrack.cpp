#include "CRealReader.h"
#include "CRealTrack.h"
#include "fCodec.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CRealTrack::CRealTrack(VO_TRACKTYPE nType, PRealTrackInfo pTrackInfo, CRealReader* pReader, VO_MEM_OPERATOR* pMemOp)
	: CBaseStreamFileTrack(nType, (VO_U8)pTrackInfo->wStreamNum, pTrackInfo->dwDuration, pReader, pMemOp)
	, m_pReader(pReader)
	, m_pInitParam(VO_NULL)
	, m_dwInitParamSize(0)
	, m_dwBitrate(pTrackInfo->dwBitrate)
	, m_dwDataOffset(pTrackInfo->dwDataOffset)
	, m_dwSeekPoints(0)
	, m_pSeekPoints(VO_NULL)
	, m_IsThumbNail(VO_FALSE)
{
}

CRealTrack::~CRealTrack()
{
	SAFE_MEM_FREE(m_pInitParam);
	SAFE_MEM_FREE(m_pSeekPoints);
}

VO_U32 CRealTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = m_dwInitParamSize;
	pHeadData->Buffer = m_pInitParam;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = m_dwBitrate;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealTrack::SetPosN(VO_S64* pPos)
{
	m_pReader->OnTrackSetPosN(this, *pPos);

	return CBaseStreamFileTrack::SetPosN(pPos);
}

VO_BOOL CRealTrack::ReadIndexFromFile(CGFileChunk* m_pFileChunk, PRealFileIndexInfo pIndexInfo)
{
	if(pIndexInfo)
	{
		if(!m_pFileChunk->FLocate(pIndexInfo->file_position))
		{
			return VO_FALSE;
		}

		SAFE_MEM_FREE(m_pSeekPoints);

		m_pSeekPoints = NEW_OBJS(RealSeekPoint, pIndexInfo->num_indices);
		if(!m_pSeekPoints)
		{
			return VO_FALSE;
		}

		m_dwSeekPoints = pIndexInfo->num_indices;

		use_big_endian_read
		VO_U16 wObjectVersion = 0;
		for(VO_U32 i = 0; i < m_dwSeekPoints; i++)
		{
			ptr_read_word(wObjectVersion);
			if(0 == wObjectVersion)
			{
				ptr_read_dword(m_pSeekPoints[i].time_stamp);
				ptr_read_dword(m_pSeekPoints[i].offset);
				//skip packet_count
				ptr_skip(4);
			}
			else
			{
				return VO_FALSE;
			}
		}
	}

	//it is not a problem if no index info is present.
	return VO_TRUE;
}

VO_U32 CRealTrack::FileIndexGetIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	VO_U32 dwIndex = 0;
	for(VO_U32 i = 0; i < m_dwSeekPoints; i++)
	{
		dwIndex = i;

		if(m_pSeekPoints[i].time_stamp >= llTimeStamp)
			break;
	}

	return dwIndex;
}

VO_U64 CRealTrack::FileIndexGetFilePosByIndex(VO_U32 dwFileIndex)
{
	return (dwFileIndex < m_dwSeekPoints) ? m_pSeekPoints[dwFileIndex].offset : -1;
}

VO_U32 CRealTrack::FileIndexGetFilePosByTime(VO_S64 llTimeStamp)
{
	VO_U32 dwIndex = 0;
	for(VO_U32 i = 0; i < m_dwSeekPoints && m_pSeekPoints[i].time_stamp <= llTimeStamp; i++)
		dwIndex = i;

	return m_pSeekPoints ? m_pSeekPoints[dwIndex].offset : -1;
}

VO_U32 CRealTrack::Unprepare()
{
	CBaseStreamFileTrack::Unprepare();

	SAFE_MEM_FREE(m_pSeekPoints);

	return VO_ERR_SOURCE_OK;
}

VO_U32 CRealTrack::GetInfo(VO_SOURCE_TRACKINFO* pTrackInfo)
{
	if( !IsInUsed() )
	{
		return VO_ERR_NOT_IMPLEMENT;
	}

	return CBaseStreamFileTrack::GetInfo(pTrackInfo);
}

VO_U32 CRealTrack::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	if( !IsInUsed() )
	{
		return VO_ERR_NOT_IMPLEMENT;
	}

	return CBaseStreamFileTrack::GetParameter(uID , pParam);
}

VO_VOID	CRealTrack::SetThumbnailFlag(VO_BOOL bThumbnail)
{
	m_IsThumbNail = bThumbnail;
}