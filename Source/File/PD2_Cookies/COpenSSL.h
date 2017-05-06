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

#ifndef __COpenSSL_VOWRAPPER_H__
#define __COpenSSL_VOWRAPPER_H__

#include "CDllLoad.h"
#include "voSSL.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

/**************************************************************************
						openssl wrapper 
load visualon openssl in dynamic way, and get the related function pointer.
the retrieved openssl function pointer will be filled in structure vosslapi.
you can add new function pointer as necessary.
**************************************************************************/
class COpenSSL
{
public:
	COpenSSL ();
	virtual ~COpenSSL (void);

	VO_VOID SetWorkpath(VO_TCHAR * pWorkPath) { m_dlload.SetWorkPath( pWorkPath ); }
	VO_BOOL LoadSSL();
	VO_VOID	FreeSSL();

public:
	vosslapi m_sslapi;

private:
	CDllLoad	m_dlload; 
	VO_BOOL		m_bssloaded;
	voCMutex	m_ssl_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif // __COpenSSL_VOWRAPPER_H__
