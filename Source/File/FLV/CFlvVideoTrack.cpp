#include "CFlvReader.h"
#include "CFlvVideoTrack.h"
#include "fCC.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CFlvVideoTrack::CFlvVideoTrack(VO_U8 btStreamNum, VO_U32 dwDuration, PFlvVideoInfo pVideoInfo, CFlvReader* pReader, VO_MEM_OPERATOR* pMemOp)
	: CFlvTrack(VOTT_VIDEO, btStreamNum, dwDuration, pReader, pMemOp)
	, m_pVideoInfo(pVideoInfo)
	,m_uFilePos(0)
	,m_dwFileIndex(0)
	,m_uThumbnailCnt(0)
{
	if( m_pVideoInfo->wWidth && (m_pVideoInfo->wWidth>100))
		m_lMaxSampleSize = VO_U32(m_pVideoInfo->wWidth) * m_pVideoInfo->wHeight * 3 / 2;
	else
		m_lMaxSampleSize = VO_U32(1920) * 1080 * 3 / 2;

	SetGlobalBufferExtSize( m_lMaxSampleSize );
}

CFlvVideoTrack::~CFlvVideoTrack()
{
}

VO_U32 CFlvVideoTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 nRC = CFlvTrack::GetSampleN(pSample);
	if(VO_ERR_SOURCE_OK != nRC)
		return nRC;

	if(4 == m_pVideoInfo->btCodecID)		//VP6 FLV video packet
	{
		/*		
		HorizontalAdjustment	UB[4]	Number of pixels to subtract from the total width. The resulting width is used on the stage, and the rightmost pixels of the video is cropped.
		VerticalAdjustment		UB[4]	Number of pixels to subtract from the total height. The resulting height is used on the stage, and the rightmost pixels of the video is cropped.
		Data					UI8[n]	Raw VP6 video stream data
		*/
		pSample->Buffer++;
		pSample->Size--;
	}
	else if(5 == m_pVideoInfo->btCodecID)	//VP6 FLV Alpha video packet
	{
		/*
		HorizontalAdjustment	UB[4]				Number of pixels to subtract from the total width. The resulting width is used on the stage, and the rightmost pixels of the video is cropped.
		VerticalAdjustment		UB[4]				Number of pixels to subtract from the from the total height. The resulting height is used on the stage, and the rightmost pixels of the video is cropped.
		OffsetToAlpha			UI24				Offset in bytes to the alpha channel video data
		Data					UI8[OffsetToAlpha]	Raw VP6 video stream data representing the color channels
		AlphaData				UI8[n]				Raw VP6 video stream data representing the alpha channel
		*/
		pSample->Buffer++;
		pSample->Size--;
	}
	
	return VO_ERR_SOURCE_OK;
}

VO_U64 CFlvVideoTrack::GetFilePosByFileIndex(VO_U32 dwFileIndex)
{
	return m_pReader->FileIndexGetFilePosByIndex(dwFileIndex);
}

VO_U32 CFlvVideoTrack::GetFileIndexByTime(VO_BOOL bForward, VO_S64 llTimeStamp)
{
	return m_pReader->FileIndexGetIndexByTime(bForward, llTimeStamp);
}

