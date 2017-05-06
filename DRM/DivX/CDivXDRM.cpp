#include "CDivXDRM.h"
#include "DrmApi.h"
#include "DrmApiExt.h"
#include <malloc.h>
#include "voOMX_Drm.h"

#define LOG_TAG "DivXDRM"

#include "voLog.h"

#define CHECK_DIVXDRM_STATE	if(!m_pDrmContext)\
	return VO_ERR_DRM_ERRORSTATE;

CDivXDRM::CDivXDRM()
: m_pDrmContext(NULL)
, m_pDecryptContext(NULL)
, m_nMode(VO_DRMMODE_NONE)
, m_btUseLimit(-1)
, m_btUseCount(-1)
, m_btCgmsa(0)
, m_btAcptb(0)
, m_btDigitalProtection(0)
, m_dwRandomSampleCount(0)
, m_iDRMMode(0)
{
}

CDivXDRM::~CDivXDRM()
{
	Close();
}

VO_U32 CDivXDRM::Open()//0, 1.5; 1, JIT
{
	Close();

	return VO_ERR_DRM_OK;
}

VO_U32 CDivXDRM::Close()
{
	if(m_pDrmContext)
	{
		drmFinalizePlayback(m_pDrmContext);

		free(m_pDrmContext);
		m_pDrmContext = NULL;
	}

	if(m_pDecryptContext)
	{
		drmFinalizePlayback(m_pDecryptContext);

		free(m_pDecryptContext);
		m_pDecryptContext = NULL;
	}

	return VO_ERR_DRM_OK;
}

VO_U32 CDivXDRM::doOpen()
{
	drmErrorCodes_t result = DRM_GENERAL_ERROR;
	uint32_t drmContextLength = 0;
	if (m_iDRMMode)
		result = drmInitSystemEx(drmContextRoleAuthentication, NULL, &drmContextLength);
	else
		result = drmInitSystem(NULL, &drmContextLength);
	if(DRM_SUCCESS != result)
		return result;

	m_pDrmContext = (VO_PBYTE)malloc(drmContextLength);
	if(!m_pDrmContext)
		return VO_ERR_OUTOF_MEMORY;

	if (m_iDRMMode)
		result = drmInitSystemEx(drmContextRoleAuthentication, m_pDrmContext, &drmContextLength);
	else
		result = drmInitSystem(m_pDrmContext, &drmContextLength);

	if(DRM_SUCCESS != result)
	{
		free(m_pDrmContext);
		m_pDrmContext = NULL;

		return result;
	}

	//for (int i = 0; i < 4; i++)
	{
		result = drmSetRandomSample(m_pDrmContext);
		if(DRM_SUCCESS == result)
			m_dwRandomSampleCount++;
	}

	return VO_ERR_DRM_OK;
}

VO_U32 CDivXDRM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	if(DIVX_DRM_ID_INITDRMMEMORY == uID)
	{
		drmErrorCodes_t result = drmInitDrmMemory();
		return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
	}
	else if (VO_PID_DRM_DRMVersion == uID)
	{
		m_iDRMMode = *((int*)pParam);
		//VOLOGI("m_iDRMMode %d", m_iDRMMode);
		return VO_ERR_DRM_OK;
	}

	CHECK_DIVXDRM_STATE

		switch(uID)
	{
		case DIVX_DRM_ID_RANDOMSAMPLE:
			{
				drmErrorCodes_t result = drmSetRandomSample(m_pDrmContext);
				if(DRM_SUCCESS == result)
					m_dwRandomSampleCount++;

				return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
			}
			break;

		default:
			break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CDivXDRM::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	if (!m_pDrmContext)
	{
		doOpen();
	}

		switch(uID)
	{
		case DIVX_DRM_ID_ACTIVATIONSTATUS:
			{
				uint8_t userId[5] = {0};
				uint32_t userIdLength = 5;

				drmErrorCodes_t result = drmGetActivationStatus(userId, &userIdLength);
				if(DRM_SUCCESS == result)
					*((VODIVXDRMDEVICESTATUS*)pParam) = DIVX_DRM_DEVICE_REGISTERED;
				else if(DRM_NOT_REGISTERED == result)
					*((VODIVXDRMDEVICESTATUS*)pParam) = DIVX_DRM_DEVICE_NOTREGISTERED;
				else if(DRM_NEVER_REGISTERED == result)
					*((VODIVXDRMDEVICESTATUS*)pParam) = DIVX_DRM_DEVICE_NEVERREGISTERED;
				else
					return VO_ERR_DRM_UNDEFINED;

				result = drmSetRandomSample(m_pDrmContext);
				if(DRM_SUCCESS == result)
					m_dwRandomSampleCount++;

				return VO_ERR_DRM_OK;
			}
			break;

		case DIVX_DRM_ID_REGCODESTRING:
			{
				drmErrorCodes_t result = DRM_SUCCESS;

				if(m_dwRandomSampleCount < 4)
				{
					for(VO_U32 i = 0; i < (4 - m_dwRandomSampleCount); i++)
						result = drmSetRandomSample(m_pDrmContext);
				}
				m_dwRandomSampleCount = 0;

				result = drmGetRegistrationCodeString(m_pDrmContext, (char*)pParam);
				return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
			}
			break;

		case DIVX_DRM_ID_DEACTCODESTRING:
			{
				drmErrorCodes_t result = drmGetDeactivationCodeString(m_pDrmContext, (char*)pParam);
				return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
			}
			break;
			
		case DIVX_DRM_ID_ISDRMMEMINITIALIZED:
			{
				drmErrorCodes_t result = drmIsDrmMemInitialized();
				if (DRM_SUCCESS == result) 
				{
					*((bool*)pParam) = true;
				}
				else
				{
					*((bool*)pParam) = false;
				}
				return DRM_SUCCESS;
			}
			break;

		default:
			break;
	}

	return VO_ERR_NOT_IMPLEMENT;
}

