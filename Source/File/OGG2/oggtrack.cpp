
#include "oggtrack.h"
#include "CBaseStreamFileReader.h"
#include "oggreader.h"
#include "voLog.h"

oggtrack::oggtrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CBaseStreamFileReader* pReader, VO_MEM_OPERATOR* pMemOp)
: CBaseStreamFileTrack( nType , btStreamNum , dwDuration , pReader , pMemOp )
, m_const_ptr_track_info( VO_NULL )
, m_pre_granule_pos(0)
, m_current_granule_pos(0)
, m_ptr_header_data(VO_NULL)
, m_header_size(0)
, m_is_cache_in_use(VO_FALSE)
{
	MemSet( &m_cache_entry , 0 , sizeof(BaseStreamMediaSampleIndexEntry) );
}

oggtrack::~oggtrack()
{
	;
}

VO_VOID oggtrack::set_trackinfo( const track_info * const_ptr_track_info )
{
	m_const_ptr_track_info = const_ptr_track_info;
}

VO_BOOL oggtrack::on_receive_packet( const ogg_packet* const_ptr_ogg_packet )
{
	if( m_is_cache_in_use )
	{
		if( const_ptr_ogg_packet->packet_size != 0 )
		{
			VO_PBYTE ptr_buffer = VO_NULL;
			m_pGlobeBuffer->GetForDirectWrite( &ptr_buffer , const_ptr_ogg_packet->packet_size );
			m_pGlobeBuffer->DirectWrite( const_ptr_ogg_packet->ptr_file_chunk , ptr_buffer , 0 , const_ptr_ogg_packet->packet_size );

			m_cache_entry.Add( const_ptr_ogg_packet->packet_size );
		}

		if( const_ptr_ogg_packet->b_is_compelete )
		{
			m_is_cache_in_use = VO_FALSE;
			/*PBaseStreamMediaSampleIndexEntry ptr_entry = new BaseStreamMediaSampleIndexEntry( m_cache_entry.time_stamp ,
																								m_cache_entry.pos_in_buffer,
																								m_cache_entry.size);*/
			PBaseStreamMediaSampleIndexEntry ptr_entry = m_pStreamFileIndex->NewEntry( m_cache_entry.time_stamp ,
																						m_cache_entry.pos_in_buffer,
																						m_cache_entry.size );
			add_sample( ptr_entry );
		}
	}
	else
	{
		VO_U64 time_stamp = calculate_timecode(const_ptr_ogg_packet->page_packet_number , const_ptr_ogg_packet->packets);

		VO_U32 pos = m_pGlobeBuffer->Add( const_ptr_ogg_packet->ptr_file_chunk , const_ptr_ogg_packet->packet_size );

		if( pos == VO_MAXU32 )
			return VO_FALSE;

		if( const_ptr_ogg_packet->b_is_compelete )
		{
			//PBaseStreamMediaSampleIndexEntry ptr_entry = new BaseStreamMediaSampleIndexEntry( time_stamp , pos , const_ptr_ogg_packet->packet_size );
			PBaseStreamMediaSampleIndexEntry ptr_entry =  m_pStreamFileIndex->NewEntry( (VO_U32)(time_stamp) , pos , const_ptr_ogg_packet->packet_size );

			add_sample( ptr_entry );
		}
		else
		{
			m_is_cache_in_use = VO_TRUE;
			m_cache_entry.time_stamp = (VO_U32)(time_stamp);
			m_cache_entry.pos_in_buffer = pos;
			m_cache_entry.size = const_ptr_ogg_packet->packet_size;
		}
	}

	return VO_TRUE;
}

// VO_U32 oggtrack::Prepare()
// {
// 	m_pStreamFileIndex = new CBaseStreamFileIndex(m_pStreamFileReader->GetMemPoolPtr());
// 	InitGlobalBuffer();
// 
// 	return CBaseTrack::Prepare();
// }

VO_U64 oggtrack::get_seek_pos( VO_S64 time_code )
{
/*
	track_index * ptr_index = m_const_ptr_track_info->ptr_index_head;

	while( ptr_index && ( (VO_U64)(time_code) > ( ptr_index->granule_pos * 1000 / m_const_ptr_track_info->info.audio_sample_rate ) ) )
	{
		set_new_page( ptr_index->granule_pos );
		ptr_index = ptr_index->ptr_next_index;
	}

	if( ptr_index )
		return ptr_index->page_pos;

	if( m_const_ptr_track_info->ptr_index_head )
		return m_const_ptr_track_info->ptr_index_head->page_pos;
*/
	double percentage = (time_code == 0)?0:(double)time_code/(double)m_dwDuration;
	VO_U64 pos = 0;
	VO_U64 size =0;
	((oggreader*)m_pStreamFileReader)->GetFileRealSize(&size);
	pos = percentage * size;

	VOLOGI("get_seek_pos---pos = %llu",pos);
	return pos;
}

VO_VOID oggtrack::Flush()
{
	m_pre_granule_pos = 0;
	m_current_granule_pos = 0;
	m_is_cache_in_use = VO_FALSE;
	MemSet( &m_cache_entry , 0 , sizeof(BaseStreamMediaSampleIndexEntry) );

	CBaseStreamFileTrack::Flush();
}
