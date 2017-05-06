
#ifndef __BUFFERSTREAM_COUNTER_H__

#define __BUFFERSTREAM_COUNTER_H__

#include "voType.h"

class bufferstream_counter
{
public:
	bufferstream_counter( VO_S32 count_size );
	~bufferstream_counter();

	VO_VOID push_id( VO_S32 id );
	VO_BOOL is_id_contain( VO_S32 id );

private:
	VO_S32 * m_ptr_array;
	VO_S32 m_pos;
	VO_S32 m_size;
	VO_S32 m_cur_size;
};

#endif