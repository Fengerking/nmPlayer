/////////////////////////////////////////////////////
//
//	DRM_API.h ( DRM API's)
//
//////////////////////////////////////////////////////
#ifndef DRM_API_h
#define DRM_API_h 1

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


/// Define Error codes
typedef enum
{
	SUCCESS=1,
	KEY_ACQUIRE_FAIL,
	SOURCE_TYPE_NOT_SUPPORTED,
	PROT_TYPE_NOT_SUPPORTED,  
	DECRYPTION_FAIL,
	PROXIMITY_ERROR,
	LICENSE_EXPIRED,
	INSUFFICIENT_RIGHTS,
	RIGHTS_ACQUIRE_FAIL,
	INTERNAL_DRM_ERROR,
	IPRM_CLOCK_SKEW_ERROR,
	FATAL_ERROR,
}DRM_ERRORS;

typedef enum  {MOTOHLS= 1} DataSourceType;

typedef enum  {NONE=1,AES128,MEDIOS,IPRM,THIRD_PART_DRM} ProtectionType;

typedef void* DRMHANDLE ;


typedef DRMHANDLE(*DRM_Init)(DataSourceType dataSource);
typedef int(*DRM_SetKey)(DRMHANDLE handle, ProtectionType pType, char* URL, unsigned char* IV );
typedef int(*DRM_Decrypt)(DRMHANDLE handle ,unsigned char* inputBuf, int inputLen, unsigned char* outBuffer, int* outputLen ,bool islastblock ) ;
typedef int(*DRM_Release)(DRMHANDLE handle );


struct DRM_Callback
{
	DRM_Init	drm_init;
	DRM_SetKey	drm_setkey;
	DRM_Decrypt drm_decrypt;
	DRM_Release drm_release;
};

#ifdef _VONAMESPACE
}
#endif


#endif
