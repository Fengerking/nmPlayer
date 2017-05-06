#pragma once
#include "interface_download_callback.h"
#include "vo_downloader.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_download_manager :
	public interface_download_callback,
	public vo_downloader
{
public:
	vo_download_manager(void);
	virtual ~vo_download_manager(void);

	//download callback
	CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size );
	CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data );
	//

	//vo_downloader
	virtual VO_BOOL set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param );
	virtual VO_BOOL start_download( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE );
	virtual VO_VOID stop_download();
	virtual VO_BOOL is_downloading();
	virtual VO_VOID get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info );
	virtual VO_BOOL is_support_seek();
	virtual VO_BOOL start_download_inthread( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE );

	virtual VO_S64 get_actual_filesize();
	//

	//set data pointer and len for check whether the server can seek or not
	virtual VO_VOID set_comparedata(VO_PBYTE pdata , VO_S32 size);

	virtual VO_U32 get_duration();

	virtual VO_VOID set_to_close(VO_BOOL is_to_exit);

	virtual VO_S32 get_net_error_type(){ return m_ptr_downloader->get_net_error_type() ;}
	virtual VO_VOID set_net_error_type(VO_S32 err_type)
	{
		m_ptr_downloader->set_net_error_type(err_type);
	}

	virtual VO_VOID slow_down(VO_U32 slowdown);

	//try to pause / run download thread to reveive data
	virtual VO_VOID pause_connection(VO_BOOL recvpause);

	virtual VO_VOID set_dlna_param(VO_VOID* pobj);

	virtual VO_VOID set_connectretry_times(VO_S32 retrytimes);

	virtual VO_VOID set_workpath(VO_TCHAR *	strWorkPath);
protected:
	vo_downloader * m_ptr_downloader;
};

#ifdef _VONAMESPACE
}
#endif
