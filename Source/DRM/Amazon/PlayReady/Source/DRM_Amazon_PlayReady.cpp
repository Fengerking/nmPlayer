#include "DRM_Amazon_PlayReady.h"
#include "voPlayReady.h"
#include "DRMToolUtility.h"
#include "voLog.h"
#include "base64.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DRM_Amazon_PlayReady::DRM_Amazon_PlayReady(void)
: m_bIsDRM(VO_FALSE)
, m_pDRMHeader(NULL)
, m_uDRMHeader(0)
, m_pCustomDRMHeader(NULL)
, m_uCustomDRMHeader(0)
, m_pTrackInfos(NULL)
{
	VOLOGR("DRM_Amazon_PlayReady");
}

DRM_Amazon_PlayReady::~DRM_Amazon_PlayReady(void)
{
	Uninit();
}

VO_U32 DRM_Amazon_PlayReady::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_VisualOn_AES128");

	return CDRM::Init(pParam);
}

VO_U32 DRM_Amazon_PlayReady::Uninit()
{
	m_bIsDRM = VO_FALSE;

	if (m_pDRMHeader)
	{
		delete []m_pDRMHeader;
		m_pDRMHeader = NULL;
		m_uDRMHeader = 0;
	}

	if (m_pCustomDRMHeader)
	{
		delete []m_pCustomDRMHeader;
		m_pCustomDRMHeader = NULL;
		m_uCustomDRMHeader = 0;
	}

	TrackInfo* pTempTI = m_pTrackInfos;
	while (pTempTI)
	{
		pTempTI = pTempTI->pNext;
		delete m_pTrackInfos;
		m_pTrackInfos = pTempTI;
	} 

	return VO_ERR_DRM2_OK;
}

VO_U32 DRM_Amazon_PlayReady::SetThirdpartyAPI(VO_PTR pParam)
{
	return VO_ERR_DRM2_OK;
}

VO_U32 DRM_Amazon_PlayReady::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	return CDRM::SetParameter(uID, pParam);
}

