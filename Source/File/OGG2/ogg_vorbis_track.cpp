
#include "ogg_vorbis_track.h"
#include "fCC.h"

ogg_vorbis_track::ogg_vorbis_track(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CBaseStreamFileReader* pReader, VO_MEM_OPERATOR* pMemOp)
:oggtrack( nType , btStreamNum , dwDuration , pReader , pMemOp )
,is_first_frame(VO_TRUE)
{
	//m_dwBufferTime = 10;
#ifdef __VO_TEST
	m_fp = fopen( "d:\\ogg_vorbis_sample.log" , "w+" );
#endif
}

ogg_vorbis_track::~ogg_vorbis_track()
{
#ifdef __VO_TEST
	fclose( m_fp );
#endif
}

VO_VOID ogg_vorbis_track::add_sample( PBaseStreamMediaSampleIndexEntry ptr_entry )
{
#ifdef __VO_TEST
	char buffer[1024];
	int size = sprintf( buffer , "Size: %d\tTime Stamp: %d\n" , ptr_entry->size & 0x7fffffff , ptr_entry->time_stamp );
	fwrite( buffer , size , 1 , m_fp );

	if( (ptr_entry->size & 0x7fffffff) == 141 )
	{
		int i = 0;
		i = i;
	}

#endif
	m_pStreamFileIndex->Add( ptr_entry );
}

VO_U64 ogg_vorbis_track::calculate_timecode( VO_U32 packet_number , VO_U32 packets_in_page )
{
	if (m_current_granule_pos == (VO_U64)(-1)) return 0;

	return (VO_U64)(( ((float)( m_current_granule_pos - m_pre_granule_pos )) / packets_in_page * packet_number + m_pre_granule_pos ) * 1000 / m_const_ptr_track_info->info.audio_sample_rate);
}

VO_U32 ogg_vorbis_track::GetBitrate(VO_U32* pdwBitrate)
{
	*pdwBitrate = m_const_ptr_track_info->info.bitrate_nominal;

	if( *pdwBitrate > 0x60000 )
		*pdwBitrate = 0;

	return VO_ERR_NONE;
}

VO_U32 ogg_vorbis_track::GetHeadData(VO_CODECBUFFER* pHeadData)
{
	pHeadData->Buffer = m_const_ptr_track_info->ptr_header_data;
	pHeadData->Length = m_const_ptr_track_info->headersize;

#ifdef __VO_TEST
	FILE * pfile = fopen( "d:\\oggheaderdata.data" , "wb+" );
	fwrite( pHeadData->Buffer , pHeadData->Length , 1 , pfile );
	fclose( pfile );
#endif

	return VO_ERR_NONE;
}

VO_U32 ogg_vorbis_track::GetMaxSampleSize(VO_U32* pdwMaxSampleSize)
{
	*pdwMaxSampleSize = VO_MAX( MAX_VORBIS_SAMPLESIZE , m_const_ptr_track_info->headersize );

	return VO_ERR_NONE;
}

VO_U32 ogg_vorbis_track::GetCodecCC(VO_U32* pCC)
{
	*pCC = AudioFlag_OGG_1;

	return VO_ERR_NONE;
}

VO_U32 ogg_vorbis_track::Init(VO_U32 nSourceOpenFlags)
{
	//if(!(VO_SOURCE_OPENPARAM_FLAG_INFOONLY & nSourceOpenFlags))
	/*{
		InitGlobalBuffer();
	}*/

	return CBaseStreamFileTrack::Init(nSourceOpenFlags);
}

VO_U32 ogg_vorbis_track::GetSampleN(VO_SOURCE_SAMPLE* pSample)
{
	if( is_first_frame )
	{
		pSample->Duration = 1;
		pSample->Time = 0;
		pSample->Size = m_const_ptr_track_info->headersize;
		pSample->Buffer = m_const_ptr_track_info->ptr_header_data;
		is_first_frame = VO_FALSE;

		return 0;
	}

	PBaseStreamMediaSampleIndexEntry pGet = VO_NULL;
	VO_BOOL bFrameDropped = VO_FALSE;
	if(!m_pStreamFileIndex->GetEntry((VOTT_VIDEO == m_nType) ? VO_TRUE : VO_FALSE, pSample->Time, &pGet, &bFrameDropped))
		return VO_ERR_SOURCE_END;

	pSample->Duration = 1;
	pSample->Time = pGet->time_stamp;

	/*VO_U32 dwMaxSampleSize = VO_MAXU32;
	GetMaxSampleSize(&dwMaxSampleSize);
	if(pGet->size > dwMaxSampleSize)
	{
		pSample->Size = 0;
		return VO_ERR_SOURCE_OK;
	}*/

	pSample->Size = pGet->size;
	if(2 == m_pGlobeBuffer->Get(&pSample->Buffer, m_pSampleData, pGet->pos_in_buffer & 0x7FFFFFFF, pGet->size))
		pSample->Buffer = m_pSampleData;

	if(pGet->IsKeyFrame())
		pSample->Size |= 0x80000000;

	m_pStreamFileIndex->RemoveInclude(pGet);

	TrackGenerateIndex();

	return VO_ERR_SOURCE_OK;
}

VO_U32 ogg_vorbis_track::GetAudioFormat(VO_AUDIO_FORMAT* pAudioFormat)
{
	pAudioFormat->Channels = m_const_ptr_track_info->info.audio_channel;
	pAudioFormat->SampleBits = 16;
	pAudioFormat->SampleRate = m_const_ptr_track_info->info.audio_sample_rate;

	return 0;
}

VO_U32 ogg_vorbis_track::CalcTheDuration()
{
	VO_U64 duration = m_const_ptr_track_info->last_granule_pos * 1000;
	VO_U64 basetime = m_const_ptr_track_info->first_granule_pos * 1000;

	duration -= basetime;
	
	if (duration % m_const_ptr_track_info->info.audio_sample_rate) {
		 duration /= m_const_ptr_track_info->info.audio_sample_rate;
		 duration++;
	} else {
		 duration /= m_const_ptr_track_info->info.audio_sample_rate;
	}

	return (VO_U32)duration;
}

VO_U64 ogg_vorbis_track::CalcTimeCodecByGranule(VO_PTR pUser, VO_U64 granule_pos)
{
	VO_U64 timecodec = granule_pos*1000;
	ogg_vorbis_track* pTrack = (ogg_vorbis_track*)pUser;

	if (timecodec % pTrack->m_const_ptr_track_info->info.audio_sample_rate) {
		 timecodec /= pTrack->m_const_ptr_track_info->info.audio_sample_rate;
		 timecodec++;
	} else {
		 timecodec /= pTrack->m_const_ptr_track_info->info.audio_sample_rate;
	}

	return timecodec;
}

