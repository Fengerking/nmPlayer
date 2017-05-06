#include "CMKVVideoTrack_RealSpecialize.h"
#include "fCC.h"
#include "voLog.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif


CMKVVideoTrack_RealSpecialize::CMKVVideoTrack_RealSpecialize( VO_U8 btStreamNum, VO_U32 dwDuration , TracksTrackEntryStruct * pVideoInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp )
:CMKVVideoTrack( btStreamNum , dwDuration , pVideoInfo , pReader , pMemOp )
,m_currenttimestamp(0)
,m_pInitParam(VO_NULL)
,m_dwInitParamSize(0)
,m_nMaxEncoderFrameSize(-1)
{
	AnalyzeOpaqueHeader( (VO_PBYTE)pVideoInfo->pCodecPrivate , pVideoInfo->CodecPrivateSize );
}

CMKVVideoTrack_RealSpecialize::~CMKVVideoTrack_RealSpecialize()
{
	SAFE_MEM_FREE(m_pInitParam);
}

VO_BOOL CMKVVideoTrack_RealSpecialize::NewFrameArrived( MKVFrame * pFrame )
{
	if(0xFFFFFFFFFFFFFFFFULL != m_ullCurrParseFilePos)
	{
		if(pFrame->pFileChunk->FGetFilePos() < m_ullCurrParseFilePos)
		{
			pFrame->pFileChunk->FSkip(pFrame->framesize);
			return VO_FALSE;
		}
		else
			m_ullCurrParseFilePos = 0xFFFFFFFFFFFFFFFFULL;
	}
#ifndef _HARDWARE_DECODER
	VO_PBYTE pBuffer = VO_NULL;
	VO_U32 buffersize = ( pFrame->framesize - 1 + sizeof( rv_frame ) + 0x3 ) & ~0x3;
	VO_U32 pos = m_pGlobeBuffer->GetForDirectWrite( &pBuffer , buffersize );

	VO_U8 segnum = 0;
	pFrame->pFileChunk->FRead( &segnum , sizeof( VO_U8 ) );
	segnum++;

	rv_frame frame;
	MemSet((VO_PTR)&frame, 0x0, sizeof(rv_frame));
	
	frame.bLastPacket = VO_FALSE;
	frame.ulDataLen = (VO_U32)(pFrame->framesize - 1 + sizeof( rv_frame ));
	frame.ulNumSegments = segnum;
	frame.usSequenceNum = 0;

	m_pGlobeBuffer->DirectWrite2( (VO_PBYTE)&frame , pBuffer , 0 , sizeof( rv_frame ) );
	m_pGlobeBuffer->DirectWrite( pFrame->pFileChunk , pBuffer , sizeof( rv_frame ) , (VO_U32)(pFrame->framesize - 1) );
#else
	VO_U32 pos = m_pGlobeBuffer->Add( pFrame->pFileChunk, pFrame->framesize );
	VO_U32 buffersize = pFrame->framesize;
	if(VO_MAXU32 == pos)
		return VO_FALSE;
#endif
	/*MKVMediaSampleIndexEntry* pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | pos) : pos), buffersize );*/
	MKVMediaSampleIndexEntry* pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | pos) : pos), buffersize );
	if(!pNew)
		return VO_FALSE;

	if( m_pTempFrameIndexListHead == NULL && m_pTempFrameIndexListTail == NULL )
	{
		m_pTempFrameIndexListHead = m_pTempFrameIndexListTail = pNew;
	}
	else
	{
		m_pTempFrameIndexListTail->next = pNew;
		pNew->pre = m_pTempFrameIndexListTail;
		m_pTempFrameIndexListTail = pNew;
	}

	if( pNew->cluster_timecode != -1 )
	{
#ifndef _HARDWARE_DECODER
		RerangeFrameTimeCode( pNew );
#endif
		m_FramesAvailable++;
	}

	return VO_TRUE;
}

VO_U32 CMKVVideoTrack_RealSpecialize::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = CBaseStreamFileTrack::GetSample(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;
#ifndef _HARDWARE_DECODER
	rv_frame* pFrame = (rv_frame*)pSample->Buffer;
	pFrame->ulTimestamp = (VO_U32)pSample->Time;
	pFrame->pData = pSample->Buffer + sizeof( rv_frame ) + pFrame->ulNumSegments * sizeof( rv_segment );
	pFrame->pSegment = (rv_segment*)(pSample->Buffer + sizeof( rv_frame ));

	if( pSample->Size & 0x80000000 )
		pFrame->usFlags = 0x80 | 2;
	else
		pFrame->usFlags = 0;
#endif
#ifdef _RAW_DUMP_
	m_Dumper.DumpVideoData(pSample->Buffer, pSample->Size&0x7FFFFFFF);
#endif
	return VO_ERR_SOURCE_OK;
}


