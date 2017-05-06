#include "CMKVAudioTrack_RealSpecialize.h"
#include "fCC.h"

CMKVAudioTrack_RealSpecialize::CMKVAudioTrack_RealSpecialize( VO_U8 btStreamNum, VO_U32 dwDuration , TracksTrackEntryStruct * pAudioInfo , VO_MEM_OPERATOR* pMemOp )
:CMKVAudioTrack( btStreamNum , dwDuration , pAudioInfo , pMemOp )
,m_interleave_factor(1)
,m_interleave_block_size(0)
,m_interleave_frame_size(0)
,m_interleave_frames_perblock(0)
,m_interleave_pattern_size(0)
,m_is_interleave(0)
,m_has_interleave_pattern(0)
,mp_interleave_pattern(VO_NULL)
,m_current_superblocksize(0)
,m_pBlockBackUp(0)
,m_isseeking_nokeyframe( VO_FALSE )
,m_ppSuperBlockBlockDataInfo(VO_NULL)
,m_current_blockdata_index(0)
, m_pInitParam(VO_NULL)
, m_dwInitParamSize(0)
,m_superblocksincache(0)
{
	AnalyzeOpaqueHeader( (VO_PBYTE)pAudioInfo->pCodecPrivate , pAudioInfo->CodecPrivateSize );

	if( m_is_interleave )
	{
		m_pBlockBackUp = (VO_PBYTE)MemAlloc( m_interleave_block_size );
		m_ppSuperBlockBlockDataInfo = (SuperBlockBlockDataInfo *)MemAlloc( m_interleave_factor * sizeof( SuperBlockBlockDataInfo ) );
	}
}

CMKVAudioTrack_RealSpecialize::~CMKVAudioTrack_RealSpecialize()
{
	if( mp_interleave_pattern )
		MemFree( mp_interleave_pattern );
	
	if( m_pBlockBackUp )
		MemFree( m_pBlockBackUp );

	if( m_ppSuperBlockBlockDataInfo )
		MemFree( m_ppSuperBlockBlockDataInfo );

	if(m_pInitParam)
		MemFree( m_pInitParam );
}

VO_U32 CMKVAudioTrack_RealSpecialize::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Length = m_dwInitParamSize;
	pHeadData->Buffer = m_pInitParam;
	return VO_ERR_SOURCE_OK;
}

//i assume that if real opaque header find in mkv ,it must be single stream
VO_VOID CMKVAudioTrack_RealSpecialize::AnalyzeOpaqueHeader( VO_PBYTE pData , VO_U32 size )
{
	if( pData == NULL )
		return;

	use_big_endian_read;
	VO_PBYTE pSrc = pData;

	VO_U32  singleormulti;

	read_dword2( singleormulti );

	if( singleormulti == 0x4D4C5449 )
		return;

	VO_U16 version;

	read_word2( version );

	switch ( version )
	{
	case 3:
		break;
	case 4:
		AnalyzeOpaqueHeader_Version4( pSrc , size - 6 );
		break;
	case 5:
		AnalyzeOpaqueHeader_Version5( pSrc , size - 6 );
		break;
	}
}

VO_VOID CMKVAudioTrack_RealSpecialize::AnalyzeOpaqueHeader_Version4( VO_PBYTE pData , VO_U32 size )
{
	if(pData && size)
	{
		VO_PBYTE pSrc = pData;

		use_big_endian_read

			VO_U8 btValue;
		VO_U16 wValue;
		/* Skip first 10 bytes */
		skip2(10);
		/* Sanity check the version and revision */
		read_word2(wValue);
		if(4 != wValue)
			return;

		read_word2(wValue);
		if(0 != wValue)
			return;

		SAFE_MEM_FREE(m_pInitParam);

		m_dwInitParamSize = sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO);
		m_pInitParam = NEW_BUFFER(m_dwInitParamSize);
		if(!m_pInitParam)
			return;

		MemSet(m_pInitParam, 0, m_dwInitParamSize);

		VORA_INIT_PARAM* pInitParam = (VORA_INIT_PARAM*)m_pInitParam;
		VORA_FORMAT_INFO* pFormat = (VORA_FORMAT_INFO*)(pInitParam + 1);
		pInitParam->format = pFormat;
		pFormat->usAudioQuality = 100;

		//skip usHeaderBytes
		skip2(2);
		read_word2(pFormat->usFlavorIndex);
		read_dword2(pFormat->ulGranularity);
		//skip ulTotalBytes
		skip2(4);
		VO_U32 Bitrate;
		read_dword2(Bitrate);
		Bitrate /= 60;
		//skip ulBytesPerMin2
		skip2(4);
		read_word2(m_interleave_factor);
		read_word2(m_interleave_block_size);
		//skip ulUserData
		skip2(4);
		read_dword2(pFormat->ulSampleRate);
		pFormat->ulSampleRate >>= 16;
		read_word2(pFormat->usBitsPerSample);
		read_word2(pFormat->usNumChannels);

		read_byte2(btValue);
		if(btValue == 4)	//skip ulInterleaverID
			skip2(4);

		read_byte2(btValue);
		if(btValue == 4)
			read_dword2(pInitParam->ulFourCC);

		//skip bIsInterleaved, bCopyByte, ucScatterType
		skip2(3);

		/* If the interleave factor is 0, make it 1. */
		if(!m_interleave_factor)
			m_interleave_factor = 1;
	}
}

