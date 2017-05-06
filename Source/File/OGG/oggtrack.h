
#ifndef __VO_OGGTRACK_H_

#define __VO_OGGTRACK_H_

#include "CBaseStreamFileTrack.h"
#include "oggcommon.h"
#include "oggparser.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class oggtrack : public CBaseStreamFileTrack
{
public:
	oggtrack(VO_TRACKTYPE nType, VO_U8 btStreamNum, VO_U32 dwDuration, CBaseStreamFileReader* pReader, VO_MEM_OPERATOR* pMemOp);
	~oggtrack();

	VO_VOID set_trackinfo( const track_info * const_ptr_track_info );
	VO_U32 get_stream_id(){ return m_const_ptr_track_info->serial_number; }
	VO_VOID set_new_page( VO_U64 granule_pos ){ m_pre_granule_pos = m_current_granule_pos; m_current_granule_pos = granule_pos; }
	VO_BOOL on_receive_packet( const ogg_packet* const_ptr_ogg_packet );

	VO_U64 get_seek_pos( VO_S64 time_code );


	//virtual VO_U32 Prepare();

	virtual VO_VOID Flush();

protected:
	virtual VO_VOID add_sample( PBaseStreamMediaSampleIndexEntry ptr_entry ) = 0;
	virtual VO_U64 calculate_timecode( VO_U32 packet_number , VO_U32 packets_in_page ) = 0;

	const track_info * m_const_ptr_track_info;

	VO_U64 m_pre_granule_pos;
	VO_U64 m_current_granule_pos;

	VO_PBYTE m_ptr_header_data;
	VO_U32 m_header_size;

private:
	//for add_sample
	BaseStreamMediaSampleIndexEntry m_cache_entry;
	VO_BOOL m_is_cache_in_use;
	//
};

#ifdef _VONAMESPACE
}
#endif

#endif