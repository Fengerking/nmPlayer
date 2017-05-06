/////////////////////////////////////////////////////
//
//	DRM_API.h ( DRM API's)
//
//////////////////////////////////////////////////////
#ifndef DRM_API_h
#define DRM_API_h 1


/// Define Error codes
typedef enum
{
	SUCCESS=1, 
	KEY_ACQUIRE_FAIL,
	SOURCE_TYPE_NOT_SUPPORTED,
	PROT_TYPE_NOT_SUPPORTED,
	DECRYPT_INPUT_LEN_ERROR,
	DECRYPTION_FAIL,
	FATAL_ERROR,
}DRM_ERRORS;

typedef enum  {MOTOHLS= 1} DataSourceType;

typedef enum  {NONE=1,AES128,MEDIOS,IPRM} ProtectionType;

typedef void* DRMHANDLE ;


typedef DRMHANDLE(*DRM_Init)(DataSourceType dataSource);
typedef int(*DRM_SetKey)(DRMHANDLE handle, ProtectionType pType, char* URL, unsigned char* IV);
typedef int(*DRM_Decrypt)(DRMHANDLE handle ,unsigned char* inputBuf, int inputLen, unsigned char* outBuffer, int* outputLen ) ;
typedef int(*DRM_Release)(DRMHANDLE handle );


struct DRM_Callback
{
	DRM_Init	drm_init;
	DRM_SetKey	drm_setkey;
	DRM_Decrypt drm_decrypt;
	DRM_Release drm_release;
};

#endif
