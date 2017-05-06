//#include "StdAfx.h"
#include "vo_headerdata_buffer.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_headerdata_buffer::vo_headerdata_buffer(void)
:m_ptr_cache(0)
,m_cache_size(0)
,m_ptr_segment_header(0)
,m_ptr_segment_tail(0)
,m_is_reusing_cache( VO_FALSE )
,m_static_last_time(0)
,m_static_last_inused_buffer(0)
,m_is_buffer_controled(VO_FALSE)
{
}

vo_headerdata_buffer::~vo_headerdata_buffer(void)
{
	uninit();
}

VO_BOOL vo_headerdata_buffer::init( VOPDInitParam * ptr_param )
{
	uninit();

	//limit the header buffer size to 16MB or less
	int nMaxBuffer = ptr_param->nMaxBuffer;
	nMaxBuffer = nMaxBuffer > 16400 ? 16400 : nMaxBuffer;

	memcpy(&m_pd_param, ptr_param, sizeof(m_pd_param));
	m_pd_param.nMaxBuffer = nMaxBuffer;
		
	m_ptr_cache = new VO_BYTE[ nMaxBuffer * 1024 ];
	if( !m_ptr_cache )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
	m_cache_size = nMaxBuffer * 1024;

//	m_ptr_cache = new VO_BYTE[ ptr_param->nMaxBuffer * 1024 ];
//	m_cache_size = ptr_param->nMaxBuffer * 1024;

	return VO_TRUE;
}

VO_VOID vo_headerdata_buffer::uninit()
{
	destroy();
}

//when in init mode, we should call phaseI_read to read data from header buffer
VO_S64 vo_headerdata_buffer::phaseI_read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
	voCAutoLock lock( &m_lock );

	while( !m_is_closed )
	{
		VO_S64 readsize = basic_read( physical_pos , buffer , toread , issync );
		if( readsize > 0 )
		{
			return readsize;
		}

		m_lock.Unlock();
		VO_BUFFER_NEED need;
		need.want_physical_pos = physical_pos;
		need.want_size = toread;

		BUFFER_CALLBACK_RET ret = m_ptr_buffer_callback->buffer_notify( BUFFER_LACK , &need );

		switch( ret )
		{
		case BUFFER_CALLBACK_ERROR:
			{
				VOLOGE("BUFFER_CALLBACK_ERROR" );
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
	} 

	return 0;
}

//when not in init mode, we should call phaseII_read to read data from header buffer
VO_S64 vo_headerdata_buffer::phaseII_read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
	voCAutoLock lock( &m_lock );
	//if it is not in init mode, then we should try to tell download thread to resume download speed
	if( m_is_buffer_controled )
	{
		long slowdown = 0;
		m_ptr_buffer_callback->buffer_notify( BUFFER_HEADER_CONTROL , &slowdown );
		m_is_buffer_controled = VO_FALSE;
	}

	return basic_read( physical_pos , buffer , toread , issync );
}

VO_S64 vo_headerdata_buffer::basic_read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
	if( m_is_initmode )
	{
		while( !m_ptr_segment_tail )
		{
			m_lock.Unlock();
			if (m_is_closed == VO_TRUE) 
			{
				m_lock.Lock();
				VOLOGI("closed when query the m_ptr_segment_tail");
				return 0;
			}
			voOS_Sleep( 100 );
			m_lock.Lock();
		}
	}

	headerdata_segment * ptr_segment = m_ptr_segment_header;

	while( ptr_segment )
	{
		if( physical_pos >= ptr_segment->physical_pos && physical_pos + toread <= ptr_segment->physical_pos + ptr_segment->size )
		{	
			memcpy( buffer , m_ptr_cache + ptr_segment->buffer_pos + ( physical_pos - ptr_segment->physical_pos ) , (size_t)toread );
			ptr_segment->last_read_pos = physical_pos + toread;
			return toread;
		}
		ptr_segment = ptr_segment->ptr_next;
	}

	return 0;
}

