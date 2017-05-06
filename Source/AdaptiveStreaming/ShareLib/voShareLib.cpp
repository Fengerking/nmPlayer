#include "ShareLib.h"
#include "voShareLib.h"
#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#if defined __cplusplus
extern "C" {
#endif

	VO_U32 VO_API voShareLibInit(VO_PTR* ppHandle, VO_PTR pReserved)
	{
		CShareLib *pShareLib = NULL;
		
		//if (pEventHandlerCB->szDRMTYPE)
			pShareLib = new CShareLib();

		if (NULL == pShareLib)
			return VO_ERR_OUTOF_MEMORY;

		*ppHandle = pShareLib;

		return VO_ERR_NONE;
	}

	VO_U32 VO_API voShareLibUninit(VO_PTR pHandle)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CShareLib *pShareLib = (CShareLib*)pHandle;
		
		delete pShareLib;
		pShareLib = NULL;
		return VO_ERR_NONE;
	}

	VO_U32 VO_API voShareLibSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CShareLib * pShareLib = (CShareLib*)pHandle;

		return pShareLib->SetParameter(uID, pParam);
	}

	VO_U32 VO_API voShareLibGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam, COPY_FUNC pFunc)
	{
		if(pHandle == NULL)
			return VO_ERR_FAILED;

		CShareLib * pShareLib = (CShareLib*)pHandle;

		return pShareLib->GetParameter(uID, pParam, pFunc);
	}


#if defined __cplusplus
}
#endif

#if defined __cplusplus
extern "C" {
#endif

VO_S32 VO_API voGetShareLibAPI (VO_ShareLib_API * pShareLib, VO_U32 uFlag)
{
	if(pShareLib)
	{
		pShareLib->Init				= voShareLibInit;
		pShareLib->Uninit			= voShareLibUninit;
		pShareLib->SetParameter		= voShareLibSetParameter;
		pShareLib->GetParameter		= voShareLibGetParameter;
	}	

	return VO_ERR_NONE;
}
#if defined __cplusplus
}
#endif