VO_U32 CFlvVideoTrack::GetCodecCC(VO_U32* pCC)
{
	switch(m_pVideoInfo->btCodecID)
	{
	case 2:	//Sorenson H.263
		*pCC = FOURCC_FLV1;
		break;

	case 4:	//On2 VP6
	case 5:	//On2 VP6 with alpha channel
		*pCC = FOURCC_FLV4;
		break;

	case 7: //avc
		*pCC = FOURCC_AVC1;
		break;

	case 3:	//Screen video
	case 6:	//Screen video version 2
	default:
		*pCC = 0;
		break;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvVideoTrack::GetVideoFormat(VO_VIDEO_FORMAT* pVideoFormat)
{
	pVideoFormat->Width = m_pVideoInfo->wWidth;
	pVideoFormat->Height = m_pVideoInfo->wHeight;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvVideoTrack::GetBitrate(VO_U32* pdwBitrate)
{
	if(pdwBitrate)
		*pdwBitrate = m_pVideoInfo->dwBytesPerSec;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvVideoTrack::GetFrameTime(VO_U32* pdwFrameTime)
{
	if(pdwFrameTime)
	{
		if(m_pVideoInfo->llAvgTimePerFrame)
			*pdwFrameTime = static_cast<VO_U32>(m_pVideoInfo->llAvgTimePerFrame / 100);
		else
		{
			VO_U32 dwCurrCount = m_pStreamFileIndex->GetCurrEntryCount();
			if(dwCurrCount < 2)
				return VO_ERR_NOT_IMPLEMENT;

			*pdwFrameTime = VO_U64(100) * (m_pStreamFileIndex->GetCurrEndTime() - m_pStreamFileIndex->GetCurrStartTime()) / (dwCurrCount - 1);
		}
	}

	return VO_ERR_SOURCE_OK;
}
VO_U32 CFlvVideoTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = m_pVideoInfo->ptr_header_data;
	pHeadData->Length = m_pVideoInfo->header_size;

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvVideoTrack::GetNearKeyFrame(VO_S64 llTime, VO_S64* pllPreviousKeyframeTime, VO_S64* pllNextKeyframeTime)
{
	return m_pReader->GetNearKeyFrame(llTime, pllPreviousKeyframeTime, pllNextKeyframeTime);
}

VO_U32 CFlvVideoTrack::GetThumbNail(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	VO_U32 nResult = VO_ERR_SOURCE_ERRORDATA;
	if(!pThumbNailInfo)
	{
		return VO_ERR_SOURCE_ERRORDATA;
	}
	switch(pThumbNailInfo->uFlag)
	{
	case VO_SOURCE_THUMBNAILMODE_INFOONLY:
		{
			VOLOGI("FLV GetThumbNailInfo entry");
			nResult = GetThumbNailInfo(pThumbNailInfo);
			VOLOGI("FLV GetThumbNailInfo nResult %x ",nResult);
		}
		break;

	case VO_SOURCE_THUMBNAILMODE_DATA:
		{
			VOLOGI("FLV GetThumbNailBuffer entry");
			nResult = GetThumbNailBuffer(pThumbNailInfo);
			VOLOGI("FLV GetThumbNailBuffer nResult %x ",nResult);
		}
		break;

	default:
		break;
	}

	return nResult;
}

VO_U32 CFlvVideoTrack::GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	if(!pThumbNailInfo)
	{
		return VO_ERR_SOURCE_ERRORDATA;
	}

	if (VOTT_VIDEO!=m_nType)
	{
		return VO_ERR_NOT_IMPLEMENT;
	}

	CvoFileDataParser* pDataParser = m_pStreamFileReader->GetFileDataParserPtr();
	if (pDataParser == NULL)
	{
		return VO_ERR_SOURCE_END;
	}

	TrackGenerateIndex();

	PBaseStreamMediaSampleIndexEntry pEntry;
	VO_BOOL bFrameDropped = VO_FALSE;
	if(!m_pStreamFileIndex->GetEntry((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, pThumbNailInfo->ullTimeStamp, &pEntry, &bFrameDropped))
		return VO_ERR_SOURCE_END;

	if (pEntry)
	{
		if (!pEntry->IsKeyFrame())
		{
			return VO_ERR_SOURCE_ERRORDATA;
		}
		pThumbNailInfo->ullFilePos = pEntry->pos_in_buffer&0x7fffffff;
		pThumbNailInfo->ullTimeStamp = (VO_S64)pEntry->time_stamp;
		pThumbNailInfo->uSampleSize = pEntry->size;
		m_pStreamFileIndex->RemoveAll();
		return VO_ERR_SOURCE_OK;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CFlvVideoTrack::GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	VO_U32 nResult = VO_ERR_SOURCE_ERRORDATA;

	if(!pThumbNailInfo)
	{
		VOLOGI("FLV GetThumbNailBuffer______________pThumbNailInfo is NULL");
		return nResult;
	}

	if(!m_pThumbnailBuffer && !m_ThumbnailBufferSize){
		VO_U32 size = 0;
		GetMaxSampleSize( &size );		
		m_pThumbnailBuffer = new VO_BYTE[size];
		m_ThumbnailBufferSize = size;
	}

	if(0 != pThumbNailInfo->uSampleSize)
	{
		VO_U32 sample_size = (pThumbNailInfo->uSampleSize & 0x7FFFFFFF);
		if(sample_size <= m_ThumbnailBufferSize )
		{
			//memcpy( m_pThumbnailBuffer , sample.Buffer , sample_size );
			CFlvReader * fpnu = (CFlvReader *)m_pStreamFileReader;
			VO_PBYTE pBuffer = m_pThumbnailBuffer;
			if(pBuffer)
			{
				fpnu->GetThumbNailBuffer(&pBuffer, sample_size , pThumbNailInfo->ullFilePos);					
			}

			pThumbNailInfo->uSampleSize = sample_size;
			pThumbNailInfo->pSampleData = m_pThumbnailBuffer;
			VOLOGI("GetThumbNailBuffer----Size = %lu, Pos =  %llu, Time= %llu",pThumbNailInfo->uSampleSize, pThumbNailInfo->ullFilePos, pThumbNailInfo->ullTimeStamp);
			nResult = VO_ERR_SOURCE_OK;
		}
	} 

	VOLOGI("FLV GetThumbNailBuffer______________nResult %x ",nResult);

	return nResult;
}



VO_U8 CFlvVideoTrack::IsCannotGenerateIndex(CvoFileDataParser* pDataParser)
{
	if(pDataParser->IsParseEnd())
		return 2;

	if(m_bParseForSelf)
	{
		if(IsGlobalBufferFull() || IsTrackBufferHasEnoughFrame())
			return 1;
	}
	else
	{
		if(m_pStreamFileReader->IsTrackBufferFull())
			return 1;
	}

	return 0;
}

VO_BOOL CFlvVideoTrack::IsTrackBufferHasEnoughFrame()
{
		PBaseStreamMediaSampleIndexEntry pEntry;
		if(m_pStreamFileIndex->GetKeyFrameEntry(&pEntry))
		{
			return VO_TRUE;
		}
		return VO_FALSE;
}
