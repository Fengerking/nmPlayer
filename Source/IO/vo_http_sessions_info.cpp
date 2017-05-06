#include "vo_http_sessions_info.h"
#include "vo_http_utils.h"
#include "NetWork/vo_socket.h"
#include "vo_http_cookie.h"
#include "voLog.h"
#ifdef ENABLE_ASYNCDNS
#include "ares.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
struct LIVESOCKETMAP
{
	VO_CHAR str_host[256];
	VO_CHAR str_port[20];
	VO_BOOL	is_https;
	Persist_HTTP * ptr_socket;
};
#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
vo_http_sessions_info::vo_http_sessions_info()
: m_module_totalmem(0)
, m_module_instancecnt(0)
, m_ptrworkpath(0)
{
	memset( m_proxyhost , 0 , sizeof( m_proxyhost ) );
	memset( m_proxyport , 0 , sizeof( m_proxyport ) );
	memset( m_useragent , 0 , sizeof( m_useragent ) );

#ifdef ENABLE_ASYNCDNS		
	int status = ares_library_init(ARES_LIB_INIT_ALL);
	if (status != ARES_SUCCESS)
	{
		VOLOGE("ares_library_init failed: %s\n", ares_strerror(status));
	}

	//store some known dns cache record
	DNS_Record dnsrecord;
	memset( &dnsrecord , 0 , sizeof(DNS_Record) );
	strcpy( dnsrecord.host , "localhost" );
	strcpy( dnsrecord.ipaddr , "127.0.0.1" );
	dnsrecord.expiredtime = voOS_GetSysTime() + DNSCACHE_EXPIREDTIME;
	UpdateDnsCache( &dnsrecord );
#endif
}

vo_http_sessions_info::~vo_http_sessions_info()
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< REDIRECTURLMAP * >::iterator redirecturl_iter = m_list_redrecturl_map.begin();
	vo_singlelink_list< REDIRECTURLMAP * >::iterator redirecturl_itere = m_list_redrecturl_map.end();

	while( redirecturl_iter != redirecturl_itere )
	{
		delete *redirecturl_iter;
		redirecturl_iter++;
	}

	m_list_redrecturl_map.reset();

	vo_singlelink_list< LIVESOCKETMAP * >::iterator iter = m_list_livesocket_map.begin();
	vo_singlelink_list< LIVESOCKETMAP * >::iterator itere = m_list_livesocket_map.end();

	while( iter != itere )
	{
		delete (*iter)->ptr_socket;
		delete *iter;
		iter++;
	}

	m_list_livesocket_map.reset();

	vo_singlelink_list< cookie_storage_node * >::iterator storage_iter = m_list_cookiestore_map.begin();
	vo_singlelink_list< cookie_storage_node * >::iterator storage_itere = m_list_cookiestore_map.end();

	while( storage_iter != storage_itere )
	{
		delete *storage_iter;
		storage_iter++;
	}

	m_list_cookiestore_map.reset();

	vo_singlelink_list< HOSTAUTHMAP * >::iterator auth_iter = m_list_hostauth_map.begin();
	vo_singlelink_list< HOSTAUTHMAP * >::iterator auth_itere = m_list_hostauth_map.end();

	while( auth_iter != auth_itere )
	{
		delete *auth_iter;
		auth_iter++;
	}

	m_list_hostauth_map.reset();

	vo_singlelink_list< DNS_Record * >::iterator dns_iter = m_list_dnsrecord_map.begin();
	vo_singlelink_list< DNS_Record * >::iterator dns_itere = m_list_dnsrecord_map.end();

	while( dns_iter != dns_itere )
	{
		delete *dns_iter;
		dns_iter++;
	}

	m_list_dnsrecord_map.reset();

#ifndef _LINUX_ANDROID
	m_sslload.FreeSSL();
#endif

#ifdef ENABLE_ASYNCDNS
    ares_library_cleanup();
#endif
}

//after free ssl lib, we should reset ssl persist socket as well.
VO_VOID vo_http_sessions_info::ResetPersistSSlsock()
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< LIVESOCKETMAP * >::iterator iter = m_list_livesocket_map.begin();
	vo_singlelink_list< LIVESOCKETMAP * >::iterator itere = m_list_livesocket_map.end();

	while( iter != itere )
	{
		if( (*iter)->is_https )
		{
			(*iter)->ptr_socket->Close();
		}
		iter++;
	}
}