VO_U32 CMKVVideoTrack_RealSpecialize::CalculateTimeCode( VO_U32 count )
{
	if( !m_pTempFrameIndexListHead )
		return 0;

	MKVMediaSampleIndexEntry * pEntry = m_pTempFrameIndexListHead;
	VO_U32 calculated = 0;

	MKVMediaSampleIndexEntry * pNext = ( MKVMediaSampleIndexEntry * )pEntry->next;

	while( pEntry )
	{
		VO_U32 offsetcount = 1;
		VO_U32 lasttimecode = pEntry->time_stamp;

		while( pNext && ( pNext->time_stamp == lasttimecode || pNext->time_stamp - lasttimecode == 1 ) && ( calculated + offsetcount <= count ) )
		{
			lasttimecode = pNext->time_stamp;
			pNext = ( MKVMediaSampleIndexEntry * )pNext->next;
			offsetcount++;
		}

		if( calculated + offsetcount > count )
			break;

		VO_U32 average = 0;
		if( pNext )
			average = ( pNext->time_stamp - pEntry->time_stamp ) / offsetcount;

		for( VO_U32 i = 0 ; i < offsetcount ; i++ )
		{
			pEntry->time_stamp = pEntry->time_stamp + average * i;
			pEntry = ( MKVMediaSampleIndexEntry * )pEntry->next;
		}

		calculated = calculated + offsetcount;
		m_BlocksInCache--;

		if( pEntry )
			pNext = ( MKVMediaSampleIndexEntry * )pEntry->next;
	}

	return calculated;
}

VO_VOID CMKVVideoTrack_RealSpecialize::AnalyzeOpaqueHeader( VO_PBYTE pData , VO_U32 size )
{
	if(pData && size >= 4)
	{
		VO_PBYTE pSrc = pData;

		use_big_endian_read

			VO_U32 dwValue;
		read_fcc2(dwValue);

		if(size >= 26)
		{
			pSrc = pData;
			read_dword2(dwValue);

			m_dwInitParamSize = sizeof(VORV_INIT_PARAM) + sizeof(VORV_FORMAT_INFO) + dwValue - 26;
			m_pInitParam = NEW_BUFFER(m_dwInitParamSize);
			MemSet((VO_PTR)m_pInitParam, 0x0, sizeof(m_dwInitParamSize));
			
			if(!m_pInitParam)
				return;

			VORV_INIT_PARAM* pInitParam = (VORV_INIT_PARAM*)m_pInitParam;
			VORV_FORMAT_INFO* pFormat = (VORV_FORMAT_INFO*)(pInitParam + 1);
			pInitParam->format = pFormat;
			pFormat->pOpaqueData = VO_PBYTE(pFormat + 1);

			pFormat->ulLength = dwValue;
			read_dword2(pFormat->ulMOFTag);
			read_dword2(pFormat->ulSubMOFTag);
			read_word2(pFormat->usWidth);
			read_word2(pFormat->usHeight);
			read_word2(pFormat->usBitCount);
			read_word2(pFormat->usPadWidth);
			read_word2(pFormat->usPadHeight);
			read_dword2(pFormat->ufFramesPerSecond);
			if(HX_RVTRVIDEO_ID == pFormat->ulSubMOFTag)
				pFormat->ulSubMOFTag = HX_RV20VIDEO_ID;
			else if(HX_RVTR_RV30_ID == pFormat->ulSubMOFTag)
				pFormat->ulSubMOFTag = HX_RV30VIDEO_ID;

			pFormat->ulOpaqueDataSize = pFormat->ulLength - 26;
			if(size - (pSrc - pData) >= pFormat->ulOpaqueDataSize)
			{
				read_pointer2(pFormat->pOpaqueData, pFormat->ulOpaqueDataSize);
				VO_S32 FrameSize[9] = {0,1,1,2,2,3,3,3,3};
				if( HX_RV30VIDEO_ID == pFormat->ulSubMOFTag )
				{
					VO_S32 numFrameSizes = 1 + ( pFormat->pOpaqueData[1] & 0x7 );
					if(numFrameSizes >= 0 && numFrameSizes < (VO_S32)sizeof(FrameSize))
					{
						m_nMaxEncoderFrameSize = FrameSize[numFrameSizes];
					}
				}
			}

			return;
		}
	}

	return;
}

