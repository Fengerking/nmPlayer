#include "DRMAdapter.h"
#include "voToolUtility.h"
#include "voPlayReady.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_U32 CheckEncrypted_SSTR(VO_CHAR *pManifest, VO_U32 uManifest)
{
	if (NULL == pManifest)
	{
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	VO_CHAR	*pTemp	= pManifest;
	VO_U32	uTemp	= uManifest;

	VO_BOOL bNeedDel = VO_FALSE;
	if (0xff == (VO_BYTE)pManifest[0] && 0xfe == (VO_BYTE)pManifest[1])
	{
		VOLOGR("convert characters code");

		bNeedDel = VO_TRUE;

#if defined _WIN32
		uTemp = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pManifest, -1, NULL, NULL, NULL, NULL);

		pTemp = new VO_CHAR[uTemp + 1];
		if (NULL == pTemp) {
			VOLOGE("can't get memory of %d", uTemp + 1);
			return VO_ERR_OUTOF_MEMORY;
		}

		memset(pTemp, 0, uTemp + 1);

		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)pManifest, -1, pTemp, uTemp, NULL, NULL);
#else
		pTemp = new VO_CHAR[uTemp + 1];
		if (NULL == pTemp) {
			VOLOGE("can't get memory of %d", uTemp + 1);
			return VO_ERR_OUTOF_MEMORY;
		}

		memset(pTemp, 0, uTemp + 1);

		VO_UnicodeToUTF8( (VO_U16 *)pManifest, vowcslen( (VO_U16 *)pManifest ), pTemp, uTemp );
#endif
	}

	VO_U32 uRet = VO_ERR_DRM2_NOTENCRYPTION;
	VO_CHAR *pStrart = strstr(pTemp, "<ProtectionHeader");
	if (pStrart)
	{
		pStrart = strstr(pStrart, "SystemID=\"");
		if (pStrart)
		{
			pStrart += strlen("SystemID=\"");
			while (*pStrart < '0' || *pStrart > '9')
				pStrart++;

#if defined _WIN32
			if (_strnicmp(pStrart, "9a04f079-9840-4286-ab92-e65be0885f95", strlen("9a04f079-9840-4286-ab92-e65be0885f95") ) == 0)
#else
			if (strncasecmp(pStrart, "9a04f079-9840-4286-ab92-e65be0885f95", strlen("9a04f079-9840-4286-ab92-e65be0885f95") ) == 0)
#endif
			{
				uRet = VO_ERR_DRM2_OK;
			}
		}
	}

	if (bNeedDel && pTemp)
		delete []pTemp;

	return uRet;
}

VO_OSMP_DRM_SOURCE_TYPE transfer(VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	switch (eSourceFormat)
	{
	case VO_DRM2SRC_MEDIAFORMAT:
		{
			return VO_OSMP_DRM_SOURCE_FR;
		}
		break;

	case VO_DRM2SRC_CHUNK:
		{
			return VO_OSMP_DRM_SOURCE_AS;
		}
		break;

	case VO_DRM2SRC_CHUNK_HLS:
		{
			return VO_OSMP_DRM_SOURCE_HLS;
		}
		break;

	case VO_DRM2SRC_CHUNK_SSTR:
		{
			return VO_OSMP_DRM_SOURCE_SSTR;
		}
		break;

	case VO_DRM2SRC_CHUNK_DASH:
		{
			return VO_OSMP_DRM_SOURCE_DASH;
		}
		break;

	case VO_DRM2SRC_RAWFILE:
		{
			return VO_OSMP_DRM_SOURCE_FILE;
		}
		break;

	default:
		break;
	}

	return VO_OSMP_DRM_SOURCE_UNKNOWN;
}

#ifdef _VONAMESPACE
}
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


DRMAdapter::DRMAdapter(void)
: m_bNeedDecrypt(VO_FALSE)
, m_pDRMAPIs(NULL)
{
	VOLOGR("DRMAdapter");
}

DRMAdapter::~DRMAdapter(void)
{
	Uninit();
}

VO_U32 DRMAdapter::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRMAdapter");

	return CDRM::Init(pParam);
}

VO_U32 DRMAdapter::Uninit()
{
	if (m_pDRMAPIs)
		m_pDRMAPIs->destroy();

	return VO_ERR_DRM2_OK;
}


VO_U32 DRMAdapter::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_DRM2_EXIT:
		{
			if (NULL == pParam) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			VO_BOOL* bExit = static_cast<VO_BOOL*>(pParam);
			if (VO_TRUE == *bExit && m_pDRMAPIs)
			{
				m_pDRMAPIs->destroy();
			}
		}
		break;

	case VO_PID_DRM2_VODRM_COMMON_API:
		{
			if (NULL == pParam) {
				VOLOGW("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			m_pDRMAPIs = (VOOSMPDRM*)pParam;

			return VO_ERR_DRM2_OK;
		}
		break;

	default:
		break;
	}

	return CDRM::SetParameter(uID, pParam);
}


