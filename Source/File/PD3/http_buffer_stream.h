
#ifndef __HTTP_BUFFER_STREAM_H__

#define __HTTP_BUFFER_STREAM_H__

#include "vo_http_downloader.h"
#include "buffer_controller.h"
#include "interface_bufferstream_callback.h"

class http_buffer_stream: 
	public interface_download_callback , 
	public interface_buffer_callback
{
public:
	http_buffer_stream( page_allocator * ptr_allocator , VO_S32 page_size , VO_S64 physical_start , VO_CHAR * str_url , VOPDInitParam * ptr_PD_param );
	~http_buffer_stream(void);

	//interface_buffer_callback
	virtual BUFFER_CALLBACK_RET buffer_lack( VO_S64 lack_pos , VO_S64 lack_size );
	virtual BUFFER_CALLBACK_RET buffer_low( VO_S64 startdownload_pos );
	virtual BUFFER_CALLBACK_RET buffer_newstart( VO_S64 start_pos ){ m_physical_start = start_pos; return BUFFER_CALLBACK_OK; }
	//

	//interface_download_callback
	virtual CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size );
	virtual CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data );
	//

	VO_BOOL start();

	VO_S64 get_physicalstart(){ return m_physical_start; }
	VO_S64 get_endboudary(){ return m_endboundary; }
	void set_endboundary( VO_S64 end );
	VO_S64 read( VO_S64 pos , VO_PBYTE ptr_buffer , VO_S64 size );

	bool operator<( const http_buffer_stream & val ){ return m_physical_start < val.m_physical_start; }

	VO_BOOL hit_test( VO_S64 readpos );

	VO_VOID set_bufferstream_callback( interface_bufferstream_callback * ptr_callback ){ m_ptr_callback = ptr_callback; }

	VO_VOID set_initmode( VO_BOOL is_init );

	VO_S32 get_id(){ return m_id; }

	VO_VOID recycle_buffer(){m_buffer.recycle_buffer();}

private:
	static VO_S32 create_id();
	static VO_S32 m_id_stub;
	VO_S32 m_id;
	static voCMutex m_id_lock;

protected:
	vo_http_downloader m_downloader;
	buffer_controller m_buffer;

	VO_S64 m_physical_start;
	VO_S64 m_endboundary;

	interface_bufferstream_callback * m_ptr_callback;
};

#endif
