#pragma once
#ifndef WINCE
#include <time.h>
#include <locale.h>
#endif
#include "voString.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_CHAR *time2str (time_t tm);
VO_CHAR *datetime2str (time_t tm);
VO_S32 current_date_time();
VO_BOOL vo_c_isdigit(VO_CHAR c);
VO_BOOL vo_c_isspace(VO_CHAR c);
VO_CHAR * strncchr( VO_CHAR * ptr_src , VO_CHAR chr );
VO_CHAR * strncstr( VO_CHAR * ptr_src , VO_CHAR * ptr_str );
VO_BOOL vostr_is_digit(VO_CHAR * ptr_data );
VO_S32 count_char( VO_CHAR * strdata , VO_CHAR ch);
VO_BOOL vostr_is_ip(VO_CHAR * ptr);
VO_BOOL Is_url_https( const VO_CHAR * ptr_url );

#ifdef WINCE 
			#define voiostrnicmp        _strnicmp
#elif WIN32
			#define voiostrnicmp        strnicmp
#else
			#define voiostrnicmp        strncasecmp
#endif

#ifdef _VONAMESPACE
}
#endif