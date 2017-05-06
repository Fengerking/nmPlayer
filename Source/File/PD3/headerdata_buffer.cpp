#include "headerdata_buffer.h"
#include "log.h"

headerdata_buffer::headerdata_buffer()
:m_headerdata_array(0)
,m_arraysize(0)
,m_is_ready(VO_FALSE)
{
	;
}

headerdata_buffer::~headerdata_buffer()
{
	if( m_headerdata_array )
	{
		for( VO_S32 i = 0 ; i < m_arraysize ; i++ )
			delete []m_headerdata_array[i].ptr_buffer;

		delete []m_headerdata_array;
	}

	m_headerdata_array = 0;
}

VO_BOOL headerdata_buffer::get_headerdata( interface_io * ptr_io , headerdata_info * ptr_info )
{
	m_arraysize = ptr_info->arraysize;
	m_headerdata_array = new HEADER_DATA[m_arraysize];
	for( VO_S32 i = 0 ; i < m_arraysize ; i++ )
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Cache Data pos: %lld , size: %lld" , ptr_info->ptr_array[i].physical_pos , ptr_info->ptr_array[i].size );
		m_headerdata_array[i].ptr_buffer = new VO_BYTE[ ptr_info->ptr_array[i].size ];
		m_headerdata_array[i].physical_pos = ptr_info->ptr_array[i].physical_pos;
		m_headerdata_array[i].size = ptr_info->ptr_array[i].size;

		ptr_io->set_pos( m_headerdata_array[i].physical_pos , VO_FILE_BEGIN );
		VO_S64 gotsize = ptr_io->read( m_headerdata_array[i].ptr_buffer , m_headerdata_array[i].size );

		if( gotsize != m_headerdata_array[i].size )
			return VO_FALSE;
	}

	m_is_ready = VO_TRUE;

	return VO_TRUE;
}

VO_S64 headerdata_buffer::read( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S64 size )
{
	if( !m_is_ready )
		return -1;

	for( VO_S32 i = 0 ; i < m_arraysize ; i++ )
	{
		if( physical_pos >= m_headerdata_array[i].physical_pos && physical_pos + size <= m_headerdata_array[i].physical_pos + m_headerdata_array[i].size )
		{
			memcpy( ptr_buffer , m_headerdata_array[i].ptr_buffer + physical_pos - m_headerdata_array[i].physical_pos , size );
			return size;
		}
	}

	return -1;
}