VO_S64 vo_headerdata_buffer::read( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 toread , VO_BOOL issync )
{
	if( physical_pos >= m_filesize && m_filesize != -1 )
	{
		VOLOGE("[moses]physical_pos: %lld , m_filesize: %lld,  toread %lld" , physical_pos , m_filesize , toread );
		return -1;
	}

	VOLOGI("physical_pos: %lld , toread %lld" , physical_pos , toread );

	if( m_is_initmode )
	{
		return phaseI_read( physical_pos , buffer , toread , issync );
	}
	else
	{
		return phaseII_read( physical_pos , buffer , toread , issync );
	}

	return 0;
}


VO_S64 vo_headerdata_buffer::write( VO_S64 physical_pos , VO_PBYTE buffer , VO_S64 towrite )
{
	//VOLOGI( "vo_headerdata_buffer::write pos: %lld , size: %lld" , physical_pos , towrite );
	if( m_is_initmode )
	{
		//check current buffer usage,and if the consume speed is too high , we will limit the download speed,
		// since it will cause a lot of problem when header buffer is full.
		check_control_buffer_usage();

		voCAutoLock lock( &m_lock );
		
		if( m_ptr_segment_tail == NULL )
		{
			m_ptr_segment_tail = new headerdata_segment();
			if( !m_ptr_segment_tail )
			{
				VOLOGE( "new obj failed" );
				return 0;
			}
			m_ptr_segment_header = m_ptr_segment_tail;
			m_ptr_segment_header->physical_pos = physical_pos;
			m_ptr_segment_header->last_read_pos = physical_pos;
			m_ptr_segment_header->buffer_pos = 0;
			m_ptr_segment_header->ptr_next = NULL;
			m_ptr_segment_header->size = (VO_S32)towrite;

			if( towrite < m_cache_size )
			{
				memcpy( m_ptr_cache , buffer , (size_t)towrite );
			}
			else
			{
				VOLOGE( "size :%lld is invalid to write" , towrite );
				return 0;
			}

			return towrite;
		}
		else
		{	
			VO_S32 cache_leftsize_afterwrite =  (VO_S32)(m_cache_size - ( m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size ) - towrite);

			if( cache_leftsize_afterwrite < 0 )
			{
				m_ptr_buffer_callback->buffer_notify( BUFFER_FULL , NULL );
				return 0;
			}

			if( m_ptr_segment_tail->physical_pos + m_ptr_segment_tail->size == physical_pos )
			{
				//the data to write is continually of former written data
				memcpy( m_ptr_cache + m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size , buffer , (size_t)towrite );
				m_ptr_segment_tail->size += (VO_S32)towrite;
				return towrite;
			}
			else
			{
				//the data to write is not continually of former written data
				headerdata_segment * ptr_segment = new headerdata_segment();
				if( !ptr_segment )
				{
					VOLOGE( "new obj failed" );
					return 0;
				}
				ptr_segment->physical_pos = physical_pos;
				ptr_segment->buffer_pos = m_ptr_segment_tail->buffer_pos + m_ptr_segment_tail->size;
				ptr_segment->last_read_pos = physical_pos;

				memcpy( m_ptr_cache + ptr_segment->buffer_pos , buffer , (size_t)towrite );
				ptr_segment->size = (VO_S32)towrite;

				ptr_segment->ptr_next = NULL;
				m_ptr_segment_tail->ptr_next = ptr_segment;
				m_ptr_segment_tail = ptr_segment;

				return towrite;
			}
		}
	}
	else
	{
		return 0;
	}
}

VO_S64 vo_headerdata_buffer::seek( VO_S64 physical_pos )
{
	return 0;
}

VO_VOID vo_headerdata_buffer::reset()
{
	voCAutoLock lock( &m_lock );
	VOLOGE("vo_headerdata_buffer_reset" );
	init(&m_pd_param);
}

VO_VOID vo_headerdata_buffer::set_initmode( VO_BOOL isset )
{
	m_is_initmode = isset;
}

