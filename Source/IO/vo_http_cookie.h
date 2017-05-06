#pragma once
#include "voYYDef_SourceIO.h"
#include "Buffer/vo_singlelink_list.hpp"
#include "vo_http_struct.h"

#ifndef WINCE
#include <time.h>
#include <locale.h>
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class vo_http_cookie
{
public:
	vo_http_cookie();
	virtual ~vo_http_cookie(void);

public:
	cookie_storage_node* get_cooki_node(){ return m_pcookienode_head; }
	VO_BOOL process_cookie( VO_CHAR * strcookie );
	VO_BOOL Is_cookie_update();
	VO_VOID set_urlinfo( VO_CHAR* host , VO_CHAR * path , VO_CHAR * port );
	VO_VOID Reset();
private:
	//analyze cookies
	VO_VOID analyze_cookie_settings();
	VO_VOID analyze_setcookie_string( VO_CHAR * str_setcookie );
	VO_BOOL analyze_name_value_pair( VO_CHAR * str_name_value_pair );
	VO_VOID analyze_unparsed_av( VO_CHAR* str_av );
	VO_BOOL analyze_attribute(VO_CHAR* str_attrib);
	VO_VOID add_cookie_namevalue( VO_CHAR * str_name , VO_CHAR * str_value);
	VO_VOID add_attribute_namevalue( VO_CHAR * str_name , VO_CHAR * str_value );
	VO_S32 analyze_cookie_dates( VO_CHAR* str_cookie_date );
	VO_BOOL	isdelimiter(VO_CHAR ch);
	VO_BOOL isnondelimiter(VO_CHAR ch);
	VO_BOOL process_datetoken(VO_CHAR* datetoken);
	VO_BOOL MapAlphaMonth2Num(VO_CHAR* month);

	//store cookie
	VO_BOOL process_cookie2storage();
	VO_VOID process_persistentflag_expiretime();
	VO_BOOL process_domain();
	VO_VOID process_path();
	VO_VOID process_secure();
	VO_VOID process_httponly();

	VO_VOID set_cookienode_invalid( VO_BOOL binvalid );
	VO_VOID destroy_cookie_info();
	VO_VOID init_cookie_node();
	VO_VOID ComputeDefaultpath(VO_CHAR *path , VO_CHAR *destpath);
	VO_BOOL IsDomainMatch(VO_CHAR * host , VO_CHAR* domain);

	VO_VOID add_storage_cookienode();
private:
	vo_singlelink_list< vo_http_cookie_namevalue_map * > m_list_cookienv_map;
	vo_http_cookie_namevalue_map * m_ptr_tempnamevalue;

	cookie_storage_node  m_http_cookie_node;
	cookie_storage_node * m_pcookienode_head;
	cookie_storage_node * m_pcookienode_tail;
	VO_CHAR m_response[8192];
	VO_BOOL m_is_cookie_update;
	VO_CHAR m_host[256];
	VO_CHAR m_path[1024];
	VO_CHAR m_port[10];
	cookies_date_found_flag m_cookie_dateflag;
#ifndef WINCE
	struct tm m_broken_time;
#endif
};

#ifdef _VONAMESPACE
}
#endif