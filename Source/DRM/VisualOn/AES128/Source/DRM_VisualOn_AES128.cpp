#include "DRM_VisualOn_AES128.h"
#include "vo_aes_decryption.h"
#include "voHLSDRM.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "CDRM_VisualOn_AES128"
#endif


CDRM_VisualOn_AES128::CDRM_VisualOn_AES128(void)
: m_pHandle(NULL)
, m_bBufvalid(VO_FALSE)
{
	VOLOGI("CDRM_VisualOn_AES128");
}

CDRM_VisualOn_AES128::~CDRM_VisualOn_AES128(void)
{
	Uninit();
}

VO_U32 CDRM_VisualOn_AES128::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_VisualOn_AES128");

	return CDRM::Init(pParam);
}

VO_U32 CDRM_VisualOn_AES128::Uninit()
{
	if (m_pHandle)
	{
		delete (fz_aes*)m_pHandle;
		m_pHandle = 0;
	}

	return VO_ERR_NONE;
}

VO_U32 CDRM_VisualOn_AES128::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
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
			if (!pDrmInfo->pDrmInfo)
				return VO_ERR_BASE;


			if (m_pHandle)
			{
				delete (fz_aes*)m_pHandle;
				m_pHandle = 0;
			}

			m_pHandle = new fz_aes;
			if (!m_pHandle)
				return VO_ERR_OUTOF_MEMORY;

			VO_BYTE* key_got = (VO_BYTE*)pDrmInfo->pDrmInfo;

			VOLOGR( "key: %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X" , key_got[0] , key_got[1] , key_got[2] , key_got[3] , key_got[4] , key_got[5] ,
					key_got[6] , key_got[7] , key_got[8] , key_got[9] , key_got[10] , key_got[11] , key_got[12] , key_got[13] , key_got[14] , key_got[15] );

			fz_aesinit( (fz_aes *)m_pHandle, (unsigned char *)key_got, 16 );
		}
		break;

	case VO_DRM_FLAG_DRMDATA:
		{
			if (!m_pHandle)
				return VO_ERR_INVALID_ARG;

			VO_DRM2_DATA * pDrmData = static_cast<VO_DRM2_DATA *>(pParam);
			S_HLS_DRM_PROCESS_INFO *pHLSDRMProcess = static_cast<S_HLS_DRM_PROCESS_INFO *>(pDrmData->sDataInfo.pInfo);
			S_HLS_DRM_ASSIST_INFO *pHLSDRMInfo = (S_HLS_DRM_ASSIST_INFO *)(pHLSDRMProcess->pInfo);

			switch (pHLSDRMProcess->eDrmProcessInfo)
			{
			case DecryptProcess_BEGIN:
				{
					VOLOGR( "iv: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X" , pHLSDRMInfo->aKey[0] , pHLSDRMInfo->aKey[1] , pHLSDRMInfo->aKey[2] , pHLSDRMInfo->aKey[3] , pHLSDRMInfo->aKey[4] , pHLSDRMInfo->aKey[5] ,
						pHLSDRMInfo->aKey[6] , pHLSDRMInfo->aKey[7] , pHLSDRMInfo->aKey[8] , pHLSDRMInfo->aKey[9] , pHLSDRMInfo->aKey[10] , pHLSDRMInfo->aKey[11] , pHLSDRMInfo->aKey[12] , pHLSDRMInfo->aKey[13] , pHLSDRMInfo->aKey[14] , pHLSDRMInfo->aKey[15] );
					fz_setiv( (fz_aes *)m_pHandle, pHLSDRMInfo->aKey);
					m_bBufvalid = VO_FALSE;
				}
				break;

			case DecryptProcess_PROCESSING:
				{
					VO_U32 dec_size = 0;

					if( pDrmData->nSize != 0 )
					{
						VO_BYTE temp[16];
						memcpy(temp, pDrmData->pData + pDrmData->nSize - 16, 16);

						if( m_bBufvalid )
						{
							dec_size += 16;
							fz_aesdecrypt( (fz_aes *)m_pHandle, m_Buf, m_Buf, 16);


							if( pDrmData->nSize - 16 > 0 )
							{
								fz_aesdecrypt( (fz_aes *)m_pHandle, pDrmData->pData, pDrmData->pData, pDrmData->nSize - 16);
								dec_size += (pDrmData->nSize - 16);

								memmove(pDrmData->pData + 16 , pDrmData->pData , pDrmData->nSize - 16 );
							}

							memcpy(pDrmData->pData, m_Buf, 16);
							memset(m_Buf, 0, 16);
						}
						else
						{
							if( pDrmData->nSize - 16 > 0 )
								fz_aesdecrypt( (fz_aes *)m_pHandle, pDrmData->pData, pDrmData->pData, pDrmData->nSize - 16);

							dec_size += (pDrmData->nSize - 16);
						}

						memcpy(m_Buf, temp, 16);
						m_bBufvalid = VO_TRUE;
					}

					*(pDrmData->pnDstSize) = dec_size;
				}
				break;

			case DecryptProcess_END:
				{
					fz_aesdecrypt( (fz_aes *)m_pHandle, m_Buf, m_Buf, 16);

					VO_U32 iSize = 16 - m_Buf[15];
					*(pDrmData->pnDstSize) = 0;
					if (0 != iSize && 16 != iSize)
					{
						memcpy(pDrmData->pData, m_Buf, iSize);
						*(pDrmData->pnDstSize) = iSize; 
					}
					m_bBufvalid = VO_FALSE;
				}
				break;

			default:
				break;
			}

			if (pDrmData->pnDstSize && *(pDrmData->pnDstSize) > 0)
			{
				VOLOGR("0x%X", pDrmData->pData[0]);
			}
		}
		break;

	default:
		break;
	}

	return VO_ERR_DRM2_OK;
}
