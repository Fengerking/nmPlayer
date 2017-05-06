
#ifndef __VO_AES_ENGINE_H__

#define __VO_AES_ENGINE_H__
#include "DRM_API.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

void * aes_init(DataSourceType dataSource);

int aes_setkey( void * handle, ProtectionType pType, char* URL, unsigned char* IV );

int aes_decrypt( DRMHANDLE handle ,unsigned char* inputBuf, int inputLen, unsigned char* outBuffer, int* outputLen ,bool islastblock ) ;

int aes_release( void * handle );


#ifdef _VONAMESPACE
}
#endif

#endif