#pragma once

#include "vo_http_stream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif



class vo_webdownload_stream
{
public:
	vo_webdownload_stream(void);
	~vo_webdownload_stream(void);


    VO_S64   GetDownLoadTime();
	VO_BOOL open( VO_CHAR * url , DownloadMode mode , VO_PBYTE ptr_key = NULL , VO_PBYTE ptr_iv = NULL , ProtectionType drm_type = NONE , DRM_Callback * ptr_drm_engine = NULL , void * drm_handle = NULL );
    VO_BOOL persist_open( VO_CHAR * url , DownloadMode mode , Persist_HTTP * ptr_persist , VO_PBYTE ptr_key = NULL , VO_PBYTE ptr_iv = NULL , ProtectionType drm_type = NONE , DRM_Callback * ptr_drm_engine = NULL , void * drm_handle = NULL );
	VO_VOID close();

	VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	VO_BOOL seek( VO_S64 pos , vo_stream_pos stream_pos  );
	VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

	VO_S64 get_content_length();
	VO_CHAR * get_mime_type();

	VO_S64 get_download_bitrate();

	void stop();

	VO_S32 get_lasterror();

	//
	VO_VOID   SetParamForHttp(VO_U32	uId, VO_VOID* pDataUser, VO_VOID*	pDataPwd);
    VO_U32    GetReDirectionURL(VO_CHAR*  pURLBuffer, VO_U32  ulLen);
	
	//

private:
	vo_http_stream * m_ptr_stream;
	VO_U32           m_ulUserPwdSet;
	S_USER_INFO      m_sUserInfo;
};

#ifdef _VONAMESPACE
}
#endif