VO_VOID CMKVAudioTrack_RealSpecialize::AnalyzeOpaqueHeader_Version5( VO_PBYTE pData , VO_U32 size )
{
	if(pData && (size >= 68) )
	{
		VO_PBYTE pSrc = pData;

		use_big_endian_read

		VO_U16 wValue;
		/* Skip first 10 bytes */
		skip2(10);
		/* Sanity check the version and revision */
		read_word2(wValue);
		if(5 != wValue)
			return;

		read_word2(wValue);
		if(0 != wValue)
			return;

		SAFE_MEM_FREE(m_pInitParam);

		m_dwInitParamSize = sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO);
		m_pInitParam = NEW_BUFFER(m_dwInitParamSize);
		if(!m_pInitParam)
			return;

		MemSet(m_pInitParam, 0, m_dwInitParamSize);

		VORA_INIT_PARAM* pInitParam = (VORA_INIT_PARAM*)m_pInitParam;
		VORA_FORMAT_INFO* pFormat = (VORA_FORMAT_INFO*)(pInitParam + 1);
		pInitParam->format = pFormat;
		pFormat->usAudioQuality = 100;

		//skip usHeaderBytes
		skip2(2);
		read_word2(pFormat->usFlavorIndex);
		read_dword2(pFormat->ulGranularity);
		//skip ulTotalBytes
		VO_U32 bitrate;
		skip2(4);
		read_dword2(bitrate);
		bitrate /= 60;
		//skip ulBytesPerMin2
		skip2(4);
		read_word2(m_interleave_factor);
		read_word2(m_interleave_block_size);
		read_word2(pFormat->ulBitsPerFrame);
		//skip ulUserData
		skip2(4);
		read_dword2(pFormat->ulSampleRate);
		pFormat->ulSampleRate >>= 16;
		read_dword2(pFormat->ulActualRate);
		pFormat->ulActualRate >>= 16;
		read_word2(pFormat->usBitsPerSample);
		read_word2(pFormat->usNumChannels);
		//skip ulInterleaverID
		skip2(4);
		read_dword2(pInitParam->ulFourCC);

		read_byte2( m_is_interleave );
		skip2(2);
		read_byte2( m_has_interleave_pattern );

		m_interleave_frame_size = (VO_U16)pFormat->ulBitsPerFrame;
		m_interleave_frames_perblock = m_interleave_block_size / m_interleave_frame_size;
		m_interleave_pattern_size = m_interleave_factor * m_interleave_frames_perblock;

		mp_interleave_pattern = (VO_U16*)MemAlloc( m_interleave_pattern_size * sizeof( VO_U16 ) );

		if( m_has_interleave_pattern != 0 )
		{

			for( VO_U32 i = 0 ; i < m_interleave_pattern_size ; i++ )
			{
				VO_U16 value;
				read_word2( value );
				mp_interleave_pattern[value] = (VO_U16)i;
			}
		}
		else
		{
			VO_U32 i = 0, b = 0, f = 0;
			VO_BOOL bEven = VO_TRUE;
			while(i < m_interleave_pattern_size)
			{
				mp_interleave_pattern[b * m_interleave_frames_perblock + f] = (VO_U16)i;
				i++;
				b += 2;
				if(b >= m_interleave_factor)
				{
					if(bEven)
					{
						bEven = VO_FALSE;
						b = 1;
					}
					else
					{
						bEven = VO_TRUE;
						b = 0;
						f++;
					}
				}
			}
		}

		{
			/* Make sure we have four bytes */
			if( size - (pSrc - pData) >= 4)
			{
				read_dword2(pFormat->ulOpaqueDataSize);
				if( size - (pSrc - pData) >= pFormat->ulOpaqueDataSize)
				{
					//re-alloc buffer
					VO_U32 dwNewInitParamSize = sizeof(VORA_INIT_PARAM) + sizeof(VORA_FORMAT_INFO) + pFormat->ulOpaqueDataSize;
					VO_PBYTE pNewInitParam = NEW_BUFFER(dwNewInitParamSize);
					if(!pNewInitParam)
						return;

					MemCopy(pNewInitParam, m_pInitParam, m_dwInitParamSize);

					SAFE_MEM_FREE(m_pInitParam);

					m_pInitParam = pNewInitParam;
					m_dwInitParamSize = dwNewInitParamSize;

					pInitParam = (VORA_INIT_PARAM*)m_pInitParam;
					pFormat = (VORA_FORMAT_INFO*)(pInitParam + 1);
					pInitParam->format = pFormat;
					pFormat->pOpaqueData = VO_PBYTE(pFormat + 1);

					read_pointer2(pFormat->pOpaqueData, pFormat->ulOpaqueDataSize);

					/* If the interleave factor is 0, make it 1. */
					if(!m_interleave_factor)
						m_interleave_factor = 1;
				}
			}
		}
	}
}
#if 0
VO_BOOL CMKVAudioTrack_RealSpecialize::NewFrameArrived( MKVFrame * pFrame )
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

	if( m_is_interleave )
	{
		if( m_isseeking_nokeyframe )
		{
			return AddNewSeekInterleaveNoKeyFrame( pFrame );
		}
		else
		{
			return AddNewInterleaveFrame( pFrame );
		}
	}
	else
	{
		//we should add va_block header before the raw data
		VO_PBYTE pBuffer = VO_NULL;
		VO_U32 pos = m_pGlobeBuffer->GetForDirectWrite( &pBuffer , sizeof(ra_block) + m_interleave_block_size );

		ra_block block;
		block.pData = pBuffer + sizeof(ra_block);
		block.ulDataLen = m_interleave_block_size;
		m_pGlobeBuffer->DirectWrite2( (VO_PBYTE)&block , pBuffer , 0 , sizeof( ra_block ) );
		m_pGlobeBuffer->DirectWrite( pFrame->pFileChunk , pBuffer , sizeof(ra_block) , (VO_U32)pFrame->framesize );

		//MKVMediaSampleIndexEntry* pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | pos) : pos), pFrame->framesize + sizeof(ra_block));
		MKVMediaSampleIndexEntry* pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | pos) : pos), (VO_U32)(pFrame->framesize + sizeof(ra_block)));
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
			m_FramesAvailable++;
		}
	}

	return VO_TRUE;
}

