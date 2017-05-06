#ifndef __BUFFER_CONTROLLER_H__

#define __BUFFER_CONTROLLER_H__

#include "page_allocator.h"
#include "voCMutex.h"
#include "interface_buffer_callback.h"
#include "buffer_judgement.h"

struct PAGE_LIST
{
	VO_PBYTE ptr_page;
	PAGE_LIST * ptr_next;
};

class buffer_controller
{
public:
	buffer_controller( page_allocator * ptr_allocator , VO_S32 page_size , VO_S64 physical_start );
	~buffer_controller();

	VO_S64 read( VO_S64 pos , VO_PBYTE ptr_buffer , VO_S64 size );
	VO_S64 write( VO_S64 pos , VO_PBYTE ptr_buffer , VO_S64 size );

	void set_endboundary( VO_S64 end_pos );
	void set_buffer_callback( interface_buffer_callback * ptr_callback ){ m_ptr_callback = ptr_callback; }

	void set_recyclebuffer( VO_BOOL is_recycle ){ m_is_recyclebuffer = is_recycle; }
	void set_filesize( VO_S64 filesize );
	VO_BOOL is_buffer_full();

	VO_VOID recycle_buffer();

protected:
	void add_page( VO_PBYTE ptr_page );

	void free_allpage();

	void free_page_until( VO_S64 pos );

protected:

	page_allocator * m_ptr_allocator;

	VO_S64 m_endboundary;
	VO_S64 m_physical_start;

	VO_S32 m_page_size;

	VO_PBYTE m_ptr_cur_page;
	VO_S64 m_cur_page_physical_start;
	VO_S64 m_cur_page_end;

	PAGE_LIST * m_pagelist_header;
	PAGE_LIST * m_pagelist_tail;

	PAGETOKEN m_pagetoken;

	VO_S32 m_pagecount;

	voCMutex m_lock;

	interface_buffer_callback * m_ptr_callback;

	buffer_judgement m_judger;
	VO_BOOL m_is_buffer_fulled;
	VO_BOOL m_is_recyclebuffer;
	VO_BOOL m_is_small_file;

	VO_S64 m_filesize;
};


#endif