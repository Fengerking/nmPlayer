#pragma once
#ifndef WINCE
#include "voYYDef_SourceIO.h"
#include <time.h>
#include <locale.h>
#endif
#include "voString.h"
#include "voOSFunc.h"
#include <ctype.h>

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
// Converts a hex character to its integer value
VO_CHAR from_hex(VO_CHAR ch);
// Converts an integer value to its hex character
VO_CHAR to_hex(VO_CHAR code);
// Returns a url-encoded version of str
// be sure to free() the returned string after use
VO_CHAR *url_encode(VO_CHAR *str);
// Returns a url-decoded version of str
// be sure to free() the returned string after use
VO_CHAR *url_decode(VO_CHAR *str);

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