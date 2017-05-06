/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

 VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

 All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

#include "COpenSSL.h"
#define LOG_TAG "COpenSSL"
#include "voLog.h"

#ifndef _LINUX
#pragma warning (disable : 4996)
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vosslapi * COpenSSL::m_pSSLAPI = NULL;

COpenSSL::COpenSSL()
: m_bssloaded(VO_FALSE)
{
	memset( &m_sslapi , 0 , sizeof(m_sslapi) );
}

COpenSSL::~COpenSSL()
{
}

/**************************************************************************
	load openssl so module and get function pointer according to the name.
	so please CALL Init before call any openssl function.
**************************************************************************/
VO_BOOL COpenSSL::LoadSSL()
{
	voCAutoLock lock(&m_ssl_lock);
	//if it is already loaded then just return.
	if( m_bssloaded )
		return VO_TRUE;
	if (m_pSSLAPI != NULL)
	{
		memcpy (&m_sslapi, m_pSSLAPI, sizeof (vosslapi));
		m_bssloaded = VO_TRUE;
		return VO_TRUE;
	}

	VOLOGI( "Use vo_openssl" );
	vostrcpy(m_dlload.m_szDllFile, _T("yySSL"));
	vostrcpy(m_dlload.m_szAPIName, _T("yyGetSSLAPI"));
	
#if defined _WIN32
	vostrcat(m_dlload.m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_dlload.m_szDllFile, _T(".so"));
#elif defined _IOS
	vostrcat(m_dlload.m_szDllFile, _T(".a"));
#endif

	if( !m_dlload.LoadLib( NULL ) )
	{
		VOLOGE( "vo_openssl load failed" );
		return VO_FALSE;
	}
	
	__voGetSSLAPI getapi = (__voGetSSLAPI)m_dlload.m_pAPIEntry;

	if( getapi )
	{
		int ret = 0;
		ret = getapi( &m_sslapi );
		VOLOGI( "vo_openssl getapi , ret: %d " , ret );
		m_bssloaded = VO_TRUE;
		if( !m_sslapi.SSL_library_init )
		{
			VOLOGE( "ssl api is empty." );
			return VO_FALSE;
		}
		m_sslapi.SSL_library_init();
		m_sslapi.SSL_load_error_strings();
	}
	else
	{
		return VO_FALSE;
	}

	return VO_TRUE;
}

VO_VOID	COpenSSL::FreeSSL()
{
	voCAutoLock lock(&m_ssl_lock);
	if( m_bssloaded )
	{
		VOLOGI( "Free vo_openssl" );
		m_bssloaded = VO_FALSE;
		m_dlload.FreeLib();
		memset( &m_sslapi , 0 , sizeof(m_sslapi) );
	}
	return;
}