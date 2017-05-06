#include "voDLNA_Param.h"

#define VODLNACONNECTION_TIMEOUT_DEFAULTVALUE 35000
#define VODLNAREAD_TIMEOUT_DEFAULTVALUE 35000

voDLNA_Param::voDLNA_Param(void):
m_is_dlna(VO_FALSE)
,m_is_connection_stalling_on_play(VO_FALSE)
,m_is_connection_stalling_on_pause(VO_TRUE)
,m_is_byterange_supported(VO_TRUE)
,m_connection_timeout(VODLNACONNECTION_TIMEOUT_DEFAULTVALUE)
,m_read_timeout(VODLNAREAD_TIMEOUT_DEFAULTVALUE)
{
	memset( m_request_append , 0 , sizeof( m_request_append ) );
	memset( m_server_response , 0 , sizeof( m_server_response ) );
}

voDLNA_Param::~voDLNA_Param(void)
{
}

VO_VOID voDLNA_Param::reset()
{
	m_is_dlna = VO_FALSE;
	m_is_connection_stalling_on_pause = VO_TRUE;
	m_is_connection_stalling_on_play = VO_FALSE;
	m_is_byterange_supported = VO_TRUE;
	m_connection_timeout = VODLNACONNECTION_TIMEOUT_DEFAULTVALUE;
	m_read_timeout = VODLNAREAD_TIMEOUT_DEFAULTVALUE;
	memset( m_request_append , 0 , sizeof( m_request_append ) );
	memset( m_server_response , 0 , sizeof( m_server_response ) );
}

VO_VOID voDLNA_Param::append_reqitem(VO_CHAR* pitem)
{
	if( strlen(pitem) && strchr( pitem , ':') )
	{
		strcat( m_request_append ,(VO_CHAR *) pitem );
		strcat( m_request_append , "\r\n" );
	}
}

VO_CHAR* voDLNA_Param::get_reqstr()
{
	return m_request_append;
}

VO_VOID voDLNA_Param::set_server_response(VO_CHAR* presponse)
{
	if( strlen( presponse ) )
	{
		strcpy( m_server_response , presponse );
	}
}

VO_CHAR* voDLNA_Param::get_server_response()
{
	return m_server_response;
}

VO_BOOL voDLNA_Param::is_range_limit()
{
	return VO_FALSE;
}

VO_VOID voDLNA_Param::set_dlna( VO_BOOL dlna )
{
	m_is_dlna = dlna;
}

VO_BOOL voDLNA_Param::is_dlna()
{
	return m_is_dlna;
}

VO_BOOL voDLNA_Param::is_byterange_support()
{
	return m_is_byterange_supported;
}

VO_VOID voDLNA_Param::set_byterange_support( VO_U32 support )
{
	if( support )
	{
		m_is_byterange_supported = VO_TRUE;
	}
	else
	{
		m_is_byterange_supported = VO_FALSE;
	}
	set_dlna();
}

VO_VOID voDLNA_Param::set_connection_stalling_on_play( VO_U32 stalling )
{
	if( stalling )
	{
		m_is_connection_stalling_on_play = VO_TRUE;
	}
	else
	{
		m_is_connection_stalling_on_play = VO_FALSE;
	}
	set_dlna();
}

VO_VOID voDLNA_Param::set_connection_stalling_on_pause( VO_U32 stalling )
{
	if( stalling )
	{
		m_is_connection_stalling_on_pause = VO_TRUE;
	}
	else
	{
		m_is_connection_stalling_on_pause = VO_FALSE;
	}
	set_dlna();
}

VO_BOOL voDLNA_Param::is_connection_stalling_on_pause()
{
	return m_is_connection_stalling_on_pause;
}

VO_BOOL voDLNA_Param::is_connection_stalling_on_play()
{
	return m_is_connection_stalling_on_play;
}

VO_VOID voDLNA_Param::set_connection_timeout( VO_U32 timeout )
{
	m_connection_timeout = timeout;
}

VO_VOID voDLNA_Param::set_read_timeout(VO_U32 timeout)
{
	m_read_timeout = timeout;
}

VO_U32  voDLNA_Param::get_connection_timeout()
{
	return m_connection_timeout;
}

VO_U32  voDLNA_Param::get_read_timeout()
{
	return m_read_timeout;
}