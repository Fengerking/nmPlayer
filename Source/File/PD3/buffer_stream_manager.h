
#ifndef __BUFFER_STREAM_MANAGER_H__

#define __BUFFER_STREAM_MANAGER_H__

#include "http_buffer_stream.h"
#include "page_allocator.h"
#include "voFile.h"
#include "interface_io.h"
#include "headerdata_buffer.h"
#include "interface_bufferstream_callback.h"
#include "bufferstream_counter.h"

#define PAGE_SIZE (100*1024)

struct STREAM_LIST
{
	http_buffer_stream * ptr_stream;

	STREAM_LIST * ptr_next;
	STREAM_LIST * ptr_pre;
};

class buffer_stream_manager
	:public interface_io
	,public interface_bufferstream_callback
	,public interface_page_allocator_callback
{
public:
	buffer_stream_manager();
	~buffer_stream_manager();

	VO_BOOL open( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param );

	//interface_io
	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_S64 size );
	virtual VO_S64 set_pos( VO_S64 seekpos , VO_FILE_POS pos );
	virtual VO_S64 get_filesize();
	//

	//interface_bufferstream_callback
	virtual BUFFERSTREAM_CALLBACK_RET need_new_bufferstream( VO_S32 bufferstream_id , VO_S64 new_pos );
	virtual BUFFERSTREAM_CALLBACK_RET set_filesize( VO_S64 filesize );
	//

	//interface_page_allocator_callback
	virtual VO_VOID page_none();
	//

	VO_BOOL cache_headerdata( headerdata_info * ptr_info );

	VO_VOID set_initmode( VO_BOOL isinit );

protected:
	STREAM_LIST * add_stream( http_buffer_stream * ptr_stream );
	VO_BOOL check_bufferstream_needed( VO_S32 bufferstream_id , VO_S64 pos );

protected:
	STREAM_LIST * m_streamlist;
	STREAM_LIST * m_streamlist_tail;
	page_allocator * m_ptr_allocator;
	VO_S64 m_cur_pos;
	headerdata_buffer m_headerdata;

	VO_CHAR *m_url;
	VOPDInitParam * m_ptr_PD_param;

	VO_BOOL m_is_init;

	voCMutex m_lock;

	VO_S64 m_filesize;

	bufferstream_counter m_counter;
};


#endif
