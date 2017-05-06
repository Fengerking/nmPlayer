#include "buffer_judgement.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

buffer_judgement::buffer_judgement( VO_S32 judgement_array_size )
:m_ptr_pos_array(0)
,m_arraysize(0)
,m_filled_size(0)
,m_next_fill_pos(0)
{
	m_ptr_pos_array = new VO_S64[ judgement_array_size ];
	m_arraysize = judgement_array_size;
}

buffer_judgement::~buffer_judgement(void)
{
	delete []m_ptr_pos_array;
}

VO_VOID buffer_judgement::push_pos( VO_S64 pos )
{
	m_ptr_pos_array[m_next_fill_pos] = pos;

	m_next_fill_pos++;

	if( m_next_fill_pos == m_arraysize )
		m_next_fill_pos = 0;

	if( m_filled_size < m_arraysize )
		m_filled_size++;
}

VO_BOOL buffer_judgement::get_judgement( VO_S64 * ptr_result )
{
	if( m_filled_size != m_arraysize )
		return VO_FALSE;

	*ptr_result = m_ptr_pos_array[0];

	for( VO_S32 i = 1 ; i < m_arraysize ; i++ )
	{
		if( m_ptr_pos_array[i] < *ptr_result )
			*ptr_result = m_ptr_pos_array[i];
	}

	return VO_TRUE;
}

VO_VOID buffer_judgement::reset()
{
	m_filled_size = 0;
	m_next_fill_pos = 0;
}
