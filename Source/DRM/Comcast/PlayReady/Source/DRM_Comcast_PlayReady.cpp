#include "DRM_Comcast_PlayReady.h"
#include "voHLSDRM.h"
#include "acm_drm_stub.h"
#include "voLog.h"

CDRM_Comcast_PlayReady::CDRM_Comcast_PlayReady(void)
: m_pHandle(NULL)
, m_uSizeFullHeader(0)
{
	VOLOGI("CDRM_CV_PR");
}

CDRM_Comcast_PlayReady::~CDRM_Comcast_PlayReady(void)
{
}

VO_U32 CDRM_Comcast_PlayReady::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_CableVision_PlayReady");

	return CDRM::Init(pParam);
}

VO_U32 CDRM_Comcast_PlayReady::Uninit()
{
	if (m_pHandle)
	{
		VOLOGI("TerminatePlayback");
		int iRet = ACM_DRM_TerminatePlayback(m_pHandle);
		if (iRet) {
			VOLOGE("TerminatePlayback failed %x", iRet);
			return VO_ERR_DRM2_BASE;
		}
	}

	return VO_ERR_DRM2_OK;
}


VO_U32 CDRM_Comcast_PlayReady::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
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
	case VO_DRM_FLAG_GETINFOLOCATION:
		{
			VO_DRM2_INFO_GET_LOCATION* pInfo_Q = static_cast<VO_DRM2_INFO_GET_LOCATION*>(pParam);

			if (0 == m_uSizeFullHeader)
			{
				VO_CHAR szLocalURl[2048] = {0};
				memcpy( szLocalURl, "file://", sizeof("file://") );
				strcat(szLocalURl, pInfo_Q->pURI);

				VOLOGI("+Init : URI %s", szLocalURl);
				int iRet = ACM_DRM_Init(szLocalURl, &m_pHandle);
				VOLOGI("-Init : URI %s", szLocalURl);
				if (iRet) {
					VOLOGE("DRM_Init %x", iRet);
					return VO_ERR_DRM2_BADDRMINFO;
				}

				unsigned uSizeShortHeader = 0;
				iRet = ACM_DRM_GetMinimumHeaderSize(m_pHandle, &uSizeShortHeader);
				if (iRet) {
					VOLOGE("DRM_GetMinimumHeaderSize 0x%08x", iRet);
					return VO_ERR_DRM2_BADDRMINFO;
				}

				*pInfo_Q->puSize	= uSizeShortHeader;
				*pInfo_Q->pullPos	= 0;
			} 
			else
			{
				*pInfo_Q->puSize	= m_uSizeFullHeader;
				*pInfo_Q->pullPos	= 0;
			}		
		}
		break;

	case VO_DRM_FLAG_DRMINFO:
		{
			switch (eSourceFormat)
			{
			case VO_DRM2SRC_RAWFILE:
				{
					VO_DRM2_INFO* pDRMInfo = static_cast<VO_DRM2_INFO *>(pParam);
					VO_DRM2_INFO_GENERAL_DATA* pIOInfo = static_cast<VO_DRM2_INFO_GENERAL_DATA *>(pDRMInfo->pDrmInfo);

					if (0 == m_uSizeFullHeader)
					{
						VOLOGI("+GetFHeaderSize : Data[0] 0x%08x, Size %d", pIOInfo->pData[0], pIOInfo->uSize);
						int iRet = ACM_DRM_GetFullHeaderSize(m_pHandle, pIOInfo->pData, pIOInfo->uSize, &m_uSizeFullHeader);
						VOLOGI("-GetFHeaderSize : Data[0] 0x%08x, Size %d", pIOInfo->pData[0], pIOInfo->uSize);
						if (iRet)
						{
							ACM_DRM_TerminatePlayback(m_pHandle);
							m_pHandle = NULL;

							if (pIOInfo->pData[0] == 0x50 &&
								pIOInfo->pData[1] == 0x52 &&
								pIOInfo->pData[2] == 0x45 &&
								pIOInfo->pData[3] == 0x07)
							{
								VOLOGE("DRM_GetFullHeaderSize 0x%08x", iRet);
								return VO_ERR_DRM2_BADDRMINFO;
							}
							else
							{
								VOLOGI("Should not ENVELOPE 0x%08x 0x%08x 0x%08x 0x%08x", pIOInfo->pData[0], pIOInfo->pData[1], pIOInfo->pData[2], pIOInfo->pData[3]);

								return VO_ERR_DRM2_NOTENCRYPTION;
							}
						}

						return VO_ERR_DRM2_NEEDRETRY;
					}
					else
					{
						VOLOGI("+ProcessFHeaderSize : Data[0] 0x%08x, Size %d", pIOInfo->pData[0], pIOInfo->uSize);
						int iRet = ACM_DRM_ProcessFullHeader(m_pHandle, pIOInfo->pData, pIOInfo->uSize);
						VOLOGI("-ProcessFHeaderSize : Data[0] 0x%08x, Size %d", pIOInfo->pData[0], pIOInfo->uSize);
						if (iRet)
						{
							VOLOGE("DRM_ProcessFullHeader %x", iRet);

							ACM_DRM_TerminatePlayback(m_pHandle);
							m_pHandle = NULL;

							return VO_ERR_DRM2_BADDRMINFO;
						}
					}
				}
				break;

			case VO_DRM2SRC_CHUNK_HLS:
				{
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

			default:
				return VO_ERR_NOT_IMPLEMENT;
			}
		}
		break;

	case VO_DRM_FLAG_CONVERTLOCATION:
		{
			VO_DRM2_DATA_CONVERT_LOCATION* pInfo_Q = static_cast<VO_DRM2_DATA_CONVERT_LOCATION*>(pParam);

			*pInfo_Q->pullActualPos	+= pInfo_Q->ullWantedPos + m_uSizeFullHeader;
			*pInfo_Q->puActualSize	= pInfo_Q->uWantedSize;
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
					if (0 == pDRMData->nSize)
						return VO_ERR_DRM2_OK;

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

			case VO_DRM2DATATYPE_RAWFILE:
				{
					unsigned long long* pull = (unsigned long long*)pDRMData->nReserved[1];
					VOLOGI("+DecryptPart : offset %llu, Data[0] 0x%08x, Size %d", *pull - m_uSizeFullHeader, pDRMData->pData[0], pDRMData->nSize);
					int iRet = ACM_DRM_DecryptWithOffset(m_pHandle, *pull - m_uSizeFullHeader, pDRMData->pData, pDRMData->nSize);
					VOLOGI("-DecryptPart : offset %llu, Data[0] 0x%08x, Size %d", *pull - m_uSizeFullHeader, pDRMData->pData[0], pDRMData->nSize);
					if (iRet)
					{
						VOLOGE("DRM_DecryptWithOffset %x", iRet);
						return VO_ERR_DRM2_BADDRMDATA;
					}

					*pDRMData->ppDstData	= pDRMData->pData;
					*pDRMData->pnDstSize	= pDRMData->nSize;
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
