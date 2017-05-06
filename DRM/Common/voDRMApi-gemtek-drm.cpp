#ifdef _DIVX_DRM
#include "CDivXDRM.h"
#endif	//_DIVX_DRM
#ifdef _WM_DRM
#include "CDxWMDRM.h"
#endif	//_WM_DRM
#include "voLog.h"

#if defined __cplusplus
extern "C" {
#endif

VO_U32 VO_API voDRMOpen(VO_PTR * ppHandle)
{
#ifdef _DIVX_DRM
	CDivXDRM* pDRM = new CDivXDRM;
#endif	//_DIVX_DRM
#ifdef _WM_DRM
	CDxWMDRM* pDRM = new CDxWMDRM;
#endif	//_WM_DRM
	if(!pDRM)
		return VO_ERR_OUTOF_MEMORY;

	VO_U32 rc = pDRM->Open();
	//only OK/DRM can be playback
	if(VO_ERR_DRM_OK != rc)
	{
		VOLOGE("drm open fail: 0x%08X!!", rc);

		delete pDRM;
		return rc;
	}

	*ppHandle = pDRM;
	return rc;
}

VO_U32 VO_API voDRMClose(VO_PTR pHandle)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	VO_U32 rc = pDRM->Close();
	delete pDRM;
	return rc;
}

VO_U32 VO_API voDRMSetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->SetParameter(uID, pParam);
}

VO_U32 VO_API voDRMGetParameter(VO_PTR pHandle, VO_U32 uID, VO_PTR pParam)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->GetParameter(uID, pParam);
}

VO_U32 VO_API voDRMSetDrmInfo(VO_PTR pHandle, VO_U32 nFlag, VO_PTR pDrmInfo)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->SetDrmInfo(nFlag, pDrmInfo);
}

VO_U32 VO_API voDRMGetDrmFormat(VO_PTR pHandle, VO_DRM_FORMAT* pDrmFormat)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->GetDrmFormat(pDrmFormat);
}

VO_U32 VO_API voCheckCopyRightResolution(VO_PTR pHandle, VO_U32 nWidth, VO_U32 nHeight) 
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->CheckCopyRightResolution(nWidth, nHeight);
}

VO_U32 VO_API voDRMCommit(VO_PTR pHandle)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->Commit();
}

VO_U32 VO_API voDRMDecryptData(VO_PTR pHandle, VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->DecryptData(pDataInfo, pData, nSize);
}

VO_U32 VO_API voDRMDecryptData2(VO_PTR pHandle, VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize)
{
	if(!pHandle)
		return VO_ERR_INVALID_ARG;

	CBaseDRM* pDRM = (CBaseDRM*)pHandle;
	return pDRM->DecryptData2(pDataInfo, pSrcData, nSrcSize, ppDstData, pnDstSize);
}

#ifdef _DIVX_DRM
VO_S32 VO_API voGetDivXDRMAPI(VO_DRM_API* pReadHandle, VO_U32 uFlag)
#endif	//_DIVX_DRM
#ifdef _WM_DRM
VO_S32 VO_API voGetWMDRMAPI(VO_DRM_API* pReadHandle, VO_U32 uFlag)
#endif	//_WM_DRM
{
	pReadHandle->Open = voDRMOpen;
	pReadHandle->Close = voDRMClose;
	pReadHandle->SetParameter = voDRMSetParameter;
	pReadHandle->GetParameter = voDRMGetParameter;
	pReadHandle->SetDrmInfo = voDRMSetDrmInfo;
	pReadHandle->GetDrmFormat = voDRMGetDrmFormat;
	pReadHandle->CheckCopyRightResolution = voCheckCopyRightResolution; 
	pReadHandle->Commit = voDRMCommit;
	pReadHandle->DecryptData = voDRMDecryptData;
	pReadHandle->DecryptData2 = voDRMDecryptData2;

	return VO_ERR_DRM_OK;
}

#if defined __cplusplus
}
#endif

