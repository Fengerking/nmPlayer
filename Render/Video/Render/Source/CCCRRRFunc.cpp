	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVideoWindow.cpp

	Contains:	CVideoWindow class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-08-31		JBF			Create file

*******************************************************************************/
#include "cmnMemory.h"
#include "CCCRRRFunc.h"

#if defined _IOS || defined _MAC_OS
#include "voOSFunc.h"
#endif

#define LOG_TAG "CCCRRRFunc"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CCCRRRFunc::CCCRRRFunc(VO_BOOL bSoft, CBaseConfig * pConfig)
	: CDllLoad ()
	, m_pConfig (pConfig)
	, m_bSoft (bSoft)
	, m_hCCRR (NULL)
{
	memset (&m_ccrrFunc, 0, sizeof (VO_VIDEO_CCRRRAPI));
}

CCCRRRFunc::~CCCRRRFunc ()
{
	if (m_hCCRR != NULL)
		m_ccrrFunc.Uninit (m_hCCRR);
}	

VO_U32 CCCRRRFunc::Init (VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag)
{
	if (m_ccrrFunc.Init == NULL)
		return VO_ERR_FAILED;

	if (m_hCCRR != NULL)
		return VO_ERR_NONE;

	VO_U32 nRC = m_ccrrFunc.Init (&m_hCCRR, hView, pMemOP, nFlag);
	if (nRC == VO_ERR_NONE)
	{
		GetProperty (&m_prop);
#if !defined __VOPRJ_INST__
#ifdef _IOS
		SetColorType (VO_COLOR_YUV_PLANAR420, VO_COLOR_ARGB32_PACKED);
#elif defined _MAC_OS
		SetColorType (VO_COLOR_YUV_PLANAR420, VO_COLOR_ARGB32_PACKED);
#elif defined _METRO
		SetColorType (VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB32_PACKED);
#else
		SetColorType (VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB565_PACKED);
#endif
#else
		SetColorType (VO_COLOR_YUV_PLANAR420, VO_COLOR_RGB32_PACKED);
#endif

		SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);
	}

	return nRC;
}

VO_U32 CCCRRRFunc::GetProperty (VO_CCRRR_PROPERTY * pProperty)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.GetProperty (m_hCCRR, pProperty);

	return nRC;
}

VO_U32 CCCRRRFunc::GetInputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.GetInputType (m_hCCRR, pColorType, nIndex);

	return nRC;
}

VO_U32 CCCRRRFunc::GetOutputType (VO_IV_COLORTYPE * pColorType, VO_U32 nIndex)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.GetOutputType (m_hCCRR, pColorType, nIndex);

	return nRC;
}

VO_U32 CCCRRRFunc::SetColorType (VO_IV_COLORTYPE nInputColor, VO_IV_COLORTYPE nOutputColor)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VOLOGI ("Input: %d, outPut: %d", nInputColor, nOutputColor);

	voCAutoLock lock( &m_Lock );
	VO_U32 nRC = m_ccrrFunc.SetColorType (m_hCCRR, nInputColor, nOutputColor);

	return nRC;
}

VO_U32 CCCRRRFunc::SetCCRRSize (VO_U32 * pInWidth, VO_U32 * pInHeight, VO_U32 * pOutWidth, VO_U32 * pOutHeight, VO_IV_RTTYPE nRotate)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock( &m_Lock );
	VO_U32 nRC = m_ccrrFunc.SetCCRRSize (m_hCCRR, pInWidth, pInHeight, pOutWidth, pOutHeight, nRotate);

	return nRC;
}

VO_U32 CCCRRRFunc::Process (VO_VIDEO_BUFFER * pVideoBuffer, VO_VIDEO_BUFFER * pOutputBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock( &m_Lock );
	VO_U32 nRC = m_ccrrFunc.Process (m_hCCRR, pVideoBuffer, pOutputBuffer, nStart, bWait);

	return nRC;
}

VO_U32 CCCRRRFunc::WaitDone (void)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.WaitDone (m_hCCRR);

	return nRC;
}

VO_U32 CCCRRRFunc::SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.SetCallBack (m_hCCRR, pCallBack, pUserData);

	return nRC;
}

VO_U32 CCCRRRFunc::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.GetVideoMemOP (m_hCCRR, ppVideoMemOP);

	return nRC;
}

VO_U32 CCCRRRFunc::SetParam (VO_U32 nID, VO_PTR pValue)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	voCAutoLock lock( &m_Lock );
	VO_U32 nRC = m_ccrrFunc.SetParam (m_hCCRR, nID, pValue);

	return nRC;
}

VO_U32 CCCRRRFunc::GetParam (VO_U32 nID, VO_PTR pValue)
{
	if (m_hCCRR == NULL)
		return VO_ERR_WRONG_STATUS;

	VO_U32 nRC = m_ccrrFunc.GetParam (m_hCCRR, nID, pValue);

	return nRC;
}


VO_U32 CCCRRRFunc::LoadLib (VO_HANDLE hInst)
{
#ifdef _LIB
	voGetVideoCCRRRAPI(&m_ccrrFunc, 0);
#elif defined _IOS
	voGetVideoCCRRRAPI(&m_ccrrFunc, 0);
#else	//_LIB
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_pConfig != NULL)
	{
		if (m_bSoft)
		{
			pDllFile = m_pConfig->GetItemText ((char*)"VideoRender", (char*)"SoftCCRRFile");
			pApiName = m_pConfig->GetItemText ((char*)"VideoRender", (char*)"SoftCCRRApi");
		}
		else
		{
			pDllFile = m_pConfig->GetItemText ((char*)"VideoRender", (char*)"CCRRFile");
			pApiName = m_pConfig->GetItemText ((char*)"VideoRender", (char*)"CCRRApi");
		}
	}

#ifdef _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
		vostrcat (m_szDllFile, _T(".Dll"));
	}
	else
	{
		vostrcpy (m_szDllFile, _T("voMMCCRRS.Dll"));
	}

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
	else
	{
		vostrcpy (m_szAPIName, _T("voGetVideoCCRRRAPI"));
	}
#elif defined LINUX
	if (pDllFile != NULL)
	{
		vostrcpy (m_szDllFile, pDllFile);
		vostrcat (m_szDllFile, _T(".so"));
	}
	else
	{
		vostrcpy (m_szDllFile, _T("voMMCCRRS.so"));
	}

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
	else
		vostrcpy (m_szAPIName, _T("voGetVideoCCRRRAPI"));
#elif defined _MAC_OS
	if (pDllFile != NULL)
	{
		vostrcpy (m_szDllFile, pDllFile);
		vostrcat (m_szDllFile, _T(".dylib"));
	}
	else
	{
		vostrcpy (m_szDllFile, _T("voMMCCRRS.dylib"));
	}

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
	else
		vostrcpy (m_szAPIName, _T("voGetVideoCCRRRAPI"));
#elif defined __SYMBIAN32__
	if (pDllFile != NULL)
	{
		vostrcpy (m_szDllFile, pDllFile);
		vostrcat (m_szDllFile, _T(".dll"));
	}
	else
	{
		vostrcpy (m_szDllFile, _T("voMMCCRRS.dll"));
	}

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
	else
		vostrcpy (m_szAPIName, _T("voGetVideoCCRRRAPI"));
#endif


	VOLOGI ("Dll File %s, API %s", m_szDllFile, m_szAPIName);

	if (CDllLoad::LoadLib (hInst) == 0)
		return 0;

	VOMMGETAPI pAPI = (VOMMGETAPI) m_pAPIEntry;
	pAPI (&m_ccrrFunc, 0);
#endif	//_LIB

	return 1;
}

