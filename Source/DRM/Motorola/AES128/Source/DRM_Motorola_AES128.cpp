#include "DRM_Motorola_AES128.h"
#include "voHLSDRM.h"
#include "voLog.h"

#ifndef LOG_TAG
#define LOG_TAG "CDRM_Motorola_AES128"
#endif


CDRM_Motorola_AES128::CDRM_Motorola_AES128(void)
: m_pAPI(NULL)
, m_hDRM(NULL)
{
	VOLOGR("CDRM_Motorola_AES128");

	memset(m_PreKeyURI, 0, 2048);
}

CDRM_Motorola_AES128::~CDRM_Motorola_AES128(void)
{
	Uninit();
}

VO_U32 CDRM_Motorola_AES128::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_Motorola_AES128");

	return CDRMStreaming::Init(pParam);
}

VO_U32 CDRM_Motorola_AES128::Uninit()
{
	if (m_hDRM)
	{
		m_pAPI->drm_release(m_hDRM);
		m_hDRM = NULL;
	}

	memset(m_PreKeyURI, 0, 2048);

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM_Motorola_AES128::SetThirdpartyAPI(VO_PTR pParam)
{
	VOLOGI("%p", pParam);

	m_pAPI = (DRM_Callback*)pParam;

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM_Motorola_AES128::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
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
			Uninit();

			if (m_pAPI)
				m_hDRM = m_pAPI->drm_init(MOTOHLS);

			if (!m_hDRM)
				return VO_ERR_OUTOF_MEMORY;
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
					VOLOGR("szCurURL %s", pDRMHLSProcess->szCurURL);
					VOLOGR("szKeyString %s", pDRMHLSProcess->szKeyString);
					VOLOGR("uSequenceNum %d", pDRMHLSProcess->uSequenceNum);

					m_uLen = 0;

					VO_CHAR szKeyURI[2048] = {0};
					VO_U32 uRet = getKeyURI_HLS(pDRMHLSProcess->szCurURL, pDRMHLSProcess->szKeyString, szKeyURI, NULL);
					if (0 != uRet) {
						VOLOGE("!getKeyURI 0x%x", uRet);
						return uRet;
					}

					if ( !IsEncrypted() )
						break;

					VO_BYTE aIV[16] = {0};
					uRet = getIV_HLS(pDRMHLSProcess->szKeyString, pDRMHLSProcess->uSequenceNum, aIV, NULL);
					if (0 != uRet) {
						VOLOGE("!getIV 0x%x", uRet);
						return uRet;
					}
					VOLOGI("iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", aIV[0], aIV[1], aIV[2], aIV[3], aIV[4], aIV[5], aIV[6], aIV[7], aIV[8], aIV[9], aIV[10], aIV[11], aIV[12], aIV[13], aIV[14], aIV[15]);

					int iRet = 0;
					if (strcmp(m_PreKeyURI, szKeyURI) == 0)
					{
						iRet = m_pAPI->drm_setkey(m_hDRM, AES128, NULL, aIV);
					}
					else
					{
						VOLOGI("input URL %s", szKeyURI);
						iRet = m_pAPI->drm_setkey(m_hDRM, AES128, szKeyURI, aIV);

						memset(m_PreKeyURI, 0, 2048);
						strcpy(m_PreKeyURI, szKeyURI);
					}
					if (iRet != SUCCESS)
					{
						VOLOGE("!drm_setkey 0x%x", iRet);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
				{
					if ( !IsEncrypted() )
						break;

					if (m_bExit)
					{
						VOLOGW("exiting");

						return VO_ERR_DRM2_ERRORSTATE;
					}

					if (pDRMData->nSize == 0)
						return VO_ERR_DRM2_OK;

					VO_U32 dec_size = 0;

					if (m_uLen + pDRMData->nSize > 752)
					{
						VO_BYTE temp[752] = {0};

						VO_U32 iTotalSize = m_uLen + pDRMData->nSize;
						VO_U32 iNeedDecrypt = (iTotalSize / 752) * 752;
						VO_U32 iLeftSize = iTotalSize - iNeedDecrypt;

						if (0 == iLeftSize)
						{
							iLeftSize = 752;
							iNeedDecrypt -= 752;
						}

						memcpy(temp, pDRMData->pData + pDRMData->nSize - iLeftSize, iLeftSize);

						VO_U32 uLen = 752 - m_uLen;
						if (uLen)
							memcpy(m_Buf + m_uLen, pDRMData->pData, uLen);

						int iDstSize = 0;
						VOLOGI("input handle %p, inputBuf %p, inputLen 752", m_hDRM, m_Buf);
						int iRet = m_pAPI->drm_decrypt(m_hDRM, m_Buf, 752, m_Buf, &iDstSize, false);
						if (SUCCESS != iRet) {
							VOLOGE("drm_decrypt 0x%x", iRet);
							return VO_ERR_DRM2_BADDRMDATA;
						}
						VOLOGI("out Size %d", iDstSize);
						dec_size += 752;


						if (pDRMData->nSize - uLen - iLeftSize > 0)
						{
							iDstSize = 0;
							VOLOGI( "input handle %p, inputLen %d", m_hDRM, (pDRMData->nSize - uLen - iLeftSize) );
							iRet = m_pAPI->drm_decrypt(m_hDRM, pDRMData->pData + uLen, pDRMData->nSize - uLen - iLeftSize, pDRMData->pData + uLen, &iDstSize, false);
							if (SUCCESS != iRet) {
								VOLOGE("drm_decrypt 0x%x", iRet);
								return VO_ERR_DRM2_BADDRMDATA;
							}
							VOLOGI("out Size %d", iDstSize);
							dec_size = dec_size + pDRMData->nSize - uLen - iLeftSize;
						}

						memmove(pDRMData->pData + 752, pDRMData->pData + uLen, pDRMData->nSize - uLen - iLeftSize);

						memcpy(pDRMData->pData, m_Buf, 752);

						memcpy(m_Buf, temp, iLeftSize);
						m_uLen = iLeftSize;
					}
					else
					{
						memcpy(m_Buf + m_uLen, pDRMData->pData, pDRMData->nSize);
						m_uLen += pDRMData->nSize;
					}

					if (1 == pDRMData->nReserved[0])
					{
						if (m_uLen % 16)
						{
							m_uLen = 0;
							VOLOGW("m_uLen %d", m_uLen);
							return VO_ERR_DRM2_BADDRMDATA;
						}

						int iDstSize = 0;
						VOLOGI("input handle %p, inputBuf %p, inputLen %d, last block", m_hDRM, m_Buf, m_uLen);
						int iRet = m_pAPI->drm_decrypt(m_hDRM, m_Buf, m_uLen, m_Buf, &iDstSize, true);
						if (SUCCESS != iRet) {
							VOLOGE("drm_decrypt 0x%x", iRet);
							return VO_ERR_DRM2_BADDRMDATA;
						}
						VOLOGI("out Size %d", iDstSize);

						if (0 == m_Buf[m_uLen - 1])
						{
							VOLOGW("last: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", m_Buf[m_uLen - 16], m_Buf[m_uLen - 15], m_Buf[m_uLen - 14], m_Buf[m_uLen - 13], m_Buf[m_uLen - 12], m_Buf[m_uLen - 11], m_Buf[m_uLen - 10], m_Buf[m_uLen - 9], m_Buf[m_uLen - 8], m_Buf[m_uLen - 7], m_Buf[m_uLen - 6], m_Buf[m_uLen - 5], m_Buf[m_uLen - 4], m_Buf[m_uLen - 3], m_Buf[m_uLen - 2], m_Buf[m_uLen - 1]);
						}
						else
						{
							m_uLen = m_uLen - m_Buf[m_uLen - 1];
						}

						if (m_uLen > 0 && m_uLen <= 752)
						{
							memcpy(pDRMData->pData + dec_size, m_Buf, m_uLen);
							dec_size += m_uLen; 
						}
						else
						{
							VOLOGW("Size is %d", m_uLen);
						}	

						m_uLen = 0;
					}

					*(pDRMData->pnDstSize) = dec_size;
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_END:
				{
					m_uLen = 0;
				}
				break;

			default:
				return VO_ERR_NOT_IMPLEMENT;
			}

			if (pDRMData->pnDstSize && *(pDRMData->pnDstSize) > 0 && 0x47 != pDRMData->pData[0])
			{
				VOLOGE("0x%X", pDRMData->pData[0]);
			}
		}
		break;

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}
