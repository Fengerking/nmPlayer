#if defined LINUX || defined _IOS_OPENSSL
#include "vo_https_stream.h"
#endif

#include "vo_aes_engine.h"
#include "vo_aes_decryption.h"

#include "vo_webdownload_stream.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _IOS
using namespace _HLS;
#endif

VO_S32 read_buffer( vo_webdownload_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size );

void * aes_init(DataSourceType dataSource)
{
	if( dataSource != MOTOHLS )
		return 0;

	return new fz_aes;
}

int aes_setkey( void * handle, ProtectionType pType, char* URL, unsigned char* IV )
{
	fz_aes * ptr_aes = ( fz_aes * )handle;

	if( !ptr_aes )
		return FATAL_ERROR;

	if( URL )
	{
		vo_webdownload_stream stream;
		if( !stream.open( URL , DOWNLOAD2MEM ) )
			return KEY_ACQUIRE_FAIL;

		VO_BYTE key_got[16];
		read_buffer( &stream , key_got , 16 );

		VOLOGI( "key: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X" , key_got[0] , key_got[1] , key_got[2] , key_got[3] , key_got[4] , key_got[5] ,
			key_got[6] , key_got[7] , key_got[8] , key_got[9] , key_got[10] , key_got[11] , key_got[12] , key_got[13] , key_got[14] , key_got[15] );

		fz_aesinit( ptr_aes , (unsigned char *)key_got, 16);
	}

	fz_setiv( ptr_aes , (unsigned char *)IV );

	return SUCCESS;
}

int aes_decrypt( void * handle, unsigned char* inputBuf, int inputLen, unsigned char* outBuffer, int* outputLen ,bool islastblock )
{
	fz_aes * ptr_aes = ( fz_aes * )handle;

	if( !ptr_aes )
		return FATAL_ERROR;

	if( inputLen % 16 )
		return DECRYPTION_FAIL;

	if( inputBuf != outBuffer )
		return DECRYPTION_FAIL;

	fz_aesdecrypt( ptr_aes , outBuffer , inputBuf , inputLen );

	*outputLen = inputLen;

	return SUCCESS;
}

int aes_release( void * handle )
{
	fz_aes * ptr_aes = ( fz_aes * )handle;

	if( !ptr_aes )
		return FATAL_ERROR;

	delete ptr_aes;

	return SUCCESS;
}

VO_S32 read_buffer( vo_webdownload_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size )
{
	VO_S64 readed = 0;

	while( readed < size )
	{
		VO_S64 readsize = ptr_stream->read( buffer , size - readed );

		if( readsize == -1 )
		{
			return (VO_S32) readed;
		}
		else if( readsize == -2 )
		{
			voOS_Sleep( 20 );
			continue;
		}

		readed += readsize;
		buffer = buffer + readsize;
	}

	return (VO_S32)readed;
}
