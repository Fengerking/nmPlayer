//#include "StdAfx.h"
#include "vo_headerdata_buffer.h"
#include "voOSFunc.h"
#include "voLog.h"
#define LOG_TAG "voCOMXAudioSink"

vo_headerdata_buffer::vo_headerdata_buffer(void)
:m_ptr_cache(0)
,m_cache_size(0)
,m_ptr_segment_header(0)
,m_ptr_segment_tail(0)
{
}

vo_headerdata_buffer::~vo_headerdata_buffer(void)
{
	uninit();
}

VO_BOOL vo_headerdata_buffer::init( VOPDInitParam * ptr_param )
{
	uninit();

	m_ptr_cache = new VO_BYTE[ ptr_param->nMaxBuffer * 1024 ];
	m_cache_size = ptr_param->nMaxBuffer * 1024;

	return VO_TRUE;
}

VO_VOID vo_headerdata_buffer::uninit()
{
	destroy();
}

VO_S64 vo_headerdata_buffer::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread )
{
	if( physical_pos >= m_filesize && m_filesize != -1 )
		return -1;

	VOLOGE( "vo_headerdata_buffer::read()  physical_pos: %lld , toread %lld , FileSize: %lld" , physical_pos , toread , m_filesize );

	if( m_is_initmode )
	{
		voCAutoLock lock( &m_lock );

		VOLOGE( "+wait!" );
		while( !m_ptr_segment_tail )
		{
			m_lock.Unlock();
			voOS_Sleep( 100 );
			m_lock.Lock();
		}
		VOLOGE( "-wait!" );

		if( physical_pos >= m_ptr_segment_tail->physical_pos )
		{
			while( physical_pos + toread > m_ptr_segment_tail->physical_pos + m_ptr_segment_tail->size )
			{

				if( m_filesize != -1 && physical_pos + toread > m_filesize )
					toread = m_filesize - physical_pos;

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

				voOS_Sleep( 100 );
				m_lock.Lock();
			}

			memcpy( buffer , m_ptr_cache + m_ptr_segment_tail->buffer_pos + ( physical_pos - m_ptr_segment_tail->physical_pos ) , toread );
			m_ptr_segment_tail->last_read_pos = physical_pos + toread;
			return toread;
		}
		else
		{
			headerdata_segment * ptr_segment = m_ptr_segment_header;

			while( ptr_segment )
			{
				if( physical_pos >= ptr_segment->physical_pos && physical_pos + toread <= ptr_segment->physical_pos + ptr_segment->size )
				{
					memcpy( buffer , m_ptr_cache + ptr_segment->buffer_pos + ( physical_pos - ptr_segment->physical_pos ) , toread );
					ptr_segment->last_read_pos = physical_pos + toread;
					return toread;
				}
				ptr_segment = ptr_segment->ptr_next;
			}

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

				voOS_Sleep( 100 );
				m_lock.Lock();
			} while ( physical_pos < m_ptr_segment_tail->physical_pos );

			m_lock.Unlock();
			VO_S32 readed = read( physical_pos , buffer , toread );
			m_lock.Lock();
			return readed;
		}
	}
	else
	{
		headerdata_segment * ptr_segment = m_ptr_segment_header;

		while( ptr_segment )
		{
			if( physical_pos >= ptr_segment->physical_pos && physical_pos + toread <= ptr_segment->physical_pos + ptr_segment->size )
			{
				memcpy( buffer , m_ptr_cache + ptr_segment->buffer_pos + ( physical_pos - ptr_segment->physical_pos ) , toread );
				ptr_segment->last_read_pos = physical_pos + toread;
				return toread;
			}
			ptr_segment = ptr_segment->ptr_next;
		}
	}

	return 0;
}

