#include "VO_AES128_SingleInstance.h"
#include "CSourceIOUtility.h"
#include "voHLSDRM.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "CAES128"
#endif

#define CACHEBUFFERSIZE 188*4


CVO_AES128_SingleInstance::CVO_AES128_SingleInstance(void)
: m_pAPI_IO(NULL)
, m_uLen(0)
{
	VOLOGR("CVO_AES128_SingleInstance");

	memset(m_PreKeyURI, 0, 2048);
}

CVO_AES128_SingleInstance::~CVO_AES128_SingleInstance(void)
{
	Uninit();
}

VO_U32 CVO_AES128_SingleInstance::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "VO_AES128_SingleInstance");

	return CDRMStreaming::Init(pParam);
}

VO_U32 CVO_AES128_SingleInstance::Uninit()
{
	m_bExit = VO_TRUE;

	return VO_ERR_DRM2_OK;
}

VO_U32 CVO_AES128_SingleInstance::getKey(VO_CHAR* pURLKey, VO_BYTE* pKey, VO_U32* puKey)
{
	if (NULL == pURLKey || NULL == pKey || NULL == puKey) {
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	VO_HANDLE h = NULL;
	voAutoIOInit init_obj(m_pAPI_IO, pURLKey, &h);
	voAutoIOOpen open_obj(m_pAPI_IO, h, VO_TRUE);

	VO_U32 uRet = ReadFixedSize(m_pAPI_IO, h, pKey, puKey, &m_bExit);
	if (0 != uRet) {
		VOLOGE("!IO ReadFixedSize 0x%x", uRet);
	}

	return uRet;
}

VO_U32 CVO_AES128_SingleInstance::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	switch (eSourceFormat)
	{
	case VO_DRM2SRC_CHUNK_HLS:
		break;

	case VO_DRM2SRC_CHUNK_SSTR:
	case VO_DRM2SRC_CHUNK_DASH:
		return 0; //TOTO

	default:
		{
			VOLOGW("!DRM Module");
			return VO_ERR_DRM2_MODULENOTFOUND;
		}
	}

	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			if (NULL == pParam) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);
			m_pAPI_IO = pDRMInfo->pAPI_IO;

			m_bExit = VO_FALSE;
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
				{
					VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pDRMData->sDataInfo.pInfo);
					if (NULL == pDRMHLSProcess) {
						VOLOGE("VO_DRM2_HSL_PROCESS_INFO is NULL");
						return VO_ERR_DRM2_BADPARAMETER;
					}

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

					if (strcmp(szKeyURI, m_PreKeyURI) != 0)
					{
						VO_BYTE aKey[16] = {0};
						VO_U32 uSize = 16;
						uRet = getKey(szKeyURI, aKey, &uSize);
						if (0 != uRet) {
							VOLOGE("!getKey 0x%x", uRet);
							return uRet;
						}
						else if (16 != uSize)
						{
							VOLOGW("key length %d", uSize);
							return VO_ERR_DRM2_ERRORSTATE;
						}

						strcpy(m_PreKeyURI, szKeyURI);

						VOLOGR("key: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X", aKey[0], aKey[1], aKey[2], aKey[3], aKey[4], aKey[5], aKey[6], aKey[7], aKey[8], aKey[9], aKey[10], aKey[11], aKey[12], aKey[13], aKey[14], aKey[15]);
						uRet = setKey(aKey, uSize);
						if (uRet) {
							VOLOGE("!setKey");
							return VO_ERR_DRM2_BADPARAMETER;
						}
					}

					VO_BYTE aIV[16] = {0};
					uRet = getIV_HLS(pDRMHLSProcess->szKeyString, pDRMHLSProcess->uSequenceNum, aIV, NULL);
					if (0 != uRet) {
						VOLOGE("!getIV 0x%x", uRet);
						return uRet;
					}

					VOLOGR("iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", aIV[0], aIV[1], aIV[2], aIV[3], aIV[4], aIV[5], aIV[6], aIV[7], aIV[8], aIV[9], aIV[10], aIV[11], aIV[12], aIV[13], aIV[14], aIV[15]);
					uRet = setIV(aIV, 16);
					if (uRet) {
						VOLOGE("!setIV");
						return VO_ERR_DRM2_BADPARAMETER;
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
						return 0;

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

						VO_U32 uSizeOutput = 0;
						VO_U32 uRet = decryptData(m_Buf, 752, m_Buf, &uSizeOutput, VO_FALSE);
						if (0 != uRet) {
							VOLOGE("!decryptData 0x%x", uRet);
							return uRet;
						}
						dec_size += uSizeOutput;

						uSizeOutput = 0;
						uRet = decryptData(pDRMData->pData + uLen, pDRMData->nSize - uLen - iLeftSize, pDRMData->pData + uLen, &uSizeOutput, VO_FALSE);
						if (0 != uRet) {
							VOLOGE("!decryptData 0x%x", uRet);
							return uRet;
						}
						dec_size += uSizeOutput;

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
							VOLOGI("m_uLen %d", m_uLen);
							return VO_ERR_DRM2_BADDRMDATA;
						}

						VO_U32 uRet = decryptData(m_Buf, m_uLen, m_Buf, &m_uLen, VO_TRUE);
						if (0 != uRet) {
							m_uLen = 0;
							VOLOGE("!decryptData 0x%x", uRet);
							return uRet;
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
				VOLOGR("0x%X", pDRMData->pData[0]);
			}
		}
		break;

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}
