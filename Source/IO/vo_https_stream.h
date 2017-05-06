
#ifndef __VO_HTTPS_STREAM_H__ 
#define __VO_HTTPS_STREAM_H__

#include "voYYDef_SourceIO.h"
#include "vo_http_stream.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum VO_SSL_METHOD
{
	VO_SSL_SSLv23,
	VO_SSL_SSLv3,
	VO_SSL_TLSv1,
	VO_SSL_UNKNOWN,
};

class Persist_HTTPS
	:public Persist_HTTP
{
public:
    Persist_HTTPS();
    ~Persist_HTTPS();

	virtual VO_S32 Open( VO_CHAR * str_host , VO_CHAR * str_port );
	virtual VO_VOID Close( VO_BOOL bCloseSocket = VO_TRUE );
	virtual VO_S32 Read( VO_PBYTE buffer , VO_S32 size );
	virtual VO_S32 Write( VO_PBYTE buffer , VO_S32 size );

	VO_VOID setsslapi(vosslapi ssl);
	VO_BOOL SSL_Init();
	VO_BOOL SetupSSLTunnel2Proxy();

	VO_PTR m_ptr_ssl;
	VO_PTR m_ptr_ctx;
	VO_PTR m_ptr_method;
	VO_SSL_METHOD m_ssl_method;
	VO_BOOL	m_bsslinit;
	vosslapi m_ssl;

private:
	VO_S32 OpenInternalLoop( VO_CHAR * str_host , VO_CHAR * str_port );
	VO_VOID CloseInternalLoop();
};

class vo_https_stream
	:public vo_http_stream
{
public:
	vo_https_stream();
	~vo_https_stream();

	virtual VO_VOID setsslapi( vosslapi ssl);
protected:
	virtual VO_BOOL open( Persist_HTTP * ptr_persist = NULL , VO_BOOL bIsAsyncOpen = VO_FALSE );

	virtual VO_BOOL resolve_url( VO_CHAR * url );

	virtual VO_BOOL IsPersistSockAvailable();
	virtual VO_BOOL IsDefaultPort();

	virtual VO_U32 generate_tunnel_request( VO_CHAR * ptr_request );

protected:
	vosslapi m_ssl;
	VO_BOOL	m_bsslinit;
};
#ifdef _VONAMESPACE
}
#endif

#endif