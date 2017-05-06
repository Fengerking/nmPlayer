
#include "page_allocator.h"
#include "voString.h"
#include "log.h"

page_allocator::page_allocator( VO_U32 mempool_size , VO_S32 page_size )
:m_ptr_mempool(0)
,m_mempool_size(mempool_size)
,m_page_size(page_size)
,m_ptr_mempool_table(0)
,m_mempool_table_size(0)
,m_token_table_size(0)
,m_use_maxpage( VO_TRUE )
,m_init_mode(VO_FALSE)
{
	m_ptr_mempool = new VO_BYTE[m_mempool_size];
	m_mempool_table_size = m_mempool_size / m_page_size;

	PRINT_LOG( LOG_LEVEL_CRITICAL , "Mempool allocated: from 0X%p to 0X%p" , m_ptr_mempool , m_ptr_mempool + m_mempool_size );

	m_ptr_mempool_table = new VO_BOOL[m_mempool_table_size];
	memset( m_ptr_mempool_table , 0 , sizeof(VO_BOOL) * m_mempool_table_size );
}

page_allocator::~page_allocator()
{
	if( m_ptr_mempool )
		delete []m_ptr_mempool;

	m_ptr_mempool = 0;
	m_mempool_size = 0;
	m_page_size = 0;

	if( m_ptr_mempool_table )
		delete []m_ptr_mempool_table;

	m_ptr_mempool_table = 0;
	m_mempool_table_size = 0;
}

PAGETOKEN page_allocator::init()
{
	voCAutoLock lock( &m_lock );
	if( m_token_table_size > MAXTOKENTABLESIZE )
		return NULL;

	PAGETOKEN ret;

	m_token_table[m_token_table_size].cur_page = 0;
	m_token_table[m_token_table_size].total_freepage = 0;

	ret = m_token_table + m_token_table_size;

	m_token_table_size++;

	VO_S32 new_max_page = m_mempool_table_size / m_token_table_size;

	for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
	{
		m_token_table[i].max_page = new_max_page;
	}

	return ret;
}

void page_allocator::uninit( PAGETOKEN token )
{
	voCAutoLock lock( &m_lock );
	VO_S32 index = get_index( (VO_PBYTE)m_token_table , (VO_PBYTE)token , sizeof( PAGEALLOC_TOKEN ) );

	memmove( (VO_PBYTE)token + index * sizeof( PAGEALLOC_TOKEN ) , (VO_PBYTE)token + (index + 1) * sizeof( PAGEALLOC_TOKEN ) , ( m_token_table_size - index ) * sizeof( PAGEALLOC_TOKEN ) );

	m_token_table_size--;

	if( m_token_table_size == 0 )
		return;

	VO_S32 new_max_page = m_mempool_table_size / m_token_table_size;

	for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
	{
		m_token_table[i].max_page = new_max_page;
	}
}

VO_S32 page_allocator::get_freeindex( PAGEALLOC_TOKEN * ptr_token )
{
	VO_S32 token_index = get_index( (VO_PBYTE)m_token_table , (VO_PBYTE)ptr_token , sizeof( PAGEALLOC_TOKEN ) );

	if( token_index % 2 == 1 )
	{
		for( VO_S32 i = m_mempool_table_size / 2 ; i < m_mempool_table_size ; i++ )
		{
			if( !m_ptr_mempool_table[i] )
				return i;
		}

		for( VO_S32 i = m_mempool_table_size / 2 - 1 ; i >= 0 ; i-- )
		{
			if( !m_ptr_mempool_table[i] )
				return i;
		}
	}
	else
	{
		for( VO_S32 i = 0 ; i < m_mempool_table_size / 2 ; i++ )
		{
			if( !m_ptr_mempool_table[i] )
				return i;
		}

		for( VO_S32 i = m_mempool_table_size - 1 ; i >= m_mempool_table_size / 2 ; i-- )
		{
			if( !m_ptr_mempool_table[i] )
				return i;
		}
	}

	return -1;
}

