#include "CMKVAudioTrack.h"
#include "fCC.h"

#ifdef _VONAMESPACE
	using namespace _VONAMESPACE;
#endif

CMKVAudioTrack::CMKVAudioTrack( VO_U8 btStreamNum, VO_U32 dwDuration , TracksTrackEntryStruct * pAudioInfo , CMKVFileReader * pReader, VO_MEM_OPERATOR* pMemOp )
:CMKVTrack( VOTT_AUDIO , btStreamNum , dwDuration , pAudioInfo , pReader , pMemOp )
,m_lastrelativetimecode(-1)
,m_ptr_privatedata(0)
,m_privatedata_size(0)
,m_pWaveFormatEx(NULL)
{
	;
}

CMKVAudioTrack::~CMKVAudioTrack()
{
	if( m_ptr_privatedata ){
		delete []m_ptr_privatedata;
		m_ptr_privatedata = NULL;
	}

	if(m_pWaveFormatEx){
		delete []m_pWaveFormatEx;
		m_pWaveFormatEx = NULL;
	}

	m_ptr_privatedata = 0;
	m_privatedata_size = 0;
}

VO_U32 CMKVAudioTrack::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	if( strcmp( (VO_CHAR*)m_pTrackInfo->str_CodecID , "A_VORBIS" ) == 0 )
	{
		if( !m_ptr_privatedata )
			perpare_ogg_headerdata();

		pHeadData->Length = m_privatedata_size;
		pHeadData->Buffer = m_ptr_privatedata;
	}
	else
	{
		pHeadData->Length = m_pTrackInfo->CodecPrivateSize;
		pHeadData->Buffer = (VO_PBYTE)m_pTrackInfo->pCodecPrivate;
	}

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVAudioTrack::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	*pdwMaxSampleSize = (VO_U32)VO_MAX( m_pTrackInfo->SampleFreq * m_pTrackInfo->Channels * 2, 2048 );

	return VO_ERR_SOURCE_OK;
}

VO_U32 CMKVAudioTrack::GetCodecCC(VO_U32* pCC)
{
	// http://wiki.multimedia.cx/index.php?title=Matroska
	/*
	A_AAC: Advanced Audio Coding
	A_AAC/MPEGx/y: Obsolete; Use A_AAC instead
	A_AC3: ATSC A/52a
	A_EAC3: ATSC A/52b [4]
	A_DTS: DTS Coherent Acoustics
	A_FLAC: FLAC
	A_MPC: Musepack SV8
	A_MPEG/L1: MP1
	A_MPEG/L2: MP2
	A_MPEG/L3: MP3
	A_PCM/INT/BIG: Uncompressed PCM, Big-Endian
	A_PCM/INT/LIT: Uncompressed PCM, Little-Endian
	A_PCM/FLOAT/IEEE: Uncompressed PCM, IEEE floating-point
	A_REAL/14_4: VSELP
	A_REAL/28_8: RealAudio 28.8
	A_REAL/ATRC: ATRAC3, using Real extradata and xor format (RealAudio atrc)
	A_REAL/COOK: RealAudio cook
	A_REAL/SIPR: RealAudio sipr
	A_TTA1: True Audio lossless
	A_VORBIS: Vorbis
	A_WAVPACK4: WavPack
	A_QUICKTIME: Quicktime compatibility (uncommon)
	A_MS/ACM: Windows ACM compatibility (uncommon)
	A_SPEEX: Speex [5]
	*/

	STRCODECID2FOURCC("A_MS/ACM", ((VO_WAVEFORMATEX *)m_pTrackInfo->pCodecPrivate)->wFormatTag);
	STRCODECID2FOURCC("A_AAC", AudioFlag_ISOMPEG4_AAC);
	STRCODECID2FOURCC("A_AC3", AudioFlag_AC3);
	STRCODECID2FOURCC("A_EAC3", AudioFlag_EAC3);
	STRCODECID2FOURCC("A_DTS", AudioFlag_DTS);
	STRCODECID2FOURCC("A_FLAC", AudioFlag_FLAC);
	STRCODECID2FOURCC("A_FLAC", AudioFlag_FLAC);
	STRCODECID2FOURCC("A_MPEG/L3", AudioFlag_MP3);
	STRCODECID2FOURCC("A_MPEG/L", AudioFlag_MP1);
	STRCODECID2FOURCC("A_PCM/", AudioFlag_MS_PCM);
	STRCODECID2FOURCC("A_REAL/COOK", AudioFlag_RA_G2);
	STRCODECID2FOURCC("A_REAL/RAAC", AudioFlag_RA_AAC);
	STRCODECID2FOURCC("A_REAL/RACP", AudioFlag_RA_AAC_PLUS);
	STRCODECID2FOURCC("A_VORBIS", AudioFlag_OGG_1);

	return VO_ERR_SOURCE_FORMATUNSUPPORT;
}

VO_BOOL CMKVAudioTrack::NewFrameArrived( MKVFrame * pFrame )
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