VO_BOOL CMKVAudioTrack_RealSpecialize::AddNewInterleaveFrame( MKVFrame * pFrame )
{
	if( m_current_superblocksize == 0 || m_current_superblocksize >= (VO_U32)(m_interleave_block_size * m_interleave_factor) )
	{
		m_superblocksincache++;

		m_current_superblocksize = 0;
		MemSet( m_ppSuperBlockBlockDataInfo , 0 , m_interleave_factor * sizeof( SuperBlockBlockDataInfo ) );
		m_current_blockdata_index = 0;

		VO_U32 blocksize = ( m_interleave_block_size + sizeof( ra_block ) + 0x3 ) & ~0x3;

		for( VO_U32 i = 0 ; i < m_interleave_factor ; i++ )
		{
			m_ppSuperBlockBlockDataInfo[i].pos = m_pGlobeBuffer->GetForDirectWrite( &( m_ppSuperBlockBlockDataInfo[i].pData ) , blocksize );
		}
		m_ppSuperBlockBlockDataInfo[0].pos |= 0x80000000;
	}

	/*MKVMediaSampleIndexEntry * pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , 
		m_ppSuperBlockBlockDataInfo[ m_current_blockdata_index ].pos , 
		sizeof( ra_block ) + pFrame->framesize );*/
	MKVMediaSampleIndexEntry * pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , 
		m_ppSuperBlockBlockDataInfo[ m_current_blockdata_index ].pos , 
		sizeof( ra_block ) + (VO_U32)(pFrame->framesize) );

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
		m_FramesAvailable++;
	}

	DeInterLeaveBlockData( pFrame );

	ra_block blockheader;
	blockheader.pData = m_ppSuperBlockBlockDataInfo[m_current_blockdata_index].pData + sizeof(ra_block);
	blockheader.ulDataLen = m_interleave_block_size;
	blockheader.ulDataFlags = 0xFFFFFFFF;

	m_pGlobeBuffer->DirectWrite2( (VO_PBYTE)&blockheader , m_ppSuperBlockBlockDataInfo[m_current_blockdata_index].pData , 0 , sizeof(ra_block) );

	m_current_blockdata_index++;
	m_current_superblocksize += m_interleave_block_size;

	return VO_TRUE;
}

