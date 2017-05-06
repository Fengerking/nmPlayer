#include "buffer_stream_manager.h"
#include "log.h"
#include "voOSFunc.h"

buffer_stream_manager::buffer_stream_manager()
:m_ptr_allocator(0)
,m_streamlist(0)
,m_cur_pos(0)
,m_is_init(VO_TRUE)
,m_filesize(-1)
,m_counter(30)
{
}

buffer_stream_manager::~buffer_stream_manager()
{
	STREAM_LIST * ptr_item = m_streamlist;

	while( ptr_item )
	{
		STREAM_LIST * ptr_temp = ptr_item;

		ptr_item = ptr_item->ptr_next;

		delete ptr_temp->ptr_stream;
		delete ptr_temp;
	}

	m_streamlist = m_streamlist_tail = 0;

	delete m_ptr_allocator;

	m_ptr_allocator = 0;
}

VO_BOOL buffer_stream_manager::open( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param )
{
	m_url = str_url;
	m_ptr_PD_param = ptr_PD_param;

	if( m_ptr_allocator == NULL )
		m_ptr_allocator = new page_allocator( ptr_PD_param->nMaxBuffer * 1024 , PAGE_SIZE );

	m_ptr_allocator->set_callback( this );
	m_ptr_allocator->set_initmode( VO_TRUE );

	http_buffer_stream * ptr_stream = new http_buffer_stream( m_ptr_allocator , PAGE_SIZE , 0 , str_url , ptr_PD_param );
	ptr_stream->set_bufferstream_callback( this );
	ptr_stream->set_initmode( m_is_init );

	if( ptr_stream->start() )
	{
		add_stream( ptr_stream );
		return VO_TRUE;
	}
	else
		return VO_FALSE;

}

VO_S64 buffer_stream_manager::read( VO_PBYTE ptr_buffer , VO_S64 size )
{
	PRINT_LOG( LOG_LEVEL_DETAIL , "Read at %lld , Read size %lld" , m_cur_pos , size );

	if( m_filesize > 0 && m_cur_pos >= m_filesize )
		return -1;

	VO_U32 start_time = voOS_GetSysTime();
	VO_S64 ret = m_headerdata.read( m_cur_pos , ptr_buffer , size );

	if( ret != -1 )
	{
		m_cur_pos = m_cur_pos + ret;
		PRINT_LOG( LOG_LEVEL_DETAIL , "Read finished! Cost %d ms Ret: %lld" , voOS_GetSysTime() - start_time , ret );
		return ret;
	}

	STREAM_LIST * ptr_item = m_streamlist;

	while( ptr_item )
	{
		if( ptr_item->ptr_stream->hit_test( m_cur_pos ) )
		{
			VO_S64 gotsize = 0;

			while( ptr_item && gotsize < size )
			{
				m_counter.push_id( ptr_item->ptr_stream->get_id() );

				VO_S64 added = ptr_item->ptr_stream->read( m_cur_pos , ptr_buffer + gotsize , size - gotsize );

				if( added < 0 )
				{
					PRINT_LOG( LOG_LEVEL_CRITICAL , "Read Error! Cost %d ms Ret: %lld" , voOS_GetSysTime() - start_time , added );
					return added;
				}

				gotsize = gotsize + added;
				m_cur_pos = m_cur_pos + added;
				ptr_item = ptr_item->ptr_next;
			}

			PRINT_LOG( LOG_LEVEL_DETAIL , "Read finished! Cost %d ms Ret: %lld" , voOS_GetSysTime() - start_time , gotsize );
			return gotsize;
		}
		else
			ptr_item = ptr_item->ptr_next;
	}

	PRINT_LOG( LOG_LEVEL_CRITICAL , "Read Error! Cost %d ms Ret: %lld" , voOS_GetSysTime() - start_time , -1 );
	return -1;
}

VO_S64 buffer_stream_manager::set_pos( VO_S64 seekpos , VO_FILE_POS pos )
{
	switch ( pos )
	{
	case VO_FILE_BEGIN:
		m_cur_pos = seekpos;
		break;
	case VO_FILE_CURRENT:
		m_cur_pos = seekpos + m_cur_pos;
		break;
	case VO_FILE_END:
		m_cur_pos = get_filesize() + seekpos;
		break;
	}

	return m_cur_pos;
}

VO_S64 buffer_stream_manager::get_filesize()
{
	return m_filesize;
}

BUFFERSTREAM_CALLBACK_RET buffer_stream_manager::need_new_bufferstream( VO_S32 bufferstream_id , VO_S64 new_pos )
{
	voCAutoLock lock( &m_lock );

	new_pos = new_pos / PAGE_SIZE * PAGE_SIZE;

	if( !check_bufferstream_needed( bufferstream_id , new_pos ) )
		return BUFFERSTREAM_CALLBACK_OK;

	PRINT_LOG( LOG_LEVEL_IMPORTANT , "**********Start new buffer stream at %lld! Begin dump buffer stream info:" , new_pos );
	STREAM_LIST * ptr_item = m_streamlist;
	while( ptr_item )
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Buffer Stream ID: %d Start: %lld End: %lld" , ptr_item->ptr_stream->get_id() , ptr_item->ptr_stream->get_physicalstart() , ptr_item->ptr_stream->get_endboudary() );
		ptr_item = ptr_item->ptr_next;
	}
	PRINT_LOG( LOG_LEVEL_IMPORTANT , "**********End" );

	http_buffer_stream * ptr_stream = new http_buffer_stream( m_ptr_allocator , PAGE_SIZE , new_pos , m_url , m_ptr_PD_param );
	ptr_stream->set_bufferstream_callback( this );
	ptr_stream->set_initmode( m_is_init );

	if( ptr_stream->start() )
	{
		add_stream( ptr_stream );

		PRINT_LOG( LOG_LEVEL_IMPORTANT , "**********After new buffer stream at %lld! Begin dump buffer stream info:" , new_pos );
		ptr_item = m_streamlist;
		while( ptr_item )
		{
			PRINT_LOG( LOG_LEVEL_IMPORTANT , "Buffer Stream ID: %d Start: %lld End: %lld" , ptr_item->ptr_stream->get_id() , ptr_item->ptr_stream->get_physicalstart() , ptr_item->ptr_stream->get_endboudary() );
			ptr_item = ptr_item->ptr_next;
		}
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "**********End" );

		return BUFFERSTREAM_CALLBACK_OK;
	}
	else
		return BUFFERSTREAM_CALLBACK_ERROR;
}

