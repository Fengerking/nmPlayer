#pragma once
#include "voDRM2.h"
#include "voAdaptiveStreamParser.h"

#if defined __cplusplus
extern "C" {
#endif

#define	VO_PID_AS_DRM2_BASE				0x432A0000						/*!< the base param ID for adaptive streaming DRM2 modules */
#define VO_PID_AS_DRM2_NEW_API			(VO_PID_AS_DRM2_BASE | 0x0001)	/*!<g> get a new DRM wrapper's callback API for multi-thread*/
#define VO_PID_AS_DRM2_CONVERT_URL		(VO_PID_AS_DRM2_BASE | 0x0002)	/*!<g> transform the URL for customer*/
#define VO_PID_AS_DRM2_STREAMING_TYPE	(VO_PID_AS_DRM2_BASE | 0x0003)	/*!<s> set the type of adaptive streaming, refer to VO_ADAPTIVESTREAMPARSER_STREAMTYPE*/

typedef struct
{
	VO_HANDLE hHandle;

	VO_U32 (VO_API * Init)(VO_PTR* ppHandle, VO_SOURCEDRM_CALLBACK2* pDrmCB, VO_SOURCE2_IO_API* pIO, VO_PTR pReserved);
	VO_U32 (VO_API * Uninit)(VO_PTR pHandle);

	VO_U32 (VO_API * PreprocessURL)(VO_PTR pHandle, const VO_CHAR* urlSrc, VO_CHAR* urlDes, VO_PTR pReserved);

	VO_U32 (VO_API * Info)(VO_PTR pHandle, VO_CHAR* szManifestURL, VO_BYTE* pManifest, VO_U32 uSizeManifest, VO_PTR pReserved);
 
	VO_U32 (VO_API * DataBegin)(VO_PTR pHandle, VO_U32 uIdentifer, VO_PTR pInfo);
	VO_U32 (VO_API * DataProcess_Chunk)(VO_PTR pHandle, VO_U32 uIdentifer, VO_U64 nOffset, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_BOOL bChunkEnd, VO_PTR pAdditionalInfo);
	VO_U32 (VO_API * DataEnd)(VO_PTR pHandle, VO_U32 uIdentifer, VO_PTR pInfo);

	VO_U32 (VO_API * Info_FR)(VO_PTR pHandle, VO_PTR pInfo, VO_DRM2_INFO_TYPE eInfoType, VO_PTR pReserved);
	VO_U32 (VO_API * DataProcess_FR)(VO_PTR pHandle, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eInfoType, VO_PTR pAdditionalInfo);


	VO_U32 (VO_API * SetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
	VO_U32 (VO_API * GetParameter)(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	VO_U32 (VO_API * PreprocessPlaylist)(VO_PTR pHandle, VO_BYTE* pPlaylist, VO_U32 uSizeBuffer, VO_U32* puSizePlaylist, VO_PTR pReserved);
} VO_StreamingDRM_API;

VO_S32 VO_API voGetStreamingDRMAPI(VO_StreamingDRM_API * pDRMHandle, VO_U32 uFlag);

#if defined __cplusplus
}
#endif