VO_BOOL vo_http_sessions_info::get_authinfo( VO_CHAR * ptr_url  , VO_CHAR * ptr_auth )
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< HOSTAUTHMAP * >::iterator iter = m_list_hostauth_map.begin();
	vo_singlelink_list< HOSTAUTHMAP * >::iterator itere = m_list_hostauth_map.end();

	VO_CHAR str_host[256];
	memset( str_host , 0 , sizeof(str_host) );
	if( !get_hostbyurl( ptr_url , str_host , sizeof(str_host) ) )
	{
		VOLOGE("get host by url failed!!!!!!");
		return VO_FALSE;
	}

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->str_host , str_host  , strlen( str_host ) ) == 0)
		{
			strcpy( ptr_auth , (*iter)->str_auth );
			return VO_TRUE;
		}

		iter++;
	}

	return VO_FALSE;
}

VO_VOID vo_http_sessions_info::add_authinfo( VO_CHAR * ptr_url , VO_CHAR * ptr_auth )
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< HOSTAUTHMAP * >::iterator iter = m_list_hostauth_map.begin();
	vo_singlelink_list< HOSTAUTHMAP * >::iterator itere = m_list_hostauth_map.end();

	VO_CHAR str_host[256];
	memset( str_host , 0 , sizeof(str_host) );
	if( !get_hostbyurl( ptr_url , str_host ,sizeof(str_host) ) )
	{
		VOLOGE("get host by url failed!!!!!!");
		return;
	}

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->str_host , str_host  , strlen( str_host ) ) == 0)
			return;	

		iter++;
	}

	HOSTAUTHMAP * ptr_hostauth= new HOSTAUTHMAP;
	if( ptr_hostauth )
	{
		strcpy( ptr_hostauth->str_host , str_host);
		strcpy( ptr_hostauth->str_auth , ptr_auth);
		m_list_hostauth_map.push_back( ptr_hostauth );
	}
	else
	{
		VOLOGE("new obj failed");
	}

	return;
}

VO_CHAR * vo_http_sessions_info::get_redirecturl( const VO_CHAR * ptr_url )
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< REDIRECTURLMAP * >::iterator iter = m_list_redrecturl_map.begin();
	vo_singlelink_list< REDIRECTURLMAP * >::iterator itere = m_list_redrecturl_map.end();

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->str_pre_url , ptr_url  , strlen( ptr_url ) ) == 0)
			return (*iter)->str_redirect_url;

		iter++;
	}

	return 0;
}

VO_VOID vo_http_sessions_info::add_redirecturl( VO_CHAR * str_url , VO_CHAR * str_rdurl)
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< REDIRECTURLMAP * >::iterator iter = m_list_redrecturl_map.begin();
	vo_singlelink_list< REDIRECTURLMAP * >::iterator itere = m_list_redrecturl_map.end();

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->str_pre_url , str_url  , strlen( str_url ) ) == 0)
			return;	

		iter++;
	}

	REDIRECTURLMAP * ptr_rdurl = new REDIRECTURLMAP;
	if( ptr_rdurl )
	{
		strcpy( ptr_rdurl->str_pre_url , str_url);
		strcpy( ptr_rdurl->str_redirect_url , str_rdurl);
		m_list_redrecturl_map.push_back( ptr_rdurl );
	}
	else
	{
		VOLOGE("new obj failed");
	}

	return;
}

