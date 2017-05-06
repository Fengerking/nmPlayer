#pragma once
#include "vo_buffer.h"
#include "voCMutex.h"
#include "voFile.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

struct headerdata_segment
{
	VO_S64 physical_pos;
	VO_S32 size;
	VO_S32 buffer_pos;
	VO_S64 last_read_pos;

	headerdata_segment * ptr_next;
};

class vo_headerdata_buffer :
	public vo_buffer
{
public:
	vo_headerdata_buffer(void);
	~vo_headerdata_buffer(void);

	//vo_buffer
	virtual VO_BOOL init( VOPDInitParam * ptr_param );
	virtual VO_VOID uninit();
	virtual VO_S64 read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync = VO_TRUE );
	virtual VO_S64 write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite );
	virtual VO_S64 seek( VO_S64 physical_pos );
	virtual VO_VOID reset();
	virtual VO_VOID close(){ m_is_closed = VO_TRUE; }
	virtual VO_VOID get_buffer_info( BUFFER_INFO * ptr_info ){};
	virtual VO_VOID get_buffering_start_info( BUFFER_INFO * ptr_info ){};
	//

	virtual VO_VOID set_initmode( VO_BOOL isset );
	virtual VO_BOOL rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number , vo_buffer * ptr_buffer );

protected:
	virtual VO_VOID destroy();
	virtual VO_VOID sort_segment();
	virtual VO_VOID reuse_cache( vo_buffer * ptr_buffer );

	virtual headerdata_segment * get_segment( VO_S64 pos );

	VO_S32	get_used_buffersize();
	VO_VOID check_control_buffer_usage();

private:
	VO_S64 phaseI_read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync = VO_TRUE );
	VO_S64 phaseII_read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync = VO_TRUE );
	VO_S64 basic_read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync = VO_TRUE );

	VO_S64 phaseI_write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite );
protected:
	VO_PBYTE m_ptr_cache;
	VO_S32 m_cache_size;

	VO_BOOL m_is_initmode;

	headerdata_segment * m_ptr_segment_header;
	headerdata_segment * m_ptr_segment_tail;
	voCMutex m_lock;

	VO_BOOL m_is_reusing_cache;

	VOPDInitParam m_pd_param;
		
	VO_U32 m_static_last_time;
	VO_S32 m_static_last_inused_buffer;
	VO_BOOL	m_is_buffer_controled;
};

#ifdef _VONAMESPACE
}
#endif