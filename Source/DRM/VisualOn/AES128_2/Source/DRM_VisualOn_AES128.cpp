#include "DRM_VisualOn_AES128.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifndef LOG_TAG
#define LOG_TAG "CDRM_VisualOn_AES128"
#endif

CDRM_VisualOn_AES128::CDRM_VisualOn_AES128(void)
: m_uOutputSeqence(0)
{
	VOLOGR("CDRM_VisualOn_AES128");
}

CDRM_VisualOn_AES128::~CDRM_VisualOn_AES128(void)
{
	Uninit();
}

VO_U32 CDRM_VisualOn_AES128::Init(VO_DRM_OPENPARAM *pParam)
{
	strcpy(m_drmCallback.szDRMTYPE, "DRM_VisualOn_AES128");

	m_VO_AES128[0].Init(pParam);
	m_VO_AES128[1].Init(pParam);
	m_VO_AES128[2].Init(pParam);

	return CDRM::Init(pParam);
}

VO_U32 CDRM_VisualOn_AES128::Uninit()
{
	m_VO_AES128[0].Uninit();
	m_VO_AES128[1].Uninit();
	m_VO_AES128[2].Uninit();

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM_VisualOn_AES128::SetThirdpartyAPI(VO_PTR pParam)
{
	m_VO_AES128[0].SetThirdpartyAPI(pParam);
	m_VO_AES128[1].SetThirdpartyAPI(pParam);
	m_VO_AES128[2].SetThirdpartyAPI(pParam);

	return VO_ERR_DRM2_OK;
}

VO_U32 CDRM_VisualOn_AES128::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	m_VO_AES128[0].SetParameter(uID, pParam);
	m_VO_AES128[1].SetParameter(uID, pParam);
	m_VO_AES128[2].SetParameter(uID, pParam);

	return CDRM::SetParameter(uID, pParam);
}

VO_U32 CDRM_VisualOn_AES128::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	switch (nFlag)
	{
	case VO_DRM_FLAG_DRMINFO:
		{
			if (VO_DRM2SRC_MEDIAFORMAT == eSourceFormat || VO_DRM2SRC_RAWFILE == eSourceFormat)
			{
				VOLOGW("!DRM Module");
				return VO_ERR_DRM2_MODULENOTFOUND;
			}

			m_VO_AES128[0].OnSourceDrm(nFlag, pParam, eSourceFormat);
			m_VO_AES128[1].OnSourceDrm(nFlag, pParam, eSourceFormat);
			m_VO_AES128[2].OnSourceDrm(nFlag, pParam, eSourceFormat);

			m_uOutputSeqence = 0;
		}
		break;

	case VO_DRM_FLAG_NEWAPI:
		{
			if (m_uOutputSeqence >= 3)
			{
				VOLOGW("No enough instance");
				return VO_ERR_DRM2_MODULENOTFOUND;
			}

			m_VO_AES128[m_uOutputSeqence].GetInternalAPI((VO_PTR*)pParam);
			m_uOutputSeqence++;
		}
		break;

	default:
		return VO_ERR_NOT_IMPLEMENT;
	}

	return VO_ERR_DRM2_OK;
}