Persist_HTTP * vo_http_sessions_info::CreatePersistHTTP( VO_CHAR * str_host, VO_CHAR * str_port , VO_BOOL bHttps )
{
	voCAutoLock lock(&m_list_lock);
		
	LIVESOCKETMAP * ptr_livesocket = new LIVESOCKETMAP;
	if( !ptr_livesocket )
	{
		VOLOGE("new obj failed");
		return NULL;
	}

	memset( ptr_livesocket->str_host , 0 , sizeof( ptr_livesocket->str_host ) );
	memset( ptr_livesocket->str_port , 0 , sizeof( ptr_livesocket->str_port ) );

	Persist_HTTP * ptr_persist = 0;
	if( bHttps )
	{
		ptr_persist = new Persist_HTTPS;
		if( !ptr_persist )
		{
			VOLOGE("new obj failed");
			delete ptr_livesocket;
			return NULL;
		}
		ptr_livesocket->is_https = VO_TRUE;
		ptr_livesocket->ptr_socket = ptr_persist;
	}
	else
	{
		ptr_persist = new Persist_HTTP;
		if( !ptr_persist )
		{
			VOLOGE("new obj failed");
			delete ptr_livesocket;
			return NULL;
		}
		ptr_livesocket->is_https = VO_FALSE;
		ptr_livesocket->ptr_socket = ptr_persist;
	}
	
	strcpy( ptr_livesocket->str_port , str_port );
	strcpy( ptr_livesocket->str_host , str_host );

	m_list_livesocket_map.push_back( ptr_livesocket );

	return ptr_persist;
}

//check whether former socket can be reused in current session.
Persist_HTTP * vo_http_sessions_info::get_persistHTTP( const VO_CHAR * ptr_url )
{
	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< LIVESOCKETMAP * >::iterator iter = m_list_livesocket_map.begin();
	vo_singlelink_list< LIVESOCKETMAP * >::iterator itere = m_list_livesocket_map.end();

	VO_CHAR str_port[20];
	VO_CHAR str_host[256];

	memset( str_port , 0 , sizeof(str_port) );
	memset( str_host , 0 , sizeof(str_host) );

	if( !get_portbyurl( ptr_url , str_port , sizeof( str_port )) || !get_hostbyurl( ptr_url , str_host , sizeof( str_host )) )
	{
		VOLOGE("+++get host and port by url failed!!!");
		return NULL;
	}

	VOLOGE("persistSocket instance count:%d" , m_list_livesocket_map.count());

	//when reuse the persist socket, we should check the host & port & http/https prefix is same or not
	while( iter != itere )
	{
		if( strstr( ptr_url , (*iter)->str_host ) && 
			!strcmp( str_port , (*iter)->str_port ) &&
			Is_url_https( ptr_url ) == (*iter)->is_https 
			)
		{
			if( (*iter)->ptr_socket->lock() )
			{
				VOLOGI("[persist_sock] reuse socket: %d" , (*iter)->ptr_socket->socket);
				return (*iter)->ptr_socket;
			}
			else
			{
				VOLOGI("[persist_sock] socket :%d still inuse by thread: 0x%08x" , (*iter)->ptr_socket->socket , (*iter)->ptr_socket->ownerid );
			}
		}

		iter++;
	}	

	//create new persist socket for use
	Persist_HTTP *ptr_persist = NULL;
	ptr_persist = CreatePersistHTTP( str_host , str_port , Is_url_https( ptr_url ));
	if( ptr_persist )
	{
		ptr_persist->lock();
		return ptr_persist;
	}
	else
	{
			return NULL;
		}
}

