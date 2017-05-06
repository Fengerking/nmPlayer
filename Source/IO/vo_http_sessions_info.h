#pragma once

#include "voYYDef_SourceIO.h"
#include "Buffer/vo_singlelink_list.hpp"
#include "voType.h"
#include "vo_http_struct.h"
#include "vo_http_stream.h"
#include "voCMutex.h"
#include "COpenSSL.h"
#include "voSource2_IO.h"
#include "voSource2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

struct LIVESOCKETMAP;
class Persist_HTTP;

class vo_http_sessions_info
{
public:
	vo_http_sessions_info();
	virtual ~vo_http_sessions_info( void );

	VO_CHAR * get_redirecturl( const VO_CHAR * ptr_url );
	VO_VOID add_redirecturl( VO_CHAR * str_url , VO_CHAR * str_rdurl);
	Persist_HTTP * get_persistHTTP( const VO_CHAR * ptr_url );
	VO_BOOL generate_cookie_str( VO_CHAR* str_url );
	VO_BOOL	get_hostbyurl( const VO_CHAR * ptr_url , VO_CHAR * ptr_host , VO_S32 size );
	VO_BOOL get_portbyurl( const VO_CHAR * ptr_url , VO_CHAR * ptr_port , VO_S32 size );
	VO_BOOL get_pathbyurl( const VO_CHAR * ptr_url , VO_CHAR * ptr_path , VO_S32 size );
	VO_BOOL IsPathMatch(VO_CHAR * uriPath , VO_CHAR* cookiePath);
	VO_BOOL IsDomainMatch(VO_CHAR * host , VO_CHAR* domain);
	VO_BOOL IsSecureMatch(VO_S32 secure_flag , VO_CHAR* url);
	VO_BOOL IsExpired(VO_S32 expire_time);
	VO_VOID update_cookie_info( cookie_storage_node* pCookieNode );
	VO_VOID UpdateExternalCookieInfo( PHTTPCookieExternal ptr_cookie );
	VO_VOID UpdateExternalHeaderInfo( VO_SOURCE2_HTTPHEADER* ptr_headinfo );
	VO_VOID UpdateProxyInfo( VO_SOURCE2_HTTPPROXY* ptr_proxyinfo );
	VO_BOOL IsPrefix(VO_CHAR * uriPath , VO_CHAR* cookiePath);
	VO_BOOL IsFirstDifferenceSlash(VO_CHAR * uriPath , VO_CHAR* cookiePath);
	VO_VOID UpdateDnsCache( DNS_Record *ptr_dnsrecord );
	VO_CHAR * QueryCachedIpbyhost( const VO_CHAR * ptr_host );

	VO_VOID add_authinfo( VO_CHAR * ptr_url , VO_CHAR * ptr_auth );
	VO_BOOL get_authinfo( VO_CHAR * ptr_url , VO_CHAR * ptr_auth );

	VO_CHAR* get_cookie(){ return m_cookies; }
	VO_CHAR* getProxyHost(){ return m_proxyhost; }
	VO_CHAR* getProxyPort(){ return m_proxyport; }
	VO_CHAR* getUserAgent(){ return m_useragent; }

	VO_VOID   setworkpath( VO_PTCHAR ptrpath ){ m_ptrworkpath = ptrpath; }
	VO_PTCHAR getworkpath(){ return m_ptrworkpath; }

	VO_VOID ResetPersistSSlsock();
public:
	VO_U64	m_module_totalmem;
	VO_S32  m_module_instancecnt;
	COpenSSL m_sslload;

private:
	VO_VOID copy_storage_cookienode( cookie_storage_node* ptr_destnode , cookie_storage_node* ptr_srcnode );
	Persist_HTTP * CreatePersistHTTP( VO_CHAR * str_host, VO_CHAR * str_port , VO_BOOL bHttps );

private:
	VO_CHAR m_cookies[2048];
	vo_singlelink_list< REDIRECTURLMAP * > m_list_redrecturl_map;
	vo_singlelink_list< LIVESOCKETMAP * > m_list_livesocket_map;
	vo_singlelink_list< cookie_storage_node * > m_list_cookiestore_map;
	vo_singlelink_list< HOSTAUTHMAP * > m_list_hostauth_map;
	vo_singlelink_list< DNS_Record * > m_list_dnsrecord_map;

	voCMutex m_list_lock;
	VO_PTCHAR m_ptrworkpath;
	VO_CHAR	m_proxyhost[512];
	VO_CHAR m_proxyport[64];
	VO_CHAR m_useragent[512];
};

#ifdef _VONAMESPACE
}
#endif