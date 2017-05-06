//#include "StdAfx.h"
#include "vo_largefile_buffer.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "vo_mem_stream.h"
#include "vo_file_stream.h"

#define LOG_TAG "voCOMXAudioSink"

#define BUFFER_FADE 1024 * 2048
#define JUDGEMENT_ARRAY_SIZE 30

vo_largefile_buffer::vo_largefile_buffer(void)
:m_physical_start(0)
,m_buffer_end(0)
,m_buffer_start(0)
,m_buffer_realsize(0)
,m_is_buffer_fulled(VO_FALSE)
,m_judger( JUDGEMENT_ARRAY_SIZE )
,m_ptr_stream(0)
,m_is_exit(VO_FALSE)
{
}

vo_largefile_buffer::~vo_largefile_buffer(void)
{
	uninit();
}

VO_BOOL vo_largefile_buffer::init( VOPDInitParam * ptr_param )
{
	uninit();

	m_is_exit = VO_FALSE;

	m_buffer_realsize =  ptr_param->nMaxBuffer * 1024 + 1;
	m_buffer_size = ptr_param->nMaxBuffer * 1024;

	VO_CHAR tempfile[1024];
	if( sizeof(TCHAR) != 1 )
	{
		wcstombs(tempfile,(wchar_t*)ptr_param->mFilePath,sizeof(tempfile));
	}

	if( ptr_param->nTempFileLocation == 0 )
	{
		m_ptr_stream = new vo_mem_stream();
	}
	else
	{
		m_ptr_stream = new vo_file_stream();
	}

	m_ptr_stream->open( tempfile , m_buffer_realsize );

	return VO_TRUE;
}

VO_VOID vo_largefile_buffer::uninit()
{
	m_is_exit = VO_TRUE;

	if( m_ptr_stream )
	{
		m_ptr_stream->close();
		delete m_ptr_stream;
		m_ptr_stream = 0;
	}

	reset();
}

VO_S64 vo_largefile_buffer::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread )
{
	voCAutoLock lock( &m_lock );

	VOLOGI( "read_buffer : physical_pos = %lld  toread = %lld  start = %d  end = %d buffersize = %d " , physical_pos , toread , m_buffer_start , m_buffer_end , m_buffer_size );

	//char str[1024];
	//memset( str , 0 , 1024 );
	//int str_size = sprintf( str , "Buffer (%I64d-%I64d) , Request (%I64d-%I64d) , Start : %d , End : %d\r\n" , m_physical_start , m_physical_start + get_buffer_usedsize() , physical_pos , physical_pos + toread , m_buffer_start , m_buffer_end );
	VOLOGE( "Buffer (%lld-%lld) , Request (%lld-%lld) , Start : %d , End : %d\r\n" , m_physical_start , m_physical_start + get_buffer_usedsize() , physical_pos , physical_pos + toread , m_buffer_start , m_buffer_end );

	VO_S32 usedsize = get_buffer_usedsize();

	if( physical_pos >= m_physical_start && physical_pos <= m_physical_start + usedsize )
	{
		VO_S32 coincide_size = m_physical_start + usedsize - physical_pos;
		if( coincide_size >= toread )
		{
			read_buffer( physical_pos , buffer , toread );
			return toread;
		}
		else
		{
			do 
			{
				m_lock.Unlock();

				VO_BUFFER_NEED need;
				need.want_physical_pos = physical_pos + coincide_size;
				need.want_size = toread - coincide_size;

				char str[1024];
				memset( str , 0 , 1024 );
				int str_size = sprintf( str , "Buffer Lack (%I64d-%I64d)\r\n" ,need.want_physical_pos , need.want_physical_pos + need.want_size );


				if( usedsize == m_buffer_size && physical_pos + toread > m_physical_start + usedsize )
				{
					reset();
				}

				BUFFER_CALLBACK_RET ret = m_ptr_buffer_callback->buffer_notify( BUFFER_LACK , &need );

				switch( ret )
				{
				case BUFFER_CALLBACK_ERROR:
					{
						VOLOGE("BUFFER_CALLBACK_ERROR");
						m_lock.Lock();
						return -1;
					}
				case BUFFER_CALLBACK_SHOULD_STOP:
					{
						m_lock.Lock();
						return -2;
					}
				}

				voOS_Sleep( 100 );
				m_lock.Lock();

				usedsize = get_buffer_usedsize();
				coincide_size = m_physical_start + usedsize - physical_pos;
			} while ( coincide_size < toread && !m_is_exit );

			if( m_is_exit )
				return -1;

			read_buffer( physical_pos , buffer , toread );
			return toread;
		}
	}
	else
	{
		do 
		{
			m_lock.Unlock();

			VO_BUFFER_NEED need;
			need.want_physical_pos = physical_pos;
			need.want_size = -1;

			char str[1024];
			memset( str , 0 , 1024 );
			int str_size = sprintf( str , "Buffer Lack (%I64d-%I64d)\r\n" ,need.want_physical_pos , need.want_physical_pos + need.want_size );

			BUFFER_CALLBACK_RET ret = m_ptr_buffer_callback->buffer_notify( BUFFER_LACK , &need );

			switch( ret )
			{
			case BUFFER_CALLBACK_ERROR:
				{
					VOLOGE("BUFFER_CALLBACK_ERROR");
					m_lock.Lock();
					return -1;
				}
			case BUFFER_CALLBACK_SHOULD_STOP:
				{
					m_lock.Lock();
					return -2;
				}
			}

			voOS_Sleep( 100 );
			m_lock.Lock();

			usedsize = get_buffer_usedsize();
		} while ( !( physical_pos >= m_physical_start && physical_pos <= m_physical_start + usedsize ) && !m_is_exit );

		if( m_is_exit )
			return -1;

		m_lock.Unlock();
		VO_S32 readsize = read( physical_pos , buffer , toread );
		m_lock.Lock();
		return readsize;
	}

	return 0;
}

