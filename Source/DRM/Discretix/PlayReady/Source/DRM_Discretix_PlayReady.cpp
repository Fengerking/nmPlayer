#include "DRM_Discretix_PlayReady.h"
#include "voPlayReady.h"
#include "voHLSDRM.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "DRM_Discretix_PlayReady"
#endif

DiscretixDRMAPI DRM_Discretix_PlayReady::m_API = {0};

extern "C"
{
	int drm1_registerContextInitializerCallback(
		DrmContextInitializerFunc			func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.Init			= func;
		DRM_Discretix_PlayReady::m_API.pUserData	= userData;

		return 0;
	}


	int drm1_registerContextDestroyCallback(
		DrmContextDestroyFunc				func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.Destroy		= func;
		DRM_Discretix_PlayReady::m_API.pUserData	= userData;

		return 0;
	}

	int drm1_registerPiffFragmentSampleDecryptionCallback(
		DrmPiffFragmentSampleDecryptionFunc func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.PiffSampleDecrypt	= func;
		DRM_Discretix_PlayReady::m_API.pUserData			= userData;

		return 0;
	}


	int drm1_registerPiffFragmentDecryptionCallback(
		DrmPiffFragmentDecryptionFunc		func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.PiffFragmentDecrypt	= func;
		DRM_Discretix_PlayReady::m_API.pUserData			= userData;

		return 0;
	}

	int drm1_registerDrmHlsPayloadDecryptionCallback(
		DrmHlsPayloadDecryptionFunc			func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.HLSDecrypt	= func;
		DRM_Discretix_PlayReady::m_API.pUserData	= userData;

		return 0;
	}

	int drm1_registerHlsFormatConvertorToDx_3_0Callback(
		HlsFormatConvertorToDx_3_0Func		func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.Convertor	= func;
		//DRM_Discretix_PlayReady::m_API.pUserData	= userData;

		return 0;
	}

	int drm1_registerDestroyPlaylistCallback(
		DestroyPlaylistFunc					func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.ReleasePlaylist	= func;
		//DRM_Discretix_PlayReady::m_API.pUserData		= userData;

		return 0;
	}

	int drm1_registerDrmEnvelopePayloadDecryptionCallback(
		DrmEnvelopePayloadDecryptionFunc    func,
		void*								userData)
	{
		VOLOGI("%p",func);

		DRM_Discretix_PlayReady::m_API.EnvelopeDecrypt	= func;
		DRM_Discretix_PlayReady::m_API.pUserData		= userData;

		return 0;
	}
};


DRM_Discretix_PlayReady::DRM_Discretix_PlayReady(void)
: m_hDRM(NULL)
, m_eHLS_DRM(HLS_DRM_UNKNOW)
{
	VOLOGR("DRM_Discretix_PlayReady");

	memset(&m_Harmonics, 0, sizeof(DXHarmonic) * 3);
	m_Harmonics[0].uID = 0xFFFFFFFF;
	m_Harmonics[1].uID = 0xFFFFFFFF;
	m_Harmonics[2].uID = 0xFFFFFFFF;
}

DRM_Discretix_PlayReady::~DRM_Discretix_PlayReady(void)
{
	Uninit();
}

VO_U32 DRM_Discretix_PlayReady::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_Discretix_PlayReady");

	return CDRMStreaming::Init(pParam);
}

VO_U32 DRM_Discretix_PlayReady::Uninit()
{
	VOLOGI("Uninit");
	
	for (int i = 0; i < 3; i++)
	{
		if (m_Harmonics[i].hDRM)
		{
			m_API.Destroy(&m_Harmonics[i].hDRM, m_API.pUserData);
			m_Harmonics[i].hDRM	= NULL;
		}

		m_Harmonics[i].uID	= 0xFFFFFFFF;
		m_Harmonics[i].uOffsetHamonic	= 0;
	}

	if (m_hDRM)
	{
		m_API.Destroy(&m_hDRM, m_API.pUserData);
		m_hDRM = NULL;
	}

	m_eHLS_DRM = HLS_DRM_UNKNOW;

	return VO_ERR_DRM2_OK;
}

VO_U32 DRM_Discretix_PlayReady::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	VOLOGI("uID %x", uID);
	
	if (VO_PID_DRM2_RUNNING == uID)
	{
		VO_BOOL* b = static_cast<VO_BOOL*>(pParam);
		if (*b == VO_FALSE)
		{
			VOLOGI("Close");
			Uninit();
		}
	}

	return CDRMStreaming::SetParameter(uID, pParam);
}

