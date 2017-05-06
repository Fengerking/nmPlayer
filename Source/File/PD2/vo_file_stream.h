
#ifndef __VO_FILE_STREAM_H__

#define __VO_FILE_STREAM_H__

#include "voType.h"
#include "vo_stream.h"
#include "stdio.h"


class vo_file_stream : public vo_stream
{
public:
	vo_file_stream();
	virtual ~vo_file_stream();

	//vo_stream
	virtual VO_BOOL open( VO_CHAR * ptr_path , VO_S32 size );
	virtual VO_VOID close();

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_S32 toread );
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_S32 towrite );
	virtual VO_S64 seek( VO_S64 pos , STREAM_POS stream_pos );
	//

private:
	FILE * m_fp;
	VO_S64 m_filesize;
};

#endif