//	VO_S64 timecode = 0; 

	VO_U32 dwPosInBuffer = 0;

	if( m_pTrackInfo->ptr_ContentCompSettings )
	{
		VO_PBYTE buffer;
		dwPosInBuffer = m_pGlobeBuffer->GetForDirectWrite( &buffer , m_pTrackInfo->ContentCompSettingsSize );

		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		m_pGlobeBuffer->DirectWrite2( m_pTrackInfo->ptr_ContentCompSettings , buffer , 0 , m_pTrackInfo->ContentCompSettingsSize );

		m_pGlobeBuffer->Add( pFrame->pFileChunk, (VO_U32)pFrame->framesize );

		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;

		pFrame->framesize = pFrame->framesize + m_pTrackInfo->ContentCompSettingsSize;
	}
	else
	{
		dwPosInBuffer = m_pGlobeBuffer->Add( pFrame->pFileChunk, (VO_U32)pFrame->framesize );

		if(VO_MAXU32 == dwPosInBuffer)
			return VO_FALSE;
	}

	//MKVMediaSampleIndexEntry* pNew = new MKVMediaSampleIndexEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), pFrame->framesize);
	MKVMediaSampleIndexEntry* pNew = ((CMKVStreamFileIndex*)m_pStreamFileIndex)->NewEntry( pFrame->TimeCode , pFrame->RelativeTimeCode , (pFrame->bIsKeyFrame ? (0x80000000 | dwPosInBuffer) : dwPosInBuffer), (VO_U32)pFrame->framesize);
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

	return VO_TRUE;
}

VO_VOID CMKVAudioTrack::NewClusterStarted()
{
	;
}

VO_VOID CMKVAudioTrack::NewBlockStarted()
{
	m_BlocksInCache++;
}

VO_VOID CMKVAudioTrack::NewBlockGroupStarted()
{
	;
}

VO_VOID CMKVAudioTrack::NewReferenceBlockArrived()
{
	;
}

VO_VOID CMKVAudioTrack::FileReadEnd()
{
	if( m_pTempFrameIndexListTail )
		m_pTempFrameIndexListTail->time_stamp = (VO_U32)(m_pSegInfo->Duration * m_pSegInfo->TimecodeScale / 1000000.);
	else
	{
		PBaseStreamMediaSampleIndexEntry FileIndex = NULL;
		if(m_pStreamFileIndex)
		{
			m_pStreamFileIndex->GetTail(&FileIndex);
			if(FileIndex)
				FileIndex->time_stamp = (VO_U32)(m_pSegInfo->Duration * m_pSegInfo->TimecodeScale / 1000000.);		
		}

	}
}

VO_VOID CMKVAudioTrack::NewClusterTimeCodeArrived( VO_S64 timecode )
{
	MKVMediaSampleIndexEntry * pEntry = FindLastEntryWithoutClusterTimeCode();

	while( pEntry )
	{
		pEntry->set_clustertimecode( timecode );

		pEntry = (MKVMediaSampleIndexEntry *)pEntry->next;
	}
}

VO_VOID CMKVAudioTrack::Flush()
{
	m_lastrelativetimecode = -1;

	return CMKVTrack::Flush();
}

VO_U32 CMKVAudioTrack::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	if( m_pTrackInfo )
	{
		pAudioFormat->Channels = m_pTrackInfo->Channels;
		pAudioFormat->SampleBits = m_pTrackInfo->BitDepth;
		pAudioFormat->SampleRate = (VO_U32)m_pTrackInfo->SampleFreq;
	}

	return 0;
}

VO_VOID CMKVAudioTrack::perpare_ogg_headerdata()
{
	VO_S32 packet_size[3];
	memset( packet_size , 0 , sizeof( packet_size ) );

	VO_PBYTE ptr_pos = (VO_PBYTE)m_pTrackInfo->pCodecPrivate;

	ptr_pos++;

	VO_S32 index = 0;
	while( index < 2 )
	{
		packet_size[index] = packet_size[index] + (VO_U32)*ptr_pos;

		if( *ptr_pos < 0xff )
			index++;

		ptr_pos++;
	}

	packet_size[index] = m_pTrackInfo->CodecPrivateSize - ( ptr_pos - (VO_PBYTE)m_pTrackInfo->pCodecPrivate ) - packet_size[0] - packet_size[1];

	m_privatedata_size = m_pTrackInfo->CodecPrivateSize - ( ptr_pos - (VO_PBYTE)m_pTrackInfo->pCodecPrivate ) + sizeof( VORBISFORMAT2 );
	m_ptr_privatedata = new VO_BYTE[ m_privatedata_size ];
	VORBISFORMAT2 * ptr_vorbis_head = (VORBISFORMAT2*)m_ptr_privatedata;

	ptr_vorbis_head->BitsPerSample = 16;
	ptr_vorbis_head->Channels = m_pTrackInfo->Channels;
	ptr_vorbis_head->SamplesPerSec = (VO_U32)m_pTrackInfo->SampleFreq;
	ptr_vorbis_head->HeaderSize[0] = packet_size[0];
	ptr_vorbis_head->HeaderSize[1] = packet_size[1];
	ptr_vorbis_head->HeaderSize[2] = packet_size[2];

	memcpy( m_ptr_privatedata + sizeof( VORBISFORMAT2 ) , ptr_pos , packet_size[0] + packet_size[1] + packet_size[2] );

}

