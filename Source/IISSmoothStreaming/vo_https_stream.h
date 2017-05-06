
#ifndef __VO_HTTPS_STREAM_H__ 

#define __VO_HTTPS_STREAM_H__

#include "vo_http_stream.h"
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

class vo_https_stream
	:public vo_http_stream
{
public:
	vo_https_stream();
	~vo_https_stream();

protected:
	virtual VO_BOOL init( VO_CHAR * url );
	virtual void uninit();

	VO_BOOL resolve_url( VO_CHAR * url );

	virtual void download_normal();
	virtual void download_chunked();

	virtual VO_BOOL get_response();

	VO_BOOL check_cert(SSL *ssl, VO_CHAR *host);

protected:
	SSL_CTX * m_ptr_ctx;
	SSL * m_ptr_ssl;
};


#endif