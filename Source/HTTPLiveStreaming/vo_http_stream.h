#pragma once
#include "vo_stream.h"
#include "vo_thread.h"
#include "DRM_API.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum DownloadMode
{
	DOWNLOAD2NONE = 0,
	DOWNLOAD2MEM = 1,
	DOWNLOAD2FILE = 2,
	DONWLOADENCRYPT = 32768,
};

enum HTTP_ERRORCODE
{
    CONNECTION_ERROR = 80,
    CONTENT_NOTFOUND,
    RESPONSE_TOOLARGE,
    GET_RESPONSE_TIMEOUT,
};

class Persist_HTTP
{
public:
    Persist_HTTP();
    ~Persist_HTTP();

    VO_VOID reset();

    VO_S32 socket;
    VO_U64 deadtime;
    VO_S32 usedtimes;
    VO_BOOL brefreshbyserver;
    VO_S32 maxusetime;
};

typedef struct
{
    char   strUserName[256];
	char   ulUserNameLen;
    char   strPasswd[256];
	char   ulUPasswdLen;	
}S_USER_INFO;


class vo_http_stream
	:public vo_stream
	,public vo_thread
{
public:
	vo_http_stream();
	virtual ~vo_http_stream(void);
	virtual VO_BOOL open( VO_CHAR * url , DownloadMode mode , VO_PBYTE ptr_key = NULL , VO_PBYTE ptr_iv = NULL , ProtectionType drm_type = NONE , DRM_Callback * ptr_drm_engine = NULL , void * drm_handle = NULL );
    virtual VO_BOOL persist_open( VO_CHAR * url , DownloadMode mode , Persist_HTTP * ptr_persist , VO_PBYTE ptr_key = NULL , VO_PBYTE ptr_iv = NULL , ProtectionType drm_type = NONE , DRM_Callback * ptr_drm_engine = NULL , void * drm_handle = NULL );
	virtual VO_VOID close();

	virtual VO_S64 read( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_S64 write( VO_PBYTE ptr_buffer , VO_U64 buffer_size );
	virtual VO_BOOL seek( VO_S64 pos , vo_stream_pos stream_pos  );
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

	virtual VO_VOID write_eos();
	virtual VO_S32 get_lasterror();

	VO_S64 get_content_length(){ return m_content_length; }
    VO_U32 GetRedirectURL(VO_CHAR*  pReceiveBuffer, VO_U32  ulLen);
	VO_CHAR * get_mime_type(){ return m_mimetype; }
    VO_VOID        SetParamForHttp(VO_U32 uId, VO_VOID* pDataUser, VO_VOID*  pDataPwd);

	virtual VO_S64 get_download_bitrate();
	virtual VO_S64 getdownloadtime();
	VO_VOID        DoTransactionForDigest();
	VO_U32         GetDigestRealm();
	VO_U32         GetDigestUserName();
	VO_U32         GetDigestPwd();
	VO_U32         GetDigestNonce();
	VO_U32         GetDigestQop();
	VO_U32         GetDigestMethod();
	VO_U32         GetDigestURI();
	VO_U32         GetDigestOpaque();
	VO_U32         GetStringWithInStartEnd(VO_CHAR* pStrSource, VO_CHAR* pStart, VO_CHAR* pEnd, VO_CHAR* pStrDesBuffer, VO_U32 ulBufferLen, VO_U32* pLen);

	virtual void stop(){ stop_download_thread(); }

protected:
	virtual VO_BOOL init( VO_CHAR * url );
	virtual void uninit();

	virtual VO_BOOL resolve_url( VO_CHAR * url );

	virtual void thread_function();
	virtual void download();

	virtual void download_normal();
	virtual void download_chunked();
	virtual void download_endless();

	virtual VO_U32 perpare_http_request( VO_CHAR * ptr_request );

	virtual VO_BOOL get_response();

	virtual VO_BOOL analyze_response();

	virtual VO_VOID stop_download_thread();

protected:
	DownloadMode m_download_mode;

	VO_CHAR m_host[256];
	VO_CHAR m_path[1024];
	VO_CHAR m_port[10];

	VO_S32	m_socket;

	VO_CHAR m_response[1024];
	VO_CHAR m_reparseurl[1024];

	VO_S32 m_status_code;
	VO_S64 m_content_length;
	VO_CHAR m_mimetype[50];
	VO_BOOL m_is_chunked;
	VO_S64 m_totaldownload;

	vo_stream * m_ptr_buffer_stream;

	VO_BOOL m_is_download_ok;

	VO_BOOL m_stop_download;

	VO_S64 m_download_starttime;
	VO_S64 m_download_endtime;

    Persist_HTTP * m_ptr_persist;

    VO_S32 m_errorcode;

    static VO_CHAR m_cookies[1024];
    static voCMutex m_cookieslock;

    static VO_U32  m_ulDigestFlag;
    static VO_CHAR m_strUserName[128];
    static VO_U32  m_ulstrUserNameLen;
    static VO_CHAR m_strPwd[128];
    static VO_U32  m_ulstrPwdLen;
    static VO_CHAR m_strNonce[128];
    static VO_U32  m_ulstrNonceLen;
    static VO_CHAR m_strMethod[128];
    static VO_U32  m_ulstrMethodLen;
    static VO_CHAR m_strRealm[128];
    static VO_U32  m_ulstrRealmLen;
    static VO_CHAR m_strURI[1024];
    static VO_U32  m_ulstrURILen;
    static VO_CHAR m_strQop[128];
    static VO_U32  m_ulstrQopLen;
    static VO_CHAR m_strOpaque[128];
    static VO_U32  m_ulstrOpaqueLen;

	static VO_U32  m_ulClientNonceCount;
	static VO_U32  m_ulClientNonce1;
	static VO_U32  m_ulClientNonce2;

	
    static VO_CHAR m_strResponse[128];
	static VO_U32  m_ulstrResponseLen;
	
	
};

#ifdef _VONAMESPACE
}
#endif