VO_S64 vo_largefile_buffer::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	voCAutoLock lock( &m_lock );

	VO_S32 usedsize = get_buffer_usedsize();
	VO_S32 leftsize = m_buffer_size - usedsize;

	if( physical_pos >= m_physical_start && physical_pos <= m_physical_start + usedsize )
	{
		if( usedsize == m_buffer_size )
		{
			return 0;
		}

		VO_S32 coincide_size = m_physical_start + usedsize - physical_pos;
		VO_S32 non_coincide_size = towrite - coincide_size;

		if( non_coincide_size <= 0 )
			return towrite;

		if( non_coincide_size < leftsize )
		{
			if( non_coincide_size >= 1 )
			{
				write_buffer( m_buffer_end , buffer + coincide_size , non_coincide_size );
				return towrite;
			}
		}
		else
		{
			write_buffer( m_buffer_end , buffer + coincide_size , leftsize );
			m_ptr_buffer_callback->buffer_notify( BUFFER_FULL , 0 );
			m_is_buffer_fulled = VO_TRUE;
			return leftsize + coincide_size;
		}
	}
	else
	{
		reset();
		m_physical_start = physical_pos;
		write_buffer( 0 , buffer , towrite );
		return towrite;
	}
}

VO_S64 vo_largefile_buffer::seek( VO_S64 physical_pos )
{
	return physical_pos;
}

VO_VOID vo_largefile_buffer::reset()
{
	m_physical_start = 0;
	m_buffer_end = 0;
	m_buffer_start = 0;
	m_is_buffer_fulled = VO_FALSE;

	m_judger.reset();
}

VO_VOID vo_largefile_buffer::write_buffer( VO_S64 file_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	VO_S32 first_write_size = m_buffer_realsize - m_buffer_end;

	if( towrite <= first_write_size )
	{
		m_ptr_stream->seek( file_pos , stream_begin );
		m_ptr_stream->write( buffer , towrite );
		m_buffer_end = m_buffer_end + towrite;
	}
	else
	{
		m_ptr_stream->seek( file_pos , stream_begin );
		m_ptr_stream->write( buffer , first_write_size );
		m_ptr_stream->seek( 0 , stream_begin );
		m_ptr_stream->write( buffer + first_write_size , towrite - first_write_size );
		m_buffer_end = towrite - first_write_size;
	}
}

VO_VOID vo_largefile_buffer::read_buffer( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread )
{
	VO_S32 read_pos = ( ( m_buffer_start + ( physical_pos - m_physical_start ) ) % m_buffer_realsize );
	VO_S32 first_read_size = m_buffer_realsize - read_pos;
	
	if( toread <= first_read_size )
	{
		m_ptr_stream->seek( read_pos , stream_begin );
		m_ptr_stream->read( buffer , toread );
	}
	else
	{
		m_ptr_stream->seek( read_pos , stream_begin );
		m_ptr_stream->read( buffer , first_read_size );

		m_ptr_stream->seek( 0 , stream_begin );
		m_ptr_stream->read( buffer + first_read_size , toread - first_read_size );
	}

	static VO_S32 ref_value = m_buffer_realsize * 2./3. < m_buffer_realsize - BUFFER_FADE ? m_buffer_realsize * 2./3. :  m_buffer_realsize - BUFFER_FADE;

	VO_S64 judgement_result;

	if( m_judger.get_judgement( &judgement_result ) )
	{
		if( judgement_result - m_physical_start >= BUFFER_FADE && get_buffer_usedsize() >= ref_value )
		{
			m_physical_start = m_physical_start + BUFFER_FADE/2;
			m_buffer_start = ( m_buffer_start + BUFFER_FADE/2 )%m_buffer_realsize;
		}
	}

 	if( m_is_buffer_fulled && get_buffer_usedsize() < m_buffer_size * 2. / 3. )
 	{
 		VO_BUFFER_NEED need;
 		need.want_physical_pos = m_physical_start + get_buffer_usedsize();
 		need.want_size = -1;
 		m_ptr_buffer_callback->buffer_notify( BUFFER_LOW , &need );
 
 		char str[1024];
 		memset( str , 0 , 1024 );
 		int size = sprintf( str , "Buffer low! download start: %I64d\r\n" , need.want_physical_pos );
 		m_is_buffer_fulled = VO_FALSE;
 	}

	m_judger.push_pos( physical_pos );
	//VOLOGI( "read_buffer : physical_pos = %d  toread = %d  start = %d  end = %d buffersize = %d " , (VO_S32)physical_pos , (VO_S32)toread , (VO_S32)m_physical_start , (VO_S32)(m_physical_start + get_buffer_usedsize()) , (VO_S32)m_buffer_size );
}

VO_VOID vo_largefile_buffer::get_buffer_info( BUFFER_INFO * ptr_info )
{
	ptr_info->physical_start = m_physical_start;
	ptr_info->usedsize = get_buffer_usedsize();
}

