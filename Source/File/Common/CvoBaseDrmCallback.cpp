#include "CvoBaseDrmCallback.h"
#include "voPlayReady.h"
#include "voWidevine.h"


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CvoBaseDrmCallback::CvoBaseDrmCallback(VO_SOURCEDRM_CALLBACK* pDrmCB)
	: m_fCallback(pDrmCB ? pDrmCB->fCallback : 0)
	, m_pUserData(pDrmCB ? pDrmCB->pUserData : 0)
{
}

CvoBaseDrmCallback::~CvoBaseDrmCallback()
{
}

VO_U32 CvoBaseDrmCallback::DRMIsSupported(VO_DRM_TYPE nType)
{
	return m_fCallback ? m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_ISSUPPORTED, &nType, 0) : VO_ERR_DRM_MODULENOTFOUND;
}


VO_U32 CvoBaseDrmCallback::DRMInfo(VO_DRM_TYPE eType, VO_PTR pInfo)
{
	if(!m_fCallback)
		return VO_ERR_DRM_MODULENOTFOUND;

	VO_SOURCEDRM_INFO infoDrm;
	infoDrm.nType		= eType;
	infoDrm.pDrmInfo	= pInfo;

	return m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_DRMINFO, &infoDrm, 0);
}

VO_U32 CvoBaseDrmCallback::DRMData(VO_DRM_DATATYPE eDataType, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData/* = NULL*/, VO_U32 *pnDesSize/* = NULL*/, VO_PTR pInfo/* = NULL*/)
{
	VO_SOURCEDRM_DATA dataDrm;
	dataDrm.sDataInfo.nDataType	= eDataType;
	dataDrm.sDataInfo.pInfo		= pInfo;
	dataDrm.pData				= pSrcData;
	dataDrm.nSize				= nSrcSize;
	dataDrm.ppDstData			= ppDesData;
	dataDrm.pnDstSize			= pnDesSize;

	return m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_DRMDATA, &dataDrm, 0);
}



VO_U32 CvoBaseDrmCallback::DRMInfo_PlayReady(VO_PBYTE pDrmHeader, VO_U32 nDrmHeader, VO_GET_LICENSE_RESPONSE fGetLicenseResponse /* = NULL */, VO_PTR pUserData /* = NULL */)
{
	if(!m_fCallback)
		return VO_ERR_DRM_MODULENOTFOUND;

	VO_PLAYREADY_INFO infoPlayReady;
	infoPlayReady.pDrmHeader = pDrmHeader;
	infoPlayReady.nDrmHeader = nDrmHeader;

	VO_SOURCEDRM_INFO infoDrm;
	memset(&infoDrm, 0, sizeof(infoDrm));
	infoDrm.nType = VO_DRMTYPE_PlayReady;
	infoDrm.pDrmInfo = &infoPlayReady;
	// for PlayReady, first reserved will be stored by GetLicenseResponse callback, East 20110802
	infoDrm.nReserved[0] = (VO_U32)fGetLicenseResponse;
	infoDrm.nReserved[1] = (VO_U32)pUserData;

	return m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_DRMINFO, &infoDrm, 0);
}

VO_U32 CvoBaseDrmCallback::DRMInfo_Widevine(VO_PBYTE pECM, VO_U32 nECM)
{
	if(!m_fCallback)
		return VO_ERR_DRM_MODULENOTFOUND;

	VO_Widevine_INFO infoWidevineDRM = {0};
	infoWidevineDRM.pHeadData	= pECM;
	infoWidevineDRM.nHeaderLen	= nECM;

	VO_SOURCEDRM_INFO infoDrm;
	infoDrm.nType = VO_DRMTYPE_Widevine;
	infoDrm.pDrmInfo = &infoWidevineDRM;

	return m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_DRMINFO, &infoDrm, 0);
}



VO_U32 CvoBaseDrmCallback::DRMData_PlayRead_PacketData(VO_PBYTE pData, VO_U32 nSize, VO_PTR pAESCTRInfo)
{
	if(!m_fCallback)
		return VO_ERR_DRM_MODULENOTFOUND;

	VO_SOURCEDRM_DATA dataDrm;
	dataDrm.sDataInfo.nDataType = VO_DRMDATATYPE_PACKETDATA;
	dataDrm.sDataInfo.pInfo = pAESCTRInfo;
	dataDrm.pData = pData;
	dataDrm.nSize = nSize;
	dataDrm.ppDstData = 0;
	dataDrm.pnDstSize = 0;

	return m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_DRMDATA, &dataDrm, 0);
}

VO_U32 CvoBaseDrmCallback::DRMData_Widevine_PESData(VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE pDesData, VO_U32 *nDesSize)
{
	if(!m_fCallback)
		return VO_ERR_DRM_MODULENOTFOUND;

	if (nSrcSize <= 16)
		return VO_ERR_DRM_OK;

	VO_SOURCEDRM_DATA dataDrm;
	dataDrm.sDataInfo.nDataType = VO_DRMDATATYPE_PACKETDATA;
	dataDrm.sDataInfo.pInfo = 0;
	dataDrm.pData = pSrcData;
	dataDrm.nSize = nSrcSize;
	dataDrm.ppDstData = 0;
	dataDrm.pnDstSize = 0;

	return m_fCallback(m_pUserData, VO_SOURCEDRM_FLAG_DRMDATA, &dataDrm, 0);
}