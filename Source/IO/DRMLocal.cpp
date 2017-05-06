#include "DRMLocal.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CDRMLocal::CDRMLocal()
{
	memset( &m_DRMCB, 0, sizeof(VO_SOURCEDRM_CALLBACK2) );
}

CDRMLocal::~CDRMLocal(void)
{
}

VO_U32 CDRMLocal::init(VO_SOURCEDRM_CALLBACK2* pDrmCB)
{
	VOLOGI("%p", pDrmCB);
	memcpy( &m_DRMCB, pDrmCB, sizeof(VO_SOURCEDRM_CALLBACK2) );

	return VO_SOURCE2_IO_OK;
}


VO_U32 CDRMLocal::getDRMInfo(VO_CHAR* szURI, VO_U64* pullPos, VO_U32* puSize)
{
	if (NULL == m_DRMCB.fCallback)
		return VO_SOURCE2_IO_FAIL;

	VO_DRM2_INFO_GET_LOCATION s_getInfo = {0};

	s_getInfo.pURI		= szURI;
	s_getInfo.pullPos	= pullPos;
	s_getInfo.puSize	= puSize;

	return m_DRMCB.fCallback(m_DRMCB.pUserData, VO_DRM_FLAG_GETINFOLOCATION, &s_getInfo, VO_DRM2SRC_RAWFILE);
}

VO_U32 CDRMLocal::setDRMInfo(VO_U64 ullPos, VO_PBYTE pData, VO_U32 uSize, VO_PTR pInfo)
{
	if (NULL == m_DRMCB.fCallback)
		return VO_SOURCE2_IO_FAIL;

	VO_DRM2_INFO s_Info;
	VO_DRM2_INFO_GENERAL_DATA s_Data = {0};

	s_Data.pData	= pData;
	s_Data.uSize	= uSize;

	s_Info.pDrmInfo = &s_Data;

	VO_U32 uRet = m_DRMCB.fCallback(m_DRMCB.pUserData, VO_SOURCEDRM_FLAG_DRMINFO, &s_Info, VO_DRM2SRC_RAWFILE);
	if (VO_ERR_DRM2_NEEDRETRY == uRet)
		return VO_SOURCE2_IO_RETRY;

	return uRet;
}

VO_U32 CDRMLocal::getOriginalFileSize(VO_U64* pullFileSize)
{
	return m_DRMCB.fCallback ? m_DRMCB.fCallback(m_DRMCB.pUserData, VO_DRM_FLAG_GETORIGINALSIZE, pullFileSize, VO_DRM2SRC_RAWFILE) : VO_SOURCE2_IO_FAIL;
}


VO_U32 CDRMLocal::getActualLocation(VO_U64 ullWantedPos, VO_U32 uWantedSize, VO_U64* pullActualPos, VO_U32* pullActualSize)
{
	if (NULL == m_DRMCB.fCallback)
		return VO_SOURCE2_IO_FAIL;

	VO_DRM2_DATA_CONVERT_LOCATION s_CL = {0};

	s_CL.ullWantedPos	= ullWantedPos;
	s_CL.uWantedSize	= uWantedSize;
	s_CL.pullActualPos	= pullActualPos;
	s_CL.puActualSize	= pullActualSize;

	return m_DRMCB.fCallback(m_DRMCB.pUserData, VO_DRM_FLAG_CONVERTLOCATION, &s_CL, VO_DRM2SRC_RAWFILE);
}

VO_U32 CDRMLocal::decryptData(VO_U64 ullActualOffset, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData/* = NULL*/, VO_U32 *pnDesSize/* = NULL*/, VO_PTR pAdditionalInfo/* = NULL*/)
{
	if (NULL == m_DRMCB.fCallback)
		return VO_SOURCE2_IO_FAIL;

	VO_DRM2_DATA s_Data;

	s_Data.sDataInfo.nDataType	= VO_DRM2DATATYPE_RAWFILE;
	s_Data.sDataInfo.pInfo		= pAdditionalInfo;
	s_Data.pData				= pSrcData;
	s_Data.nSize				= nSrcSize;
	s_Data.ppDstData			= ppDesData;
	s_Data.pnDstSize			= pnDesSize;
	s_Data.nReserved[1]			= (VO_U32)&ullActualOffset;

	return m_DRMCB.fCallback(m_DRMCB.pUserData, VO_SOURCEDRM_FLAG_DRMDATA, &s_Data, VO_DRM2SRC_RAWFILE);
}

