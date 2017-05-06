#include "DRM_CableVision_PlayReady.h"
#include "voHLSDRM.h"
#include "acm_drm_stub.h"
#include "voLog.h"

CDRM_CableVision_PlayReady::CDRM_CableVision_PlayReady(void)
: m_pHandle(NULL)
{
	VOLOGI("CDRM_CV_PR");
}

CDRM_CableVision_PlayReady::~CDRM_CableVision_PlayReady(void)
{
	Uninit();
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
		VOLOGI("TerminatePlayback");
		int iRet = ACM_DRM_TerminatePlayback(m_pHandle);
		m_pHandle = NULL;

		if (iRet) {
			VOLOGE("TerminatePlayback failed %x", iRet);
			return VO_ERR_DRM2_BASE;
		}
	}

	return VO_ERR_DRM2_OK;
}


VO_U32 CDRM_CableVision_PlayReady::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	switch (eSourceFormat)
	{
	case VO_DRM2SRC_MEDIAFORMAT:
		{
			VOLOGE("!DRM Module");
			return VO_ERR_DRM2_MODULENOTFOUND;
		}
		break;

	case VO_DRM2SRC_CHUNK_SSTR:
	case VO_DRM2SRC_CHUNK_DASH:
		return 0; //TOTO

	default:
		break;
	}

	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			if (VO_DRM2SRC_MEDIAFORMAT == eSourceFormat)
			{
				VOLOGW("Only support file mode");
				return VO_ERR_DRM2_MODULENOTFOUND;
			}

			Uninit();

			VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);

			VOLOGI("+InitWithManifest : pURL %s", pDRMInfo->pURL);
			int iRet = ACM_DRM_InitWithManifest(pDRMInfo->pURL, (char*)pDRMInfo->pManifestData, &m_pHandle);
			VOLOGI("-InitWithManifest : pURL %s", pDRMInfo->pURL);
			if (iRet) {
				VOLOGE("InitWithManifest failed 0x%x", iRet);
				return VO_ERR_DRM2_BADDRMINFO;
			}
		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			VO_DRM2_DATA * pDRMData = static_cast<VO_DRM2_DATA *>(pParam);
			VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pDRMData->sDataInfo.pInfo);

			switch (pDRMData->sDataInfo.nDataType)
			{
			case VO_DRM2DATATYPE_CHUNK_BEGIN:
				{
					VOLOGI("SegmentBegin : ulSequenceID %d", pDRMHLSProcess->uSequenceNum);
					int iRet = ACM_DRM_SegmentBegin(m_pHandle, pDRMHLSProcess->uSequenceNum);
					if (iRet) {
						VOLOGE("SegmentBegin failed 0x%x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
				{
					VOLOGI("+DecryptPart : ulSequenceID %d, pData[0] %x, nSize %d", pDRMHLSProcess->uSequenceNum, pDRMData->pData[0], pDRMData->nSize);
					int iRet = ACM_DRM_DecryptPart(m_pHandle, pDRMHLSProcess->uSequenceNum, pDRMData->pData, pDRMData->nSize);
					VOLOGI("-DecryptPart : ulSequenceID %d, pData[0] %x, nSize %d", pDRMHLSProcess->uSequenceNum, pDRMData->pData[0], pDRMData->nSize);
					if (iRet) {
						VOLOGE("DecryptPart failed 0x%x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_END:
				{
					VOLOGI("SegmentEnd : ulSequenceID %d", pDRMHLSProcess->uSequenceNum);
					int iRet = ACM_DRM_SegmentEnd(m_pHandle, pDRMHLSProcess->uSequenceNum);
					if (iRet) {
						VOLOGE("SegmentEnd failed 0x%x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			default:
				return VO_ERR_NOT_IMPLEMENT;
			}
		}
		break;

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}
