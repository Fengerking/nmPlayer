#include "DRM_Conax_PlayReady.h"
#include <string.h>


#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

DRM_Conax_PlayReady::DRM_Conax_PlayReady(conaxDrmAPI* pParam)
: m_eType(VO_OSMP_DRM_SOURCE_UNKNOWN)
, m_pAPIs(pParam)
, m_hContext(0)
{
}

DRM_Conax_PlayReady::~DRM_Conax_PlayReady(void)
{
	if (m_hContext)
	{
		m_pAPIs->drmUinit(m_hContext);
		m_hContext = 0;
	}
}

int DRM_Conax_PlayReady::init()
{
	if (m_hContext)
	{
		m_pAPIs->drmUinit(m_hContext);
		m_hContext = 0;
	}

	int iRet = m_pAPIs->drmInit(&m_hContext);
	if (0 != iRet)
	{
		return VO_OSMP_DRM_ERR_BASE;
	}

	return VO_OSMP_DRM_ERR_NONE;
}



int DRM_Conax_PlayReady::processHeader(VO_OSMP_DRM_SOURCE_TYPE eType, const unsigned char* pHeader, int iHeaderSize)
{
	m_eType = eType;

	if (VO_OSMP_DRM_SOURCE_SSTR == eType)
	{
		int iRet = m_pAPIs->drmPiffOpen( (const char*)pHeader, iHeaderSize, m_hContext);
		if (0 != iRet)
		{
			return VO_OSMP_DRM_ERR_BASE;
		}
	}

	return VO_OSMP_DRM_ERR_NONE;
}

int DRM_Conax_PlayReady::decryptSample(int iPosition, unsigned char* pData, int* piSize, void* pAdditionalInfo)
{
	if (VO_OSMP_DRM_SOURCE_SSTR != m_eType)
		return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;

	VO_OSMP_DRM_PIFF_INFO * pInfo= static_cast<VO_OSMP_DRM_PIFF_INFO*>(pAdditionalInfo);

	int iRet = m_pAPIs->drmPiffDecrypt(pData, *piSize, pData, piSize, pInfo->pSampleEncryptionBox, pInfo->iSampleEncryptionBox, pInfo->iSampleIndex, pInfo->iTrackID, m_hContext);
	if (0 != iRet)
	{
		return VO_OSMP_DRM_ERR_BASE;
	}

	return VO_OSMP_DRM_ERR_NONE;
}

int DRM_Conax_PlayReady::beginSegment(int identifer, void* pAdditionalInfo)
{
	if (VO_OSMP_DRM_SOURCE_HLS == m_eType)
	{
		VO_OSMP_DRM_HLS_INFO* pDRMHLSProcess = static_cast<VO_OSMP_DRM_HLS_INFO*>(pAdditionalInfo);
		int iRet = m_pAPIs->drmHLSOpen(pDRMHLSProcess->szKeyString, strlen(pDRMHLSProcess->szKeyString), m_hContext);
		if (0 != iRet)
		{
			return VO_OSMP_DRM_ERR_BASE;
		}
	}

	return VO_OSMP_DRM_ERR_NONE;
}

int DRM_Conax_PlayReady::decryptSegment(int identifer, long long llPosition, unsigned char* pData, int* piSize, bool bLastBlock, void* pAdditionalInfo)
{
	if (VO_OSMP_DRM_SOURCE_HLS == m_eType)
	{
		VO_OSMP_DRM_HLS_INFO* pDRMHLSProcess = static_cast<VO_OSMP_DRM_HLS_INFO*>(pAdditionalInfo);

		int iRet = m_pAPIs->drmHLSDecrypt(pData, *piSize, pData, reinterpret_cast<unsigned int*>(piSize), pDRMHLSProcess->iSequenceNum, bLastBlock ? 1 : 0, m_hContext);
		if (0 != iRet)
		{
			return VO_OSMP_DRM_ERR_BASE;
		}
	}

	return VO_OSMP_DRM_ERR_NONE;
}

int DRM_Conax_PlayReady::endSegment(int identifer)
{
	return VO_OSMP_DRM_ERR_NONE;
}