VO_BOOL CMKVAudioTrack_RealSpecialize::AddNewSeekInterleaveNoKeyFrame( MKVFrame * pFrame )
{
	if( m_pTempFrameIndexListTail && m_pTempFrameIndexListTail->IsKeyFrame() )
	{
		m_current_superblocksize = 0;
		MemSet( m_ppSuperBlockBlockDataInfo , 0 , m_interleave_factor * sizeof( SuperBlockBlockDataInfo ) );
		m_current_blockdata_index = 0;

		VO_U32 blocksize = ( m_interleave_block_size + sizeof( ra_block ) + 0x3 ) & ~0x3;

		for( VO_U32 i = 0 ; i < m_interleave_factor ; i++ )
		{
			m_ppSuperBlockBlockDataInfo[i].pos = m_pGlobeBuffer->GetForDirectWrite( &( m_ppSuperBlockBlockDataInfo[i].pData ) , blocksize );
		}
		m_ppSuperBlockBlockDataInfo[0].pos |= 0x80000000;

		/*
		MKVMediaSampleIndexEntry * pNew = new MKVMediaSampleIndexEntry( m_pTempFrameIndexListTail->cluster_timecode , 
					m_pTempFrameIndexListTail->relative_timecode , 
					m_ppSuperBlockBlockDataInfo[ m_current_blockdata_index ].pos , 
					sizeof( ra_block ) + pFrame->framesize );*/
		MKVMediaSampleIndexEntry * pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( m_pTempFrameIndexListTail->cluster_timecode , 
			m_pTempFrameIndexListTail->relative_timecode , 
			m_ppSuperBlockBlockDataInfo[ m_current_blockdata_index ].pos , 
			sizeof( ra_block ) + (VO_U32)(pFrame->framesize) );
		

		MKVMediaSampleIndexEntry * pEntry = m_pTempFrameIndexListHead;

		while( pEntry )
		{
			MKVMediaSampleIndexEntry * pTemp = pEntry;
			pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;

			((CMKVStreamFileIndex*)m_pStreamFileIndex)->DeleteEntry( pTemp );
		}

		m_pTempFrameIndexListHead = m_pTempFrameIndexListTail = pNew;

		if( pNew->cluster_timecode != -1 )
		{
			m_FramesAvailable++;
		}

		DeInterLeaveBlockDataFromBuffer( m_pBlockBackUp );

		ra_block blockheader;
		blockheader.pData = m_ppSuperBlockBlockDataInfo[m_current_blockdata_index].pData + sizeof(ra_block);
		blockheader.ulDataLen = m_interleave_block_size;
		blockheader.ulDataFlags = 0xFFFFFFFF;

		m_pGlobeBuffer->DirectWrite2( (VO_PBYTE)&blockheader , m_ppSuperBlockBlockDataInfo[m_current_blockdata_index].pData , 0 , sizeof(ra_block) );

		m_current_blockdata_index++;
		m_current_superblocksize += m_interleave_block_size;
		m_isseeking_nokeyframe = VO_FALSE;

		return AddNewInterleaveFrame( pFrame );

	}
	else
	{
		/*MKVMediaSampleIndexEntry* pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | 0) : 0), pFrame->framesize + sizeof(ra_block));*/
		MKVMediaSampleIndexEntry* pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | 0) : 0), (VO_U32)pFrame->framesize + sizeof(ra_block));
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
		if (!m_pBlockBackUp || pFrame->framesize > m_interleave_block_size)
		{
			pFrame->pFileChunk->FSkip(pFrame->framesize);
		}
		else
		{
			pFrame->pFileChunk->FRead(m_pBlockBackUp,(VO_U32)pFrame->framesize);
		//	m_pGlobeBuffer->DirectWrite( pFrame->pFileChunk , m_pBlockBackUp , 0 , pFrame->framesize );
		}
		
	}

	return VO_TRUE;
}

