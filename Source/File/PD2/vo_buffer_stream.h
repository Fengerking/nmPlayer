#pragma once
#include "vo_buffer.h"
#include "voFile.h"

class vo_buffer_stream
{
public:
	vo_buffer_stream( vo_buffer * ptr_buffer );
	virtual ~vo_buffer_stream(void);

	virtual VO_S32 read( VO_PBYTE buffer , VO_U32 toread );
	virtual VO_S64 seek( VO_S64 seekpos , VO_FILE_POS pos );

	virtual VO_S64 get_filesize();
	virtual VO_S64 get_filepos(){ return m_filepos; }

	virtual VO_VOID set_to_close( VO_BOOL is_closing ){ m_is_closing = is_closing; }

protected:
	vo_buffer * m_ptr_buffer;
	VO_S64 m_filepos;
	VO_BOOL m_is_closing;
};
