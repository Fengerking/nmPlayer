#pragma once

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


enum mux_stream_pos
{
	vo_stream_begin,
	vo_stream_current,
	vo_stream_end,
};

class CMuxerStream
{
public:
	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size ) = 0;
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size ) = 0;
	virtual VO_BOOL seek( VO_S64 pos , mux_stream_pos stream_pos  ) = 0;
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size ) = 0;

	virtual VO_VOID write_eos() = 0;

	virtual VO_S32 get_lasterror() = 0;
	virtual VO_S64 size() = 0;
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE
