#pragma once
#include "voString.h"
#include "Buffer/vo_singlelink_list.hpp"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif
/*****************************************
** http 1.1 status code definitions ****
****************************************/

/**************************************** 
**					Informational 1xx.	***
*****************************************/
#define HTTP_STATUS_CONTINUE			  100
#define HTTP_STATUS_SWITCH_PROTOCOL		  101

/**************************************** 
**					Successful 2xx.	***** 
*****************************************/
#define HTTP_STATUS_SUCCESSFUL			      2
#define HTTP_STATUS_OK                    200
#define HTTP_STATUS_CREATED               201
#define HTTP_STATUS_ACCEPTED              202
#define HTTP_STATUS_NONAUTHORITIVE		  203
#define HTTP_STATUS_NO_CONTENT            204
#define HTTP_STATUS_RESET_CONTENT		  205
#define HTTP_STATUS_PARTIAL_CONTENTS      206

/**************************************** 
**					Redirection 3xx.  ***
*****************************************/
#define HTTP_STATUS_REDIRECTION			  3
#define HTTP_STATUS_MULTIPLE_CHOICES      300
#define HTTP_STATUS_MOVED_PERMANENTLY     301
#define HTTP_STATUS_MOVED_TEMPORARILY     302
#define HTTP_STATUS_SEE_OTHER             303 
#define HTTP_STATUS_NOT_MODIFIED          304
#define HTTP_STATUS_USE_PROXY			  305
#define HTTP_STATUS_UNUSED_1			  306
#define HTTP_STATUS_TEMPORARY_REDIRECT    307 

/**************************************** 
**					Client error 4xx. *** 
*****************************************/
#define HTTP_STATUS_CLIENT_ERROR	      4
#define HTTP_STATUS_BAD_REQUEST           400
#define HTTP_STATUS_UNAUTHORIZED          401
#define HTTP_STATUS_PAYMENT_REQ			  402
#define HTTP_STATUS_FORBIDDEN             403
#define HTTP_STATUS_NOT_FOUND             404
#define HTTP_STATUS_METHOD_NOT_ALLOWED	  405
#define HTTP_STATUS_NOT_ACCEPTABLE		  406
#define HTTP_STATUS_PROXY_UNAUTHORIZED	  407
#define HTTP_STATUS_REQUEST_TIMEOUT		  408
#define HTTP_STATUS_CONFLICT			  409
#define HTTP_STATUS_GONE				  410
#define HTTP_STATUS_LENGTH_REQUIRED		  411
#define HTTP_STATUS_PRECONDITION_FAIL	  412
#define HTTP_STATUS_REQ_ENTITY_TOOLARGE	  413
#define HTTP_STATUS_REQ_URI_TOOLARGE	  414
#define HTTP_STATUS_UNSUPPORT_MEDIA_TYPE  415
#define HTTP_STATUS_RANGE_NOT_SATISFIABLE 416
#define HTTP_STATUS_EXPECTATION_FAIL	  417

/**************************************** 
**					 Server errors 5xx.***  
*****************************************/
#define HTTP_STATUS_SERVER_ERROR          5
#define HTTP_STATUS_INTERNAL              500
#define HTTP_STATUS_NOT_IMPLEMENTED       501
#define HTTP_STATUS_BAD_GATEWAY           502
#define HTTP_STATUS_UNAVAILABLE           503
#define HTTP_STATUS_GATEWAY_TIMEOUT		  504
#define HTTP_STATUS_VERSIONNOT_SUPPORT	  505

#define DNSCACHE_EXPIREDTIME (24 * 60 * 60 * 1000)

enum DOWNLOAD_STATUS
{
	VO_CONNECTING = 1,
	VO_GOTRESPONSE,
	VO_DOWNLOAD_CONTENT,
	VO_DOWNLOAD_NORMAL_EXIT,
	VO_DOWNLOAD_INTERRUPT,
	VO_DOWNLOAD_PARTIALLY,
};

enum DOWNLOAD_MODE
{
	VO_DOWNLOAD_NORMAL = 1,
	VO_DOWNLOAD_CHUNK,
	VO_DOWNLOAD_ENDLESS,
};

struct vo_http_download_info
{
	VO_U32  download_start_time;
	VO_U32  download_end_time;
	VO_S64  total_download;
	VO_U32	download_speed;
	VO_S32  error_code;
	VO_S32  download_status;
	VO_S32  download_mode;
	VO_U32  get_response_time;
};

struct vo_http_cookie_attrib_map
{
	VO_CHAR str_attrib_name[256];
	VO_CHAR str_attrib_value[1024];
};

struct vo_http_cookie_namevalue_map
{
	VO_CHAR str_cookie_name[256];
	VO_CHAR str_cookie_value[2048];
	vo_singlelink_list< vo_http_cookie_attrib_map * > m_list_attrenv_map;
};

 
typedef struct tagHTTPCookieExternal
{
	VO_PCHAR name;					//name of the cookie, must not be null
	VO_PCHAR value;					//value of the cookie, must not be null

	VO_S32 expire;					//specifies a UNIX time_t values that defines the valid life time of that cookie. Once the expiration 
									//date has been reached, the cookie will no longer be stored or given out.it must be UTC time.

	VO_PCHAR path;					//is used to specify the subset of URLs in a domain for which the cookie is valid.
	VO_PCHAR domain;				//indicates the domain for which the cookie should be sent

	VO_S32  secure;					//if a cookie is marked secure, it will only be transmitted if the communications channel with 
									//the host is a secure one. A secure cookie will only be sent to the server when a request 
									//is made using SSL and the HTTPS protocol.
									//defalut to be 0.
} HTTPCookieExternal, *PHTTPCookieExternal;

struct REDIRECTURLMAP
{
	VO_CHAR str_pre_url[2048];
	VO_CHAR str_redirect_url[2048];
};

struct HOSTAUTHMAP
{
	VO_CHAR str_host[1024];
	VO_CHAR str_auth[1024];
};

//cookie's Storage Model
struct  cookie_storage_node
{
	VO_CHAR name[256];
	VO_CHAR value[1024];
	VO_S32  expiry_time;
	VO_CHAR domain[256];
	VO_CHAR path[256];
	VO_S32 creation_time;
	VO_S32 last_access_time;
	int persistent_flag;
	int host_only_flag;
	int secure_only_flag;
	int http_only_flag;
	VO_BOOL invalid;
	cookie_storage_node * pNext;
};

enum IO_HTTPDATA2BUFFER_DATA_TYPE
{
	VO_CONTENT_DATA = 1,
	VO_CONTENT_LEN,
	VO_CONTENT_MEMINFO_PLUS,
	VO_CONTENT_MEMINFO_MINUS,
	VO_CONTENT_DOWNLOAD_SPEED,
	VO_CONTENT_OPEN_SUCCESS,
	VO_CONTENT_OPEN_FAILED,
	VO_CONTENT_READAVAILABLE,
	VO_CONTENT_COOKIE_UPDATED,
	VO_CONTENT_DNSINFO_UPDATED,
	VO_CONTENT_DNSINFO_QUERY,
	VO_CONTENT_DOWNLOADSETTING_UPDATED,
};

struct VO_HTTP_DOWNLOAD_BUFFER
{
	VO_HANDLE	download_handle;
	VO_HANDLE	buffer_handle;
};

typedef VO_S64 (VO_API * IO_HTTPDATA2BUFFER_CALLBACK_FUNC)( VO_HANDLE phandle, VO_PBYTE buffer, VO_U64 size , IO_HTTPDATA2BUFFER_DATA_TYPE type );
struct VO_HTTP_DOWNLOAD2BUFFERCALLBACK
{
	VO_HANDLE handle;
	IO_HTTPDATA2BUFFER_CALLBACK_FUNC HttpStreamCBFunc;
};

//cookie date found flag
struct cookies_date_found_flag
{
	VO_BOOL found_time;
	VO_BOOL found_day_of_month;
	VO_BOOL found_month;
	VO_BOOL found_year;
};

//dns record info
struct DNS_Record
{
	VO_CHAR host[256];
	VO_CHAR ipaddr[65];
	VO_U32	expiredtime;
};

#ifdef _VONAMESPACE
}
#endif