VO_U32 DRM_Discretix_PlayReady::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	if (VO_DRM2SRC_CHUNK_DASH == eSourceFormat)
		return 0; //TOTO

	switch (nFlag)
	{
	case VO_DRM_FLAG_NEWAPI:
		{
			VOLOGI("VO_DRM_FLAG_QUERYAPI");
			GetInternalAPI((VO_PTR*)pParam);
		}
		break;

	case VO_DRM_FLAG_GETINFOLOCATION:
		{
			VO_DRM2_INFO_GET_LOCATION* pInfo_Q = static_cast<VO_DRM2_INFO_GET_LOCATION*>(pParam);
			if (NULL == pParam || NULL == pInfo_Q->puSize || NULL == pInfo_Q->pullPos) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			if (0xFFFFFFFF == m_Harmonics[0].uID)
			{
				Uninit();

				m_Harmonics[0].uID = 1;

				*pInfo_Q->puSize	= 12;
				*pInfo_Q->pullPos	= 0;
			} 
			else
			{
				*pInfo_Q->puSize	= m_Harmonics[0].uOffsetHamonic;
				*pInfo_Q->pullPos	= 0;
			}
		}
		break;

	case VO_DRM_FLAG_PLAYLIST:
		{
			VO_DRM2_CONVERT_PLAYLIST* pCP = static_cast<VO_DRM2_CONVERT_PLAYLIST*>(pParam);
			if (NULL == pParam || NULL == pCP->pPlaylist || NULL == pCP->puSizePlaylist) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			if (VO_DRM2SRC_CHUNK_HLS == eSourceFormat)
			{
				unsigned char *pConvert	= NULL;
				unsigned int iConvert	= 0;

				VOLOGI("Playlsit %p %d", pCP->pPlaylist, *pCP->puSizePlaylist);
				unsigned uRet = m_API.Convertor(pCP->pPlaylist, *pCP->puSizePlaylist, &pConvert, &iConvert);
				if (uRet)
				{
					VOLOGW("Convertor 0x%x", uRet);
				}
				else if (iConvert > pCP->uSizeBuffer)
				{
					VOLOGW("OUTPUT BUFFER SMALL");

					m_API.ReleasePlaylist(pConvert);

					return VO_ERR_OUTPUT_BUFFER_SMALL;
				}
				else if (iConvert && pConvert)
				{
					memcpy(pCP->pPlaylist, pConvert, iConvert);
					*pCP->puSizePlaylist = iConvert;

					VOLOGI("pConvert %p, iConvert %d", pConvert, iConvert);
					m_API.ReleasePlaylist(pConvert);
				}
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

					if (0 == m_Harmonics[0].uOffsetHamonic)
					{
						if (0x50 == pIOInfo->pData[0] &&
							0x52 == pIOInfo->pData[1] &&
							0x45 == pIOInfo->pData[2] &&
							0x7 == pIOInfo->pData[3])
						{
							memcpy(&m_Harmonics[0].uOffsetHamonic, pIOInfo->pData + 8, 4);

							VOLOGI("PR %d", m_Harmonics[0].uOffsetHamonic);

							return VO_ERR_DRM2_NEEDRETRY;
						}
						else
						{
							return VO_ERR_DRM2_NOTENCRYPTION;
						}
					}
					else
					{
						m_Harmonics[0].uID = 0xFFFFFFFF;

						if (NULL == m_API.Init || NULL == m_API.Destroy || NULL == m_API.EnvelopeDecrypt) {
							VOLOGE("NULL Init");
							return VO_ERR_DRM2_NO_DRM_API;
						}

						if (m_hDRM)
						{
							m_API.Destroy(&m_Harmonics[0].hDRM, m_API.pUserData);
							m_hDRM = NULL;
						}

						VO_U32 uHamonicHeader = 0;
						memcpy(&uHamonicHeader, pIOInfo->pData + 4, 4);

						unsigned uRet = m_API.Init(pIOInfo->pData, uHamonicHeader, DRM_CONTENT_ENVELOPE, NULL, &m_Harmonics[0].hDRM, m_API.pUserData);
						if (uRet) {
							VOLOGE("Init 0x%x", uRet);
							return VO_ERR_DRM2_BADDRMINFO;
						}
					}
				}
				break;

			case VO_DRM2SRC_CHUNK_HLS:
				{
					Uninit();

					VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);
					VOLOGI("Manifest %p %d", pDRMInfo->pManifestData, pDRMInfo->uSizeManifest);

					if (strstr( (char*)pDRMInfo->pManifestData, "#EXT-X-DXPLAYREADY" ) ||
						strstr( (char*)pDRMInfo->pManifestData, "#EXT-X-DXDRM" ) ||
						strstr( (char*)pDRMInfo->pManifestData, "#EXT-X-DXDRMINFO" ) ||
						strstr( (char*)pDRMInfo->pManifestData, "#EXT-X-CISCO-PROT-HEADER" ) )
					{
						if (NULL == m_API.Init || NULL == m_API.Destroy || NULL == m_API.Convertor || NULL == m_API.ReleasePlaylist) {
							VOLOGE("NULL Init");
							return VO_ERR_DRM2_NO_DRM_API;
						}

						unsigned uRet = m_API.Init( (unsigned char *)pDRMInfo->pManifestData, pDRMInfo->uSizeManifest, DRM_CONTENT_HLS, NULL, &m_hDRM, m_API.pUserData );
						if (DRM_SUCCESS == uRet)
						{
							m_eHLS_DRM = HLS_DRM_DX;
						}
						else
						{
							VOLOGE("Init 0x%x", uRet);
							return VO_ERR_DRM2_BADDRMINFO;
						}
					}
				}
				break;

			case VO_DRM2SRC_CHUNK_SSTR:
				{
					Uninit();

					VO_DRM2_INFO_ADAPTIVESTREAMING * pDRMInfo = static_cast<VO_DRM2_INFO_ADAPTIVESTREAMING *>(pParam);
					VOLOGI("Manifest %p %d", pDRMInfo->pManifestData, pDRMInfo->uSizeManifest);

					VO_BYTE *pDrmHeader = NULL;
					VO_U32 nDrmHeader = 0;
					VO_U32 uRet = getProtectionHeader_SSTR((VO_CHAR *)pDRMInfo->pManifestData, pDRMInfo->uSizeManifest, &pDrmHeader, &nDrmHeader, VO_TRUE);
					delete []pDrmHeader;

					if (uRet) {
						VOLOGE("!getProtectionHeader_SSTR");
						return uRet;
					}

					if ( !IsEncrypted() )
						return VO_ERR_DRM2_OK;

					if (NULL == m_API.Init || NULL == m_API.Destroy || NULL == m_API.PiffSampleDecrypt) {
						VOLOGE("NULL Init");
						return VO_ERR_DRM2_NO_DRM_API;
					}

					uRet = m_API.Init( (unsigned char *)pDRMInfo->pManifestData, pDRMInfo->uSizeManifest, DRM_CONTENT_IIS_MANIFEST, NULL, &m_hDRM, m_API.pUserData );
					if (uRet) {
						VOLOGE("Init 0x%x", uRet);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;

			case VO_DRM2SRC_MEDIAFORMAT:
				{
					Uninit();

					VO_DRM2_INFO* pDRMInfo = static_cast<VO_DRM2_INFO *>(pParam);
					VO_DRM2_INFO_GENERAL_DATA* pInfo = static_cast<VO_DRM2_INFO_GENERAL_DATA *>(pDRMInfo->pDrmInfo);

					VO_BYTE* pRPHeadr	= NULL;
					VO_U32 uRPHeadr		= 0;
					VO_U32 uRet = getRPRightsManagementHeader_PIFF(pInfo->pData, pInfo->uSize, &pRPHeadr, &uRPHeadr);
					if (uRet) {
						VOLOGE("!Get PR_RMH 0x%x", uRet);
						return uRet;
					}

					if (NULL == m_API.Init || NULL == m_API.Destroy || NULL == m_API.PiffSampleDecrypt) {
						VOLOGE("NULL Init");
						return VO_ERR_DRM2_NO_DRM_API;
					}

					uRet = m_API.Init(pRPHeadr, uRPHeadr, DRM_CONTENT_PIFF, NULL, &m_hDRM, m_API.pUserData);
					if (uRet) {
						VOLOGE("Init 0x%x", uRet);
						return VO_ERR_DRM2_BADDRMINFO;
					}
				}
				break;
			}
		}
		break;

	case VO_DRM_FLAG_GETORIGINALSIZE:
		{
			if (NULL == pParam) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			VO_U64* pullFileSize = static_cast<VO_U64*>(pParam);

			*pullFileSize -= m_Harmonics[0].uOffsetHamonic;
		}
		break;

	case VO_DRM_FLAG_CONVERTLOCATION:
		{
			VO_DRM2_DATA_CONVERT_LOCATION* pInfo_Q = static_cast<VO_DRM2_DATA_CONVERT_LOCATION*>(pParam);
			if (NULL == pParam || NULL == pInfo_Q->pullActualPos || NULL == pInfo_Q->puActualSize) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			*pInfo_Q->pullActualPos	+= pInfo_Q->ullWantedPos + m_Harmonics[0].uOffsetHamonic;
			*pInfo_Q->puActualSize	= pInfo_Q->uWantedSize;
		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			VO_DRM2_DATA * pDRMData = static_cast<VO_DRM2_DATA *>(pParam);

			switch (pDRMData->sDataInfo.nDataType)
			{
			case VO_DRM2DATATYPE_CHUNK_BEGIN:
				break;

			case VO_DRM2DATATYPE_CHUNK_PROCESSING:
				{
					if (VO_DRM2SRC_CHUNK_HLS != eSourceFormat || 0 == pDRMData->nSize)
						break;

					if (NULL == pParam) {
						VOLOGE("VO_DRM2_DATA is NULL");
						return VO_ERR_DRM2_BADPARAMETER;
					}

					VO_DRM2_HSL_PROCESS_INFO *pDRMHLSProcess = static_cast<VO_DRM2_HSL_PROCESS_INFO *>(pDRMData->sDataInfo.pInfo);
					VO_U64 ullChunkOffset = *((VO_U64*)pDRMData->nReserved[1]);

					if (0 == ullChunkOffset	 &&
						0x50 == pDRMData->pData[0] &&
						0x52 == pDRMData->pData[1] &&
						0x45 == pDRMData->pData[2] &&
						0x7 == pDRMData->pData[3])
					{
						voCAutoLock lock(&m_lock);

						for (int i = 0; i < 3; i++)
						{
							if (0xFFFFFFFF == m_Harmonics[i].uID)
							{
								m_Harmonics[i].uID = pDRMData->nReserved[2];

								VO_U32 uHamonicHeader = 0;
								memcpy(&uHamonicHeader, pDRMData->pData + 4, 4);
								m_Harmonics[i].uOffsetHamonic = 0;
								memcpy(&m_Harmonics[i].uOffsetHamonic, pDRMData->pData + 8, 4);

								if (m_Harmonics[i].hDRM)
								{
									m_API.Destroy(&m_Harmonics[i].hDRM, m_API.pUserData);
									m_Harmonics[i].hDRM = NULL;
								}

								unsigned uRet = m_API.Init(pDRMData->pData, uHamonicHeader, DRM_CONTENT_ENVELOPE, NULL, &m_Harmonics[i].hDRM, m_API.pUserData);
								VOLOGI("ID 0x%x, Harmonic %d, index %d, hDRM %p", pDRMData->nReserved[2], m_Harmonics[i].uOffsetHamonic, i, m_Harmonics[i].hDRM);
								if (uRet) {
									VOLOGE("Init 0x%x", uRet);
									return VO_ERR_DRM2_BADDRMINFO;
								}

								pDRMData->nSize -= m_Harmonics[i].uOffsetHamonic;

								memmove(pDRMData->pData, pDRMData->pData + m_Harmonics[i].uOffsetHamonic, pDRMData->nSize);
								ullChunkOffset += m_Harmonics[i].uOffsetHamonic;

								m_eHLS_DRM = HLS_DRM_HARMONIC;

								break;
							}
						}
					}

					VOLOGI("VO_DRM2DATATYPE_CHUNK_PROCESSING %d", m_eHLS_DRM);
					if (HLS_DRM_UNKNOW == m_eHLS_DRM)
						return VO_ERR_DRM2_OK;

					if (HLS_DRM_HARMONIC == m_eHLS_DRM)
					{
						voCAutoLock lock(&m_lock);

						for (int i = 0; i < 3; i++)
						{
							if (pDRMData->nReserved[2] == m_Harmonics[i].uID)
							{
								VO_U32 uBlockOffset	= (ullChunkOffset - m_Harmonics[i].uOffsetHamonic) / 16;
								VO_U32 uByteOffset	= (ullChunkOffset - m_Harmonics[i].uOffsetHamonic) % 16;

								VOLOGI("ID 0x%x, Harmonic %d, index %d, hDRM %p", pDRMData->nReserved[2], m_Harmonics[i].uOffsetHamonic, i, m_Harmonics[i].hDRM);
								VOLOGI("ChunkPos %lld Data %x DataSize %d", ullChunkOffset, pDRMData->pData[0], pDRMData->nSize);
								unsigned uRet = m_API.EnvelopeDecrypt(uBlockOffset, uByteOffset, pDRMData->pData, pDRMData->nSize, m_Harmonics[i].hDRM, m_API.pUserData);
								if (uRet) {
									VOLOGE("EnvelopeDecrypt 0x%x", uRet);
									return VO_ERR_DRM2_BADDRMDATA;
								}

								break;
							}
						}
					}
					else if (HLS_DRM_DX == m_eHLS_DRM)
					{
						VO_U32 uBlockOffset	= ullChunkOffset / 16;
						VO_U32 uByteOffset	= ullChunkOffset % 16;

						VOLOGI("Key %s KeyLen %d ChunkPos %lld Data %p DataSize %d", pDRMHLSProcess->szKeyString, strlen(pDRMHLSProcess->szKeyString), ullChunkOffset, pDRMData->pData, pDRMData->nSize);
						unsigned uRet = m_API.HLSDecrypt(pDRMHLSProcess->szKeyString, strlen(pDRMHLSProcess->szKeyString), uBlockOffset, uByteOffset, pDRMData->pData, pDRMData->nSize, m_hDRM, m_API.pUserData);
						if (uRet) {
							VOLOGE("HLSDecrypt 0x%x", uRet);
							return VO_ERR_DRM2_BADDRMDATA;
						}
					}

					*(pDRMData->pnDstSize) = pDRMData->nSize;
				}
				break;

			case VO_DRM2DATATYPE_CHUNK_END:
				{
					if (VO_DRM2SRC_CHUNK_HLS != eSourceFormat)
						break;

					voCAutoLock lock(&m_lock);

					for (int i = 0; i < 3; i++)
					{
						if (pDRMData->nReserved[2] == m_Harmonics[i].uID)
						{
							VOLOGI("ID 0x%x, Harmonic %d, index %d, hDRM %p", pDRMData->nReserved[2], m_Harmonics[i].uOffsetHamonic, i, m_Harmonics[i].hDRM);
							if (m_Harmonics[i].hDRM)
							{
								m_API.Destroy(&m_Harmonics[i].hDRM, m_API.pUserData);
								m_Harmonics[i].hDRM				= NULL;
							}

							m_Harmonics[i].uID				= 0xFFFFFFFF;
							m_Harmonics[i].uOffsetHamonic	= 0;

							break;
						}
					}
				}
				break;

			case VO_DRM2DATATYPE_MEDIASAMPLE:
				{
					if (NULL == pParam) {
						VOLOGE("VO_DRM2_DATA is NULL");
						return VO_ERR_DRM2_BADPARAMETER;
					}

					if ( ( VO_DRM2SRC_CHUNK_SSTR == eSourceFormat && IsEncrypted() ) ||
						VO_DRM2SRC_MEDIAFORMAT == eSourceFormat)
					{
						VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO * pInfo = (VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO *)pDRMData->sDataInfo.pInfo;

						VOLOGI("Data pointer: %p , Size: %d , Box pointer: %p , Box Size: %d , SampleIndex: %d , TrackID: %d", 
							pDRMData->pData, pDRMData->nSize, pInfo->pSampleEncryptionBox, pInfo->uSampleEncryptionBoxSize, pInfo->uSampleIndex, pInfo->uTrackId);
						unsigned uSize = pDRMData->nSize;
						unsigned uRet = m_API.PiffSampleDecrypt(pDRMData->pData, pDRMData->nSize, pDRMData->pData, &uSize, pInfo->pSampleEncryptionBox, pInfo->uSampleEncryptionBoxSize, pInfo->uSampleIndex, pInfo->uTrackId, m_hDRM, m_API.pUserData);
						if (uRet) {
							VOLOGE("PiffSampleDecrypt 0x%x", uRet);
						}

						return VO_ERR_DRM2_BADDRMDATA;
					}
				}
				break;

			case VO_DRM2DATATYPE_RAWFILE:
				{
					if (NULL == pParam) {
						VOLOGE("VO_DRM2_DATA is NULL");
						return VO_ERR_DRM2_BADPARAMETER;
					}

					VO_U32 uBlockOffset = ( *((VO_U64*)pDRMData->nReserved[1]) - m_Harmonics[0].uOffsetHamonic) / 16;
					VO_U32 uByteOffset = ( *((VO_U64*)pDRMData->nReserved[1]) - m_Harmonics[0].uOffsetHamonic) % 16;

					VOLOGR("uBlockOffset %d, uByteOffset %d, Data %x DataSize %d", uBlockOffset, uByteOffset, pDRMData->pData[0], pDRMData->nSize);
					unsigned uRet = m_API.EnvelopeDecrypt(uBlockOffset, uByteOffset, pDRMData->pData, pDRMData->nSize, m_Harmonics[0].hDRM, m_API.pUserData);
					if (uRet) {
						VOLOGE("EnvelopeDecrypt 0x%x", uRet);
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
