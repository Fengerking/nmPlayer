#include "CBaseDRM.h"

CBaseDRM::CBaseDRM()
{
}

CBaseDRM::~CBaseDRM()
{
}

VO_U32 CBaseDRM::Open()
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::Close()
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseDRM::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CBaseDRM::SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo)
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::GetDrmFormat(VO_DRM_FORMAT* pDrmFormat)
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::CheckCopyRightResolution(VO_U32 nWidth, VO_U32 nHeight)
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::Commit()
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize)
{
	return VO_ERR_DRM_OK;
}

VO_U32 CBaseDRM::DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize)
{
	return VO_ERR_DRM_OK;
}