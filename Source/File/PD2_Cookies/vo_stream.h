

#ifndef __VO_STREAM_H__

#define __VO_STREAM_H__

#include "voType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum STREAM_POS
{
	stream_begin,
	stream_cur,
	stream_end,
};

class vo_stream
{
public:
	vo_stream(){}
	virtual ~vo_stream(){}

	virtual VO_BOOL open( VO_CHAR * ptr_path , VO_S32 size ) = 0;
	virtual VO_VOID close() = 0;

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_S32 toread ) = 0;
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_S32 towrite ) = 0;
	virtual VO_S64 seek( VO_S64 pos , STREAM_POS stream_pos ) = 0;
};

#ifdef _VONAMESPACE
}
#endif
#endif