VO_U32 CDivXDRM::SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo)
{
	doOpen();

	CHECK_DIVXDRM_STATE

		VO_DIVXDRM_INFO* pInfo = (VO_DIVXDRM_INFO*)pDrmInfo;
	if(pInfo->nStrdChunk != drmGetStrdSize() + 8)
		return VO_ERR_DRM_BADDRMINFO;

	drmErrorCodes_t result = drmInitPlayback(m_pDrmContext, pInfo->pStrdChunk + 8);
	if(DRM_SUCCESS != result)
		return result;

	uint8_t rentalMessageFlag = 0;
	result = drmQueryRentalStatus(m_pDrmContext, &rentalMessageFlag, &m_btUseLimit, &m_btUseCount);

	VOLOGI("result: %u ", result);	
	VOLOGI("rentalMessageFlag: %u ", rentalMessageFlag);	
	VOLOGI("m_btUseLimit: %u ",m_btUseLimit);	
	VOLOGI("m_btUseCount: %u ", m_btUseCount);	

	if(DRM_SUCCESS != result)
	{
		if (DRM_RENTAL_EXPIRED == result) // exception!! if rental file, ignore the expire info 
		{
			m_nMode = VO_DRMMODE_RENTAL_COUNT;	
		}
		else
		{
			m_nMode = VO_DRMMODE_NONE;
			m_btUseCount = m_btUseLimit = -1;
			return result;
		}
	}
	else
		m_nMode = (m_btUseLimit > 0) ? VO_DRMMODE_RENTAL_COUNT : VO_DRMMODE_PURCHASE;

	uint8_t cgmsaSignal = 0;
	result = drmQueryCgmsa(m_pDrmContext, &cgmsaSignal);
	if(DRM_SUCCESS != result)
		return result;
	else
		m_btCgmsa = cgmsaSignal;

	uint8_t acptbSignal = 0;
	result = drmQueryAcptb(m_pDrmContext, &acptbSignal);
	if(DRM_SUCCESS != result)
		return result;
	else
		m_btAcptb = acptbSignal;

	uint8_t digitalProtectionSignal = 0;
	result = drmQueryDigitalProtection(m_pDrmContext, &digitalProtectionSignal);
	if(DRM_SUCCESS != result)
		return result;
	else
		m_btDigitalProtection = digitalProtectionSignal;

	uint8_t ict = 0;
	result = drmQueryIct(m_pDrmContext, &ict);
	if(DRM_SUCCESS != result)
		return result;

	return VO_ERR_DRM_OK;
}

VO_U32 CDivXDRM::GetDrmFormat(VO_DRM_FORMAT* pDrmFormat)
{
	CHECK_DIVXDRM_STATE

		if(!pDrmFormat)
			return VO_ERR_DRM_BADPARAMETER;

	pDrmFormat->nMode = m_nMode;
	if(VO_DRMMODE_RENTAL_COUNT == pDrmFormat->nMode)
	{
		pDrmFormat->uInfo.iRentalCount.nTotal = m_btUseLimit;
		pDrmFormat->uInfo.iRentalCount.nUsed = m_btUseCount;
	}

	pDrmFormat->iOutputProtect.m_btCgmsa = m_btCgmsa;
	pDrmFormat->iOutputProtect.m_btAcptb = m_btAcptb;
	pDrmFormat->iOutputProtect.m_btDigitalProtection = m_btDigitalProtection;

	VOLOGI ("mode: %d, total: 0x%08X, used: 0x%08X", pDrmFormat->nMode, pDrmFormat->uInfo.iRentalCount.nTotal, pDrmFormat->uInfo.iRentalCount.nUsed);

	return VO_ERR_DRM_OK;
}