VO_U32 DRM_Amazon_PlayReady::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			Uninit();

			VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);
			if (VO_DRM2SRC_CHUNK_SSTR == eSourceFormat)
			{
				VO_U32 uRet = SSTR_getProtectionHeader( (VO_CHAR*)pDRMInfo->pManifestData, pDRMInfo->uSizeManifest, &m_pDRMHeader, &m_uDRMHeader );
				if (VO_ERR_DRM2_OK == uRet)
				{
					VO_U32 uRet = TriggerInitDataCallbackEvent();
					if (uRet != 0) 
						return uRet;
				}
				else if (VO_ERR_DRM2_NOTENCRYPTION == uRet)
				{
					;
				}
				else
				{
					VOLOGE("SSTR_getProtectionHeader %d", uRet);
					return uRet;
				}
			}
		}
		break;

	case VO_DRM_FLAG_DRM_PROCECTION:
		{
			voCAutoLock lock(&m_lock);

			VO_DRM2_INFO * pDRMInfo = static_cast<VO_DRM2_INFO *>(pParam);
			VO_DRM2_INFO_GENERAL_DATA* p = static_cast<VO_DRM2_INFO_GENERAL_DATA*>(pDRMInfo->pDrmInfo);

			ProtectionSystemSpecificHeader s_ProtectionSystemSpecificHeader = {0};
			VO_U32 uRet = parseProtectionSystemSpecificHeaderBox(p->pData, p->uSize, &s_ProtectionSystemSpecificHeader);
			if (uRet) {
				VOLOGE("parseProtectionSystemSpecificHeader %d", uRet);
				return uRet;
			}

			const unsigned char uuidContentProtectionSystem[] = {0x9a, 0x04, 0xf0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xab, 0x92, 0xe6, 0x5b, 0xe0, 0x88, 0x5f, 0x95};
			if (memcmp(s_ProtectionSystemSpecificHeader.uuidSystem, uuidContentProtectionSystem, 16) == 0)
			{
				VOLOGI("Protection System Specific Header");
				if (m_pDRMHeader)
				{
					delete []m_pDRMHeader;
					m_pDRMHeader = NULL;
					m_uDRMHeader = 0;
				}

				m_pDRMHeader = (VO_BYTE*)Base64Encode( (VO_CHAR*)s_ProtectionSystemSpecificHeader.pData, s_ProtectionSystemSpecificHeader.uSize );
				m_uDRMHeader = strlen( (VO_CHAR*)m_pDRMHeader );
				m_uDRMHeader = m_uDRMHeader / 2 * 2;


				VO_U32 uRet = TriggerInitDataCallbackEvent();
				if (uRet != 0) 
					return uRet;
			}
		}
		break;

	case VO_DRM_FLAG_DRM_TRACK_INFO:
		{
			voCAutoLock lock(&m_lock);

			VO_DRM2_INFO * pDRMInfo = static_cast<VO_DRM2_INFO *>(pParam);
			VO_DRM2_TRACKDECYPTION_INFO* p = static_cast<VO_DRM2_TRACKDECYPTION_INFO*>(pDRMInfo->pDrmInfo);

			TrackInfo* pATrackInfo = new TrackInfo();
			memset( pATrackInfo, 0, sizeof(TrackInfo) );
			VO_U32 uRet = parseTrackEncryptionBox(p->pTrackEncryptionBox, p->uTrackEncryptionBox, &pATrackInfo->s_TrackEncryption);
			if (uRet) {
				VOLOGE("parseTrackEncryptionBox %d", uRet);
				delete pATrackInfo;

				return uRet;
			}
			pATrackInfo->uTrackID = p->uTrackID;

			if (NULL == m_pTrackInfos)
			{
				m_pTrackInfos = pATrackInfo;
			}
			else
			{
				TrackInfo* pTrackInfo = m_pTrackInfos;
				do 
				{
					if (NULL == pTrackInfo->pNext)
					{
						pTrackInfo->pNext = pATrackInfo;
						break;
					}

					pTrackInfo = pTrackInfo->pNext;
				} while (pTrackInfo);
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
			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
			case VO_DRM2DATATYPE_CHUNK_END:
				break;

			case VO_DRM2DATATYPE_SEQUENCEHEADER:
				{
					voCAutoLock lock(&m_lock);

					if (!m_bIsDRM)
						break;

					VO_BYTE* pHeader = (m_pCustomDRMHeader) ? m_pCustomDRMHeader : m_pDRMHeader;
					VO_U32   uHeader = (m_pCustomDRMHeader) ? m_uCustomDRMHeader : m_uDRMHeader;

					VO_U32 uTotalSize = pDRMData->nSize + uHeader + sizeof(VO_U32) * 2 + 2;
					if (uTotalSize > *pDRMData->pnDstSize) {
						*pDRMData->pnDstSize = uTotalSize;
						return VO_ERR_DRM2_OUTPUT_BUF_SMALL;
					}
					VO_U32 uRet = combine(pDRMData->pData, &pDRMData->nSize, pHeader, uHeader);

					if (uRet) {
						VOLOGE("%d", uRet);

						return uRet;
					}

					*pDRMData->ppDstData = pDRMData->pData;
					*pDRMData->pnDstSize = pDRMData->nSize;
				}
				break;

			case VO_DRM2DATATYPE_MEDIASAMPLE:
				{
					VO_DRM2_SAMPLEDECYPTION_INFO * pInfo = static_cast<VO_DRM2_SAMPLEDECYPTION_INFO *>(pDRMData->sDataInfo.pInfo);

					if (!m_bIsDRM || 0 == pInfo->uSampleEncryptionBoxSize)
						break;

					SampleEncryption s_EncryptionInfo;
					memset( &s_EncryptionInfo, 0, sizeof(SampleEncryption) );

					TrackInfo* pTrackInfos = m_pTrackInfos;
					while (pTrackInfos)
					{
						if (pTrackInfos->uTrackID == pInfo->uTrackId)
						{
							s_EncryptionInfo.s_TrackEncryption.uAlgorithmID	= pTrackInfos->s_TrackEncryption.uAlgorithmID;
							s_EncryptionInfo.s_TrackEncryption.uSizeIV		= pTrackInfos->s_TrackEncryption.uSizeIV;
							memcpy(s_EncryptionInfo.s_TrackEncryption.KID, pTrackInfos->s_TrackEncryption.KID, 16);

							break;
						}

						pTrackInfos = pTrackInfos->pNext;
					}

					if (VO_DRM2_DATAINFO_SINGLE_SAMPLEDECYPTION == pDRMData->sDataInfo.nReserved[0])
					{
						VO_U32 uRet = parseCencSampleAuxiliaryDataFormat(pInfo->pSampleEncryptionBox, pInfo->uSampleEncryptionBoxSize, &s_EncryptionInfo);
						if (uRet) {
							VOLOGE("parseCencSampleAuxiliaryDataFormat %d", uRet);
							return uRet;
						}
					} 
					else
					{
						VO_U32 uRet = parseSampleEncryptionBox(pInfo->pSampleEncryptionBox, pInfo->uSampleEncryptionBoxSize, pInfo->uSampleIndex, &s_EncryptionInfo);
						if (uRet) {
							VOLOGE("parseSampleEncryptionBox %d", uRet);
							return uRet;
						}
					}

					VO_U32 uNeedSize = sizeof(SampleEncryption) + s_EncryptionInfo.hNumberOfEntries * sizeof(SampleEncryption::SubSampleEncryptionEntry);
					if (sizeof(VO_U32) * 2 + 2 + uNeedSize > 1024)
					{
						VOLOGE("%d", uNeedSize);
						return VO_ERR_DRM2_OUTPUT_BUF_SMALL;
					}

					VO_BYTE* pTemp = pDRMData->pData + pDRMData->nSize;
					memcpy( pTemp, &pDRMData->nSize, sizeof(VO_U32) );
					pTemp += sizeof(VO_U32);
					memset(pTemp, 1, 1);//ID 1 = main data
					pTemp += 1;

					memcpy( pTemp, &s_EncryptionInfo, sizeof(SampleEncryption) );

					SampleEncryption* p = (SampleEncryption*)pTemp;
					pTemp += sizeof(SampleEncryption);

					if (s_EncryptionInfo.hNumberOfEntries > 0)
					{
						p->pSubEntries = NULL;

						memcpy(pTemp, s_EncryptionInfo.pSubEntries, sizeof(SampleEncryption::SubSampleEncryptionEntry) * s_EncryptionInfo.hNumberOfEntries);
						pTemp += sizeof(SampleEncryption::SubSampleEncryptionEntry) * s_EncryptionInfo.hNumberOfEntries;

						delete []s_EncryptionInfo.pSubEntries;
					}

					memcpy( pTemp, &uNeedSize, sizeof(VO_U32) );
					pTemp += sizeof(VO_U32);
					memset(pTemp, 2, 1);//ID 2 = append data
					pTemp += 1;

					*pDRMData->pnDstSize = pDRMData->nSize + sizeof(VO_U32) * 2 + 2 + uNeedSize;
					//VOLOGI("uTrackId %d *pDRMData->pnDstSize %d uNeedSize %d", pInfo->uTrackId, *pDRMData->pnDstSize, uNeedSize);

					//{
					//	VO_BYTE* temptemp = new VO_BYTE[*pDRMData->pnDstSize];
					//	memcpy(temptemp, pDRMData->pData, *pDRMData->pnDstSize);

					//	VO_BYTE* pData = NULL;
					//	VO_U32   uDataLen = 0;
					//	SampleEncryption *pEncryptionInfo = NULL;
					//	//uRet = depart(temptemp, *pDRMData->pnDstSize, &pData, &uDataLen, &pAppend, &uAppendLen);
					//	uRet = departWithSampleEncryption(temptemp, *pDRMData->pnDstSize, &pData, &uDataLen, &pEncryptionInfo);

					//	delete[] temptemp;

					//	*pDRMData->pnDstSize = uDataLen;
					//}
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


VO_U32 DRM_Amazon_PlayReady::TriggerInitDataCallbackEvent()
{
	m_bIsDRM = VO_TRUE;
	VO_DRM2_INFO_WITH_RESPONSE s_DRMInfo = {0};
	s_DRMInfo.pData	= m_pDRMHeader;
	s_DRMInfo.uSize	= m_uDRMHeader;
	s_DRMInfo.fAllocate = fAllocate;
	VOLOGR("+CB");
	VO_U32 uRet = m_cbVerify.HTTP_Callback(m_cbVerify.hHandle, VO_SOURCE2_CB_DRM_INITDATA, &s_DRMInfo);
	VOLOGR("-CB");

	if (uRet) {  
		VOLOGE("HTTP_Callback %d", uRet);
		return VO_ERR_DRM2_BADDRMINFO;
	}

	if (s_DRMInfo.uRetSize > 0) {
		// assign custom DRM init data
		if (m_pCustomDRMHeader)
		{
			delete []m_pCustomDRMHeader;
			m_pCustomDRMHeader = NULL;
			m_uCustomDRMHeader = 0;
		}

		m_pCustomDRMHeader = s_DRMInfo.pRetData;
		m_uCustomDRMHeader = s_DRMInfo.uRetSize;

		VOLOGI("s_DRMInfo.uCustomSize %d", s_DRMInfo.uRetSize); 
	}

	return uRet;
}

