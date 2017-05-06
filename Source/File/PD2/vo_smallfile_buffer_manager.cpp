
#include "vo_smallfile_buffer_manager.h"
#include "vo_mem_stream.h"
#include "vo_file_stream.h"
#include "voOSFunc.h"
#include "voLog.h"

#define LOG_TAG "voCOMXAudioSink"

vo_smallfile_buffer_manager::vo_smallfile_buffer_manager()
:m_ptr_segment_begin(0)
,m_ptr_segment_end(0)
,m_ptr_stream(0)
,m_is_exit(VO_FALSE)
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

VO_S64 vo_smallfile_buffer_manager::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread )
{
	voCAutoLock lock( &m_lock );
	VO_S64 readed = 0;

	VOLOGI( "vo_smallfile_buffer_manager::read() %d   %d" , (VO_S32)physical_pos , (VO_S32)toread );

	BUFFER_SEGMENT * ptr_seg = get_segment( physical_pos );

	if( ptr_seg )
	{
		VO_S64 canread = ptr_seg->physical_pos + ptr_seg->size - physical_pos;

		if( canread >= toread )
		{
			m_ptr_stream->seek( physical_pos , stream_begin );
			readed = m_ptr_stream->read( buffer , toread );
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

				voOS_Sleep( 200 );

				m_lock.Lock();

				ptr_seg = get_segment( physical_pos );
				canread = ptr_seg->physical_pos + ptr_seg->size - physical_pos;

			} while ( (canread < toread) && !m_is_exit );

			if( m_is_exit )
				return -1;

			m_ptr_stream->seek( physical_pos , stream_begin );
			readed = m_ptr_stream->read( buffer , toread );
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

			voOS_Sleep( 200 );

			m_lock.Lock();
		} while ( !get_segment( physical_pos ) && !m_is_exit );

		if( m_is_exit )
			return -1;

		m_lock.Unlock();
		readed = read( physical_pos , buffer , toread );
		m_lock.Lock();
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
			m_ptr_stream->write( buffer + towrite - actualwritesize , actualwritesize );

			combine_segment( ptr_seg );
		}
	}
	else
	{
		ptr_seg = new BUFFER_SEGMENT();
		ptr_seg->physical_pos = physical_pos;
		ptr_seg->size = towrite;
		ptr_seg->ptr_next = ptr_seg->ptr_pre = 0;

		m_ptr_stream->seek(  physical_pos , stream_begin );
		m_ptr_stream->write( buffer , towrite );

		combine_segment( ptr_seg , VO_TRUE );
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

VO_VOID vo_smallfile_buffer_manager::set_filesize( VO_S64 filesize )
{
	m_filesize = filesize;
	m_ptr_stream->open( m_tempfile , m_filesize );
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



