#include "DRM_CableVision_PlayReady.h"
#include "voHLSDRM.h"
#include "voDRM.h"
#include "acm_drm_stub.h"
#include "voLog.h"

CDRM_CableVision_PlayReady::CDRM_CableVision_PlayReady(void)
: m_pHandle(NULL)
{
	VOLOGI("CDRM_CV_PR");
}

CDRM_CableVision_PlayReady::~CDRM_CableVision_PlayReady(void)
{
}

VO_U32 CDRM_CableVision_PlayReady::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_CableVision_PlayReady");

	return CDRM::Init(pParam);
}

VO_U32 CDRM_CableVision_PlayReady::Uninit()
{
	if (m_pHandle)
	{
		VOLOGR("TerminatePlayback");
		int iRet = ACM_DRM_TerminatePlayback(m_pHandle);
		if (iRet) {
			VOLOGE("TerminatePlayback failed %x", iRet);
			return VO_ERR_DRM2_BASE;
		}
	}

	return VO_ERR_NONE;
}


VO_U32 CDRM_CableVision_PlayReady::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	int iRet = 0;

	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			if (VO_DRM2SRC_MEDIAFORMAT == eSourceFormat)
			{
				VOLOGW("Only support file mode");
				return VO_ERR_DRM2_MODULENOTFOUND;
			}

			VO_DRM2_INFO * pDrmInfo = static_cast<VO_DRM2_INFO *>(pParam);
			if (VO_DRM2TYPE_Irdeto != pDrmInfo->nType)
				return VO_ERR_DRM_MODULENOTFOUND;

			S_IrdetoDRM_INFO *pIrdetoInfo = static_cast<S_IrdetoDRM_INFO *>(pDrmInfo->pDrmInfo);

			VOLOGR("+InitWithManifest : pURL %s", pIrdetoInfo->pURL);
			iRet = ACM_DRM_InitWithManifest(pIrdetoInfo->pURL, pIrdetoInfo->pManifest, &m_pHandle);
			VOLOGR("-InitWithManifest : pURL %s", pIrdetoInfo->pURL);
			if (iRet) {
				VOLOGE("InitWithManifest failed %x", iRet);
				return VO_ERR_DRM2_BADDRMINFO;
			}
		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			VO_DRM2_DATA * pDrmData = static_cast<VO_DRM2_DATA *>(pParam);
			S_HLS_DRM_PROCESS_INFO *pHLSDRMProcess = static_cast<S_HLS_DRM_PROCESS_INFO *>(pDrmData->sDataInfo.pInfo);
			S_HLS_DRM_ASSIST_INFO *pHLSDRMInfo = (S_HLS_DRM_ASSIST_INFO *)(pHLSDRMProcess->pInfo);

			switch (pHLSDRMProcess->eDrmProcessInfo)
			{
			case DecryptProcess_BEGIN:
				{
					VOLOGR("SegmentBegin : ulSequenceID %d", pHLSDRMInfo->ulSequenceID);
					iRet = ACM_DRM_SegmentBegin(m_pHandle, pHLSDRMInfo->ulSequenceID);
					if (iRet) {
						VOLOGE("SegmentBegin failed %x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			case DecryptProcess_PROCESSING:
				{
					VOLOGR("+DecryptPart : ulSequenceID %d, pData[0] %x, nSize %d", pHLSDRMInfo->ulSequenceID, pDrmData->pData[0], pDrmData->nSize);
					iRet = ACM_DRM_DecryptPart(m_pHandle, pHLSDRMInfo->ulSequenceID, pDrmData->pData, pDrmData->nSize);
					VOLOGR("-DecryptPart : ulSequenceID %d, pData[0] %x, nSize %d", pHLSDRMInfo->ulSequenceID, pDrmData->pData[0], pDrmData->nSize);
					if (iRet) {
						VOLOGE("DecryptPart failed %x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			case DecryptProcess_END:
				{
					VOLOGR("SegmentEnd : ulSequenceID %d", pHLSDRMInfo->ulSequenceID);
					iRet = ACM_DRM_SegmentEnd(m_pHandle, pHLSDRMInfo->ulSequenceID);
					if (iRet) {
						VOLOGE("SegmentEnd failed %x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			default:
				break;
			}
		}
		break;

	default:
		break;
	}

	return VO_ERR_DRM_OK;
}
