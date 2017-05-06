#include "vo_http_cookie.h"
#include "voString.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "vo_http_utils.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#define ONEWEEKSECONDS	(3600 * 24 * 7)

vo_http_cookie::vo_http_cookie()
: m_ptr_tempnamevalue(NULL)
 ,m_pcookienode_head(NULL)
 ,m_pcookienode_tail(NULL)
 ,m_is_cookie_update(VO_FALSE)
{
}

vo_http_cookie::~vo_http_cookie(void)
{
	destroy_cookie_info();
}


VO_VOID vo_http_cookie::Reset()
{
	destroy_cookie_info();

	m_is_cookie_update = VO_FALSE;
	memset( m_response , 0 , sizeof(m_response) );
	memset( m_host , 0 , sizeof(m_host) );
	memset( m_path , 0 , sizeof(m_path) );
	memset( m_port , 0 , sizeof(m_port) );
}

VO_BOOL vo_http_cookie::process_cookie( VO_CHAR * strcookie )
{
	VO_BOOL ret = VO_FALSE;

	m_is_cookie_update = VO_FALSE;

	destroy_cookie_info();

	if( strlen( strcookie ) < sizeof(m_response) )
	{
		strcpy( m_response , strcookie );
	}
	else
	{
		VOLOGE(	"cookie string to large");
		return VO_FALSE;
	}

	//analyze cookie setting from the response of server
	analyze_cookie_settings();

	//process and store cookie info  
	ret = process_cookie2storage();
	if( !ret )
	{
		set_cookienode_invalid( VO_TRUE );
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_VOID vo_http_cookie::set_cookienode_invalid( VO_BOOL binvalid )
{
	m_http_cookie_node.invalid = binvalid;
}

VO_VOID vo_http_cookie::destroy_cookie_info()
{
	vo_singlelink_list< vo_http_cookie_namevalue_map * >::iterator cookie_iter = m_list_cookienv_map.begin();
	vo_singlelink_list< vo_http_cookie_namevalue_map * >::iterator cookie_itere = m_list_cookienv_map.end();

	while( cookie_iter != cookie_itere )
	{
		//delete cookie attributes first
		m_ptr_tempnamevalue = (vo_http_cookie_namevalue_map *)(*cookie_iter);
		vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator iter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
		vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator itere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

		while( iter != itere )
		{
			delete *iter;
			iter++;
		}

		m_ptr_tempnamevalue->m_list_attrenv_map.reset();

		delete *cookie_iter;
		cookie_iter++;
	}

	m_list_cookienv_map.reset();

	m_ptr_tempnamevalue = 0;

	while(m_pcookienode_head)
	{
		cookie_storage_node *pNode = m_pcookienode_head->pNext;
		delete m_pcookienode_head;
		m_pcookienode_head = pNode;
	}
	m_pcookienode_head = 0;
	m_pcookienode_tail = 0;
}

//Set-Cookie: SID=31d4d96e407aad42; Path=/; Domain=sohu.com; Secure; HttpOnly
VO_VOID vo_http_cookie::analyze_setcookie_string( VO_CHAR * str_setcookie )
{
	VO_CHAR * str_name_value_pair = NULL;
	VO_CHAR * str_av = NULL;
	VO_CHAR temp[4096];
	VO_BOOL bnamevalue_valid =  VO_FALSE;

	VO_CHAR * ptr_pos = strstr( str_setcookie , ";" );
	if( ptr_pos )
	{
		//both cookie-av and name-value pair is available
		//for example:
		//Set-Cookie: SID=31d4d96e407aad42; Path=/; Domain=sohu.com; Secure; HttpOnly
		if( ptr_pos - str_setcookie > 0 )
		{
			memset( temp , 0 , sizeof(temp) );
			if( ptr_pos - str_setcookie >= sizeof(temp) )
			{
				VOLOGE("avoid stack corruption, so exit");
				return;
			}
			memcpy( temp , str_setcookie , ptr_pos - str_setcookie );
			str_name_value_pair = temp;
			bnamevalue_valid = analyze_name_value_pair( str_name_value_pair );
		}
		else
		{
			return;
		}

		//try to found the end of line
		VO_CHAR * str_linend = ptr_pos;
		while( *str_linend && *str_linend != '\r' && *( str_linend+1 ) != '\n')
		{
			str_linend++;
		}

		if( str_linend - ptr_pos > 0 && bnamevalue_valid )
		{
			memset( temp , 0 , sizeof(temp) );
			if( str_linend - ptr_pos >= sizeof(temp) )
			{
				VOLOGE("avoid stack corruption, so exit");
				return;
			}
			memcpy( temp , ptr_pos , str_linend - ptr_pos );
			str_av = temp;
			analyze_unparsed_av( str_av );
		}
		else
		{
			return;
		}
	}
	else
	{
		//no cookie-av , only name value pair is available
		//for example:  
		//Set-Cookie: SID=31d4d96e407aad42
		//try to found the end of line
		VO_CHAR * str_linend = str_setcookie;
		while( *str_linend && *str_linend != '\r' && *( str_linend+1 ) != '\n')
		{
			str_linend++;
		}

		if( str_linend - str_setcookie > 0 )
		{
			memset( temp , 0 , sizeof(temp) );
			if( str_linend - str_setcookie >= sizeof(temp) )
			{
				VOLOGE("avoid stack corruption, so exit");
				return;
			}
			memcpy( temp , str_setcookie , str_linend - str_setcookie );
			str_name_value_pair = temp;
			analyze_name_value_pair( str_name_value_pair );
		}
		else
		{
			return;
		}
	}
}

VO_VOID vo_http_cookie::add_attribute_namevalue( VO_CHAR * str_name , VO_CHAR * str_value )
{
	if( NULL == m_ptr_tempnamevalue )
	{
		return;
	}

	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator iter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator itere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->str_attrib_name , str_name , strlen( str_name ) ) == 0)
			return;	

		iter++;
	}

	vo_http_cookie_attrib_map * ptr_namevalue = new vo_http_cookie_attrib_map;
	if( !ptr_namevalue )
	{
		VOLOGE("new obj failed");
		return;
	}
	strcpy( ptr_namevalue->str_attrib_name , str_name);
	strcpy( ptr_namevalue->str_attrib_value , str_value);

	if( m_ptr_tempnamevalue )
	{
		m_ptr_tempnamevalue->m_list_attrenv_map.push_back( ptr_namevalue );
	}
}

