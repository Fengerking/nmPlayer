// voDRMShell.cpp : 定义 DLL 应用程序的导出函数。
//

#include "voDRM2.h"

#ifdef _DRM_LABGENCY
#include "DRMShell_Labgency2.h"
#elif defined _DRM_AUTHENTEC
#include "DRMShell_Authentec.h"
#elif defined _DRM_VERIMATRIX
#include "DRMShell_Verimatrix.h"
#elif defined _DRM_IRDETO
#include "DRMShell_Irdeto.h"
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_U32 VO_API voDRMInit(VO_PTR* ppHandle, VO_DRM_OPENPARAM *pParam)
	{
#ifdef _DRM_LABGENCY
		CDRMShell * pDRM = new CDRMShell_Labgency2();
#elif defined _DRM_AUTHENTEC
		CDRMShell * pDRM = new CDRMShell_Authentec();
#elif defined _DRM_VERIMATRIX
		CDRMShell *pDRM = new CDRMShell_Verimatrix();
#elif defined _DRM_IRDETO
		CDRMShell *pDRM = new CDRMShell_Irdeto();
#endif

		if( pDRM->Init(pParam) )
		{
			delete pDRM;
			*ppHandle = 0;
			return VO_ERR_SOURCE_OPENFAIL;
		}

		*ppHandle = pDRM;

		return VO_ERR_SOURCE_OK;
	}

	VO_U32 VO_API voDRMUninit(VO_PTR pHandle)
	{
		CDRMShell * pDRM = (CDRMShell*)pHandle;

		return pDRM->Uninit();
	}

	VO_U32 VO_API voDRMSetThirdpartyAPI(VO_PTR pHandle, VO_PTR pParam)
	{
		CDRMShell * pDRM = (CDRMShell*)pHandle;

		return pDRM->SetThirdpartyAPI(pParam);
	}

	VO_U32 VO_API voDRMGetInternalAPI(VO_PTR pHandle, VO_PTR *ppParam)
	{
		CDRMShell * pDRM = (CDRMShell*)pHandle;

		return pDRM->GetInternalAPI(ppParam);
	}

	VO_U32 VO_API voDRMSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		CDRMShell * pDRM = (CDRMShell*)pHandle;

		return pDRM->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voDRMGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		CDRMShell * pDRM = (CDRMShell*)pHandle;

		return pDRM->GetParameter(uID, pParam);
	}


	VO_S32 VO_API voGetDRMShellAPI (VO_DRM2_API * pDRMHandle, VO_U32 uFlag)
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

		return VO_ERR_SOURCE_OK;
	}

#if defined __cplusplus
}
#endif


