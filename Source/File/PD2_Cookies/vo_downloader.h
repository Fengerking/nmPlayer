#ifndef _VO_DOWNLOADER_H

#define _VO_DOWNLOADER_H

#include "interface_download_callback.h"
#include "voStreaming.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define DOWNLOADTOEND -1

struct VO_DOWNLOAD_INFO
{
	VO_S64 start_download_pos;
	VO_S64 download_size;
	VO_S64 cur_download_pos;
	VO_S64 average_speed;
	VO_BOOL is_speed_reliable;
	VO_BOOL is_update_buffer;
};

class vo_downloader
{
public:
	vo_downloader():m_ptr_callback(0),m_filesize(-1), m_is_stopped(VO_FALSE){}
	virtual ~vo_downloader(){}

	virtual VO_VOID set_download_callback( interface_download_callback * ptr_callback ){ m_ptr_callback = ptr_callback; }
	virtual VO_BOOL set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param ) = 0;
	virtual VO_BOOL start_download( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE ) = 0;
	virtual VO_VOID stop_download() = 0;
	virtual VO_BOOL is_downloading() = 0;
	virtual VO_VOID get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info ) = 0;
	virtual VO_BOOL is_support_seek() = 0;
	virtual VO_BOOL start_download_inthread( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE ) = 0;

	virtual VO_S64 get_actual_filesize(){ return m_filesize; }

	virtual VO_U32 get_duration(){ return 0; }

	//set data pointer and len for check whether the server can seek or not
	virtual VO_VOID set_comparedata(VO_PBYTE pdata , VO_S32 size) = 0;

	virtual VO_VOID set_to_close(VO_BOOL is_to_exit) = 0;

	virtual VO_S32 get_net_error_type() = 0;
	virtual VO_VOID set_net_error_type(VO_S32 err_type) = 0;

	//try to slow down download speed
	virtual VO_VOID slow_down(VO_U32 slowdown) = 0;

	//try to pause / run download thread to reveive data
	virtual VO_VOID pause_connection(VO_BOOL recvpause) = 0;

	virtual VO_VOID set_dlna_param(VO_VOID* pobj) = 0;

	virtual VO_VOID set_connectretry_times(VO_S32 retrytimes) = 0;
	virtual VO_VOID set_workpath(VO_TCHAR *	strWorkPath) = 0;
protected:
	interface_download_callback * m_ptr_callback;
	VO_S64 m_filesize;
	VO_BOOL m_is_stopped;
};

#ifdef _VONAMESPACE
}
#endif

#endif