VO_VOID vo_http_cookie::add_cookie_namevalue( VO_CHAR * str_name , VO_CHAR * str_value)
{
	vo_singlelink_list< vo_http_cookie_namevalue_map * >::iterator iter = m_list_cookienv_map.begin();
	vo_singlelink_list< vo_http_cookie_namevalue_map * >::iterator itere = m_list_cookienv_map.end();

	while( iter != itere )
	{
		if(voiostrnicmp( (*iter)->str_cookie_name , str_name , strlen( str_name ) ) == 0)
			return;	

		iter++;
	}

	vo_http_cookie_namevalue_map * ptr_namevalue = new vo_http_cookie_namevalue_map;
	if( !ptr_namevalue )
	{
		VOLOGE("new obj failed");
		return;
	}
	strcpy( ptr_namevalue->str_cookie_name , str_name);
	strcpy( ptr_namevalue->str_cookie_value , str_value);

	//record the current cookie name value object, in order to access it's attributes list
	m_ptr_tempnamevalue = ptr_namevalue;
	m_ptr_tempnamevalue->m_list_attrenv_map.reset();

	m_list_cookienv_map.push_back( ptr_namevalue );
}

//Set-Cookie: SID=31d4d96e407aad42
VO_BOOL vo_http_cookie::analyze_name_value_pair( VO_CHAR * str_name_value_pair )
{
	VO_CHAR * ptr_name = NULL;
	VO_CHAR * ptr_value = NULL;
	VO_CHAR cookiename[256];
	VO_CHAR cookievalue[2048];
	memset( cookiename, 0 , sizeof( cookiename ));
	memset( cookievalue, 0 , sizeof( cookievalue ));

	VO_CHAR * ptr_pos = strstr( str_name_value_pair , "=" );
	if( !ptr_pos )
	{
		return VO_FALSE;
	}

	ptr_pos = str_name_value_pair + strlen( "Set-Cookie:" );

	//remove leading WSP characters of name
	while( *ptr_pos == ' ')
	{
		ptr_pos++;
	}

	ptr_name = ptr_pos;
	ptr_pos = strstr( str_name_value_pair , "=" );
	if( ptr_pos - ptr_name > 0 )
	{
		//remember remove trailing WSP character of name
		while( *(ptr_pos-1) == ' ')
		{
			ptr_pos--;
		}

		if( ptr_pos - ptr_name > 0 && ptr_pos - ptr_name < sizeof(cookiename) )
		{
			memcpy( cookiename , ptr_name , ptr_pos - ptr_name);
		}
		else
		{
			return VO_FALSE;
		}
	}
	else
	{
		return VO_FALSE;
	}

	ptr_pos = strstr( str_name_value_pair , "=" );
	ptr_value = ptr_pos + 1;
	while( *ptr_value == ' ')
	{
		//remove leading WSP characters value
		ptr_value++;
	}

	while( *ptr_pos && ( *ptr_pos != '\r' && *( ptr_pos+1 ) != '\n') )
	{
		ptr_pos++;
	}

	if( ptr_pos - ptr_value > 0 )
	{
		//!!!!!!!!!!!!!!!!!!!!!!!!!!waiting to be complete!!!!!!!!!!!!!!
		//remember remove trailing WSP character of value
		while( *(ptr_pos-1) == ' ')
		{
			ptr_pos--;
		}

		if( ptr_pos - ptr_value > 0 && ptr_pos - ptr_value < sizeof(cookievalue))
		{
			memcpy( cookievalue , ptr_value , ptr_pos - ptr_value);
		}
	}

	add_cookie_namevalue(cookiename ,cookievalue);

	return VO_TRUE;
}

