#ifndef __CvoBaseDrmCallback_H__
#define __CvoBaseDrmCallback_H__

#include "voYYDef_filcmn.h"
#include "voSource.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CvoBaseDrmCallback
{
public:
	CvoBaseDrmCallback(VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~CvoBaseDrmCallback();

	//VO_ERR_DRM_OK mean supported, otherwise mean not supported
	virtual VO_U32	DRMIsSupported(VO_DRM_TYPE nType);

	virtual VO_U32	DRMInfo(VO_DRM_TYPE eType, VO_PTR pInfo);
	virtual VO_U32	DRMData(VO_DRM_DATATYPE eDataType, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData = NULL, VO_U32 *pnDesSize = NULL, VO_PTR pAdditionalInfo = NULL);

public:
	virtual VO_U32		DRMInfo_PlayReady(VO_PBYTE pDrmHeader, VO_U32 nDrmHeader, VO_GET_LICENSE_RESPONSE fGetLicenseResponse = NULL, VO_PTR pUserData = NULL);
	virtual VO_U32		DRMData_PlayRead_PacketData(VO_PBYTE pData, VO_U32 nSize, VO_PTR pAESCTRInfo);

	virtual VO_U32		DRMInfo_Widevine(VO_PBYTE pECM, VO_U32 nECM);
	virtual VO_U32		DRMData_Widevine_PESData(VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE pDesData, VO_U32 *nDesSize);

protected:
	VOSOURCEDRMCALLBACK		m_fCallback;
	VO_PTR					m_pUserData;
};

#ifdef _VONAMESPACE
}
#endif

#endif	//__CvoBaseDrmCallback_H__
