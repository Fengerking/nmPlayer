#ifndef _VO_DOWNLOADER_H

#define _VO_DOWNLOADER_H

#include "interface_download_callback.h"
#include "voStreaming.h"

#define DOWNLOADTOEND -1

struct VO_DOWNLOAD_INFO
{
	VO_S64 start_download_pos;
	VO_S64 download_size;
	VO_S64 cur_download_pos;
	VO_S64 average_speed;
	VO_BOOL is_speed_reliable;
};

class vo_downloader
{
public:
	vo_downloader():m_filesize(-1),m_ptr_callback(0){}
	virtual ~vo_downloader(){}

	virtual VO_VOID set_download_callback( interface_download_callback * ptr_callback ){ m_ptr_callback = ptr_callback; }
	virtual VO_BOOL set_url( VO_CHAR * str_url , VOPDInitParam * ptr_PD_param ) = 0;
	virtual VO_BOOL start_download( VO_S64 download_pos = 0 , VO_S64 download_size = DOWNLOADTOEND , VO_BOOL is_async = VO_TRUE ) = 0;
	virtual VO_VOID stop_download() = 0;
	virtual VO_BOOL is_downloading() = 0;
	virtual VO_VOID get_dowloadinfo( VO_DOWNLOAD_INFO * ptr_info ) = 0;
	virtual VO_BOOL is_support_seek() = 0;

	virtual VO_S64 get_actual_filesize(){ return m_filesize; }

	virtual VO_U32 get_duration(){ return 0; }

protected:
	interface_download_callback * m_ptr_callback;
	VO_S64 m_filesize;
};

#endif