//Domain=sohu.com
VO_BOOL vo_http_cookie::analyze_attribute(VO_CHAR* str_attrib)
{
	VO_CHAR * ptr_name = NULL;
	VO_CHAR * ptr_value = NULL;
	VO_CHAR attribname[256];
	VO_CHAR attribvalue[1024];
	memset( attribname, 0 , sizeof( attribname ));
	memset( attribvalue, 0 , sizeof( attribvalue ));

	VO_CHAR * ptr_pos = strstr( str_attrib , "=" );

	//only attribute name is available
	if( !ptr_pos )
	{
		ptr_pos = str_attrib;
		//remove leading WSP characters
		while( *ptr_pos == ' ')
		{
			ptr_pos++;
		}
		ptr_name = ptr_pos;

		//reach to line end or string end
		while( *ptr_pos  && ( *ptr_pos != '\r' && *( ptr_pos+1 ) != '\n'))
		{
			ptr_pos++;
		}

		if( ptr_pos - ptr_name > 0 )
		{
			//remember remove trailing WSP character of value
			while( *(ptr_pos-1) == ' ')
			{
				ptr_pos--;
			}

			if( ptr_pos - ptr_name > 0 && ptr_pos - ptr_name < sizeof(attribname) )
			{
				memcpy( attribname , ptr_name , ptr_pos - ptr_name);
			}
		}
	}
	else
	{
		ptr_name = str_attrib;
		//remove leading WSP characters of name
		while( *ptr_name == ' ')
		{
			ptr_name++;
		}

		if( ptr_pos - ptr_name > 0 )
		{
			//remember remove trailing WSP character of name
			while( *(ptr_pos-1) == ' ')
			{
				ptr_pos--;
			}

			if( ptr_pos - ptr_name > 0 && ptr_pos - ptr_name < sizeof(attribname) )
			{
				memcpy( attribname , ptr_name , ptr_pos - ptr_name);
			}
			else
			{
				return VO_FALSE;
			}
		}
		else
		{
			return VO_FALSE;
		}

		ptr_pos = strstr( str_attrib , "=" );
		ptr_value = ptr_pos + 1;
		while( *ptr_value == ' ')
		{
			//remove leading WSP characters value
			ptr_value++;
		}

		//reach to string end or line end
		while( *ptr_pos  && ( *ptr_pos != '\r' && *( ptr_pos+1 ) != '\n') )
		{
			ptr_pos++;
		}

		if( ptr_pos - ptr_value > 0 )
		{
			//remember remove trailing WSP character of value
			while( *(ptr_pos-1) == ' ')
			{
				ptr_pos--;
			}

			if( ptr_pos - ptr_value > 0 && ptr_pos - ptr_value < sizeof(attribvalue))
			{
				memcpy( attribvalue , ptr_value , ptr_pos - ptr_value);
			}
		}
	}
	

	add_attribute_namevalue(attribname ,attribvalue);

	return VO_TRUE;
}

