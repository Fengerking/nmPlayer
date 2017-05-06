
#ifndef _VO_BUFFER__H

#define _VO_BUFFER__H

#include "interface_buffer_callback.h"
#include "voStreaming.h"
#include "voFile.h"

struct BUFFER_INFO
{
	VO_S64 physical_start;
	VO_S64 usedsize;
};

class vo_buffer
{
public:
	vo_buffer():m_ptr_buffer_callback(0),m_filesize(0),m_is_stoped(VO_TRUE),m_is_initmode(VO_FALSE){}
	virtual ~vo_buffer(){}

	virtual VO_VOID set_buffer_callback( interface_buffer_callback * ptr_buffer_callback ){ m_ptr_buffer_callback = ptr_buffer_callback; }

	virtual VO_BOOL init( VOPDInitParam * ptr_param ) = 0;
	virtual VO_VOID uninit() = 0;
	virtual VO_S64 read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread ) = 0;
	virtual VO_S64 write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite ) = 0;
	virtual VO_S64 seek( VO_S64 physical_pos ) = 0;
	virtual VO_VOID reset() = 0;

	virtual BUFFER_CALLBACK_RET notify( BUFFER_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data )
	{
		if( m_ptr_buffer_callback )
			return m_ptr_buffer_callback->buffer_notify( id , ptr_data );
		else
			return BUFFER_CALLBACK_OK;
	}

	virtual VO_VOID set_download_stoped( VO_BOOL is_stopped ){ m_is_stoped = is_stopped; }

	virtual VO_VOID set_filesize( VO_S64 filesize ){ m_filesize = filesize; }
	virtual VO_S64 get_filesize(){ return m_filesize; }

	virtual VO_VOID get_buffer_info( BUFFER_INFO * ptr_info ) = 0;

	virtual VO_BOOL rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number ){ return VO_TRUE; }

	virtual VO_VOID set_initmode( VO_BOOL isset ){ m_is_initmode = isset; }


protected:
	interface_buffer_callback * m_ptr_buffer_callback;
	VO_BOOL m_is_stoped;
	VO_S64 m_filesize;
	VO_BOOL m_is_initmode;
};


#endif