BUFFERSTREAM_CALLBACK_RET buffer_stream_manager::set_filesize( VO_S64 filesize )
{
	m_filesize = filesize;

	if( m_filesize > 0 && m_filesize < m_ptr_allocator->get_mempool_size() )
		m_ptr_allocator->set_smallfile();

	return BUFFERSTREAM_CALLBACK_OK;
}

STREAM_LIST * buffer_stream_manager::add_stream( http_buffer_stream * ptr_stream )
{
	STREAM_LIST * ptr_element = new STREAM_LIST;
	memset( ptr_element , 0 , sizeof(STREAM_LIST) );

	ptr_element->ptr_stream = ptr_stream;

	if( m_streamlist == 0 )
	{
		m_streamlist_tail = m_streamlist = ptr_element;
	}
	else
	{
		STREAM_LIST * ptr_item = m_streamlist;

		while( ptr_item )
		{
			if( *(ptr_item->ptr_stream) < *ptr_stream  )
				ptr_item = ptr_item->ptr_next;
			else
			{
				ptr_element->ptr_next = ptr_item;
				ptr_element->ptr_pre = ptr_item->ptr_pre;
				ptr_item->ptr_pre = ptr_element;
				if( ptr_element->ptr_pre )
				{
					ptr_element->ptr_pre->ptr_next = ptr_element;
					ptr_element->ptr_pre->ptr_stream->set_endboundary( ptr_stream->get_physicalstart() - 1 );
				}

				ptr_stream->set_endboundary( ptr_item->ptr_stream->get_physicalstart() - 1 );

				if( ptr_item == m_streamlist )
					m_streamlist = ptr_element;

				break;
			}
		}

		if( !ptr_item )
		{
			m_streamlist_tail->ptr_stream->set_endboundary( ptr_stream->get_physicalstart() - 1 );
			m_streamlist_tail->ptr_next = ptr_element;
			ptr_element->ptr_pre = m_streamlist_tail;
			m_streamlist_tail = ptr_element;
		}
	}

	return ptr_element;
}

VO_BOOL buffer_stream_manager::cache_headerdata( headerdata_info * ptr_info )
{
	PRINT_LOG( LOG_LEVEL_IMPORTANT , "**********Cache headerdata, List all the buffer needed:" );
	for( VO_S32 i = 0 ; i < ptr_info->arraysize ; i++ )
	{
		PRINT_LOG( LOG_LEVEL_IMPORTANT , "Pos: %lld , Size: %lld" , ptr_info->ptr_array[i].physical_pos , ptr_info->ptr_array[i].size );
	}
	PRINT_LOG( LOG_LEVEL_IMPORTANT , "**********End" );

	VO_S64 cur_pos = m_cur_pos;
	VO_BOOL ret;
	ret = m_headerdata.get_headerdata( this , ptr_info );
	m_cur_pos = cur_pos;

	return ret;
}

VO_VOID buffer_stream_manager::set_initmode( VO_BOOL isinit )
{
	m_is_init = isinit;

	STREAM_LIST * ptr_item = m_streamlist;

	while( ptr_item )
	{
		ptr_item->ptr_stream->set_initmode( isinit );
		ptr_item = ptr_item->ptr_next;
	}

	if( m_ptr_allocator )
		m_ptr_allocator->set_initmode( m_is_init );
}

VO_BOOL buffer_stream_manager::check_bufferstream_needed( VO_S32 bufferstream_id , VO_S64 pos )
{
	STREAM_LIST * ptr_item = m_streamlist;

	while( ptr_item )
	{
		if( bufferstream_id != ptr_item->ptr_stream->get_id() && ptr_item->ptr_stream->hit_test( pos ) )
			return VO_FALSE;
		
		ptr_item = ptr_item->ptr_next;
	}

	return VO_TRUE;
}

VO_VOID buffer_stream_manager::page_none()
{
	STREAM_LIST * ptr_item = m_streamlist;

	m_streamlist = m_streamlist_tail = 0;

	while( ptr_item )
	{
		STREAM_LIST * ptr_temp = 0;
		if( m_counter.is_id_contain( ptr_item->ptr_stream->get_id() ) )
		{
			if( m_streamlist == 0 )
			{
				m_streamlist = m_streamlist_tail = ptr_item;
				m_streamlist->ptr_pre = 0;
			}
			else
			{
				m_streamlist_tail->ptr_next = ptr_item;
				ptr_item->ptr_pre = m_streamlist_tail;
				m_streamlist_tail = ptr_item;
			}

			ptr_item->ptr_stream->recycle_buffer();
		}
		else
		{
			ptr_temp = ptr_item;
		}

		ptr_item = ptr_item->ptr_next;

		if( ptr_temp )
		{
			delete ptr_temp->ptr_stream;
			delete ptr_temp;
		}
	}

	m_streamlist_tail->ptr_next = 0;
}