
#include "vo_file_stream.h"

vo_file_stream::vo_file_stream()
:m_fp(0)
,m_filesize(0)
{
	;
}

vo_file_stream::~vo_file_stream()
{
	close();
}

VO_BOOL vo_file_stream::open( VO_CHAR * ptr_path , VO_S32 size )
{
	close();

	m_fp = fopen( ptr_path , "wb+" );
	m_filesize = size;

	VO_BYTE fillvalue = 0;

	fwrite( &fillvalue , 1 , size , m_fp );

	fseek( m_fp , 0 , SEEK_SET );

	return VO_TRUE;
}

VO_VOID vo_file_stream::close()
{
	if( m_fp )
	{
		fclose( m_fp );
		m_fp = 0;
	}

	m_filesize = 0;
}

VO_S64 vo_file_stream::read( VO_PBYTE ptr_buffer , VO_S32 toread )
{
	fread( ptr_buffer , toread , 1 , m_fp );

	return toread;
}

VO_S64 vo_file_stream::write( VO_PBYTE ptr_buffer , VO_S32 towrite )
{
	fwrite( ptr_buffer , towrite , 1 , m_fp );

	return towrite;
}

VO_S64 vo_file_stream::seek( VO_S64 pos , STREAM_POS stream_pos )
{
	switch( stream_pos )
	{
	case stream_begin:
		fseek( m_fp , pos , SEEK_SET );
		break;
	case stream_cur:
		fseek( m_fp , pos , SEEK_SET );
		break;
	case stream_end:
		fseek( m_fp , pos , SEEK_END );
		break;
	}

	return ftell( m_fp );
}