//; Path=/; Domain=sohu.com; Secure; HttpOnly
VO_VOID vo_http_cookie::analyze_unparsed_av( VO_CHAR* str_av )
{
	VO_CHAR temp[2048];
	VO_CHAR * ptr_pos = str_av;

	//if the string is empty
	if( !ptr_pos )
	{
		return;
	}

	//discard the first character %x3b(";")
	if( *ptr_pos == ';')
	{
		ptr_pos++;
	}

	//Remove leading WSP characters
	while( *ptr_pos == ' ')
	{
		ptr_pos++;
	}

	//if it had reach to line end
	if( *ptr_pos == '\r' || *ptr_pos == '\n')
	{
		return;
	}

	//check if the remaining unparsed-attributes contain ";"
	VO_CHAR * ptr_sep = strstr( ptr_pos , ";" );
	if( !ptr_sep )
	{
		analyze_attribute( ptr_pos );
		return;
	}
	else
	{
		if( ptr_sep - ptr_pos > 0 )
		{
			memset( temp , 0 , sizeof(temp) );
			if( ptr_sep - ptr_pos >= sizeof(temp) )
			{
				VOLOGE("avoid stack corruption, so exit");
				return;
			}
			memcpy( temp , ptr_pos , ptr_sep - ptr_pos );
			VO_CHAR *str_attrib = temp;
			analyze_attribute( str_attrib );
		}
		analyze_unparsed_av( ptr_sep );
		return;
	}
}

