#include "voDRM.h"
#include "DRM.h"
#include "voLog.h"
#include "voIndex.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CDRM::CDRM(void)
: m_bExit(VO_FALSE)
{
	memset( &m_drmCallback, 0, sizeof(VOSOURCEDRMCALLBACK2) );
	memset( &m_DRM_OpenParam, 0, sizeof(VO_DRM_OPENPARAM) );
	memset( &m_cEventCallBack, 0, sizeof(VO_SOURCE2_EVENTCALLBACK) );
	memset( &m_cbVerify, 0, sizeof(VO_SOURCE2_IO_HTTP_VERIFYCALLBACK) );
	memset(m_szPackPath, 0, 512);
}

CDRM::~CDRM(void)
{
	VOLOGUNINIT();
}

VO_U32 CDRM::Init(VO_DRM_OPENPARAM *pParam)
{
	VOLOGR("Init");

	m_drmCallback.pUserData = this;
	m_drmCallback.fCallback = fSourceDrmProc;

	memcpy( &m_DRM_OpenParam, pParam, sizeof(VO_DRM_OPENPARAM) );

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM::Uninit()
{
	m_bExit = VO_TRUE;

	return VO_ERR_DRM2_OK;
}


VO_U32 CDRM::GetInternalAPI(VO_PTR *ppParam)
{
	if (NULL == ppParam) {
		VOLOGE("empty pointor");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	VOLOGR("GetInternalAPI %p", &m_drmCallback);
	*ppParam = &m_drmCallback;

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_SOURCE2_EVENTCALLBACK:
		{
			if (pParam)
			{
				memcpy( &m_cEventCallBack, pParam, sizeof(VO_SOURCE2_EVENTCALLBACK) );
			}
		}
		break;

	case VO_PID_DRM2_PackagePath:
		{
			if (pParam)
			{
				VOLOGR("%s", pParam);

				vostrcpy( m_szPackPath, static_cast<VO_TCHAR*>(pParam) );

				VOLOGINIT(m_szPackPath);
			}
		}
		break;

	case VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK:
		{
			if (pParam)
			{
				memcpy( &m_cbVerify, pParam, sizeof(VO_SOURCE2_IO_HTTP_VERIFYCALLBACK) );

				VOLOGR("VO_PID_SOURCE2_HTTPVERIFICATIONCALLBACK hHandle %p, HTTP_Callback %p", m_cbVerify.hHandle, m_cbVerify.HTTP_Callback);
			}
		}
		break;

	case VO_PID_DRM2_EXIT:
		{
			if (NULL == pParam) {
				VOLOGE("empty pointor");
				return VO_ERR_DRM2_BADPARAMETER;
			}

			m_bExit = *( static_cast<VO_BOOL*>(pParam) );
			VOLOGI("EXIT %d", m_bExit);
		}
		break;

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

VO_U32 getRPRightsManagementHeader_PIFF(VO_BYTE* p, VO_U32 uSize, VO_PBYTE *ppHeader, VO_U32* puLength)
{
	if (uSize < 0x3A || NULL == p || NULL == ppHeader || NULL == puLength)
	{
		VOLOGE("input data %d", uSize);
		return VO_ERR_DRM2_BADPARAMETER;
	}

	const unsigned char uuidProtectionSystemSpecificHeader[] = {0xD0, 0x8A, 0x4F, 0x18, 0x10, 0xF3, 0x4A, 0x82, 0xB6, 0xC8, 0x32, 0xD8, 0xAB, 0xA1, 0x83, 0xD3};
	if ( memcmp(p + 8, uuidProtectionSystemSpecificHeader, 16) )
	{
		VOLOGE("Protection System Specific Header Box");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	const unsigned char uuidPlayReady[] = {0x9A, 0x04, 0xF0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xAB, 0x92, 0xE6, 0x5B, 0xE0, 0x88, 0x5F, 0x95};
	if ( memcmp(p + 28, uuidPlayReady, 16) )
	{
		VOLOGE("!Play Ready");
		return VO_ERR_DRM2_BADPARAMETER;
	}

	//int iLengthPR = 0;
	//memcpy(&iLengthPR, p + 48, 4);

	int iRecordCount = 0;
	memcpy(&iRecordCount, p + 52, 2);

	p += 54;
	for (int i = 0; i < iRecordCount; i++)
	{
		int iRecordType = 0;
		memcpy(&iRecordType, p, 2);
		p += 2;

		int iRecordLength = 0;
		memcpy(&iRecordLength, p, 2);
		p += 2;

		if (0x1 == iRecordType)
		{
			*ppHeader = p;
			*puLength = iRecordLength;

			return VO_ERR_DRM2_OK;
		}
		else
		{
			p += iRecordLength;
		}
	}

	return VO_ERR_DRM2_BADPARAMETER;
}

#ifdef _VONAMESPACE
}
#endif
