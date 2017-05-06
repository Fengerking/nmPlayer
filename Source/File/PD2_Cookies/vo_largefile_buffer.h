#pragma once
#include "vo_buffer.h"
#include "voCMutex.h"
//#include "buffer_judgement.h"
#include "vo_stream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_largefile_buffer :
	public vo_buffer
{
public:
	vo_largefile_buffer(void);
	virtual ~vo_largefile_buffer(void);

	//vo_buffer
	virtual VO_BOOL init( VOPDInitParam * ptr_param );
	virtual VO_VOID uninit();
	virtual VO_S64 read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync = VO_TRUE );
	virtual VO_S64 write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite );
	virtual VO_S64 seek( VO_S64 physical_pos );
	virtual VO_VOID reset();
	virtual VO_VOID close(){}

	virtual VO_VOID get_buffer_info( BUFFER_INFO * ptr_info );
	virtual VO_VOID get_buffering_start_info( BUFFER_INFO * ptr_info );
	//

protected:

	virtual VO_S32 get_buffer_usedsize()
	{
		return ( m_buffer_end - m_buffer_start + m_buffer_realsize ) % m_buffer_realsize;
	}

	virtual VO_VOID write_buffer( VO_S64 file_pos , VO_PBYTE buffer , VO_S64 towrite );
	virtual VO_VOID read_buffer( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread );

protected:

	volatile VO_S64 m_physical_start;
	volatile VO_S32 m_buffer_start;
	volatile VO_S32 m_buffer_end;
	volatile VO_S64 m_buffer_full_physical;

	VO_S32 m_buffer_realsize;
	VO_S32 m_buffer_size;

	VO_BOOL m_is_buffer_fulled;

	voCMutex m_lock;

	//buffer_judgement m_judger;

	vo_stream * m_ptr_stream;

	VO_BOOL m_is_exit;

	VO_S32	m_buffer_high_cnt;
};

#ifdef _VONAMESPACE
}
#endif