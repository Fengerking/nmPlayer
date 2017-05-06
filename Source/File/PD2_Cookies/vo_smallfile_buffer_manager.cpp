
#include "vo_smallfile_buffer_manager.h"
#include "vo_mem_stream.h"
#include "vo_file_stream.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_smallfile_buffer_manager::vo_smallfile_buffer_manager()
:m_ptr_segment_begin(0)
,m_ptr_segment_end(0)
,m_ptr_stream(0)
,m_is_exit(VO_FALSE)
,m_is_alldata_downloaded(VO_FALSE)
,m_download_pos(0)
,m_notify_freq(0)
,m_last_time(0)
{
	memset( m_tempfile , 0 , sizeof( m_tempfile ) );
}

vo_smallfile_buffer_manager::~vo_smallfile_buffer_manager()
{
	uninit();
}

VO_BOOL vo_smallfile_buffer_manager::init( VOPDInitParam * ptr_param )
{
	uninit();

	m_is_exit = VO_FALSE;

	if( ptr_param->nTempFileLocation == 0 ) //memory
	{
		m_ptr_stream = new vo_mem_stream();
	}
	else
	{
		m_ptr_stream = new vo_file_stream();

		if( sizeof(TCHAR) != 1 )
		{
			wcstombs(m_tempfile,(wchar_t*)ptr_param->mFilePath,sizeof(m_tempfile));
		}
	}

	if( !m_ptr_stream )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_VOID vo_smallfile_buffer_manager::uninit()
{
	m_is_exit = VO_TRUE;

	BUFFER_SEGMENT * ptr_seg = m_ptr_segment_begin;

	while( ptr_seg )
	{
		BUFFER_SEGMENT * ptr_temp = ptr_seg;
		ptr_seg = ptr_seg->ptr_next;

		delete ptr_temp;
	}

	m_ptr_segment_begin = m_ptr_segment_end = 0;

	if( m_ptr_stream )
	{
		delete m_ptr_stream;
		m_ptr_stream = 0;
	}
}

VO_S64 vo_smallfile_buffer_manager::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
	voCAutoLock lock( &m_lock );
	VO_S64 readed = 0;

	VOLOGI("vo_smallfile_buffer_manager::read() %lld   %lld" , physical_pos , toread );

	BUFFER_SEGMENT * ptr_seg = get_segment( physical_pos );

	if( ptr_seg )
	{
		VO_S64 canread = ptr_seg->physical_pos + ptr_seg->size - physical_pos;

		if( canread >= toread )
		{
			m_ptr_stream->seek( physical_pos , stream_begin );
			readed = m_ptr_stream->read( buffer , (VO_S32)toread );
		}
		else
		{
			do 
			{
				m_lock.Unlock();

				VO_BUFFER_NEED need;
				need.want_physical_pos = physical_pos;
				need.want_size = toread;

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

				voOS_Sleep( 200 );

				m_lock.Lock();

				ptr_seg = get_segment( physical_pos );
				canread = ptr_seg->physical_pos + ptr_seg->size - physical_pos;

			} while ( (canread < toread) && !m_is_exit );

			if( m_is_exit )
				return -1;

			m_ptr_stream->seek( physical_pos , stream_begin );
			readed = m_ptr_stream->read( buffer , (VO_S32)toread );
		}
	}
	else
	{
		do 
		{
			m_lock.Unlock();

			VO_BUFFER_NEED need;
			need.want_physical_pos = physical_pos;
			need.want_size = toread;
			
			BUFFER_CALLBACK_RET ret = m_ptr_buffer_callback->buffer_notify( BUFFER_LACK , &need );

			if( !issync )
				return 0;

			switch( ret )
			{
			case BUFFER_CALLBACK_ERROR:
				{
					VOLOGI( "BUFFER_CALLBACK_ERROR");
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

			voOS_Sleep( 200 );

			m_lock.Lock();
		} while ( !get_segment( physical_pos ) && !m_is_exit );

		if( m_is_exit )
			return -1;

		m_lock.Unlock();
		readed = read( physical_pos , buffer , toread );
		m_lock.Lock();
	}

	//if data has not been downloaded totally, we should try to start the download thread.for issue 10705
	if( m_notify_freq++ > 100 && !m_is_alldata_downloaded)
	{
		//we should not notify start download too frequently, else it will cause double download
		VO_U32 cur_time = voOS_GetSysTime();
		if( cur_time - m_last_time > 3000 )
		{
			m_last_time = cur_time;	
			VOLOGI("[dongy]notify buffer low, and pos: %lld" , m_download_pos );

			m_notify_freq = 0;

			VO_BUFFER_NEED need;
			need.want_physical_pos = m_download_pos;
			need.want_size = -1;
			m_ptr_buffer_callback->buffer_notify( BUFFER_LOW , &need );
		}
	}
	
	return readed;
}

VO_S64 vo_smallfile_buffer_manager::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	voCAutoLock lock( &m_lock );
	BUFFER_SEGMENT * ptr_seg = get_segment( physical_pos );

	if( ptr_seg )
	{
		VO_S64 actualwritesize = physical_pos + towrite - ptr_seg->physical_pos - ptr_seg->size;

		if( actualwritesize > 0 )
		{
			ptr_seg->size = ptr_seg->size + actualwritesize;

			m_ptr_stream->seek( physical_pos + towrite - actualwritesize , stream_begin );
			m_ptr_stream->write( buffer + towrite - actualwritesize , (VO_S32)actualwritesize );

			combine_segment( ptr_seg );
		}
	}
	else
	{
		ptr_seg = new BUFFER_SEGMENT();
		if( !ptr_seg )
		{
			VOLOGE( "new obj failed" );
			return 0;
		}
		ptr_seg->physical_pos = physical_pos;
		ptr_seg->size = towrite;
		ptr_seg->ptr_next = ptr_seg->ptr_pre = 0;

		m_ptr_stream->seek(  physical_pos , stream_begin );
		m_ptr_stream->write( buffer , (VO_S32)towrite );

		combine_segment( ptr_seg , VO_TRUE );
	}

	if( is_alldata_downloaded() )
	{
		m_is_alldata_downloaded = VO_TRUE;
	}
	else
	{
		m_notify_freq = 0;
	}

	return towrite;
}

VO_S64 vo_smallfile_buffer_manager::seek( VO_S64 physical_pos )
{
	m_curpos = physical_pos;
	return physical_pos;
}

VO_VOID vo_smallfile_buffer_manager::reset()
{
	;
}

VO_VOID vo_smallfile_buffer_manager::get_buffer_info( BUFFER_INFO * ptr_info )
{
	if( m_ptr_segment_begin && m_ptr_segment_begin->physical_pos == 0 )
	{
		BUFFER_SEGMENT  * ptr_seg = get_segment( m_curpos );
		if( ptr_seg )
		{
			ptr_info->physical_start = ptr_seg->physical_pos;
			ptr_info->usedsize = ptr_seg->size;
		}
		else
		{
			ptr_info->physical_start = m_ptr_segment_begin->physical_pos;
			ptr_info->usedsize = m_ptr_segment_begin->size;
		}
	}
	else
	{
		ptr_info->physical_start = 0;
		ptr_info->usedsize = 0;
	}
}

VO_VOID vo_smallfile_buffer_manager::get_buffering_start_info( BUFFER_INFO * ptr_info )
{
	if( m_ptr_segment_begin && m_ptr_segment_begin->physical_pos == 0 )
	{
		ptr_info->physical_start = m_ptr_segment_begin->physical_pos;
		ptr_info->usedsize = m_ptr_segment_begin->size;
	}
	else
	{
		ptr_info->physical_start = 0;
		ptr_info->usedsize = 0;
	}
}

VO_VOID vo_smallfile_buffer_manager::set_filesize( VO_S64 filesize )
{
	m_filesize = filesize;
	m_ptr_stream->open( m_tempfile , (VO_S32)m_filesize );
}


VO_BOOL	vo_smallfile_buffer_manager::is_alldata_downloaded()
{
	BUFFER_SEGMENT * ptr_seg = m_ptr_segment_begin;
	
	VO_S64 totalsize = 0;
	VO_S64 pos = m_filesize;

	while( ptr_seg )
	{
		if( ptr_seg->physical_pos >= 0 &&  ptr_seg->size >= 0 )
		{
			totalsize += ptr_seg->size;
			if( ptr_seg->physical_pos + ptr_seg->size <= pos)
			{
				pos = ptr_seg->physical_pos + ptr_seg->size;
			}
		}
		ptr_seg = ptr_seg->ptr_next;
	}

	if( totalsize >= m_filesize)
	{
		return VO_TRUE;
	}

	m_download_pos = pos;

	return VO_FALSE;
}

BUFFER_SEGMENT * vo_smallfile_buffer_manager::get_segment( VO_S64 physical_pos )
{
	BUFFER_SEGMENT * ptr_seg = m_ptr_segment_begin;

	while( ptr_seg )
	{
		if( physical_pos >= ptr_seg->physical_pos && physical_pos <= ptr_seg->physical_pos + ptr_seg->size )
		{
			return ptr_seg;
		}

		ptr_seg = ptr_seg->ptr_next;
	}

	return NULL;
}

VO_VOID vo_smallfile_buffer_manager::combine_segment( BUFFER_SEGMENT * ptr_segment , VO_BOOL new_segment/* = VO_FALSE*/ )
{
	if( new_segment )
		add_segment( ptr_segment );

	//determine pre
	//it seems we will not use this
	//

	//determine next
	BUFFER_SEGMENT * ptr_next_seg = ptr_segment->ptr_next;

	if( ptr_next_seg )
	{
		VO_S64 end = ptr_segment->physical_pos + ptr_segment->size;

		BUFFER_SEGMENT * ptr_end_seg = ptr_next_seg;

		while( ptr_end_seg && end >= ptr_end_seg->physical_pos )
		{
			ptr_end_seg = ptr_end_seg->ptr_next;
		}

		BUFFER_SEGMENT * ptr_last_include_seg;
		
		if( ptr_end_seg )
			ptr_last_include_seg = ptr_end_seg->ptr_pre;
		else
			ptr_last_include_seg = m_ptr_segment_end;

		ptr_segment->size = ptr_last_include_seg->physical_pos + ptr_last_include_seg->size - ptr_segment->physical_pos;

		while( ptr_next_seg != ptr_end_seg )
		{
			BUFFER_SEGMENT * ptr_temp = ptr_next_seg;
			ptr_next_seg = ptr_next_seg->ptr_next;
			delete ptr_temp;
		}

		ptr_segment->ptr_next = ptr_end_seg;

		if( ptr_end_seg )
			ptr_end_seg->ptr_pre = ptr_segment;	
		else
		{
			m_ptr_segment_end = ptr_segment;
		}
	}
	//
}

VO_VOID vo_smallfile_buffer_manager::add_segment( BUFFER_SEGMENT * ptr_segment )
{
	BUFFER_SEGMENT * ptr_seg = m_ptr_segment_begin;

	while( ptr_seg )
	{
		if( ptr_segment->physical_pos < ptr_seg->physical_pos )
		{
			if( ptr_seg->ptr_pre )
			{
				ptr_segment->ptr_pre = ptr_seg->ptr_pre;
				ptr_segment->ptr_next = ptr_seg;
				ptr_seg->ptr_pre->ptr_next = ptr_segment;
				ptr_seg->ptr_pre = ptr_segment;
			}
			else
			{
				m_ptr_segment_begin = ptr_segment;
				ptr_segment->ptr_next = ptr_seg;
				ptr_seg->ptr_pre = ptr_segment;
			}

			break;
		}

		ptr_seg = ptr_seg->ptr_next;
	}

	if( !ptr_seg )
	{
		ptr_segment->ptr_pre = m_ptr_segment_end;
		
		if( m_ptr_segment_end )
			m_ptr_segment_end->ptr_next = ptr_segment;

		m_ptr_segment_end = ptr_segment;

		if( !m_ptr_segment_begin )
			m_ptr_segment_begin = ptr_segment;
	}
}



