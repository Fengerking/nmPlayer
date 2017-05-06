#include "http_buffer_stream.h"
#include "voOSFunc.h"
#include "log.h"

voCMutex http_buffer_stream::m_id_lock;
VO_S32 http_buffer_stream::m_id_stub = 0;

http_buffer_stream::http_buffer_stream( page_allocator * ptr_allocator , VO_S32 page_size , VO_S64 physical_start , VO_CHAR * str_url , VOPDInitParam * ptr_PD_param )
:m_buffer( ptr_allocator , page_size , physical_start )
,m_physical_start(physical_start)
,m_endboundary(-1)
{
	m_id = create_id();

	m_buffer.set_buffer_callback( this );
	m_downloader.set_download_callback( this );

	m_downloader.set_url( str_url , ptr_PD_param );
}

http_buffer_stream::~http_buffer_stream(void)
{
	m_downloader.stop_download();
}

VO_BOOL http_buffer_stream::start()
{
	return m_downloader.start_download( m_physical_start );
}

BUFFER_CALLBACK_RET http_buffer_stream::buffer_lack( VO_S64 lack_pos , VO_S64 lack_size )
{
	VO_DOWNLOAD_INFO info;
	m_downloader.get_dowloadinfo( &info );

	PRINT_LOG( LOG_LEVEL_NORMAL , "Buffer lack! Lack pos: %lld , Lack size: %lld , Cur download start: %lld , Cur download pos: %lld" , lack_pos , lack_size , info.start_download_pos , info.cur_download_pos );

	if( info.cur_download_pos + 3 * 1024 * 1024 < lack_pos )
	{
		m_ptr_callback->need_new_bufferstream( get_id() , lack_pos );
		return BUFFER_CALLBACK_DATA_LACKTOOMUCH;
	}
	else
	{
		if( !m_downloader.is_downloading() )
		{
			if( m_buffer.is_buffer_full() )
			{
				m_ptr_callback->need_new_bufferstream( get_id() , lack_pos );
				return BUFFER_CALLBACK_DATA_LACKTOOMUCH;
			}
			else
			{
				m_downloader.start_download( info.cur_download_pos );
			}
		}
	}

	voOS_Sleep( 100 );

	return BUFFER_CALLBACK_OK;
}

BUFFER_CALLBACK_RET http_buffer_stream::buffer_low( VO_S64 startdownload_pos )
{
	PRINT_LOG( LOG_LEVEL_IMPORTANT , "Buffer low, Start download from %lld" , startdownload_pos );
	m_downloader.stop_download();
	m_downloader.start_download( startdownload_pos );
	return BUFFER_CALLBACK_OK;
}

CALLBACK_RET http_buffer_stream::received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size )
{
	VO_S64 writesize = m_buffer.write( physical_pos , ptr_buffer , size );

	if( writesize == -1 )
		return CALLBACK_BUFFER_WRITEFAIL;

	if( writesize == -2 )
		return CALLBACK_OK;

	if( writesize < size )
		return CALLBACK_BUFFER_FULL;
}

CALLBACK_RET http_buffer_stream::download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data )
{
	switch ( id )
	{
	case DOWNLOAD_FILESIZE:
		{
			VO_S64 * ptr_size = (VO_S64 *)ptr_data;
			m_buffer.set_filesize( *ptr_size );
			m_ptr_callback->set_filesize( *ptr_size );
		}
		break;
	}
	return CALLBACK_OK;
}

void http_buffer_stream::set_endboundary( VO_S64 end )
{
	m_buffer.set_endboundary( end );
	m_endboundary = end;
}

VO_S64 http_buffer_stream::read( VO_S64 pos , VO_PBYTE ptr_buffer , VO_S64 size )
{
	return m_buffer.read( pos , ptr_buffer , size );
}

VO_BOOL http_buffer_stream::hit_test( VO_S64 readpos )
{
	if( m_endboundary == -1 )
		return readpos >= m_physical_start ? VO_TRUE : VO_FALSE;
	else
	{
		return ( (readpos >= m_physical_start) && (readpos <= m_endboundary) ) ? VO_TRUE : VO_FALSE;
	}
}

VO_VOID http_buffer_stream::set_initmode( VO_BOOL is_init )
{
	m_buffer.set_recyclebuffer( is_init ? VO_FALSE : VO_TRUE );
}

VO_S32 http_buffer_stream::create_id()
{
	voCAutoLock lock( &m_id_lock );
	return m_id_stub++;
}