PAGEALLOC_RET page_allocator::alloc_page( PAGETOKEN token , VO_PBYTE* ptr_page )
{
	voCAutoLock lock( &m_lock );
	VO_S32 index = -1;
	PAGEALLOC_TOKEN * ptr_token = ( PAGEALLOC_TOKEN * )token;

	index = get_freeindex( ptr_token );

	if( index == -1 )
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "***********No Page Left!!! Begin Dump Page Usage:" );
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Total Page: %d Use Max Page: %s" , m_mempool_table_size , m_use_maxpage ? "True" : "False" );
		for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Token ----- Page using: %d ; Max Page can use: %d ; Total Free Page: %lld" , m_token_table[i].cur_page , m_token_table[i].max_page , m_token_table[i].total_freepage );
		}
		PRINT_LOG( LOG_LEVEL_CRITICAL , "***********End" );

		m_ptr_callback->page_none();

		PRINT_LOG( LOG_LEVEL_CRITICAL , "***********After clean Page" );
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Total Page: %d Use Max Page: %s" , m_mempool_table_size , m_use_maxpage ? "True" : "False" );
		for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Token ----- Page using: %d ; Max Page can use: %d ; Total Free Page: %lld" , m_token_table[i].cur_page , m_token_table[i].max_page , m_token_table[i].total_freepage );
		}
		PRINT_LOG( LOG_LEVEL_CRITICAL , "***********End" );

		index = get_freeindex( ptr_token );

		if( index == -1 )
			return PAGEALLOC_NOMOREPAGE;
	}

	/*if( ptr_token->cur_page >= ptr_token->max_page )
		adjust_alltoken_maxpage();*/

	if( /*!m_init_mode && */m_use_maxpage && ptr_token->cur_page >= ptr_token->max_page )
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "***********No Page For this Token %p!!! Begin Dump Page Usage:" , token );
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Total Page: %d Use Max Page: %s" , m_mempool_table_size , m_use_maxpage ? "True" : "False" );
		for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
		{
			PRINT_LOG( LOG_LEVEL_CRITICAL , "Token %p ----- Page using: %d ; Max Page can use: %d ; Total Free Page: %lld" , m_token_table+i , m_token_table[i].cur_page , m_token_table[i].max_page , m_token_table[i].total_freepage );
		}
		PRINT_LOG( LOG_LEVEL_CRITICAL , "***********End" );
		return PAGEALLOC_NOMOREPAGE;
	}

	*ptr_page = m_ptr_mempool + index * m_page_size;
	ptr_token->cur_page++;

	m_ptr_mempool_table[index] = VO_TRUE;

	//PRINT_LOG( LOG_LEVEL_DETAIL , "Page Allocated , alloc index: %d , alloc pointer: 0X%p" , index , *ptr_page );

	return PAGEALLOC_OK;
}

void page_allocator::adjust_alltoken_maxpage()
{
	VO_S64 total = 0;
	for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
	{
		total = total + m_token_table[i].total_freepage;
	}

	if( total <= 30 )
		return;

	for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
	{
		m_token_table[i].max_page = m_mempool_table_size * m_token_table[i].total_freepage / total < m_mempool_table_size / 10 ? m_mempool_table_size / 10 : m_mempool_table_size * m_token_table[i].total_freepage / total ;
	}

	PRINT_LOG( LOG_LEVEL_CRITICAL , "***********After adjust token maxpage:" );
	PRINT_LOG( LOG_LEVEL_CRITICAL , "Total Page: %d Use Max Page: %s" , m_mempool_table_size , m_use_maxpage ? "True" : "False" );
	for( VO_S32 i = 0 ; i < m_token_table_size ; i++ )
	{
		PRINT_LOG( LOG_LEVEL_CRITICAL , "Token %p ----- Page using: %d ; Max Page can use: %d ; Total Free Page: %lld" , m_token_table+i , m_token_table[i].cur_page , m_token_table[i].max_page , m_token_table[i].total_freepage );
	}
	PRINT_LOG( LOG_LEVEL_CRITICAL , "***********End" );
}

void page_allocator::free_page( PAGETOKEN token , VO_PBYTE ptr_page )
{
	voCAutoLock lock( &m_lock );
	VO_S32 index = get_index( m_ptr_mempool , ptr_page , m_page_size );

	//PRINT_LOG( LOG_LEVEL_DETAIL , "Page Free , free index: %d , free pointer: 0X%p" , index , ptr_page );

	PAGEALLOC_TOKEN * ptr_token = ( PAGEALLOC_TOKEN * )token;

	m_ptr_mempool_table[index] = VO_FALSE;
	ptr_token->cur_page--;
	ptr_token->total_freepage++;
}

VO_S32 page_allocator::get_totalpage_count( PAGETOKEN token )
{
	PAGEALLOC_TOKEN * ptr_token = ( PAGEALLOC_TOKEN * )token;
	return ptr_token->max_page;
}

VO_S32 page_allocator::get_usedpage_count( PAGETOKEN token )
{
	PAGEALLOC_TOKEN * ptr_token = ( PAGEALLOC_TOKEN * )token;
	return ptr_token->cur_page;
}