VO_VOID CMKVAudioTrack_RealSpecialize::DeInterLeaveBlockData( MKVFrame * pFrame )
{
	VO_U16 wFrameIndex = (VO_U16)(m_current_blockdata_index * m_interleave_frames_perblock);	//frame index
	for(VO_U16 i = 0; i < m_interleave_frames_perblock; i++)
	{
		VO_U16 wDstFrameIndex = mp_interleave_pattern[wFrameIndex + i];
		VO_U16 wDstBlockIndex = VO_U16(wDstFrameIndex / m_interleave_frames_perblock);
		wDstFrameIndex = wDstFrameIndex % m_interleave_frames_perblock;

		m_pGlobeBuffer->DirectWrite(pFrame->pFileChunk, m_ppSuperBlockBlockDataInfo[wDstBlockIndex].pData, 
			sizeof(ra_block) + wDstFrameIndex * m_interleave_frame_size, 
			m_interleave_frame_size);
	}
}

VO_VOID CMKVAudioTrack_RealSpecialize::DeInterLeaveBlockDataFromBuffer( VO_PBYTE pData )
{
	VO_U16 wFrameIndex = VO_U16(m_current_blockdata_index * m_interleave_frames_perblock);	//frame index
	for(VO_U16 i = 0; i < m_interleave_frames_perblock; i++)
	{
		VO_U16 wDstFrameIndex = mp_interleave_pattern[wFrameIndex + i];
		VO_U16 wDstBlockIndex = VO_U16(wDstFrameIndex / m_interleave_frames_perblock);
		wDstFrameIndex = wDstFrameIndex % m_interleave_frames_perblock;

		m_pGlobeBuffer->DirectWrite2(pData + i * m_interleave_frame_size, m_ppSuperBlockBlockDataInfo[wDstBlockIndex].pData, 
			sizeof(ra_block) + wDstFrameIndex * m_interleave_frame_size, 
			m_interleave_frame_size);
	}
}

VO_VOID CMKVAudioTrack_RealSpecialize::NewBlockGroupStarted()
{
	m_pTempFrameLastGroupPointer = m_pTempFrameIndexListTail;
}

VO_VOID CMKVAudioTrack_RealSpecialize::NewReferenceBlockArrived()
{
	if( !m_pTempFrameLastGroupPointer )
	{
		MKVMediaSampleIndexEntry *pEntry = m_pTempFrameIndexListHead; 
		while( pEntry )
		{
			pEntry->SetKeyFrame( VO_FALSE );
			pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;
		}
		return;
	}

	m_pTempFrameLastGroupPointer = (MKVMediaSampleIndexEntry *)m_pTempFrameLastGroupPointer->next;

	while( m_pTempFrameLastGroupPointer )
	{
		m_pTempFrameLastGroupPointer->SetKeyFrame( VO_FALSE );

		m_pTempFrameLastGroupPointer = (MKVMediaSampleIndexEntry *)m_pTempFrameLastGroupPointer->next;
	}
}
#endif

VO_BOOL CMKVAudioTrack_RealSpecialize::SampleNotify(VO_PTR pUser, VO_PTR wParam , VO_PTR lParam )
{
/*
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

	if( m_is_interleave )
	{
		if( m_isseeking_nokeyframe )
		{
			return AddNewSeekInterleaveNoKeyFrame( pFrame );
		}
		else
		{
			return AddNewInterleaveFrame( pFrame );
		}
	}
	else
	{
		//we should add va_block header before the raw data
		VO_PBYTE pBuffer = VO_NULL;
		VO_U32 pos = m_pGlobeBuffer->GetForDirectWrite( &pBuffer , sizeof(ra_block) + m_interleave_block_size );

		ra_block block;
		block.pData = pBuffer + sizeof(ra_block);
		block.ulDataLen = m_interleave_block_size;
		m_pGlobeBuffer->DirectWrite2( (VO_PBYTE)&block , pBuffer , 0 , sizeof( ra_block ) );
		m_pGlobeBuffer->DirectWrite( pFrame->pFileChunk , pBuffer , sizeof(ra_block) , (VO_U32)pFrame->framesize );

		//MKVMediaSampleIndexEntry* pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | pos) : pos), pFrame->framesize + sizeof(ra_block));
		MKVMediaSampleIndexEntry* pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | pos) : pos), (VO_U32)(pFrame->framesize + sizeof(ra_block)));
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
			m_FramesAvailable++;
		}
	}
*/
	return VO_TRUE;
}