VO_U32 CDivXDRM::CheckCopyRightResolution(VO_U32 nWidth, VO_U32 nHeight)
{
	VO_U32 nResult = VO_ERR_DRM_OK;
	CHECK_DIVXDRM_STATE

	VOLOGE("m_btCgmsa : %u", m_btCgmsa);
	VOLOGE("m_btAcptb : %u", m_btAcptb);
	VOLOGE("m_btDigitalProtection : %u", m_btDigitalProtection);
	VOLOGE("nWidth : %u", nWidth);
	VOLOGE("nHeight : %u", nHeight);

	if (m_btCgmsa != 0 || m_btAcptb != 0 || m_btDigitalProtection != 0) 
	{
		if (nWidth >= VO_MAX_PROTECTED_WIDTH || nHeight >= VO_MAX_PROTECTED_HEIGHT)		
		{
			nResult = VO_ERR_DRM_BADRESOLUTION;
		}
	}

	return nResult;	
}

VO_U32 CDivXDRM::Commit()
{
	CHECK_DIVXDRM_STATE

	drmErrorCodes_t result = drmCommitPlayback(m_pDrmContext);
	if(DRM_SUCCESS != result)
		return result;

	if (m_iDRMMode == 0)
		return VO_ERR_DRM_OK;

	uint32_t drmDecryptionLength = 0;
	result = drmInitSystemEx(drmContextRoleDecryption, NULL, &drmDecryptionLength);
	if(DRM_SUCCESS != result)
		return result;

	m_pDecryptContext = (VO_PBYTE)malloc(drmDecryptionLength);
	if(!m_pDecryptContext)
		return VO_ERR_OUTOF_MEMORY;

	result = drmInitSystemEx(drmContextRoleDecryption, m_pDecryptContext, &drmDecryptionLength);
	if(DRM_SUCCESS != result)
	{
		free(m_pDecryptContext);
		m_pDecryptContext = NULL;

		return result;
	}

	//VO_DIVXDRM_INFO* pInfo = (VO_DIVXDRM_INFO*)pDrmInfo;
	//if(pInfo->nStrdChunk != drmGetStrdSize() + 8)
	//	return VO_ERR_DRM_BADDRMINFO;

	result = drmInitPlayback(m_pDecryptContext, NULL /*pInfo->pStrdChunk + 8*/);
	if(DRM_SUCCESS != result)
		return result;

	result = drmCommitPlayback(m_pDecryptContext);
	return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
}

VO_U32 CDivXDRM::DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize)
{
	CHECK_DIVXDRM_STATE

		drmErrorCodes_t result = DRM_SUCCESS;
	if(VO_DRMDATATYPE_AUDIOSAMPLE == pDataInfo->nDataType)
		result = drmDecryptAudio(m_pDrmContext, pData, nSize);
	else if(VO_DRMDATATYPE_VIDEOSAMPLE == pDataInfo->nDataType)
		result = drmDecryptVideo(m_pDrmContext, pData, nSize, (uint8_t*)pDataInfo->pInfo);
	else if(VO_DRMDATATYPE_JITSAMPLE == pDataInfo->nDataType)
		result = drmDecryptVideoEx(m_pDecryptContext, pData, nSize);
	else
		return VO_ERR_DRM_BADPARAMETER;

	return (DRM_SUCCESS == result) ? VO_ERR_DRM_OK : result;
}

VO_U32 CDivXDRM::DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize)
{
	CHECK_DIVXDRM_STATE

	drmErrorCodes_t result = DRM_SUCCESS;
	if(VO_DRMDATATYPE_VIDEOSAMPLE == pDataInfo->nDataType)
	{
		uint8_t* ddInfoWritePtr = drmGetDDInfoWriteBufferEx(m_pDrmContext);
		if (ddInfoWritePtr)
		{
			memcpy(ddInfoWritePtr, pDataInfo->pInfo, 10);
		} 
		else
		{
			return DRM_GENERAL_ERROR;
		}
		

		//uint32_t tempBufferLength = drmGetMaxBitstreamPayloadLengthEx(drmStreamTypeVideoMPEG4ASP);

		uint8_t tempBuffer[256] = {0};
		uint32_t tempBufferLength = sizeof(tempBuffer);
		drmStreamType_t type;
		uint8_t mpeg4StartCode[] = { 0x00, 0x00, 0x01 };

		/* The Demuxer should already know what stream to expect but because this application
		is not stream aware, a simple check for MPEG4 start code of 001 will tell me what
		stream is in this file. */
		if (memcmp( pSrcData, mpeg4StartCode, sizeof(mpeg4StartCode) ) == 0)
		{
			type = drmStreamTypeVideoMPEG4ASP;
		}
		else
		{
			type = drmStreamTypeVideoH264;
		}

		result = drmInsertBitstreamPayloadEx(m_pDrmContext, type, tempBuffer, &tempBufferLength);
		if(DRM_SUCCESS != result)
			return result;

		memmove(pSrcData + tempBufferLength, pSrcData, nSrcSize);
		memcpy(pSrcData, tempBuffer, tempBufferLength);

		//result = drmDecryptVideoEx(m_pDecryptContext, pSrcData, nSrcSize + tempBufferLength);
		//if(DRM_SUCCESS != result)
		//	return result;

		*ppDstData = pSrcData;
		*pnDstSize = nSrcSize + tempBufferLength;
	}
	else
		return VO_ERR_DRM_BADPARAMETER;

	return VO_ERR_DRM_OK;

}