VO_S64 vo_headerdata_buffer::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	if( m_is_initmode )
	{
		voCAutoLock lock( &m_lock );
		if( m_ptr_segment_tail == NULL )
		{
			m_ptr_segment_header = m_ptr_segment_tail = new headerdata_segment();
			m_ptr_segment_header->physical_pos = physical_pos;
			m_ptr_segment_header->last_read_pos = physical_pos;
			m_ptr_segment_header->buffer_pos = 0;
			m_ptr_segment_header->ptr_next = NULL;
			m_ptr_segment_header->size = towrite;

			memcpy( m_ptr_cache , buffer , towrite );

			return towrite;
		}
		else
		{
			if( m_ptr_segment_tail->physical_pos < physical_pos && m_ptr_segment_tail->physical_pos + m_ptr_segment_tail->size >= physical_pos )
			{
				VO_S32 physicalwritesize = towrite - ( m_ptr_segment_tail->physical_pos + m_ptr_segment_tail->size - physical_pos );
				VO_S32 cache_leftsize_afterwrite =  m_cache_size - ( m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size ) - physicalwritesize;
				VO_S32 writen = towrite;

				if( physicalwritesize < 0 )
					return 0;

				if( cache_leftsize_afterwrite + physicalwritesize == 0 )
					return 0;

				if( cache_leftsize_afterwrite > 0 )
				{
					memcpy( m_ptr_cache + m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size , buffer + ( towrite - physicalwritesize ) , physicalwritesize );
					m_ptr_segment_tail->size = m_ptr_segment_tail->size + physicalwritesize;
					return writen;
				}
				else
				{
					memcpy( m_ptr_cache + m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size , buffer + ( towrite - physicalwritesize ) , physicalwritesize + cache_leftsize_afterwrite );
					physicalwritesize = physicalwritesize + cache_leftsize_afterwrite;
					m_ptr_segment_tail->size = m_ptr_segment_tail->size + physicalwritesize;

					m_ptr_buffer_callback->buffer_notify( BUFFER_FULL , NULL );
					return writen + cache_leftsize_afterwrite;
				}
			}
			else
			{
				scan_to_savespace();

				VO_S32 cache_leftsize_afterwrite =  m_cache_size - ( m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size ) - towrite;
				VO_S32 writen = towrite;

				if( cache_leftsize_afterwrite + towrite == 0 )
					return 0;

				headerdata_segment * ptr_segment = new headerdata_segment();
				ptr_segment->physical_pos = physical_pos;
				ptr_segment->buffer_pos = m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size;
				ptr_segment->last_read_pos = physical_pos;
				ptr_segment->ptr_next = NULL;

				if( cache_leftsize_afterwrite > 0 )
				{
					memcpy( m_ptr_cache + m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size , buffer , towrite );
					writen = ptr_segment->size = towrite;
				}
				else
				{
					memcpy( m_ptr_cache + m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size , buffer , towrite + cache_leftsize_afterwrite );
					writen = ptr_segment->size = towrite + cache_leftsize_afterwrite;

					m_ptr_buffer_callback->buffer_notify( BUFFER_FULL , NULL );
				}

				m_ptr_segment_tail->ptr_next = ptr_segment;
				m_ptr_segment_tail = ptr_segment;

				return writen;
			}
		}
	}
	else
		return 0;
}

VO_S64 vo_headerdata_buffer::seek( VO_S64 physical_pos )
{
	return 0;
}

VO_VOID vo_headerdata_buffer::reset()
{
	;
}

VO_VOID vo_headerdata_buffer::set_initmode( VO_BOOL isset )
{
	m_is_initmode = isset;
}

VO_VOID vo_headerdata_buffer::scan_to_savespace()
{
	if( m_ptr_segment_tail )
	{
		VO_S32 readsize = m_ptr_segment_tail->last_read_pos - m_ptr_segment_tail->physical_pos;
		m_ptr_segment_tail->size = readsize;
	}
}

VO_BOOL vo_headerdata_buffer::rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number , vo_buffer * ptr_buffer )
{
	sort_segment();

	if( ptr_buffer )
	{
		reuse_cache( ptr_buffer );
	}

	if( info_number == 0 )
	{
		destroy();
		return VO_TRUE;
	}

	VO_S32 totalsize = 0;

	for( VO_S32 i = 0 ; i < info_number ; i++ )
	{
		totalsize = totalsize + ptr_info[i].size;
	}

	VO_PBYTE ptr_temp = new VO_BYTE[totalsize];
	headerdata_segment * m_ptr_temp_header = 0;
	headerdata_segment * m_ptr_temp_tail = 0;

	VO_S32 buffer_pos = 0;

	for( VO_S32 i = 0 ; i < info_number ; i++ )
	{
		headerdata_segment * ptr_segment = m_ptr_segment_header;

		while( ptr_segment )
		{
			if( ptr_info[i].physical_pos >= ptr_segment->physical_pos && ptr_info[i].physical_pos <= ptr_segment->physical_pos + ptr_segment->size )
			{
				headerdata_segment * ptr_new_segment = new headerdata_segment();
				ptr_new_segment->buffer_pos = buffer_pos;
				ptr_new_segment->physical_pos = ptr_info[i].physical_pos;
				ptr_new_segment->ptr_next = 0;
				
				VO_S32 cpysize = ( ptr_info[i].physical_pos + ptr_info[i].size < ptr_segment->physical_pos + ptr_segment->size ? ptr_info[i].physical_pos + ptr_info[i].size : ptr_segment->physical_pos + ptr_segment->size ) - ptr_info[i].physical_pos;
				memcpy( ptr_temp + buffer_pos , m_ptr_cache + ptr_segment->buffer_pos + ( ptr_info[i].physical_pos - ptr_segment->physical_pos ) , cpysize );

				ptr_new_segment->size = cpysize;

				ptr_segment = ptr_segment->ptr_next;

				VO_S32 leftsize = ptr_info[i].size - ptr_new_segment->size;

				while( ptr_segment )
				{
					if( ptr_segment->physical_pos == ptr_new_segment->physical_pos + ptr_new_segment->size )
					{
						cpysize = ptr_segment->size < leftsize ? ptr_segment->size : leftsize;
						memcpy( ptr_temp + ptr_new_segment->size , m_ptr_cache + ptr_segment->buffer_pos , cpysize );
						ptr_new_segment->size = ptr_new_segment->size + cpysize;

						leftsize = leftsize - cpysize;

						if( leftsize <= 0 )
							break;
					}
					else
					{
						break;
					}
					ptr_segment = ptr_segment->ptr_next;
				}

				if( m_ptr_temp_tail == 0 )
				{
					m_ptr_temp_header = m_ptr_temp_tail = ptr_new_segment;
				}
				else
				{
					m_ptr_temp_tail->ptr_next = ptr_new_segment;
					m_ptr_temp_tail = ptr_new_segment;
				}

				buffer_pos += ptr_info[i].size;
				break;
			}

			ptr_segment = ptr_segment->ptr_next;
		}
	}

	destroy();

	m_ptr_cache = ptr_temp;
	m_cache_size = totalsize;
	m_ptr_segment_header = m_ptr_temp_header;
	m_ptr_segment_tail = m_ptr_temp_tail;

	headerdata_segment * ptr_segment = m_ptr_segment_header;

	while( ptr_segment )
	{
		VO_S32 actaulsize = ptr_segment->ptr_next ? ptr_segment->ptr_next->buffer_pos - ptr_segment->buffer_pos : m_cache_size - ptr_segment->buffer_pos;

		if( ptr_segment->size < actaulsize )
		{
			VO_BUFFER_NEED cache;
			cache.want_size = actaulsize - ptr_segment->size;
			cache.want_physical_pos = ptr_segment->physical_pos + ptr_segment->size;

			m_ptr_buffer_callback->buffer_notify( BUFFER_GET_CACHE , &cache );
		}

		ptr_segment = ptr_segment->ptr_next;
	}

	ptr_segment = m_ptr_segment_header;

	while( ptr_segment )
	{
		VO_S32 actaulsize = ptr_segment->ptr_next ? ptr_segment->ptr_next->buffer_pos - ptr_segment->buffer_pos : m_cache_size - ptr_segment->buffer_pos;

		if( ptr_segment->size < actaulsize )
		{
			return VO_FALSE;
		}

		ptr_segment = ptr_segment->ptr_next;
	}

	return VO_TRUE;
}