VO_U32 CMKVAudioTrack_RealSpecialize::GetSample(VO_SOURCE_SAMPLE* pSample)
{
	VO_U32 rc = CBaseTrack::GetSample(pSample);
	if(VO_ERR_SOURCE_OK != rc)
		return rc;

	ra_block* pBlock = (ra_block*)pSample->Buffer;
	pBlock->ulTimestamp = (VO_U32)pSample->Time;
	pBlock->pData = pSample->Buffer + sizeof(ra_block);
	pBlock->ulDataFlags = 0xffffffff;

	if( pSample->Size & 0x80000000 )
		m_superblocksincache--;

	return VO_ERR_SOURCE_OK;
}
/*
VO_VOID CMKVAudioTrack_RealSpecialize::CheckCacheAndTransferTempData()
{
	if( m_is_interleave )
	{
		int number_to_transfer = 0;

		if( m_IsFileReadEnd == VO_TRUE )
		{
			number_to_transfer = CalculateTimeCode( 0xffffffff );
		}
		else if( m_FramesAvailable >= 20 && m_superblocksincache >= 3 )
		{
			number_to_transfer = CalculateTimeCode( m_FramesAvailable / 2 );
		}

		TransferTempData( number_to_transfer );
	}
	else
	{
		CMKVTrack::CheckCacheAndTransferTempData();
	}
}
*/
VO_U32 CMKVAudioTrack_RealSpecialize::GetCodecCC(VO_U32* pCC)
{
	STRCODECID2FOURCC("A_REAL/COOK", AudioFlag_RA_G2);
	STRCODECID2FOURCC("A_REAL/RAAC", AudioFlag_RA_AAC);
	STRCODECID2FOURCC("A_REAL/RACP", AudioFlag_RA_AAC_PLUS);

	return VO_ERR_SOURCE_OK;
}
/*
VO_U32 CMKVAudioTrack_RealSpecialize::SetPosN(VO_S64* pPos)
{
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime(VO_TRUE, *pPos, &pGet);
	if(-1 == nRes)
		return VO_ERR_SOURCE_END;
	else if(1 == nRes)	//retry
	{
		VO_BOOL bParseEnd = VO_FALSE;
		while(1 == nRes && !bParseEnd)	//retry!!
		{
			if( m_pGlobeBuffer )
				m_pGlobeBuffer->RemoveTo( (pGet->pos_in_buffer & 0x7fffffff) + pGet->size );
			m_pStreamFileIndex->RemoveAll();

			if(!TrackGenerateIndex())
				bParseEnd = VO_TRUE;

			nRes = m_pStreamFileIndex->GetEntryByTime(VO_TRUE, *pPos, &pGet);
			if(-1 == nRes)
				return VO_ERR_SOURCE_END;
		}

		if(0 != nRes)
			return VO_ERR_SOURCE_END;

		*pPos = pGet->time_stamp;
		m_pStreamFileIndex->RemoveUntil(pGet);
		TrackGenerateIndex();
	}
	else	//ok
	{
		*pPos = pGet->time_stamp;
		m_pStreamFileIndex->RemoveUntil(pGet);
//		m_pStreamFileReader->FileGenerateIndex();
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVAudioTrack_RealSpecialize::SetPosK(VO_S64* pPos)
{
	m_isseeking_nokeyframe = VO_TRUE;
	return CBaseStreamFileTrack::SetPosK( pPos );
}

VO_U32 CMKVAudioTrack_RealSpecialize::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL droped;
	if(!m_pStreamFileIndex->GetEntry(VO_TRUE, pSample->Time, &pGet , &droped))
		return VO_ERR_SOURCE_END;

	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

	VO_U32 dwMaxSampleSize = VO_MAXU32;
	GetMaxSampleSize(&dwMaxSampleSize);
	if(pGet->size > dwMaxSampleSize)
	{
		pSample->Size = 0;
		return VO_ERR_SOURCE_OK;
	}

	pSample->Size = pGet->size;
	if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size))
		pSample->Buffer = m_pSampleData;

	if(pGet->IsKeyFrame())
		pSample->Size |= 0x80000000;

	m_pStreamFileIndex->RemoveInclude(pGet);

	TrackGenerateIndex();

	return VO_ERR_SOURCE_OK;
}
*/
VO_VOID CMKVAudioTrack_RealSpecialize::Flush()
{
	m_superblocksincache = 0;
	CMKVAudioTrack::Flush();
}

