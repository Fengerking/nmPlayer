#include "voStreamingDRM.h"
#include "AdaptiveStreamingDRM.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_U32 VO_API voStreamingDRMInit(VO_PTR* ppHandle, VO_SOURCEDRM_CALLBACK2* pDrmCB, VO_SOURCE2_IO_API* pIO, VO_PTR pReserved)
	{
		CAdaptiveStreamingDRM *pDRM = NULL;
		
		//if (pDrmCB->szDRMTYPE)
			pDRM = new CAdaptiveStreamingDRM(pDrmCB, pIO, pReserved);

		if (NULL == pDRM)
			return VO_ERR_OUTOF_MEMORY;

		*ppHandle = pDRM;

		return VO_ERR_NONE;
	}

	VO_U32 VO_API voStreamingDRMUninit(VO_PTR pHandle)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM *pDRM = (CAdaptiveStreamingDRM*)pHandle;
		delete pDRM;

		return VO_ERR_NONE;
	}

	VO_U32 VO_API voStreamingDRMPreprocessURL(VO_PTR pHandle, const VO_CHAR* urlSrc, VO_CHAR* urlDes, VO_PTR pReserved)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->PreprocessURL(urlSrc, urlDes, pReserved);
	}

	VO_U32 VO_API voStreamingDRMInfo(VO_PTR pHandle, VO_CHAR* szManifestURL, VO_BYTE* pManifest, VO_U32 uSizeManifest, VO_PTR pReserved)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->Info(szManifestURL, pManifest, uSizeManifest, pReserved);
	}

	VO_U32 VO_API voStreamingDRMDataBegin(VO_PTR pHandle, VO_U32 uIdentifer, VO_PTR pInfo)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->DataBegin(uIdentifer, pInfo);
	}

	VO_U32 VO_API voStreamingDRMDataProcess_Chunk(VO_PTR pHandle, VO_U32 uIdentifer, VO_U64 nOffset, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_BOOL bChunkEnd, VO_PTR pAdditionalInfo)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->DataProcess_Chunk(uIdentifer, nOffset, pSrcData, nSrcSize, ppDesData, pnDesSize, bChunkEnd, pAdditionalInfo);
	}

	VO_U32 VO_API voStreamingDRMDataEnd(VO_PTR pHandle, VO_U32 uIdentifer, VO_PTR pInfo)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->DataEnd(uIdentifer, pInfo);
	}

	VO_U32 VO_API voStreamingDRMInfo_FR(VO_PTR pHandle, VO_PTR pInfo, VO_DRM2_INFO_TYPE eInfoType, VO_PTR pReserved)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->Info_FR(pInfo, eInfoType, pReserved);
	}

	VO_U32 VO_API voStreamingDRMDataProcess_FR(VO_PTR pHandle, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eInfoType, VO_PTR pAdditionalInfo)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->DataProcess_FR(pSrcData, nSrcSize, ppDesData, pnDesSize, eDataType, eInfoType, pAdditionalInfo);
	}

	VO_U32 VO_API voStreamingDRMSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voStreamingDRMGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->GetParameter(uID, pParam);
	}

	VO_U32 VO_API voStreamingDRMPreprocessPlaylist(VO_PTR pHandle, VO_BYTE* pPlaylist, VO_U32 uSizeBuffer, VO_U32* puSizePlaylist, VO_PTR pReserved)
	{
		if (NULL == pHandle)
			return VO_RET_SOURCE2_EMPTYPOINTOR;

		CAdaptiveStreamingDRM * pDRM = (CAdaptiveStreamingDRM*)pHandle;
		return pDRM->PreprocessPlaylist(pPlaylist, uSizeBuffer, puSizePlaylist, pReserved);
	}

#if defined __cplusplus
}
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_S32 VO_API voGetStreamingDRMAPI (VO_StreamingDRM_API * pDRMHandle, VO_U32 uFlag)
	{
		if(pDRMHandle)
		{
			pDRMHandle->Init				= voStreamingDRMInit;
			pDRMHandle->Uninit				= voStreamingDRMUninit;
			pDRMHandle->PreprocessURL		= voStreamingDRMPreprocessURL;
			pDRMHandle->Info				= voStreamingDRMInfo;
			pDRMHandle->DataBegin			= voStreamingDRMDataBegin;
			pDRMHandle->DataProcess_Chunk	= voStreamingDRMDataProcess_Chunk;
			pDRMHandle->DataEnd				= voStreamingDRMDataEnd;
			pDRMHandle->Info_FR				= voStreamingDRMInfo_FR;
			pDRMHandle->DataProcess_FR		= voStreamingDRMDataProcess_FR;
			pDRMHandle->SetParameter		= voStreamingDRMSetParameter;
			pDRMHandle->GetParameter		= voStreamingDRMGetParameter;
			pDRMHandle->PreprocessPlaylist	= voStreamingDRMPreprocessPlaylist;
		}

		return VO_ERR_NONE;
	}
#if defined __cplusplus
}
#endif