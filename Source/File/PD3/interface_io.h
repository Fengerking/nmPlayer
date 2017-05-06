
#ifndef __INTERFACE_IO_H__

#define __INTERFACE_IO_H__
#include "voType.h"
#include "voFile.h"

class interface_io
{
public:
	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_S64 size ) = 0;
	virtual VO_S64 set_pos( VO_S64 seekpos , VO_FILE_POS pos ) = 0;
	virtual VO_S64 get_filesize() = 0;
};

#endif