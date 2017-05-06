#ifndef __HEADERDATA_BUFFER_H__

#define __HEADERDATA_BUFFER_H__

#include "interface_io.h"
#include "voFile.h"

struct HEADER_DATA
{
	VO_S64 physical_pos;
	VO_S64 size;
	VO_PBYTE ptr_buffer;
};

class headerdata_buffer
{
public:
	headerdata_buffer();
	~headerdata_buffer();

	VO_BOOL get_headerdata( interface_io * ptr_io , headerdata_info * ptr_info );
	VO_S64 read( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S64 size );

protected:

	HEADER_DATA * m_headerdata_array;
	VO_S32 m_arraysize;

	VO_BOOL m_is_ready;
};

#endif