VO_BOOL vo_http_sessions_info::IsPrefix(VO_CHAR * uriPath , VO_CHAR* cookiePath)
{
	VO_CHAR * ptr_src = 0;
	VO_CHAR * ptr_dest = 0;
	ptr_src = uriPath;
	ptr_dest = cookiePath;
	//skip char '/' and white space
	while( *ptr_src == '/' || *ptr_src == ' ')ptr_src++;
	while( *ptr_dest == '/' || *ptr_dest == ' ')ptr_dest++;

	if(ptr_src == strstr( ptr_src ,ptr_dest ))
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_sessions_info::IsFirstDifferenceSlash(VO_CHAR * uriPath , VO_CHAR* cookiePath)
{
	VO_CHAR * ptr_src = 0;
	VO_CHAR * ptr_dest = 0;
	ptr_src = uriPath;
	ptr_dest = cookiePath;

	while( *ptr_dest && *ptr_src 
		&& *ptr_dest == *ptr_src )
	{
		ptr_src++;
		ptr_dest++;
	}

	if( *ptr_src == '/')
	{
		return VO_TRUE;
	}

	return VO_FALSE;
	
}

//Pass  :IsPathMatch("/hello/facebook/" , "/");
//Pass  :IsPathMatch("/hello/facebook/" , "/hello"); 
//Pass  :IsPathMatch("/hello/facebook/" , "hello");
//Pass  :IsPathMatch("/hello/facebook/" , "hello/");
//Pass  :IsPathMatch("/hello/facebook/" , "/hello/");
VO_BOOL vo_http_sessions_info::IsPathMatch(VO_CHAR * uriPath , VO_CHAR* cookiePath)
{
	if( strlen(cookiePath) == strlen(uriPath) && 0 == voiostrnicmp( uriPath , cookiePath , strlen(cookiePath)) )
	{
		return VO_TRUE;
	}

	if( ( IsPrefix( uriPath ,cookiePath )) && '/' == cookiePath[strlen(cookiePath) - 1] )
	{
		return VO_TRUE;
	}

	if( IsPrefix( uriPath ,cookiePath ) &&  IsFirstDifferenceSlash(uriPath ,cookiePath) )
	{
		return VO_TRUE;
	}

	VOLOGI("path not match, Req path:%s , cookiepath:%s" , uriPath , cookiePath );
	return VO_FALSE;
}

VO_BOOL vo_http_sessions_info::IsDomainMatch(VO_CHAR * host , VO_CHAR* domain)
{
	//identical
	if( strlen(host) == strlen(domain) && !voiostrnicmp( host , domain , strlen(domain)) )
	{
		return VO_TRUE;
	}

	VO_CHAR* ptrpos = strncstr( host ,domain);
	if( ptrpos && 
		((ptrpos - host == strlen(host) - strlen(domain)) || ( ptrpos - host == strlen(host) - strlen(domain) - 1 && host[strlen(host) - 1] == '.')) &&
		!vostr_is_ip( host )
		)
	{
		return VO_TRUE;
	}

	VOLOGI("domain not match, host:%s , domain:%s" , host , domain );
	return VO_FALSE;
}

VO_BOOL vo_http_sessions_info::IsSecureMatch(VO_S32 secure_flag , VO_CHAR* url)
{
	if( 0 == secure_flag || ( 1 == secure_flag && Is_url_https(url)))
	{
		return VO_TRUE;
	}

	VOLOGI("secure not match, secure flag:%d , url:%s" , secure_flag , url );
	return VO_FALSE;
}

VO_BOOL vo_http_sessions_info::IsExpired(VO_S32 expire_time)
{
	if( current_date_time() >= expire_time )
	{
		VOLOGI("the cookie has been expired now" );
		return VO_TRUE;
	}

	return VO_FALSE;
}

//Cookie: SID=31d4d96e407aad42; lang=en-US
VO_BOOL vo_http_sessions_info::generate_cookie_str( VO_CHAR* str_url )
{
	voCAutoLock lock(&m_list_lock);
	VO_BOOL	bcookie = VO_FALSE;
	VO_CHAR str_host[256];
	memset( str_host , 0 , sizeof(str_host) );
	VO_CHAR str_path[2048];
	memset( str_path , 0 , sizeof(str_path) );
	
	if( !get_hostbyurl( str_url , str_host ,sizeof(str_host) ) )
	{
		VOLOGE("get host by url failed!!!");
		return VO_FALSE;
	}

	if( !get_pathbyurl( str_url , str_path , sizeof(str_path) ))
	{
		VOLOGE("get path by url failed!!!");
		return VO_FALSE;
	}

	memset( m_cookies , 0 , sizeof( m_cookies ) );

	vo_singlelink_list< cookie_storage_node * >::iterator iter = m_list_cookiestore_map.begin();
	vo_singlelink_list< cookie_storage_node * >::iterator itere = m_list_cookiestore_map.end();
	
	if( iter == itere )
	{
		return VO_FALSE;
	}

	strcat( m_cookies , "Cookie: ");
	while( iter != itere )
	{
		if( IsDomainMatch( str_host , (*iter)->domain ) && 
			IsPathMatch( str_path, (*iter)->path )  &&
			!IsExpired((*iter)->expiry_time) &&
			IsSecureMatch((*iter)->secure_only_flag , str_url)
			)

		{
			//when the user agent generates an HTTP request, the user agent MUST NOT attach more than one Cookie header field,
			//but it is ok to contain several Cookie into one header.
			if( bcookie )
			{
				strcat( m_cookies , "; ");
			}

			strcat( m_cookies , (*iter)->name );
			strcat( m_cookies , "=");
			strcat( m_cookies , (*iter)->value );

			//update last access time
			(*iter)->last_access_time = current_date_time();
			bcookie = VO_TRUE;
		}

		iter++;
	}

	strcat( m_cookies , "\r\n");

	if( !bcookie )
	{
		VOLOGI("cookie not found for this url");
		memset( m_cookies , 0 , sizeof(m_cookies) );
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_VOID vo_http_sessions_info::copy_storage_cookienode( cookie_storage_node* ptr_destnode , cookie_storage_node* ptr_srcnode )
{
	if( !ptr_destnode || !ptr_srcnode )
	{
		return;
	}

	strcpy( ptr_destnode->name , ptr_srcnode->name );
	strcpy( ptr_destnode->value , ptr_srcnode->value );
	strcpy( ptr_destnode->domain , ptr_srcnode->domain );
	strcpy( ptr_destnode->path , ptr_srcnode->path );

	ptr_destnode->expiry_time = ptr_srcnode->expiry_time;
	ptr_destnode->creation_time = ptr_srcnode->creation_time;
	ptr_destnode->last_access_time = ptr_srcnode->last_access_time;
	ptr_destnode->persistent_flag = ptr_srcnode->persistent_flag;
	ptr_destnode->host_only_flag = ptr_srcnode->host_only_flag;
	ptr_destnode->secure_only_flag = ptr_srcnode->secure_only_flag;
	ptr_destnode->http_only_flag = ptr_srcnode->http_only_flag;
	ptr_destnode->invalid = ptr_srcnode->invalid;
}

//set http proxy info, and store it in global storage for future usage.
VO_VOID vo_http_sessions_info::UpdateProxyInfo( VO_SOURCE2_HTTPPROXY* ptr_proxyinfo )
{
	//check parameter validity
	if( !ptr_proxyinfo )
	{
		VOLOGE("the proxy parameter setin is null");
		return;
	}

	memset( m_proxyhost , 0 , sizeof( m_proxyhost ) );
	memset( m_proxyport , 0 , sizeof( m_proxyport ) );

	if( strlen( ptr_proxyinfo->szProxyHost ) < sizeof(m_proxyhost) 
		&& strlen( ptr_proxyinfo->szProxyPort ) < sizeof(m_proxyport) )
	{
		strcpy( m_proxyhost , ptr_proxyinfo->szProxyHost );
		strcpy( m_proxyport , ptr_proxyinfo->szProxyPort );
	}
	else
	{
		VOLOGE("the proxy host or port is too long");
	}
}

//set http header info from external, and store it in global storage for future usage.
VO_VOID vo_http_sessions_info::UpdateExternalHeaderInfo( VO_SOURCE2_HTTPHEADER* ptr_headinfo )
{
	//check parameter validity
	if( !ptr_headinfo )
	{
		VOLOGE("the parameter setin is null");
		return;
	}

	if( !voiostrnicmp( ptr_headinfo->szHeaderName ,  "Set-Cookie"  , strlen( "Set-Cookie" ) ) )
	{
		//we need to process and update cookie info to storage,waiting to be completed
		VOLOGI("%s" , ptr_headinfo->szHeaderValue );
		VO_CHAR	 setcookie[2048];
		strcpy( setcookie , "Set-Cookie: ");
		strcat( setcookie , ptr_headinfo->szHeaderValue );

		vo_http_cookie http_cookie;
		http_cookie.Reset();

		//when set cookie from user side, we should allow not set these info.
		//http_cookie.set_urlinfo((VO_CHAR*)ptr_headinfo->szDomain , (VO_CHAR*)ptr_headinfo->szPath ,(VO_CHAR*) "80");
		if(http_cookie.process_cookie( setcookie ))
		{
			update_cookie_info( http_cookie.get_cooki_node() );
		}
	}
	else if(  !voiostrnicmp( ptr_headinfo->szHeaderName ,  "User-Agent"  , strlen( "User-Agent" ) ) )
	{
		VOLOGI("%s" , ptr_headinfo->szHeaderValue );
		memset( m_useragent , 0 , sizeof( m_useragent ) );
		strcpy( m_useragent , ptr_headinfo->szHeaderValue);
	}
	else
	{
		//other header info , just print and wait to be completed..
		VOLOGI("%s" , ptr_headinfo->szHeaderValue );
	}
}

//set http storage mode cookie info from external.
VO_VOID vo_http_sessions_info::UpdateExternalCookieInfo( PHTTPCookieExternal ptr_cookie )
{
	voCAutoLock lock(&m_list_lock);
	if( !ptr_cookie )
	{
		return;
	}

	//push the node to storage according to rule...................

	vo_singlelink_list< cookie_storage_node * >::iterator iter = m_list_cookiestore_map.begin();
	vo_singlelink_list< cookie_storage_node * >::iterator itere = m_list_cookiestore_map.end();

	VO_BOOL bnode_exist = VO_FALSE;
	while( iter != itere )
	{
		if(
			voiostrnicmp( (*iter)->name ,  ptr_cookie->name  , strlen(  ptr_cookie->name ) ) == 0 &&
			voiostrnicmp( (*iter)->domain ,  ptr_cookie->domain  , strlen(  ptr_cookie->domain ) ) == 0 &&
			voiostrnicmp( (*iter)->path ,  ptr_cookie->path  , strlen(  ptr_cookie->path ) ) == 0 
			)
		{
			bnode_exist = VO_TRUE;
			break;
		}

		iter++;
	}

	if( !bnode_exist )
	{
		cookie_storage_node* ptr_newnode = new cookie_storage_node;
		if( !ptr_newnode )
		{
			return;
		}
		strcpy( ptr_newnode->name , ptr_cookie->name);
		strcpy( ptr_newnode->value , ptr_cookie->value);
		strcpy( ptr_newnode->domain , ptr_cookie->domain);
		strcpy( ptr_newnode->path , ptr_cookie->path);
		ptr_newnode->expiry_time = ptr_cookie->expire;
		ptr_newnode->secure_only_flag = ptr_cookie->secure;
		ptr_newnode->creation_time = current_date_time();
		ptr_newnode->last_access_time = current_date_time();
		ptr_newnode->invalid = VO_FALSE;
		ptr_newnode->host_only_flag = 0;
		ptr_newnode->http_only_flag = 0;
		ptr_newnode->persistent_flag = 1;

		m_list_cookiestore_map.push_back( ptr_newnode );
	}
	else
	{
		//update exist field value
		(*iter)->expiry_time = ptr_cookie->expire ;
		strcpy( (*iter)->value , ptr_cookie->value );
		(*iter)->secure_only_flag = ptr_cookie->secure;
		(*iter)->last_access_time = current_date_time();
		(*iter)->invalid = VO_FALSE;
	}
	
	
	//remove expired cookie node
	//to be completed..............
}

VO_VOID vo_http_sessions_info::update_cookie_info( cookie_storage_node* pCookieNode )
{
	voCAutoLock lock(&m_list_lock);
	if( !pCookieNode )
	{
		return;
	}

	cookie_storage_node * ptr_node = pCookieNode;

	while( ptr_node )
	{
		if( ptr_node->invalid )
		{
			ptr_node = ptr_node->pNext;
			continue;
		}

		VOLOGI("cookie name: %s ,	 value: %s ,	domain:%s ,	path:%s ,	expire time:%d	" , 
			ptr_node->name , ptr_node->value , ptr_node->domain , ptr_node->path , ptr_node->expiry_time );
		//push the node to storage according to rule...................

		vo_singlelink_list< cookie_storage_node * >::iterator iter = m_list_cookiestore_map.begin();
		vo_singlelink_list< cookie_storage_node * >::iterator itere = m_list_cookiestore_map.end();

		VO_BOOL bnode_exist = VO_FALSE;
		while( iter != itere )
		{
			if(
				voiostrnicmp( (*iter)->name ,  ptr_node->name  , strlen(  ptr_node->name ) ) == 0 &&
				voiostrnicmp( (*iter)->domain ,  ptr_node->domain  , strlen(  ptr_node->domain ) ) == 0 &&
				voiostrnicmp( (*iter)->path ,  ptr_node->path  , strlen(  ptr_node->path ) ) == 0 
				)
			{
				bnode_exist = VO_TRUE;
				break;
			}

			iter++;
		}

		if( !bnode_exist )
		{
			cookie_storage_node* ptr_newnode = new cookie_storage_node;
			if( !ptr_newnode )
			{
				return;
			}
			copy_storage_cookienode( ptr_newnode , ptr_node );

			m_list_cookiestore_map.push_back( ptr_newnode );
		}
		else
		{
			if( !ptr_node->invalid )
			{
				//update exist field value
				(*iter)->expiry_time = ptr_node->expiry_time ;
				(*iter)->last_access_time = ptr_node->last_access_time ;
				(*iter)->creation_time = ptr_node->creation_time ;
				strcpy( (*iter)->value , ptr_node->value );
				(*iter)->persistent_flag = ptr_node->persistent_flag;
				(*iter)->host_only_flag = ptr_node->host_only_flag;
				(*iter)->secure_only_flag = ptr_node->secure_only_flag;
				(*iter)->http_only_flag = ptr_node->http_only_flag;
			}
		}

		ptr_node = ptr_node->pNext;
	}
	
	//remove expired cookie node
	//to be completed..............

}

//get url direcory path, for example: https://hls.ftgroup-devices.com/testCookie2/testCookieIndex.php?file=index.m3u8
//the path should be: /testCookie2
VO_BOOL vo_http_sessions_info::get_pathbyurl( const VO_CHAR * ptr_url , VO_CHAR * ptr_path , VO_S32 size )
{
	VO_CHAR * ptr = strncstr( (VO_CHAR *)ptr_url , (VO_CHAR*)"http://" );

	if( ptr )
	{
		ptr = ptr + strlen( "http://" );
	}
	else
	{
		ptr = strncstr( (VO_CHAR *)ptr_url , (VO_CHAR*)"https://" );
		if( ptr )
		{
			ptr = ptr + strlen( "https://" );
		}
		else
		{
			ptr = (VO_CHAR *)ptr_url;
		}
	}

	VO_CHAR * ptr_temp = strchr( ptr , '/');

	if( ptr_temp )
	{
		if( strlen( ptr_temp ) >= (unsigned int )size )
		{
			VOLOGE("avoid stack corruption, so exit");
			return VO_FALSE;
		}
		else
		{
			strcpy( ptr_path , ptr_temp);
		}
	}
	else
	{
		strcpy( ptr_path , "/" );
	}

	return VO_TRUE;
}

VO_BOOL vo_http_sessions_info::get_portbyurl( const VO_CHAR * ptr_url , VO_CHAR * ptr_port , VO_S32 size )
{
	VO_CHAR str_host[256];
	memset( str_host , 0 , sizeof(str_host) );

	//http://m.tvpot.daum.net 
	//we should deal with such case

	VO_CHAR * ptr = (VO_CHAR *)ptr_url;

	if( voiostrnicmp( ptr , "http://" ,strlen("http://")) == 0 )
	{
		ptr = ptr + strlen( "http://" );
	}
	else
	{
		if( voiostrnicmp( ptr , "https://" ,strlen("https://")) == 0 )
		{
			ptr = ptr + strlen( "https://" );
		}
		else
		{
			ptr = (VO_CHAR *)ptr_url;
		}
	}

	VO_CHAR * ptr_div = strchr( ptr , '/' );
	
	if(ptr_div == NULL)
	{
		if( strlen( (const char *)ptr ) >= sizeof( str_host )  )
		{
			VOLOGE("avoid stack corruption, so exit");
			return VO_FALSE;
		}
		else
		{
			strcpy( str_host , ptr);
		}
	}
	else
	{
		if( ptr_div - ptr >= sizeof( str_host ) )
		{
			VOLOGE("avoid stack corruption, so exit");
			return VO_FALSE;
		}
		else
		{
			strncpy( str_host , ptr , ptr_div - ptr );
		}
	}

	ptr_div = strrchr( str_host , ':' );

	if( ptr_div )
	{
		ptr_div++;
		if( strlen( ptr_div ) >= (unsigned int )size )
		{
			VOLOGE("avoid stack corruption, so exit");
			return VO_FALSE;
		}
		else
		{
			strcpy( ptr_port , ptr_div );
		}
		
		if( !vostr_is_digit(ptr_port) )
		{
			strcpy( ptr_port , "80" );
		}
	}
	else
	{
		strcpy( ptr_port , "80" );
	}

	return VO_TRUE;
}

VO_BOOL	vo_http_sessions_info::get_hostbyurl( const VO_CHAR * ptr_url , VO_CHAR * ptr_host , VO_S32 size )
{
	VO_BOOL bhttps = VO_FALSE;
	VO_CHAR * ptr_pos = (VO_CHAR *)ptr_url;
	if( voiostrnicmp( ptr_pos , "http://" ,strlen("http://")) != 0 )
	{
		if( voiostrnicmp( ptr_pos , "https://" ,strlen("https://")) == 0 )
		{
			bhttps = VO_TRUE;
		}
		else
		{
			ptr_pos = NULL;
		}
	}

	if( ptr_pos )
	{
		if(bhttps)
		{
			ptr_pos = ptr_pos + strlen( "https://" );
		}
		else
		{
			ptr_pos = ptr_pos + strlen( "http://" );
		}
		
		VO_CHAR * ptr_end = strchr( ptr_pos , '/' );
		if( ptr_end )
		{
			if( ptr_end - ptr_pos >= size )
			{
				VOLOGE("avoid stack corruption, so exit");
				return VO_FALSE;
			}
			memcpy( ptr_host , ptr_pos , ptr_end - ptr_pos );

			//check if the host contain port
			ptr_end = strrchr( ptr_host , ':');
			if( ptr_end && vostr_is_digit(ptr_end+1) )
			{
				*ptr_end = '\0';
			}

			return VO_TRUE;
		}
	}

	return VO_FALSE;
}

//update dns record info 
VO_VOID vo_http_sessions_info::UpdateDnsCache( DNS_Record *ptr_dnsrecord )
{
	voCAutoLock lock(&m_list_lock);
	if( !ptr_dnsrecord )
	{
		return;
	}

	vo_singlelink_list< DNS_Record * >::iterator iter = m_list_dnsrecord_map.begin();
	vo_singlelink_list< DNS_Record * >::iterator itere = m_list_dnsrecord_map.end();

	VO_BOOL bnode_exist = VO_FALSE;
	while( iter != itere )
	{
		if( voiostrnicmp( (*iter)->host ,  ptr_dnsrecord->host  , strlen(  ptr_dnsrecord->host ) ) == 0 )
		{
			bnode_exist = VO_TRUE;
			break;
		}

		iter++;
	}

	if( !bnode_exist )
	{
		DNS_Record* ptr_newnode = new DNS_Record;
		if( !ptr_newnode )
		{
			return;
		}
		strcpy( ptr_newnode->host , ptr_dnsrecord->host);
		strcpy( ptr_newnode->ipaddr , ptr_dnsrecord->ipaddr);
		ptr_newnode->expiredtime = ptr_dnsrecord->expiredtime;

		m_list_dnsrecord_map.push_back( ptr_newnode );
	}
	else
	{
		//update exist field value
		if( voiostrnicmp( (*iter)->ipaddr ,  ptr_dnsrecord->ipaddr  , strlen(  ptr_dnsrecord->ipaddr ) ) != 0 )
		{
			strcpy( (*iter)->ipaddr , ptr_dnsrecord->ipaddr );
		}
		else
		{
			VOLOGI("keep old ip: %s for host: %s" , ptr_dnsrecord->ipaddr , ptr_dnsrecord->host );
		}
		
		(*iter)->expiredtime = ptr_dnsrecord->expiredtime;
	}
}

VO_CHAR * vo_http_sessions_info::QueryCachedIpbyhost( const VO_CHAR * ptr_host )
{
	if( !ptr_host )
	{
		return 0;
	}

	voCAutoLock lock(&m_list_lock);
	vo_singlelink_list< DNS_Record * >::iterator iter = m_list_dnsrecord_map.begin();
	vo_singlelink_list< DNS_Record * >::iterator itere = m_list_dnsrecord_map.end();

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->host , ptr_host  , strlen( ptr_host ) ) == 0)
		{
			if((*iter)->expiredtime > voOS_GetSysTime())
			{
				return (*iter)->ipaddr;
			}
			else
			{
				VOLOGE(" Cached DNS Record is expired " );
			}
		}
			

		iter++;
	}

	return 0;
}