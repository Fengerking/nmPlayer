	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDrmEngine.cpp

	Contains:	CDrmEngine class file

	Written by:	East Zhou

	Change History (most recent first):
	2010-03-12		East		Create file

*******************************************************************************/
#include "voString.h"
#include "CDrmEngine.h"

#define CHECK_DRM_HANDLE	if(m_hDrm == 0)\
	return VO_ERR_WRONG_STATUS;

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef VO_S32 (VO_API * VOGETDRMAPI)(VO_DRM_API* pReadHandle, VO_U32 uFlag);

CDrmEngine::CDrmEngine(VO_DRM_TYPE nType)
	: mpDrmAPI(NULL)
	, m_hDrm(NULL)
	, m_nType(nType)
{
}

CDrmEngine::~CDrmEngine()
{
	Close();
}

VO_U32 CDrmEngine::Open()
{
	if(m_hDrm)
		return VO_ERR_NONE;

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->Open(&m_hDrm);
	}

	return m_funDec.Open(&m_hDrm);
}

VO_U32 CDrmEngine::Close()
{
	if(m_hDrm)
	{
		if(mpDrmAPI != NULL)
		{
			mpDrmAPI->Close(m_hDrm);
		}
		else
		{
			m_funDec.Close(m_hDrm);
		}

		m_hDrm = NULL;
	}

	return VO_ERR_NONE;
}

VO_U32 CDrmEngine::SetParameter(VO_U32 uID, VO_PTR pParam)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->SetParameter(m_hDrm, uID, pParam);
	}

	return 	m_funDec.SetParameter(m_hDrm, uID, pParam);
}

VO_U32 CDrmEngine::GetParameter(VO_U32 uID, VO_PTR pParam)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->GetParameter(m_hDrm, uID, pParam);
	}

	return 	m_funDec.GetParameter(m_hDrm, uID, pParam);
}

VO_U32 CDrmEngine::SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->SetDrmInfo(m_hDrm, nFlag, pDrmInfo);
	}

	return m_funDec.SetDrmInfo(m_hDrm, nFlag, pDrmInfo);
}

VO_U32 CDrmEngine::GetDrmFormat(VO_DRM_FORMAT* pDrmFormat)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->GetDrmFormat(m_hDrm, pDrmFormat);
	}

	return m_funDec.GetDrmFormat(m_hDrm, pDrmFormat);
}

VO_U32 CDrmEngine::CheckCopyRightResolution(VO_U32 nWidth, VO_U32 nHeight)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->CheckCopyRightResolution(m_hDrm, nWidth, nHeight);
	}

	return m_funDec.CheckCopyRightResolution(m_hDrm, nWidth, nHeight);
}

VO_U32 CDrmEngine::Commit()
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->Commit(m_hDrm);
	}

	return m_funDec.Commit(m_hDrm);
}

VO_U32 CDrmEngine::DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->DecryptData(m_hDrm, pDataInfo, pData, nSize);
	}

	return m_funDec.DecryptData(m_hDrm, pDataInfo, pData, nSize);
}

VO_U32 CDrmEngine::DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize)
{
	CHECK_DRM_HANDLE

	if(mpDrmAPI != NULL)
	{
		return mpDrmAPI->DecryptData2(m_hDrm, pDataInfo, pSrcData, nSrcSize, ppDstData, pnDstSize);
	}

	return m_funDec.DecryptData2(m_hDrm, pDataInfo, pSrcData, nSrcSize, ppDstData, pnDstSize);
}

VO_U32 CDrmEngine::LoadLib(VO_HANDLE hInst)
{
	if(VO_DRMTYPE_DIVX == m_nType)
	{
		vostrcpy(m_szDllFile, _T("voDivXDRM"));
		vostrcpy(m_szAPIName, _T("voGetDivXDRMAPI"));
	}
	else if(VO_DRMTYPE_WindowsMedia == m_nType)
	{
		vostrcpy (m_szDllFile, _T("voWMDRM"));
		vostrcpy (m_szAPIName, _T("voGetWMDRMAPI"));
	}

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, _T(".so"));
#endif

	if(CDllLoad::LoadLib(hInst) == 0)
		return 0;

	VOGETDRMAPI pAPI = (VOGETDRMAPI)m_pAPIEntry;
	pAPI(&m_funDec, 0);

	return 1;
}
