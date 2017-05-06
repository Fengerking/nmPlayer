#include "DRM_Nextscape_PlayReady.h"
#include "voPlayReady.h"
#include "voHLSDRM.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "CDRM_Nextscape_PlayReady"
#endif

ActivateCallback CDRM_Nextscape_PlayReady::m_fActivate = NULL;
DecryptCallback CDRM_Nextscape_PlayReady::m_fDecrypt = NULL;
void * CDRM_Nextscape_PlayReady::m_pCustomData = NULL;


#ifdef __cplusplus
extern "C" {
#endif

	int RegistCallback(ActivateCallback p_activate, DecryptCallback p_callback, void * p_custom_data)
	{
		VOLOGI("p_activate %p, p_callback %p, p_custom_data %p", p_activate, p_callback, p_custom_data);
		return CDRM_Nextscape_PlayReady::RegistCallback(p_activate, p_callback, p_custom_data);
	}

#ifdef __cplusplus
};
#endif

CDRM_Nextscape_PlayReady::CDRM_Nextscape_PlayReady(void)
: m_iDRM(0)
, m_bHLSInit(false)
, m_pManifestData(NULL)
, m_uSizeManifest(0)
{
	VOLOGI("CDRM_Nextscape_PlayReady");
}

CDRM_Nextscape_PlayReady::~CDRM_Nextscape_PlayReady(void)
{
	Uninit();
}

VO_U32 CDRM_Nextscape_PlayReady::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_Nextscape_PlayReady");

	return CDRM::Init(pParam);
}

VO_U32 CDRM_Nextscape_PlayReady::Uninit()
{
	m_iDRM = 0;
	m_bHLSInit = false;

	if (m_pManifestData)
	{
		delete []m_pManifestData;
		m_pManifestData = NULL;
		m_uSizeManifest = 0;
	}

	return VO_ERR_NONE;
}

VO_U32 CDRM_Nextscape_PlayReady::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	if(pParam == NULL)
		return VO_ERR_DRM2_BADDRMINFO;
	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			if (NULL == pParam) {
				VOLOGE("VO_DRM_FLAG_DRMINFO is NULL");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			Uninit();

			switch (eSourceFormat)
			{
			case VO_DRM2SRC_CHUNK_HLS:
				{
					VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);

					m_pManifestData =  new VO_BYTE[pDRMInfo->uSizeManifest];
					memcpy(m_pManifestData, pDRMInfo->pManifestData, pDRMInfo->uSizeManifest);

					m_uSizeManifest = pDRMInfo->uSizeManifest;
				}
				break;

			case VO_DRM2SRC_CHUNK_SSTR:
				{
					VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);

					int iRet = m_fActivate(pDRMInfo->pManifestData, pDRMInfo->uSizeManifest, NULL, 0, CONTENT_SSTR, &m_iDRM, m_pCustomData);
					if (0 != iRet) {
						VOLOGE("fActivate %d, bDRM %d", iRet, m_iDRM);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;

			case VO_DRM2SRC_MEDIAFORMAT:
				{
					VO_DRM2_INFO* pDRMInfo = static_cast<VO_DRM2_INFO *>(pParam);
					VO_DRM2_INFO_GENERAL_DATA* pInfo = static_cast<VO_DRM2_INFO_GENERAL_DATA *>(pDRMInfo->pDrmInfo);

					if(pInfo->uSize <= 0)
						return VO_ERR_DRM2_BADDRMINFO;

					if(pInfo->uSize <= 0 || pInfo->pData == NULL)
						return VO_ERR_DRM2_BADDRMINFO;
					int iRet = m_fActivate(NULL, 0, pInfo->pData, pInfo->uSize, CONTENT_PIFF, &m_iDRM, m_pCustomData);
					if (0 != iRet) {
						VOLOGE("fActivate %d, bDRM %d", iRet, m_iDRM);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;
			}

		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			if (NULL == pParam) {
				VOLOGE("VO_DRM2_DATA is NULL");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			VO_DRM2_DATA * pDRMData = static_cast<VO_DRM2_DATA *>(pParam);
			switch (pDRMData->sDataInfo.nDataType)
			{
			case VO_DRM2DATATYPE_CHUNK_BEGIN:
				break;

			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
				{
					if (VO_DRM2SRC_CHUNK_HLS == eSourceFormat)
					{
						if (!m_bHLSInit &&
							0x50 == pDRMData->pData[0] &&
							0x52 == pDRMData->pData[1] &&
							0x45 == pDRMData->pData[2] &&
							0x7 == pDRMData->pData[3])
						{
							int iHeaderSize_hamonic = 0;
							memcpy(&iHeaderSize_hamonic, pDRMData->pData + 4, 4);

							int iRet = m_fActivate(m_pManifestData, m_uSizeManifest, pDRMData->pData, iHeaderSize_hamonic, CONTENT_HLS, &m_iDRM, m_pCustomData);
							if (0 != iRet) {
								VOLOGE("fActivate %d, bDRM %d", iRet, m_iDRM);
								return VO_ERR_DRM2_BADDRMINFO;
							}

							m_bHLSInit = true;
						}

						VO_U64 ullChunkOffset = *((VO_U64*)pDRMData->nReserved[1]);

						if(pDRMData->nSize <= 0)
							return VO_ERR_DRM2_BADDRMINFO;
						int iRet = m_fDecrypt(pDRMData->pData, pDRMData->nSize, pDRMData->pData, pDRMData->pnDstSize, NULL, 0, ullChunkOffset, 0, m_pCustomData);
						if (0 != iRet) {
							VOLOGE("fDecrypt %d, SrcSize %d, DesSize %d, ullChunkOffset %lld", iRet, pDRMData->nSize, *pDRMData->pnDstSize, ullChunkOffset);
							return VO_ERR_DRM2_BADDRMDATA;
						}
					}
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_END:
				break;

			case VO_DRM2DATATYPE_MEDIASAMPLE:
				{
					//if (0 == m_iDRM)
					//	return 0;

					VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO * pInfo = (VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO *)pDRMData->sDataInfo.pInfo;

					VOLOGI("Current sample size %d pdata 0x%08x",pDRMData->nSize,pDRMData->pData);
					if(pDRMData->nSize <= 0)
						return VO_ERR_DRM2_BADDRMINFO;
					int iRet = m_fDecrypt(pDRMData->pData, pDRMData->nSize, pDRMData->pData, pDRMData->pnDstSize, pInfo->pSampleEncryptionBox, pInfo->uSampleEncryptionBoxSize, pInfo->uSampleIndex, pInfo->uTrackId, m_pCustomData);
					if (0 != iRet) {
						VOLOGE("fDecrypt %d, SrcSize %d, DesSize %d", iRet, pDRMData->nSize, *pDRMData->pnDstSize);
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
