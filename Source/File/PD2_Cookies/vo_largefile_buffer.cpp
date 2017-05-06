//#include "StdAfx.h"
#include "vo_largefile_buffer.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "vo_mem_stream.h"
#include "vo_file_stream.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

//#define BUFFER_FADE 1024 * 2048
//Support no buffering backward seek
#define BUFFER_FADE 1024 * 1024 * 15
#define JUDGEMENT_ARRAY_SIZE 30

vo_largefile_buffer::vo_largefile_buffer(void)
:m_physical_start(0)
,m_buffer_start(0)
,m_buffer_end(0)
,m_buffer_full_physical(0)
,m_buffer_realsize(0)
,m_buffer_size(0)
,m_is_buffer_fulled(VO_FALSE)
//,m_judger( JUDGEMENT_ARRAY_SIZE )
,m_ptr_stream(0)
,m_is_exit(VO_FALSE)
,m_buffer_high_cnt(0)
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

	if( !m_ptr_stream )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
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

VO_S64 vo_largefile_buffer::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
	voCAutoLock lock( &m_lock );

	VOLOGI( "Buffer (%lld-%lld) , Request (%lld-%lld) , Start : %d , End : %d" , m_physical_start , m_physical_start + get_buffer_usedsize() , physical_pos , physical_pos + toread , m_buffer_start , m_buffer_end );

	VO_S32 usedsize = get_buffer_usedsize();

	if( physical_pos >= m_physical_start && physical_pos <= m_physical_start + usedsize )
	{
		VO_S32 coincide_size = (VO_S32)(m_physical_start + usedsize - physical_pos);
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

				VOLOGI( "Buffer Lack (%lld-%lld)" ,need.want_physical_pos , need.want_physical_pos + need.want_size );


				if( usedsize == m_buffer_size && physical_pos + toread > m_physical_start + usedsize )
				{
					reset();
				}

				BUFFER_CALLBACK_RET ret = m_ptr_buffer_callback->buffer_notify( BUFFER_LACK , &need );

				if( !issync )
					return 0;

				switch( ret )
				{
				case BUFFER_CALLBACK_ERROR:
					{
						VOLOGI("BUFFER_CALLBACK_ERROR");
						m_lock.Lock();
						return -1;
					}
				case BUFFER_CALLBACK_SHOULD_STOP:
					{
						m_lock.Lock();
						return -2;
					}
				case BUFFER_CALLBACK_OK:
					{
						;
					}
				}

				voOS_Sleep( 100 );
				m_lock.Lock();

				usedsize = get_buffer_usedsize();
				coincide_size = (VO_S32)(m_physical_start + usedsize - physical_pos);
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

			VOLOGI( "Buffer Lack (%lld-%lld)" ,need.want_physical_pos , need.want_physical_pos + need.want_size );

			BUFFER_CALLBACK_RET ret = m_ptr_buffer_callback->buffer_notify( BUFFER_LACK , &need );

			if( !issync )
				return 0;

			switch( ret )
			{
			case BUFFER_CALLBACK_ERROR:
				{
					VOLOGI("BUFFER_CALLBACK_ERROR");
					m_lock.Lock();
					return -1;
				}
			case BUFFER_CALLBACK_SHOULD_STOP:
				{
					m_lock.Lock();
					return -2;
				}
			case BUFFER_CALLBACK_OK:
				{
					;
				}
			}

			voOS_Sleep( 100 );
			m_lock.Lock();

			usedsize = get_buffer_usedsize();
		} while ( !( physical_pos >= m_physical_start && physical_pos <= m_physical_start + usedsize ) && !m_is_exit );

		if( m_is_exit )
			return -1;

		m_lock.Unlock();
		VO_S32 readsize = (VO_S32)read( physical_pos , buffer , toread );
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

		VO_S32 coincide_size = (VO_S32)(m_physical_start + usedsize - physical_pos);
		VO_S32 non_coincide_size = (VO_S32)(towrite - coincide_size);

		if( non_coincide_size <= 0 )
			return towrite;

		if( non_coincide_size < leftsize )
		{
			if( non_coincide_size >= 1 )
			{
				write_buffer( m_buffer_end , buffer + coincide_size , non_coincide_size );
				return towrite;
			}
			else
			{
				return towrite;
			}
		}
		else
		{
			write_buffer( m_buffer_end , buffer + coincide_size , leftsize );
			m_ptr_buffer_callback->buffer_notify( BUFFER_FULL , 0 );
			m_is_buffer_fulled = VO_TRUE;
			//record physical pos when buffer full.
			m_buffer_full_physical = physical_pos + leftsize;
			VOLOGI("[yoohoo]buffer_full_find");
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

	//m_judger.reset();
}