VO_U32 DRMAdapter::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	//hls只有具体到每个additionalinfo才能判断加密
	//ISS先判断
	if (NULL == m_pDRMAPIs)
	{
		VOLOGE("No engine API");
		return VO_ERR_DRM2_NO_DRM_API;
	}

	if (VO_DRM2SRC_CHUNK_DASH == eSourceFormat)
		return VO_ERR_DRM2_OK;//unsupport

	switch (nFlag)
	{
	case VO_DRM_FLAG_CONVERTURL:
		{
			if (VO_DRM2SRC_CHUNK != eSourceFormat)
				return VO_ERR_DRM2_BADPARAMETER;

			VO_DRM2_CONVERT_URL* pCU = static_cast<VO_DRM2_CONVERT_URL *>(pParam);
			if (NULL == pParam || NULL == pCU->urlSrc || NULL == pCU->urlDes) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			int iRet = m_pDRMAPIs->init();
			if (iRet)
			{
				VOLOGE("init 0x%x", iRet);
				return iRet;
			}

			int iDesURI = 2048; 
			return m_pDRMAPIs->processURI(transfer(eSourceFormat), pCU->urlSrc, pCU->urlDes, &iDesURI);
		}
		break;

	case VO_DRM_FLAG_PLAYLIST:
		{
			VO_DRM2_CONVERT_PLAYLIST* pCP = static_cast<VO_DRM2_CONVERT_PLAYLIST*>(pParam);
			if (NULL == pParam || NULL == pCP->pPlaylist || NULL == pCP->puSizePlaylist) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			VOLOGI("Playlsit %p %d", pCP->pPlaylist, *pCP->puSizePlaylist);
			int iDesPlaylist = pCP->uSizeBuffer;
			int iRet = m_pDRMAPIs->processPlaylist(transfer(eSourceFormat), (char*)pCP->pPlaylist, *pCP->puSizePlaylist, (char*)pCP->pPlaylist, &iDesPlaylist);
			if (0 != iRet)
			{
				VOLOGW("processPlaylist 0x%x", iRet);
			}
			else if (VO_OSMP_DRM_ERR_NOT_MODIFIED == iRet)
			{
				*pCP->puSizePlaylist = iDesPlaylist;
			}

			return iRet;
		}
		break;

	case VO_DRM_FLAG_DRMINFO:
		{
			VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);
			if (VO_DRM2SRC_CHUNK_SSTR == eSourceFormat)
			{
				int iRet = CheckEncrypted_SSTR((VO_CHAR*)pDRMInfo->pManifestData, pDRMInfo->uSizeManifest);
				if (VO_ERR_DRM2_NOTENCRYPTION == iRet)
				{
					m_bNeedDecrypt = VO_FALSE;
					break;
				}
				else if (VO_ERR_DRM2_OK == iRet)
				{
					m_bNeedDecrypt = VO_TRUE;
				}
				else
				{
					return VO_ERR_DRM2_BADPARAMETER;
				}
			}

			int iRet = m_pDRMAPIs->processHeader(transfer(eSourceFormat), pDRMInfo->pManifestData, pDRMInfo->uSizeManifest);
			if (0 != iRet)
			{
				VOLOGE("ProcessHeaderInfo 0x%x", iRet);
				return iRet;
			}
		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			VO_DRM2_DATA * pDRMData = static_cast<VO_DRM2_DATA *>(pParam);

			if (VO_DRM2SRC_CHUNK_SSTR == eSourceFormat && VO_FALSE == m_bNeedDecrypt)
				return VO_ERR_DRM2_OK;

			switch (pDRMData->sDataInfo.nDataType)
			{
			case VO_DRM2DATATYPE_CHUNK_BEGIN:
				{
					return m_pDRMAPIs->beginSegment(pDRMData->nReserved[2], pDRMData->sDataInfo.pInfo);
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
			case VO_DRM2DATATYPE_RAWFILE:
				{
					if (NULL == pParam) {
						VOLOGE("VO_DRM2_DATA is NULL");
						return VO_ERR_DRM2_BADPARAMETER;
					}

					VO_U64 ullChunkOffset = *((VO_U64*)pDRMData->nReserved[1]);
					*pDRMData->pnDstSize = pDRMData->nSize;
					return m_pDRMAPIs->decryptSegment(pDRMData->nReserved[2], ullChunkOffset, pDRMData->pData, reinterpret_cast<int*>(pDRMData->pnDstSize), pDRMData->nReserved[0] ? true : false, pDRMData->sDataInfo.pInfo);
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_END:
				{
					return m_pDRMAPIs->endSegment(pDRMData->nReserved[2]);
				}
				break;

			case VO_DRM2DATATYPE_MEDIASAMPLE:
				{
					if (VO_DRM2SRC_CHUNK_SSTR == eSourceFormat)
					{
						VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO * pInfo= static_cast<VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO*>(pDRMData->sDataInfo.pInfo);

						VO_OSMP_DRM_PIFF_INFO s_DRM_PIFF_Info = {0};
						s_DRM_PIFF_Info.pSampleEncryptionBox	= pInfo->pSampleEncryptionBox;
						s_DRM_PIFF_Info.iSampleEncryptionBox	= pInfo->uSampleEncryptionBoxSize;
						s_DRM_PIFF_Info.iSampleIndex			= pInfo->uSampleIndex;
						s_DRM_PIFF_Info.pTrackEncryptionBox		= NULL;
						s_DRM_PIFF_Info.iTrackEncryptionBox		= 0;
						s_DRM_PIFF_Info.iTrackID				= pInfo->uTrackId;

						return m_pDRMAPIs->decryptSample(0, pDRMData->pData, reinterpret_cast<int*>(&pDRMData->nSize), &s_DRM_PIFF_Info);
					}
					else
					{
						return m_pDRMAPIs->decryptSample(0, pDRMData->pData, reinterpret_cast<int*>(&pDRMData->nSize), pDRMData->sDataInfo.pInfo);
					}
				}
				break;

			default:
				return VO_ERR_NOT_IMPLEMENT;
			}
		}
		break;

	case VO_DRM_FLAG_GETINFOLOCATION:
	case VO_DRM_FLAG_CONVERTLOCATION:
	case VO_DRM_FLAG_GETORIGINALSIZE:
	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}