//for cookie date string parse
VO_BOOL	vo_http_cookie::isdelimiter(VO_CHAR ch)
{
	if( 0x09 == ch || 
		( ch >= 0x20 && ch <= 0x2f) || 
		( ch >= 0x3b && ch <= 0x40) || 
		( ch >= 0x5b && ch <= 0x60) || 
		( ch >= 0x7b && ch <= 0x7e) )
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_BOOL vo_http_cookie::isnondelimiter(VO_CHAR ch)
{
	if(  ch == ':' ||
		(ch >= 0x00 && ch <= 0x08) ||
		(ch >= 0x0a && ch <= 0x1f) ||
		(ch >= 0x30 && ch <= 0x3a) ||
		(ch >= 0x41 && ch <= 0x5a) ||
		(ch >= 0x61 && ch <= 0x7a) ||
		(ch >= 0x7f)
		)
	{
		return VO_TRUE;
	}

	return VO_FALSE;
}

VO_S32 vo_http_cookie::analyze_cookie_dates( VO_CHAR* str_cookie_date )
{
#ifndef WINCE
	memset( &m_cookie_dateflag , 0 , sizeof( m_cookie_dateflag ) );	
	
	//initlize broken down time
	memset( &m_broken_time , 0 , sizeof(m_broken_time) );
	m_broken_time.tm_isdst = -1;

	VO_CHAR * ptr_temp = str_cookie_date;

	while( *ptr_temp )
	{
		while( isdelimiter( *ptr_temp ) )
		{
			ptr_temp++;
		}

		VO_CHAR * ptr_datetoken_begin = ptr_temp;
		while( *ptr_temp && isnondelimiter( *ptr_temp ) )
		{
			ptr_temp++;
		}

		VO_CHAR temp[20];
		memset( temp , 0 , sizeof(temp) );
		strncpy( temp , ptr_datetoken_begin , ptr_temp - ptr_datetoken_begin );

		if( !process_datetoken(temp) )
		{
			return -1;
		}

		if( m_cookie_dateflag.found_time &&
			m_cookie_dateflag.found_year &&
			m_cookie_dateflag.found_month &&
			m_cookie_dateflag.found_day_of_month)
		{
			return (VO_S32)mktime( &m_broken_time );
		}
	}
#endif
	return -1;
}

VO_BOOL vo_http_cookie::process_datetoken(VO_CHAR* datetoken)
{
#ifndef WINCE
	VO_CHAR temp[20];
	VO_S32 cnt = count_char(datetoken , ':');
	if( cnt )
	{
		//date token match the time production
		if( m_cookie_dateflag.found_time )
		{
			//time has already been set, so return;
			return VO_TRUE;
		}
		
		if( cnt != 2 )
		{
			return VO_FALSE;
		}
		else
		{
			VO_S32 hours = 0;
			VO_S32 minutes = 0;
			VO_S32 seconds = 0;
			VO_CHAR * pfirst = strchr( datetoken , ':' );
			VO_CHAR * psecond = strrchr( datetoken , ':' );

			strncpy( temp , datetoken , pfirst - datetoken );
			hours = atoi( temp );
			if( hours > 23 )
			{
				return VO_FALSE;
			}
			m_broken_time.tm_hour = hours;

			strncpy( temp , pfirst + 1 , psecond - ( pfirst + 1 ) );
			minutes = atoi( temp );
			if( minutes > 59 )
			{
				return VO_FALSE;
			}
			m_broken_time.tm_min = minutes;

			strcpy( temp , psecond + 1 );
			seconds = atoi( temp );
			if( seconds > 59 )
			{
				return VO_FALSE;
			}
			m_broken_time.tm_sec = seconds;

			m_cookie_dateflag.found_time = VO_TRUE;
			return VO_TRUE;
		}
	}
	else
	{
		if( vostr_is_digit(datetoken) )
		{
			//match day-of-month or year production
			if( m_cookie_dateflag.found_month )
			{
				//match year production
				VO_S32 year = atoi(datetoken);
				if( year >=70 && year <= 99 )
				{
					year += 1900; 
				}
				if( year >=0 && year <= 69 )
				{
					year += 2000; 
				}
				if( year < 1601)
				{
					return VO_FALSE;
				}
				m_broken_time.tm_year = year - 1900;
				m_cookie_dateflag.found_year = VO_TRUE;
				return VO_TRUE;
			}
			else
			{
				//match day-of-month production
				VO_S32 day_of_month = atoi(datetoken);
				if( day_of_month < 1 || day_of_month > 31 )
				{
					return VO_FALSE;
				}
				else
				{
					m_broken_time.tm_mday = day_of_month;
					m_cookie_dateflag.found_day_of_month = VO_TRUE;
					return VO_TRUE;
				}
			}
		}
		else
		{
			//maybe match month production,but it may also be day of week.
			if( !m_cookie_dateflag.found_month )
			{
				if( MapAlphaMonth2Num(datetoken) )
				{
					m_cookie_dateflag.found_month = VO_TRUE;
					return VO_TRUE;
				}
				else
				{
					return VO_TRUE;
				}
			}
			else
			{
				//month has already been found before
				return VO_TRUE;
			}
		}
	}
#endif
	return VO_FALSE;
}

VO_BOOL vo_http_cookie::MapAlphaMonth2Num(VO_CHAR* month)
{
#ifndef WINCE
	if( strncstr(month,(VO_CHAR*)"jan") )
	{
		m_broken_time.tm_mon = 0;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"feb") )
	{
		m_broken_time.tm_mon = 1;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"mar") )
	{
		m_broken_time.tm_mon = 2;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"apr") )
	{
		m_broken_time.tm_mon = 3;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"may") )
	{
		m_broken_time.tm_mon = 4;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"jun") )
	{
		m_broken_time.tm_mon = 5;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"jul") )
	{
		m_broken_time.tm_mon = 6;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"aug") )
	{
		m_broken_time.tm_mon = 7;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"sep") )
	{
		m_broken_time.tm_mon = 8;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"oct") )
	{
		m_broken_time.tm_mon = 9;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"nov") )
	{
		m_broken_time.tm_mon = 10;
		return VO_TRUE;
	}
	if( strncstr(month,(VO_CHAR*)"dec") )
	{
		m_broken_time.tm_mon = 11;
		return VO_TRUE;
	}
