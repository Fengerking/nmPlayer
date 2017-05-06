
#ifndef __VO_AES_ENGINE_H__

#define __VO_AES_ENGINE_H__
#include "DRM_API.h"

void * aes_init(DataSourceType dataSource);

int aes_setkey( void * handle, ProtectionType pType, char* URL, unsigned char* IV );

int aes_decrypt( void * handle, unsigned char* inputBuf, int inputLen, unsigned char* outBuffer, int* outputLen ) ;

int aes_release( void * handle );

#endif