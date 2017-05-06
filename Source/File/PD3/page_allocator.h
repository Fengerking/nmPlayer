#ifndef __PAGE_ALLOCATOR_H__

#define __PAGE_ALLOCATOR_H__

#include "voType.h"
#include "voCMutex.h"

typedef void * PAGETOKEN;

#define MAXTOKENTABLESIZE 16

enum PAGEALLOC_RET
{
	PAGEALLOC_OK = 0,
	PAGEALLOC_NOMOREPAGE = 1,
};

struct PAGEALLOC_TOKEN
{
	VO_S32 max_page;
	VO_S32 cur_page;
	VO_S64 total_freepage;
};

class interface_page_allocator_callback
{
public:
	virtual VO_VOID page_none() = 0;
};


class page_allocator
{
public:
	page_allocator( VO_U32 mempool_size , VO_S32 page_size );
	~page_allocator();

	PAGETOKEN init();
	void uninit( PAGETOKEN token );

	PAGEALLOC_RET alloc_page( PAGETOKEN token , VO_PBYTE* ptr_page );
	void free_page( PAGETOKEN token , VO_PBYTE ptr_page );
	VO_S32 get_totalpage_count( PAGETOKEN token );
	VO_S32 get_usedpage_count( PAGETOKEN token );

	VO_S64 get_mempool_size(){  return m_mempool_size; };

	VO_VOID set_smallfile(){ m_use_maxpage = VO_FALSE; }
	VO_VOID set_initmode( VO_BOOL is_init ){ m_init_mode = is_init; }

	VO_VOID set_callback( interface_page_allocator_callback * ptr_callback ){ m_ptr_callback = ptr_callback; }

protected:
	inline VO_S32 get_index( const VO_PBYTE ptr_org , const VO_PBYTE ptr_offset , VO_S32 unit_size )
	{
		VO_S32 offset = ptr_offset - ptr_org;
		return offset / unit_size;
	}

	void adjust_alltoken_maxpage();
	VO_S32 get_freeindex( PAGEALLOC_TOKEN * ptr_token );

protected:

	VO_PBYTE m_ptr_mempool;
	VO_U32 m_mempool_size;

	VO_S32 m_page_size;

	VO_BOOL * m_ptr_mempool_table;
	VO_S32 m_mempool_table_size;

	PAGEALLOC_TOKEN m_token_table[MAXTOKENTABLESIZE];
	VO_S32 m_token_table_size;

	voCMutex m_lock;

	VO_BOOL m_use_maxpage;
	VO_BOOL m_init_mode;

	interface_page_allocator_callback * m_ptr_callback;
};


#endif