#endif
	return VO_FALSE;
}

VO_VOID vo_http_cookie::analyze_cookie_settings()
{
	VO_CHAR * ptr_cookietest = m_response;

	//Set-Cookie: SID=31d4d96e407aad42; Path=/; Domain=sohu.com; Secure; HttpOnly
	//Set-Cookie: lang=en-US; Path=/; Domain=sohu.com
	VO_CHAR * ptr_setcookie_str = strncstr( ptr_cookietest , (VO_CHAR*)"Set-Cookie: " );
	while( ptr_setcookie_str )
	{
		analyze_setcookie_string( ptr_setcookie_str );
		ptr_cookietest = ptr_setcookie_str + 1;
		ptr_setcookie_str = strncstr( ptr_cookietest , (VO_CHAR*)"Set-Cookie: " );
		m_ptr_tempnamevalue = NULL;
	}
}

VO_BOOL vo_http_cookie::IsDomainMatch(VO_CHAR * host , VO_CHAR* domain)
{
	//identical
	if( strlen(host) == strlen(domain) && 0 == voiostrnicmp( host , domain , strlen(domain)) )
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

	return VO_FALSE;
}

VO_BOOL vo_http_cookie::process_domain()
{
	if( NULL == m_ptr_tempnamevalue )
	{
		return VO_FALSE;
	}

	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator aviter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator avitere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

	VO_BOOL valueset = VO_FALSE;
	VO_CHAR domain[256];
	memset( domain , 0 , sizeof(domain));
	while( aviter != avitere )
	{
		if(voiostrnicmp( (*aviter)->str_attrib_name , "Domain" , strlen( "Domain" ) ) == 0)
		{
			if( strlen( (*aviter)->str_attrib_value ) > 0 )
			{
				//Domain attribute should Domainmatch the origin server
				if( strlen(m_host) > 0 && !IsDomainMatch(m_host , (*aviter)->str_attrib_value) )
				{
					VOLOGE("cookie is reject since the Domain attribute did not include the origin server");
					return VO_FALSE;
				}
				else
				{
					strcpy( domain , (*aviter)->str_attrib_value );
				}
				valueset = VO_TRUE;
				break;
			}
			else
			{
				VOLOGE("!!!!!cookie Domain value is empty !!!!!!");
			}
		}

		aviter++;
	}

	if( !valueset )
	{
		domain[0] = '\0';
	}

	//process public suffixes... waiting to be completed......

	//compare with request-host
	if( valueset )
	{
		m_http_cookie_node.host_only_flag = 0;
		if( domain[0] == '.' )
		{
			strcpy( m_http_cookie_node.domain , domain + 1 );
		}
		else
		{
			strcpy( m_http_cookie_node.domain , domain );
		}
	}
	else
	{
		m_http_cookie_node.host_only_flag = 1;
		if( strlen(m_host) >0 )
		{
			strcpy( m_http_cookie_node.domain , m_host );
		}
		else
		{
			VOLOGE("domain field miss and can't compute default domain");
			return VO_FALSE;
		}
	}

	return VO_TRUE;
}