VO_VOID vo_largefile_buffer::write_buffer( VO_S64 file_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	VO_S32 first_write_size = m_buffer_realsize - m_buffer_end;

	if( towrite <= first_write_size )
	{
		m_ptr_stream->seek( file_pos , stream_begin );
		m_ptr_stream->write( buffer , (VO_S32)towrite );
		m_buffer_end = (VO_S32)(m_buffer_end + towrite);
	}
	else
	{
		m_ptr_stream->seek( file_pos , stream_begin );
		m_ptr_stream->write( buffer , first_write_size );
		m_ptr_stream->seek( 0 , stream_begin );
		m_ptr_stream->write( buffer + first_write_size , (VO_S32)(towrite - first_write_size) );
		m_buffer_end = (VO_S32)(towrite - first_write_size);
	}
}

VO_VOID vo_largefile_buffer::read_buffer( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread )
{
	VO_S32 read_pos = ( ( m_buffer_start + ( physical_pos - m_physical_start ) ) % m_buffer_realsize );
	VO_S32 first_read_size = m_buffer_realsize - read_pos;
	
	if( toread <= first_read_size )
	{
		m_ptr_stream->seek( read_pos , stream_begin );
		m_ptr_stream->read( buffer , (VO_S32)toread );
	}
	else
	{
		m_ptr_stream->seek( read_pos , stream_begin );
		m_ptr_stream->read( buffer , first_read_size );

		m_ptr_stream->seek( 0 , stream_begin );
		m_ptr_stream->read( buffer + first_read_size ,(VO_S32)(toread - first_read_size) );
	}

    //VO_S32 ref_value = (VO_S32)(m_buffer_realsize * 2./3. < m_buffer_realsize - BUFFER_FADE ? m_buffer_realsize * 2./3. :  m_buffer_realsize - BUFFER_FADE);

	//VO_S64 judgement_result;

//	if( m_judger.get_judgement( &judgement_result ) )
	{
	//	if( judgement_result - m_physical_start >= BUFFER_FADE && get_buffer_usedsize() >= ref_value )
		if( physical_pos - m_physical_start >= BUFFER_FADE )
		{
			VOLOGI("[yoohoo]adjust-buffer-fading inused buffer size : %d  m_buffer_realsize: %d", get_buffer_usedsize() , m_buffer_realsize);
			m_physical_start = m_physical_start + BUFFER_FADE/10;
			m_buffer_start = ( m_buffer_start + BUFFER_FADE/10 )%m_buffer_realsize;
		}
	}

	//we should limit download speed in dlna case, else buffer full will happen, and tcp connection will be closed.
	long slowdown = 0;
	if( get_buffer_usedsize() >= 30 * 1024 * 1024 && m_buffer_high_cnt++ > 10 )
	{
		m_buffer_high_cnt = 0;
		//try to tell download thread to slow down download speed
		//sleep 100ms every recv 2k data
		slowdown = 100;		
		m_ptr_buffer_callback->buffer_notify( BUFFER_HIGH , &slowdown );
	}

	if(get_buffer_usedsize() <= 15 * 1024 * 1024 && m_buffer_high_cnt++ > 10 )
	{
		m_buffer_high_cnt = 0;
		//try to tell download thread to resume download speed
		slowdown = 0;
		m_ptr_buffer_callback->buffer_notify( BUFFER_HIGH , &slowdown );
	}

 //	if( m_is_buffer_fulled && get_buffer_usedsize() < m_buffer_size * 2. / 3. )
	if(m_is_buffer_fulled && m_buffer_full_physical - physical_pos < m_buffer_size * 3. / 20.)
 	{
		VOLOGI("[yoohoo]buffer_low_find");
 		VO_BUFFER_NEED need;
 		need.want_physical_pos = m_physical_start + get_buffer_usedsize();
 		need.want_size = -1;
 		m_ptr_buffer_callback->buffer_notify( BUFFER_LOW , &need );
 
 		char str[1024];
 		memset( str , 0 , 1024 );
 		//int size = sprintf( str , "Buffer low! download start: %I64d\r\n" , need.want_physical_pos );
		sprintf( str , "Buffer low! download start: %lld\r\n" , need.want_physical_pos );
 		m_is_buffer_fulled = VO_FALSE;
 	}

//	m_judger.push_pos( physical_pos );
	//VOLOGI( "read_buffer : physical_pos = %d  toread = %d  start = %d  end = %d buffersize = %d " , (VO_S32)physical_pos , (VO_S32)toread , (VO_S32)m_physical_start , (VO_S32)(m_physical_start + get_buffer_usedsize()) , (VO_S32)m_buffer_size );
}

VO_VOID vo_largefile_buffer::get_buffer_info( BUFFER_INFO * ptr_info )
{
	ptr_info->physical_start = m_physical_start;
	ptr_info->usedsize = get_buffer_usedsize();
}

VO_VOID vo_largefile_buffer::get_buffering_start_info( BUFFER_INFO * ptr_info )
{
	ptr_info->physical_start = m_physical_start;
	ptr_info->usedsize = get_buffer_usedsize();
}