VO_VOID vo_headerdata_buffer::destroy()
{
	if( m_ptr_cache )
	{
		delete []m_ptr_cache;
		m_ptr_cache = 0;
	}

	m_cache_size = 0;

	headerdata_segment * ptr_segment = m_ptr_segment_header;

	while( ptr_segment )
	{
		headerdata_segment * ptr_temp = ptr_segment;
		ptr_segment = ptr_segment->ptr_next;
		delete ptr_temp;
	}

	m_ptr_segment_header = m_ptr_segment_tail = 0;
}

VO_VOID vo_headerdata_buffer::sort_segment()
{
	headerdata_segment * ptr_temp_header = NULL;
	headerdata_segment * ptr_temp_tail = NULL;

	headerdata_segment * ptr_segment = m_ptr_segment_header;

	while( ptr_segment )
	{
		headerdata_segment * ptr_next = ptr_segment->ptr_next;
		ptr_segment->ptr_next = NULL;

		if( ptr_temp_tail == NULL )
			ptr_temp_header = ptr_temp_tail = ptr_segment;
		else
		{
			headerdata_segment * ptr_temp_segment = ptr_temp_header;
			headerdata_segment * ptr_pre_temp_segment = ptr_temp_header;

			while( ptr_temp_segment )
			{
				if( ptr_temp_segment->physical_pos > ptr_segment->physical_pos )
				{
					if( ptr_pre_temp_segment == ptr_temp_segment )
					{
						ptr_temp_header = ptr_segment;
						ptr_temp_header->ptr_next = ptr_temp_segment;
					}
					else
					{
						ptr_pre_temp_segment->ptr_next = ptr_segment;
						ptr_segment->ptr_next = ptr_temp_segment;
					}
					break;
				}

				ptr_pre_temp_segment = ptr_temp_segment;
				ptr_temp_segment = ptr_temp_segment->ptr_next;
			}

			if( !ptr_temp_segment )
			{
				ptr_temp_tail->ptr_next = ptr_segment;
				ptr_temp_tail = ptr_segment;
			}
		}

		ptr_segment = ptr_next;
	}

	m_ptr_segment_header = ptr_temp_header;
	m_ptr_segment_tail = ptr_temp_tail;
}

VO_VOID vo_headerdata_buffer::reuse_cache( vo_buffer * ptr_buffer )
{
	if( m_ptr_segment_header->physical_pos != 0 )
		return;

	ptr_buffer->reset();
	headerdata_segment * ptr_segment = m_ptr_segment_header;

	while( ptr_segment )
	{
		BUFFER_INFO info;
		ptr_buffer->get_buffer_info( &info );
		if( ptr_segment->physical_pos == info.physical_start + info.usedsize )
		{
			ptr_buffer->write( ptr_segment->physical_pos , m_ptr_cache + ptr_segment->buffer_pos , ptr_segment->size );
		}
		else
		{
			break;
		}

		ptr_segment = ptr_segment->ptr_next;
	}
}