VO_U32 CMKVVideoTrack_RealSpecialize::GetHeadData(VO_CODECBUFFER* pHeadData)
{
#ifndef _HARDWARE_DECODER
	pHeadData->Length = m_dwInitParamSize - sizeof(VORV_INIT_PARAM);
	pHeadData->Buffer = m_pInitParam + sizeof(VORV_INIT_PARAM);
#else
	pHeadData->Length = m_pTrackInfo->CodecPrivateSize;
	pHeadData->Buffer = (VO_PBYTE)m_pTrackInfo->pCodecPrivate;
#endif
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVVideoTrack_RealSpecialize::GetCodec(VO_U32* pCodec)
{
	*pCodec = VO_VIDEO_CodingRV;
	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVVideoTrack_RealSpecialize::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	VO_U32 result = VO_ERR_NONE;

	switch (uID)
	{
	case VO_PID_VIDEO_MAXENCODERFRAMESIZE:
		{
			*((int*)pParam) = m_nMaxEncoderFrameSize;
			result = VO_ERR_NONE;
		}
		break;
	default:
		result = CMKVTrack::GetParameter(uID, pParam);
		break;
	}

	return result;
}

VO_U32 CMKVVideoTrack_RealSpecialize::GetCodecCC(VO_U32* pCC)
{
	if(!m_pInitParam){
		return VO_ERR_OUTOF_MEMORY;
	}
	VORV_INIT_PARAM* pInitParam = (VORV_INIT_PARAM*)m_pInitParam;
	VORV_FORMAT_INFO* pFormat = (VORV_FORMAT_INFO*)(pInitParam + 1);		
	*pCC = pFormat->ulSubMOFTag;

	return VO_ERR_SOURCE_OK;
	
}

VO_U32 CMKVVideoTrack_RealSpecialize::GetThumbNailInfo(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	VOLOGI("GetThumbNailInfo----start");

	if(!pThumbNailInfo){
		return VO_ERR_SOURCE_ERRORDATA;
	}

	CMKVStreamFileIndex *pMKVFileIndex = (CMKVStreamFileIndex *)m_pStreamFileIndex;
	CMKVDataParser* pDataParser = (CMKVDataParser *)m_pStreamFileReader->GetFileDataParserPtr();

	if (!IsListEmpty( &(m_pSegInfo->CuesCuePointEntryListHeader) ))
	{
		VO_U64 nPosNextKeyFrame = 0;
		nPosNextKeyFrame = pDataParser->GetPositionWithCuesInfoByCount(m_pSegInfo);

		if(0xFFFFFFFFFFFFFFFFULL == nPosNextKeyFrame){
			return VO_ERR_SOURCE_END;
		}

		if (nPosNextKeyFrame > pDataParser->GetCurrParseFilePos())
		{
			pDataParser->Reset();
		}
	//	VOLOGI("SetStartFilePos == %d",voOS_GetSysTime());
		pDataParser->SetStartFilePos(nPosNextKeyFrame);
	//	VOLOGI("SetStartFilePos1 == %d",voOS_GetSysTime());
	}

	TrackGenerateIndex();

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL bFrameDropped = VO_FALSE;
	if(!m_pStreamFileIndex->GetEntry((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, pThumbNailInfo->ullTimeStamp, &pGet, &bFrameDropped))
		return VO_ERR_SOURCE_END;

	if(pGet->IsKeyFrame())
	{
		pThumbNailInfo->uSampleSize = pGet->size;
		pThumbNailInfo->uSampleSize |= 0x80000000;
		pThumbNailInfo->ullFilePos = pGet->pos_in_buffer;
		pThumbNailInfo->ullTimeStamp = pGet->time_stamp;
	}else{
		return VO_ERR_SOURCE_ERRORDATA;
	}

	m_pStreamFileIndex->RemoveInclude(pGet);
	VOLOGI("GetThumbNailInfo----end---uSampleSize=%lu, ullFilePos=%llu, ullTimeStamp=%lld", pThumbNailInfo->uSampleSize&0x7fffffff, pThumbNailInfo->ullFilePos&0x7fffffff, pThumbNailInfo->ullTimeStamp);
	return VO_ERR_SOURCE_OK;
}


VO_U32 CMKVVideoTrack_RealSpecialize::GetThumbNailBuffer(VO_SOURCE_THUMBNAILINFO* pThumbNailInfo)
{
	VOLOGI("GetThumbNailBuffer----start");

	VO_U32 nResult = VO_ERR_SOURCE_ERRORDATA;

	if(!pThumbNailInfo){
		return nResult;
	}

	if(!m_pThumbnailBuffer && !m_ThumbnailBufferSize){
		VO_U32 size = 0;
		GetMaxSampleSize( &size );		
		m_pThumbnailBuffer = new VO_BYTE[size];
		m_ThumbnailBufferSize = size;
	}

	if(2 == m_pGlobeBuffer->Peek(&pThumbNailInfo->pSampleData, m_pThumbnailBuffer, pThumbNailInfo->ullFilePos & 0x7FFFFFFF, pThumbNailInfo->uSampleSize & 0x7FFFFFFF))
		pThumbNailInfo->pSampleData = m_pSampleData;
	pThumbNailInfo->uSampleSize &=0x7fffffff;

#ifndef _HARDWARE_DECODER
		rv_frame* pFrame = (rv_frame*)pThumbNailInfo->pSampleData;
		pFrame->ulTimestamp = (VO_U32)pThumbNailInfo->ullTimeStamp;
		pFrame->pData = pThumbNailInfo->pSampleData + sizeof( rv_frame ) + pFrame->ulNumSegments * sizeof( rv_segment );
		pFrame->pSegment = (rv_segment*)(pThumbNailInfo->pSampleData + sizeof( rv_frame ));
	
		if( pThumbNailInfo->uSampleSize & 0x80000000 )
			pFrame->usFlags = 0x80 | 2;
		else
			pFrame->usFlags = 0;
#endif
	nResult = VO_ERR_SOURCE_OK;

	VOLOGI("GetThumbNailBuffer----end");

	return nResult;
}

