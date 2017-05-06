
#include "bufferstream_counter.h"

bufferstream_counter::bufferstream_counter( VO_S32 count_size )
:m_ptr_array(0)
,m_size(count_size)
,m_pos(0)
,m_cur_size(0)
{
	m_ptr_array = new VO_S32[count_size];
}

bufferstream_counter::~bufferstream_counter()
{
	if( m_ptr_array )
		delete []m_ptr_array;

	m_ptr_array = 0;
}

VO_VOID bufferstream_counter::push_id( VO_S32 id )
{
	m_ptr_array[m_pos] = id;
	m_pos++;

	if( m_pos >= m_size )
		m_pos = 0;

	if( m_cur_size < m_size )
		m_cur_size++;
}

VO_BOOL bufferstream_counter::is_id_contain( VO_S32 id )
{
	if( m_cur_size < m_size )
		return VO_TRUE;

	for( VO_S32 i = 0 ; i < m_size ; i++ )
	{
		if( m_ptr_array[i] == id )
			return VO_TRUE;
	}

	return VO_FALSE;
}