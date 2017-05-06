#include "vo_http_utils.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

//Format the provided time according to the specified format.  The
//format is a string with format elements supported by strftime.  
static VO_CHAR *fmttime (time_t t, const char *fmt)
{
  static VO_CHAR output[32];
#ifndef WINCE
  struct tm *tm = localtime(&t);
  if (!tm)
  {
	  return (VO_CHAR*)" ";
  }
  if (!strftime(output, sizeof(output), fmt, tm))
  {
	  return (VO_CHAR*)" ";
  }
  return output;
#else
  return (VO_CHAR*)" ";
#endif
}

// Return pointer to a static char[] buffer in which zero-terminated
// string-representation of TM (in form hh:mm:ss) is printed.
// If TM is NULL, the current time will be used

VO_CHAR *time2str (time_t t)
{
	return fmttime(t, "%H:%M:%S");
}

// Like the above, but include the date: YYYY-MM-DD hh:mm:ss. 

VO_CHAR *datetime2str (time_t t)
{
	return fmttime(t, "%Y-%m-%d %H:%M:%S");
}

VO_S32 current_date_time()
{
#ifndef WINCE
	time_t cookies_now;
	return (VO_S32)time( &cookies_now );
#else
	return 0;
#endif
}

VO_BOOL vo_c_isdigit(VO_CHAR c)
{
	return ( (c > '9' || c < '0') ? VO_FALSE : VO_TRUE );
}

VO_BOOL vo_c_isspace(VO_CHAR c)
{
	return ( c == ' ' ? VO_TRUE : VO_FALSE );
}

VO_CHAR * strncchr( VO_CHAR * ptr_src , VO_CHAR chr )
{
	VO_CHAR  * ptr = NULL;
	if( chr >= 'a' && chr <= 'z' )
	{
		VO_CHAR * ptr_low = NULL;
		VO_CHAR * ptr_up = NULL;

		ptr_low = strchr( ptr_src , chr );
		ptr_up = strchr( ptr_src , chr + 'A' - 'a' );

		ptr = ptr_low < ptr_up ? ptr_low : ptr_up;

		if( ptr == NULL )
			ptr = (VO_U32)ptr_low + ptr_up;
	}
	else if( chr >= 'A' && chr <= 'Z' )
	{
		VO_CHAR * ptr_low = NULL;
		VO_CHAR * ptr_up = NULL;

		ptr_up = strchr( ptr_src , chr );
		ptr_low = strchr( ptr_src , chr - 'A' + 'a' );

		ptr = ptr_low < ptr_up ? ptr_low : ptr_up;

		if( ptr == NULL )
			ptr = (VO_U32)ptr_low + ptr_up;
	}
	else
	{
		ptr = strchr( ptr_src , chr );
	}

	return ptr;
}

VO_CHAR * strncstr( VO_CHAR * ptr_src , VO_CHAR * ptr_str )
{
	VO_CHAR * ptr = strncchr( ptr_src , ptr_str[0] );


	while( ptr )
	{
		if(voiostrnicmp( ptr , ptr_str , strlen( ptr_str ) ) == 0)
			return ptr;
		else
			ptr = strncchr( ptr + 1 , ptr_str[0] );
	}

	return NULL;
}

VO_BOOL vostr_is_digit(VO_CHAR * ptr_data )
{
	for( int i=0; i < (int)strlen(ptr_data) ; i++ )
	{
		if( *(ptr_data + i) > '9' || *(ptr_data + i) < '0' )
		{
			return VO_FALSE;
		}
	}
	return VO_TRUE;
}

VO_S32 count_char( VO_CHAR * strdata , VO_CHAR ch) 
{
  VO_S32 count = 0;

  for (VO_U32 i = 0; i < strlen(strdata); i++)
  {
    if ( strdata[i] == ch)
	{
		count++;
	}
  }

  return count;
}

VO_BOOL vostr_is_ip(VO_CHAR * ptr)
{
	if( !(*ptr) )
	{
		return VO_FALSE;
	}

	while( *ptr)
	{
		if( *ptr != '.' && !vo_c_isdigit(*ptr) )
		{
			return VO_FALSE;
		}
		ptr++;
	}

	return VO_TRUE;
}

VO_BOOL Is_url_https( const VO_CHAR * ptr_url )
{
	if( strncstr( (VO_CHAR *)ptr_url ,(VO_CHAR *)  "https://" ) ==  ptr_url )
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

// Converts a hex character to its integer value
VO_CHAR from_hex(VO_CHAR ch) 
{
	return vo_c_isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

// Converts an integer value to its hex character
VO_CHAR to_hex(VO_CHAR code) 
{
	static VO_CHAR hex[] = "0123456789abcdef";
	return hex[code & 15];
}

// Returns a url-encoded version of str
// be sure to free() the returned string after use
VO_CHAR *url_encode(VO_CHAR *str) 
{
	VO_CHAR *pstr = str;
	VO_CHAR *buf = (VO_CHAR *)malloc(strlen(str) * 3 + 1);
	VO_CHAR *pbuf = buf;
	while (*pstr) 
	{
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
			*pbuf++ = *pstr;
		else if (*pstr == ' ') 
			*pbuf++ = '+';
		else 
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

// Returns a url-decoded version of str
// be sure to free() the returned string after use
VO_CHAR *url_decode(VO_CHAR *str) 
{
	VO_CHAR *pstr = str;
	VO_CHAR *buf = (VO_CHAR *)malloc(strlen(str) + 1);
	VO_CHAR *pbuf = buf;
	while (*pstr) 
	{
		if (*pstr == '%') 
		{
			if (pstr[1] && pstr[2]) 
			{
				*pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
				pstr += 2;
			}
		} 
		else if (*pstr == '+') 
		{ 
			*pbuf++ = ' ';
		} 
		else 
		{
			*pbuf++ = *pstr;
		}
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}


#ifdef _VONAMESPACE
}
#endif
