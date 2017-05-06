
#include "vo_aes_engine.h"
#include "vo_aes_decryption.h"
//#include "vo_https_stream.h"
#include "vo_http_stream.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _IOS
using namespace _DASH;
#endif

VO_S32 read_buffer( vo_http_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size );

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
		unsigned char key[16];
		/*if( strstr( URL , "https://" ) == URL )
		{
			vo_https_stream stream;
			if( !stream.open( URL , DOWNLOAD2MEM ) )
				return KEY_ACQUIRE_FAIL;

			read_buffer( &stream , (VO_PBYTE)key , 16 );
		}
		else*/ if( strstr( URL , "http://" ) == URL )
		{
			vo_http_stream stream;
			if( !stream.open( URL , DOWNLOAD2MEM ) )
				return KEY_ACQUIRE_FAIL;

			read_buffer( &stream , (VO_PBYTE)key , 16 );
		}

		VOLOGI( "key: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X" , key[0] , key[1] , key[2] , key[3] , key[4] , key[5] ,
			key[6] , key[7] , key[8] , key[9] , key[10] , key[11] , key[12] , key[13] , key[14] , key[15] );

		fz_aesinit( ptr_aes , (unsigned char *)key, 16);
	}

	fz_setiv( ptr_aes , (unsigned char *)IV );

	return SUCCESS;
}

int aes_decrypt( void * handle, unsigned char* inputBuf, int inputLen, unsigned char* outBuffer, int* outputLen )
{
	fz_aes * ptr_aes = ( fz_aes * )handle;

	if( !ptr_aes )
		return FATAL_ERROR;

	if( inputLen % 16 )
		return DECRYPT_INPUT_LEN_ERROR;

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

VO_S32 read_buffer( vo_http_stream * ptr_stream , VO_PBYTE buffer , VO_S32 size )
{
	VO_S64 readed = 0;

	while( readed < size )
	{
		VO_S64 readsize = ptr_stream->read( buffer , size - readed );

		if( readsize == -1 )
		{
			return readed;
		}
		else if( readsize == -2 )
		{
			voOS_Sleep( 20 );
			continue;
		}

		readed += readsize;
		buffer = buffer + readsize;
	}

	return readed;
}