VO_U32 CMKVAudioTrack::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
//	static bool is_first_frame = true;
	VO_U32	result = 0;

/*	
	if( is_first_frame && strcmp( (VO_CHAR*)m_pTrackInfo->str_CodecID , "A_VORBIS" ) == 0 )
	{
		pSample->Buffer = m_ptr_privatedata;
		pSample->Duration = 1;
		pSample->Size = m_privatedata_size;
		pSample->Time = 0;
		is_first_frame = false;
		result = 0;
	}
	else
	{
		is_first_frame = false;
		result = CBaseStreamFileTrack::GetSampleN( pSample );
	}
*/
	result = CBaseStreamFileTrack::GetSampleN( pSample );

	//if the data is pcm, change the data from bit-ending to little-ending.
	if((strcmp((VO_CHAR*)m_pTrackInfo->str_CodecID, "A_PCM/INT/BIG") == 0) && VO_NULL !=  pSample->Buffer)
	{
		VO_U8 bitlenth = 8;
		if(!m_pTrackInfo->ptr_ContentCompSettings)
		{
			pSample->Buffer += m_pTrackInfo->ContentCompSettingsSize;	
			pSample->Size -= m_pTrackInfo->ContentCompSettingsSize;
		}
		BigToLittlePCM(pSample->Buffer, (VO_U32)pSample->Size & 0x7FFFFFFF, m_pTrackInfo->BitDepth/bitlenth);
	}
#ifdef _RAW_DUMP_
	m_Dumper.DumpAudioData(pSample->Buffer, pSample->Size&0x7FFFFFFF);
#endif
	return result;
}

VO_BOOL CMKVAudioTrack::BigToLittlePCM(VO_PBYTE pData, VO_U32 nSize, VO_U32 nBitDepth)
{
	if(nBitDepth != PCM_BIT_16 && nBitDepth != PCM_BIT_24 && nBitDepth != PCM_BIT_32)
		return VO_FALSE;
	
	if(nSize < (sizeof(VO_U8)*nBitDepth))
		return VO_FALSE;
	
	VO_U32	bitdepth = nBitDepth;
	VO_U8*	pbegin = (VO_U8*)pData;
	VO_U8*	pend = pbegin+bitdepth-1;
	VO_U8	tmp = 0;
	VO_U32	len = nSize;
	VO_U32	num =bitdepth/2;
	VO_U32 	offset = 0;

	while(len >= bitdepth)
	{
		for(offset = 0; offset < num; offset++)
		{
			pend = pbegin+bitdepth - offset -1;
			pbegin += offset;

			tmp = *pbegin;
			*pbegin = *pend;
			*pend = tmp;
		}

		pbegin += bitdepth - offset + 1;
		pend = pbegin+bitdepth - 1;
		len -= bitdepth;
	}

	return VO_TRUE;
}

//VO_U32 CMKVAudioTrack::SetPosN(VO_S64* pPos)
//{
//	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
//	VO_S32 nRes = m_pStreamFileIndex->GetEntryByTime(m_bVideo, *pPos, &pGet);
//	if(-1 == nRes)
//		return VO_ERR_SOURCE_END;
//	else if(1 == nRes)	//retry
//	{
//		VO_BOOL bParseEnd = VO_FALSE;
//		while(1 == nRes && !bParseEnd)	//retry!!
//		{
//			m_pStreamFileIndex->RemoveAll();
//			//if(m_pGlobeBuffer)
//				//m_pGlobeBuffer->Flush();
//			if( m_pGlobeBuffer )
//				m_pGlobeBuffer->RemoveTo( (pGet->pos_in_buffer & 0x7fffffff) + pGet->size );
//
//			if(!TrackGenerateIndex())
//				bParseEnd = VO_TRUE;
//
//			nRes = m_pStreamFileIndex->GetEntryByTime(m_bVideo, *pPos, &pGet);
//			if(-1 == nRes)
//				return VO_ERR_SOURCE_END;
//		}
//
//		if(0 != nRes)
//			return VO_ERR_SOURCE_END;
//
//		*pPos = pGet->time_stamp;
//		m_pStreamFileIndex->RemoveUntil(pGet);
//		TrackGenerateIndex();
//	}
//	else	//ok
//	{
//		*pPos = pGet->time_stamp;
//		m_pStreamFileIndex->RemoveUntil(pGet);
//		m_pStreamFileReader->FileGenerateIndex();
//	}
//
//	return VO_ERR_SOURCE_OK;
//}