VO_BOOL vo_headerdata_buffer::rerrange_cache( headerdata_element * ptr_info , VO_S32 info_number , vo_buffer * ptr_buffer )
{
	m_is_reusing_cache = VO_TRUE;

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
		totalsize = (VO_S32)(totalsize + ptr_info[i].size);
	}

	VO_PBYTE ptr_temp = new VO_BYTE[totalsize];
	if( !ptr_temp )
	{
		VOLOGE( "new obj failed" );
		return VO_FALSE;
	}
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
				if( !ptr_new_segment )
				{
					VOLOGE( "new obj failed" );
					return VO_FALSE;
				}
				ptr_new_segment->buffer_pos = buffer_pos;
				ptr_new_segment->physical_pos = ptr_info[i].physical_pos;
				ptr_new_segment->ptr_next = 0;
				
				VO_S32 cpysize = (VO_S32)(( ptr_info[i].physical_pos + ptr_info[i].size < ptr_segment->physical_pos + ptr_segment->size ? ptr_info[i].physical_pos + ptr_info[i].size : ptr_segment->physical_pos + ptr_segment->size ) - ptr_info[i].physical_pos);
				memcpy( ptr_temp + buffer_pos , m_ptr_cache + ptr_segment->buffer_pos + ( ptr_info[i].physical_pos - ptr_segment->physical_pos ) , cpysize );

				ptr_new_segment->size = cpysize;

				ptr_segment = ptr_segment->ptr_next;

				VO_S32 leftsize = (VO_S32)(ptr_info[i].size - ptr_new_segment->size);

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

				buffer_pos += (VO_S32)ptr_info[i].size;
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
	voCAutoLock lock( &m_lock );
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
	m_static_last_time = 0;
	m_static_last_inused_buffer = 0;
	m_is_buffer_controled = VO_FALSE;
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

headerdata_segment * vo_headerdata_buffer::get_segment( VO_S64 pos )
{
	headerdata_segment * ptr_seg = m_ptr_segment_header;

	while( ptr_seg )
	{
		if( pos >= ptr_seg->physical_pos && pos <= ptr_seg->physical_pos + ptr_seg->size )
			break;
		
		ptr_seg = ptr_seg->ptr_next;
	}

	return ptr_seg;
}

//get the used buffer size of header buffer
VO_S32	vo_headerdata_buffer::get_used_buffersize()
{
	voCAutoLock lock( &m_lock );
	if(!m_ptr_segment_header || !m_ptr_segment_tail)
	{
		return 0;
	}

	VO_S32 used_buffer_size = 0;
	headerdata_segment * ptr_segment = m_ptr_segment_header;
	while(ptr_segment)
	{
		if(ptr_segment->buffer_pos + ptr_segment->size > used_buffer_size)
		{
			used_buffer_size = ptr_segment->buffer_pos + ptr_segment->size;
		}
		ptr_segment = ptr_segment->ptr_next;
	}

	return used_buffer_size;
}

//check current buffer usage,and if the download speed is too high , we will limit the download speed, 
//since it will cause problem when header buffer is full.
VO_VOID vo_headerdata_buffer::check_control_buffer_usage()
{
	if( m_static_last_time == 0)
	{
		m_static_last_inused_buffer = 0;
		m_static_last_time = voOS_GetSysTime();
	}

	//if already check and control the download speed, then we should not check again.
	if( !m_is_buffer_controled )
	{
		VO_S32 inused_buffer = get_used_buffersize();

		VO_U32 cur_time = voOS_GetSysTime();
		VO_U32 timelost = cur_time - m_static_last_time;
		//we will statistic the download speed every 100ms
		if( timelost > 100)
		{
			//make sure the gap buffer used value is positive
			if( inused_buffer > m_static_last_inused_buffer)
			{
				VO_S32 avgspeed = (VO_S32)(( inused_buffer - m_static_last_inused_buffer ) * 1000. / timelost);
				if( avgspeed > 5 * 1024 * 1024)
				{
					//try to tell download thread to slow down download speed
					long slowdown = 0;
					slowdown = 1;
					m_ptr_buffer_callback->buffer_notify( BUFFER_HEADER_CONTROL , &slowdown );
					m_is_buffer_controled = VO_TRUE;
				}
			}

			m_static_last_inused_buffer = inused_buffer;
			m_static_last_time = cur_time;
		}
	}
	else
	{
		return;
	}
}