VO_VOID vo_http_cookie::process_path()
{
	if( NULL == m_ptr_tempnamevalue )
	{
		return;
	}

	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator aviter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator avitere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

	while( aviter != avitere )
	{
		if(voiostrnicmp( (*aviter)->str_attrib_name , "Path"  , strlen( "Path" ) ) == 0)
		{
			if( strlen( (*aviter)->str_attrib_value ) <=0 || (*aviter)->str_attrib_value[0] != '/' )
			{
				strcpy( m_http_cookie_node.path , "/" );
			}
			else
			{
				strcpy( m_http_cookie_node.path , (*aviter)->str_attrib_value );
			}
			return;
		}

		aviter++;
	}

	//the Path attribute is Omitted by server, so we need to compute the default-path of cookie from request-uri.
	VO_CHAR path[1024];
	memset( path , 0 , sizeof(path) );
	ComputeDefaultpath( m_path , path );
	strcpy( m_http_cookie_node.path , path );
}


VO_VOID vo_http_cookie::ComputeDefaultpath(VO_CHAR *path , VO_CHAR *destpath)
{
	if( strlen( path ) <= 0 ||  path[0] != '/' || 1 == count_char( path , '/') )
	{
		strcpy( destpath , "/" );
		return;
	}

	if( count_char( path , '/') > 1 )
	{
		VO_CHAR * plastslash = strrchr( path , '/' );
		if( plastslash )
		{
			strncpy( destpath , path , plastslash - path );
			return;
		}
	}
}

VO_VOID vo_http_cookie::process_secure()
{
	if( NULL == m_ptr_tempnamevalue )
	{
		return;
	}

	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator aviter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator avitere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

	m_http_cookie_node.secure_only_flag = 0;
	while( aviter != avitere )
	{
		if(voiostrnicmp( (*aviter)->str_attrib_name , "Secure"  , strlen( "Secure" ) ) == 0)
		{
			m_http_cookie_node.secure_only_flag = 1;
			return;
		}

		aviter++;
	}
}

VO_VOID vo_http_cookie::process_httponly()
{
	if( NULL == m_ptr_tempnamevalue )
	{
		return;
	}

	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator aviter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator avitere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

	m_http_cookie_node.http_only_flag = 0;
	while( aviter != avitere )
	{
		if(voiostrnicmp( (*aviter)->str_attrib_name , "HttpOnly"  , strlen( "HttpOnly" ) ) == 0)
		{
			m_http_cookie_node.http_only_flag = 1;
			return;
		}

		aviter++;
	}
}

VO_VOID vo_http_cookie::process_persistentflag_expiretime()
{
	if( NULL == m_ptr_tempnamevalue )
	{
		return;
	}

	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator aviter = m_ptr_tempnamevalue->m_list_attrenv_map.begin();
	vo_singlelink_list< vo_http_cookie_attrib_map * >::iterator avitere = m_ptr_tempnamevalue->m_list_attrenv_map.end();

	VO_BOOL valueset = VO_FALSE;
	while( aviter != avitere )
	{
		if(voiostrnicmp( (*aviter)->str_attrib_name , "Max-Age"  , strlen( "Max-Age" ) ) == 0)
		{
			m_http_cookie_node.persistent_flag = 1;
			if( vostr_is_digit( (*aviter)->str_attrib_value ) )
			{
				m_http_cookie_node.expiry_time = current_date_time() + atoi((*aviter)->str_attrib_value);
				valueset = VO_TRUE;
				break;
			}
		}

		if(voiostrnicmp( (*aviter)->str_attrib_name , "Expires"  , strlen( "Expires" ) ) == 0)
		{
			m_http_cookie_node.persistent_flag = 1;
			if(  vostr_is_digit( (*aviter)->str_attrib_value ) )
			{
				if( vostr_is_digit((*aviter)->str_attrib_value) )
				{
					//if it is digit, it maybe is calendar time, it did not follow spec, but we need to deal with it.
					m_http_cookie_node.expiry_time = atoi((*aviter)->str_attrib_value);
				}
				else
				{
					VO_S32 calendar_utc = analyze_cookie_dates((*aviter)->str_attrib_value);
					m_http_cookie_node.expiry_time = calendar_utc;
				}
				
				valueset = VO_TRUE;
			}
		}

		aviter++;
	}

	if( !valueset )
	{
		m_http_cookie_node.persistent_flag = 0;
		//if both expire and MaxAge not exist, then set expire time one week later.
		m_http_cookie_node.expiry_time = current_date_time() + ONEWEEKSECONDS;
	}
}

