

#include "vo_mem_stream.h"
#include "voString.h"

vo_mem_stream::vo_mem_stream()
:m_ptr_buffer(0)
,m_current_pos(0)
{
	;
}

vo_mem_stream::~vo_mem_stream()
{
	close();
}

VO_BOOL vo_mem_stream::open( VO_CHAR * ptr_path , VO_S32 size )
{
	close();

	m_ptr_buffer = new VO_BYTE[size];
	m_buffersize = size;

	return VO_TRUE;
}

VO_VOID vo_mem_stream::close()
{
	if( m_ptr_buffer )
	{
		delete []m_ptr_buffer;
		m_ptr_buffer = 0;
	}
}

VO_S64 vo_mem_stream::read( VO_PBYTE ptr_buffer , VO_S32 toread )
{
	memcpy( ptr_buffer , m_ptr_buffer + m_current_pos , toread );

	m_current_pos = m_current_pos + toread;

	return toread;
}

VO_S64 vo_mem_stream::write( VO_PBYTE ptr_buffer , VO_S32 towrite )
{
	memcpy( m_ptr_buffer + m_current_pos , ptr_buffer , towrite );

	m_current_pos = m_current_pos + towrite;

	return towrite;
}

VO_S64 vo_mem_stream::seek( VO_S64 pos , STREAM_POS stream_pos )
{
	switch( stream_pos )
	{
	case stream_begin:
		m_current_pos = pos;
		break;
	case stream_end:
		m_current_pos = m_buffersize + pos;
		break;
	case stream_cur:
		m_current_pos = m_current_pos + pos;
		break;
	}

	if( m_current_pos > m_buffersize )
		m_current_pos = m_buffersize;
	else if( m_current_pos < 0 )
		m_current_pos = 0;

	return m_current_pos;
}


