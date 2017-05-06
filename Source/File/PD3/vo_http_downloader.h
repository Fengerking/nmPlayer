#pragma once
#include "vo_downloader.h"
#include "vo_thread.h"

class vo_http_downloader :
	public vo_downloader,
	public vo_thread
{
public:
	vo_http_downloader(void);
	virtual ~vo_http_downloader(void);

	//vo_downloader
	virtual VO_BOOL set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param );
	virtual VO_BOOL start_download( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE );
	virtual VO_VOID stop_download();
	virtual VO_BOOL is_downloading();
	virtual VO_VOID get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info );
	virtual VO_BOOL is_support_seek(){ return m_is_server_canseek; }
	//

	virtual VO_U32 get_duration(){ return m_duration; }

protected:

	//vo_thread
	virtual VO_VOID thread_function();
	virtual void stop();
	//

	virtual VO_BOOL resolve_url( VO_CHAR * url );
	virtual VO_S32 perpare_request( VO_CHAR * ptr_request , VO_S64 download_pos , VO_S64 download_size );
	virtual VO_BOOL get_response();
	virtual VO_BOOL analyze_response();

	virtual VO_VOID download_normal();
	virtual VO_VOID download_chunked();
	virtual VO_VOID download_endless();

	virtual VO_VOID analyze_proxy();
	virtual VO_VOID urlencode();
	inline unsigned char toHex(const unsigned char x);

	//for test
	//virtual VO_BOOL vo_socket_connect2( VO_S32 * ptr_socket , VO_CHAR * str_host , VO_CHAR * str_port );
	//


protected:

	VOPDInitParam * m_ptr_PD_param;

	VO_CHAR m_host[256];
	VO_CHAR m_path[1024];
	VO_CHAR m_port[10];
	VO_CHAR m_url[1024];

	VO_BOOL m_use_proxy;
	VO_CHAR m_proxy_host[256];
	VO_CHAR m_proxy_port[10];

	VO_CHAR m_reparseurl[1024];
	VO_CHAR m_cookies[1024];

	VO_S32 m_status_code;
	VO_S64 m_content_length;
	VO_CHAR m_mimetype[50];
	VO_BOOL m_is_chunked;

	VO_S32 m_socket;

	VO_BOOL m_is_server_canseek;
	VO_BOOL m_is_url_encoded;

	VO_CHAR m_response[1024];

	VO_BOOL m_stop_download;
	VO_BOOL m_is_downloading;

	VO_S64 m_current_physical_pos;
	VO_S64 m_downloaded;
	VO_S64 m_start_download_pos;

	VO_S64 m_download_start_time;

	VO_U32 m_duration;
};
