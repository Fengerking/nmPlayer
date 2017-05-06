#include "DRM_Verimatrix_AES128_Win.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	VO_U32 VO_API voDRMInit(VO_PTR* ppHandle, VO_DRM_OPENPARAM *pParam)
	{
		VOLOGI("++++++++++++++++++++++++++++here voDRM INIT");
		CDRM_Verimatrix_AES128 *pDRM = new CDRM_Verimatrix_AES128();
		pDRM->GetUDID();
		if (NULL == pDRM)
			return VO_ERR_OUTOF_MEMORY;

		*ppHandle = pDRM;

		return VO_ERR_DRM2_OK;
	}

	VO_U32 VO_API voDRMUninit(VO_PTR pHandle)
	{
		VOLOGI("here into uninit ========================================");
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM_Verimatrix_AES128 *pDRM = (CDRM_Verimatrix_AES128*)pHandle;
		pDRM->destroy();

		delete pDRM;

		return VO_OSMP_DRM_ERR_NONE;
	}

	VO_U32 VO_API voDRMSetThirdpartyAPI(VO_PTR pHandle, VO_PTR pParam)
	{
		return VO_OSMP_DRM_ERR_NONE;
	}

	VO_U32 VO_API voDRMGetInternalAPI(VO_PTR pHandle, VO_PTR *ppParam)
	{
		return VO_OSMP_DRM_ERR_NONE;
	}

	VO_U32 VO_API voDRMSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM_Verimatrix_AES128 * pDRM = (CDRM_Verimatrix_AES128*)pHandle;
		return pDRM->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voDRMGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if (NULL == pHandle) {
			VOLOGE("empty pointor");
			return VO_ERR_DRM2_BADPARAMETER;
		}

		CDRM_Verimatrix_AES128 * pDRM = (CDRM_Verimatrix_AES128*)pHandle;
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

#if defined __cplusplus
}
#endif