

#ifndef __VO_MEM_STREAM_H__

#define __VO_MEM_STREAM_H__

#include "voType.h"
#include "vo_stream.h"


class vo_mem_stream : public vo_stream
{
public:
	vo_mem_stream();
	virtual ~vo_mem_stream();

	//vo_stream
	virtual VO_BOOL open( VO_CHAR * ptr_path , VO_S32 size );
	virtual VO_VOID close();

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_S32 toread );
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_S32 towrite );
	virtual VO_S64 seek( VO_S64 pos , STREAM_POS stream_pos );
	//

private:

	VO_PBYTE m_ptr_buffer;
	VO_S32 m_buffersize;
	VO_S32 m_current_pos;
};

#endif