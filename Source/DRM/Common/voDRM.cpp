// voDRM.cpp : 定义 DLL 应用程序的导出函数。
//
#ifdef _DRM_VISUALON_AES128
#include "DRM_VisualOn_AES128.h"
#elif defined _DRM_CABLEVISION
#include "DRM_CableVision_PlayReady.h"
#elif defined _DRM_COMCAST
#include "DRM_Comcast_PlayReady.h"
#elif defined _DRM_SHOWTIME
#include "DRM_Showtime_PRWM.h"
#elif defined _DRM_VISUALON_EMPTY
#include "DRM_VisualOn_Empty.h"
#elif defined _DRM_VERIMATRIX
#include "DRM_Verimatrix_AES128.h"
#elif defined _DRM_MOTO_AES128
#include "DRM_Motorola_AES128.h"
#elif defined _DRM_DISCRETIX_PLAYREADY
#include "DRM_Discretix_PlayReady.h"
#elif defined _DRM_NEXTSCAPE_PLAYREADY
#include "DRM_Nextscape_PlayReady.h"
#elif defined _DRM_ADAPTER
#include "DRMAdapter.h"
#elif defined _DRM_AMAZON_PLAYREADY
#include "DRM_Amazon_PlayReady.h"
#endif
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif

	VO_U32 VO_API voDRMInit(VO_PTR* ppHandle, VO_DRM_OPENPARAM *pParam)
	{
#ifdef _DRM_VISUALON_AES128
		CDRM *pDRM = new CDRM_VisualOn_AES128();
#elif defined _DRM_CABLEVISION
		CDRM *pDRM = new CDRM_CableVision_PlayReady();
#elif defined _DRM_COMCAST
		CDRM *pDRM = new CDRM_Comcast_PlayReady();
#elif defined _DRM_SHOWTIME
		CDRM *pDRM = new CDRM_Showtime_PRWM();
#elif defined _DRM_VISUALON_EMPTY
		CDRM *pDRM = new CDRM_VisualOn_Empty();
#elif defined _DRM_VERIMATRIX
		CDRM *pDRM = new CDRM_Verimatrix_AES128();
#elif defined _DRM_MOTO_AES128
		CDRM *pDRM = new CDRM_Motorola_AES128();
#elif defined _DRM_DISCRETIX_PLAYREADY
		CDRM *pDRM = new DRM_Discretix_PlayReady();
#elif defined _DRM_NEXTSCAPE_PLAYREADY
		CDRM *pDRM = new CDRM_Nextscape_PlayReady();
#elif defined _DRM_ADAPTER
		CDRM *pDRM = new DRMAdapter();
#elif defined _DRM_AMAZON_PLAYREADY
		CDRM *pDRM = new DRM_Amazon_PlayReady();
#endif

		if (NULL == pDRM)
			return VO_ERR_OUTOF_MEMORY;

		VO_U32 uRet = pDRM->Init(pParam);
		if (uRet)
		{
			delete pDRM;
			*ppHandle = 0;
			return uRet;
		}

		*ppHandle = pDRM;

		return VO_ERR_DRM2_OK;
	}

	VO_U32 VO_API voDRMUninit(VO_PTR pHandle)
	{
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM * pDRM = (CDRM*)pHandle;

		VO_U32 ret = pDRM->Uninit();

		delete pDRM;

		return ret;
	}

	VO_U32 VO_API voDRMSetThirdpartyAPI(VO_PTR pHandle, VO_PTR pParam)
	{
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM * pDRM = (CDRM*)pHandle;
		return pDRM->SetThirdpartyAPI(pParam);
	}

	VO_U32 VO_API voDRMGetInternalAPI(VO_PTR pHandle, VO_PTR *ppParam)
	{
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM * pDRM = (CDRM*)pHandle;
		return pDRM->GetInternalAPI(ppParam);
	}

	VO_U32 VO_API voDRMSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM * pDRM = (CDRM*)pHandle;
		return pDRM->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voDRMGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
#if defined _DRM_VERIMATRIX && defined _IOS
		if (VO_PID_DRM2_UNIQUE_IDENTIFIER == uID)
			return CDRM_Verimatrix_AES128::getUniqueID(pParam);
#endif

		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM * pDRM = (CDRM*)pHandle;
		return pDRM->GetParameter(uID, pParam);
	}


	VO_S32 VO_API voGetDRMAPI (VO_DRM2_API * pDRMHandle, VO_U32 uFlag)
	{
		if(pDRMHandle)
		{
			pDRMHandle->Init				= voDRMInit;
			pDRMHandle->Uninit				= voDRMUninit;
			pDRMHandle->SetThirdpartyAPI	= voDRMSetThirdpartyAPI;
			pDRMHandle->GetInternalAPI		= voDRMGetInternalAPI;
			pDRMHandle->SetParameter		= voDRMSetParameter;
			pDRMHandle->GetParameter		= voDRMGetParameter;
		}

		return VO_ERR_DRM2_OK;
	}

#ifndef _VONAMESPACE
#if defined __cplusplus
}
#endif
#endif