VO_VOID vo_http_cookie::init_cookie_node()
{
	memset( m_http_cookie_node.name , 0 , sizeof( m_http_cookie_node.name ) );
	memset( m_http_cookie_node.value , 0 , sizeof( m_http_cookie_node.value ) );
	memset( m_http_cookie_node.domain , 0 , sizeof( m_http_cookie_node.domain ) );
	memset( m_http_cookie_node.path , 0 , sizeof( m_http_cookie_node.path ) );

	m_http_cookie_node.expiry_time = 0;
	m_http_cookie_node.creation_time = 0;
	m_http_cookie_node.last_access_time = 0;
	m_http_cookie_node.host_only_flag = 0;
	m_http_cookie_node.http_only_flag = 0;
	m_http_cookie_node.invalid = VO_FALSE;
	m_http_cookie_node.persistent_flag = 0;
	m_http_cookie_node.secure_only_flag = 0;
	m_http_cookie_node.pNext = 0;
}

VO_BOOL vo_http_cookie::process_cookie2storage()
{
	vo_singlelink_list< vo_http_cookie_namevalue_map * >::iterator iter = m_list_cookienv_map.begin();
	vo_singlelink_list< vo_http_cookie_namevalue_map * >::iterator itere = m_list_cookienv_map.end();

	while( iter != itere )
	{
		init_cookie_node();

		strcpy( m_http_cookie_node.name , (*iter)->str_cookie_name );
		strcpy( m_http_cookie_node.value , (*iter)->str_cookie_value );
		m_http_cookie_node.creation_time = current_date_time();
		m_http_cookie_node.last_access_time = current_date_time();
		m_ptr_tempnamevalue = (vo_http_cookie_namevalue_map *)(*iter);

		VO_BOOL ret = VO_FALSE;

		m_http_cookie_node.invalid = VO_FALSE;

		process_persistentflag_expiretime();

		ret = process_domain();
		if( !ret )
		{
			iter++;
			continue;
		}

		process_path();
		process_secure();
		process_httponly();

		m_is_cookie_update = VO_TRUE;

		add_storage_cookienode();

		iter++;
	}

	if( m_is_cookie_update )
	{
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_VOID vo_http_cookie::add_storage_cookienode()
{
	cookie_storage_node * pcookie_node = new cookie_storage_node;

	strcpy( pcookie_node->name, m_http_cookie_node.name);
	strcpy( pcookie_node->value, m_http_cookie_node.value );
	strcpy( pcookie_node->domain, m_http_cookie_node.domain );
	strcpy( pcookie_node->path, m_http_cookie_node.path );

	pcookie_node->expiry_time = m_http_cookie_node.expiry_time;
	pcookie_node->creation_time = m_http_cookie_node.creation_time;
	pcookie_node->last_access_time = m_http_cookie_node.last_access_time;
	pcookie_node->host_only_flag = m_http_cookie_node.host_only_flag;
	pcookie_node->http_only_flag = m_http_cookie_node.http_only_flag;
	pcookie_node->invalid = m_http_cookie_node.invalid;
	pcookie_node->persistent_flag = m_http_cookie_node.persistent_flag;
	pcookie_node->secure_only_flag = m_http_cookie_node.secure_only_flag;
	pcookie_node->pNext = NULL;

	if( m_pcookienode_head )
	{
		m_pcookienode_tail->pNext = pcookie_node;
		m_pcookienode_tail = pcookie_node;
	}
	else
	{
		m_pcookienode_head = pcookie_node;
		m_pcookienode_tail = pcookie_node;
	}
}

VO_BOOL vo_http_cookie::Is_cookie_update()
{
	return m_is_cookie_update;
}

VO_VOID vo_http_cookie::set_urlinfo( VO_CHAR* host , VO_CHAR * path , VO_CHAR * port )
{
	strcpy( m_host , host );
	strcpy( m_path , path );
	strcpy( m_port , port );
	return;
}
