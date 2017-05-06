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
//function name for vossl
#ifndef VO_SSL_H
#define VO_SSL_H

#include "voYYDef_Type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define VO_API

typedef struct
{
	int (VO_API * SSL_library_init) ();
	void (VO_API * SSL_load_error_strings) ();
	void * (VO_API * SSLv23_client_method) ();
	void * (VO_API * SSLv3_client_method) ();
	void * (VO_API * TLSv1_client_method) ();
	void * (VO_API * SSL_CTX_new) ( void * meth );
	void * (VO_API * SSL_new) ( void * ctx );
	int (VO_API * SSL_shutdown) ( void * s );
	void (VO_API * SSL_free) ( void * s );
	void (VO_API * SSL_CTX_free) ( void * ctx );
	int (VO_API * SSL_set_fd) ( void * s , int fd );
	int (VO_API * RAND_poll) ();
	int (VO_API * RAND_status) ();
	void (VO_API * RAND_seed) (const void* buf, int num );
	int (VO_API * SSL_connect) ( void * s );
	int (VO_API * SSL_get_error) ( const void * s , int ret_code );
	int (VO_API * SSL_write) ( void * s , const void* buf ,int num );
	int (VO_API * SSL_read) ( void * s , void* buf ,int num );
}vosslapi;


typedef int (VO_API * __voGetSSLAPI) (void * sslapi);

int voGetSSLAPI(void * sslapi);

#ifdef __cplusplus
}
#endif

#endif
