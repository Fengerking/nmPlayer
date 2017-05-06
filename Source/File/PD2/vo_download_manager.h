#pragma once
#include "interface_download_callback.h"
#include "vo_downloader.h"

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

	virtual VO_S64 get_actual_filesize();
	//

	virtual VO_U32 get_duration();

protected:
	vo_downloader * m_ptr_downloader;
};
