#pragma once

#include "voYYDef_SourceIO.h"
#include "voType.h"
#include "voSource2_IO.h"
#include "voSource2.h"
#include "vo_http_sessions_info.h"
#include "CSourceIOBase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class vo_http_session :public CSourceIOBase
{
public:
	vo_http_session();
	~vo_http_session();

	virtual VO_U32 Init( VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback );
	virtual VO_U32 UnInit();

	virtual VO_U32 Open( VO_BOOL bIsAsyncOpen );
	virtual VO_U32 Close();

	virtual VO_U32 Read( VO_VOID * ptr_buf , VO_U32 size , VO_U32 * ptr_size );
	virtual VO_U32 	Write (VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pWrittenSize);

	virtual VO_U32 SetPos( VO_S64 pos , VO_SOURCE2_IO_POS relative_pos , VO_S64 *llActualPos );

	virtual VO_U32 Flush ();

	virtual VO_U32 GetSize( VO_U64 * ptr_size );

	virtual VO_U32 GetParam( VO_U32 id , VO_PTR ptr_param );
	virtual VO_U32 SetParam( VO_U32 id , VO_PTR ptr_param );

	virtual VO_U32 GetLastError();

	virtual VO_U32	GetSource(VO_PTR* pSource);

	virtual VO_U32	GetCurPos(VO_U64* llPos);

	static VO_S64 http_stream_callback(VO_HANDLE phandle, VO_PBYTE buffer, VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type = VO_CONTENT_DATA );
	
protected:
	VO_S64	write_data( VO_PBYTE buffer, VO_U64 size );
	VO_VOID	filesize2mem( VO_U64 size );
	VO_VOID control_downloadspeed( VO_S32 speed );
	VO_VOID PostOpenprocess();
	VO_BOOL setsslapi();
	void SetIOFileOpenStatus( VO_S32 status ){ m_IOFileOpenStatus = status ;}
	VO_S32	GetIOFileOpenStatus(){ return m_IOFileOpenStatus; }
	VO_U32	GetStartopenTime(){ return m_open_start_time; }
	VO_VOID UpdateCookieImmediately();
	VO_S32  IONotify( VO_U32 uID , VO_PTR pParam1, VO_PTR pParam2 );
	VO_VOID UpdatePersistInfo();
	VO_VOID UpdateDNSCache();
	VO_VOID QueryDNSRecord( VO_PBYTE ptrhost );
	VO_S64 UpdateDownloadSetting( VO_CHAR* ptr_url );
private:
	VO_HTTP_DOWNLOAD_BUFFER	m_handle;
	VO_HTTP_DOWNLOAD2BUFFERCALLBACK m_write2buffercb;
	VO_SOURCE2_IO_HTTPCALLBACK m_iocb;
	VO_S32	m_append_datacnt;
	voCMutex m_sslock;
	//0 : opening 1: opened and success -1: opened but failed. other value: invalid
	VO_S32	m_IOFileOpenStatus;
	VO_U32 m_open_start_time;
};

#ifdef _VONAMESPACE
}
#endif