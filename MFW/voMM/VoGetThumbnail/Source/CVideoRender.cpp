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
#include <stdio.h>
#include "CVideoRender.h"

#include "COutVideoRender.h"

#ifdef _WIN32
#include "CGDIVideoRender.h"
#ifdef _VO_FILEPARSER
#include "CDDVideoRender.h"
#endif // _VO_FILEPARSER
#endif // _WIN32

#if defined(__VOTT_PC__) && defined(LINUX)
#   include "CSDLDraw.h"
#endif 

#include "voLog.h"

typedef VO_S32 (VO_API * VOGETVIDEORENDERAPI) (VO_VIDEO_RENDERAPI * pRender, VO_U32 nFlag);

CVideoRender::CVideoRender (VO_PTR hInst, VO_PTR hView, VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	, m_hView (hView)
	, m_hRender (NULL)
	, m_pCallBack (NULL)
	, m_pUserData (NULL)
	, m_pRender (NULL)
{
	VOLOGF ();

	m_fmtVideo.Width = 0;
	m_fmtVideo.Height = 0;

#ifdef _WIN32
#ifdef _VO_FILEPARSER
	m_pRender = new CDDVideoRender (m_hInst, m_hView, m_pMemOP);
#else
	m_pRender = new CGDIVideoRender (m_hInst, m_hView, m_pMemOP);
#endif // _VO_FILEPARSER
#endif // _WIN32

#if defined(__VOTT_PC__) && defined(LINUX)
	m_pRender = new CSDLDraw (m_hInst, m_hView, m_pMemOP);
#endif 

}

CVideoRender::~CVideoRender ()
{
	VOLOGF ();

	Uninit ();

	if (m_pRender != NULL)
		delete m_pRender;
}

VO_U32 CVideoRender::Init (VO_VIDEO_FORMAT * pFormat)
{
	VOLOGF ();

	if (m_pRender == NULL)
	{
#if defined(LINUX)
		m_pRender = new COutVideoRender (m_hInst, m_hView, m_pMemOP);
#else
		m_pRender = new COutVideoRender (m_hInst, m_hView, m_pMemOP);
#endif // LINUX
	}
	m_pRender->SetCallBack (m_pCallBack, m_pUserData);
	SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);
	//m_pRender->SetWorkPath (m_pWorkPath);

	if (m_pRender != NULL)
		return m_pRender->SetVideoInfo ( pFormat->Width, pFormat->Height, VO_COLOR_YUV_PLANAR420);

	Uninit ();

	if (LoadLib (m_hInst) == 0)
		return 0;

	VO_U32 nRC = m_funRender.Init (&m_hRender, m_hView, m_pMemOP, 0);

	nRC = m_funRender.SetVideoInfo (m_hRender, pFormat->Width, pFormat->Height, VO_COLOR_YUV_PLANAR420);

	return VO_ERR_NONE;
}

VO_U32 CVideoRender::Uninit (void)
{
	VOLOGF ();

	if (m_hRender != NULL)
	{
		m_funRender.Uninit (m_hRender);
		m_hRender = NULL;
	}

	return 0;
}

VO_U32 CVideoRender::SetVideoInfo (VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	VOLOGF ();

	m_fmtVideo.Width = nWidth;
	m_fmtVideo.Height = nHeight;

	if (m_pRender != NULL)
		return m_pRender->SetVideoInfo (nWidth, nHeight, nColor);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.SetVideoInfo (m_hRender, nWidth, nHeight, nColor);
}

VO_U32 CVideoRender::SetDispRect (VO_PTR hView, VO_RECT * pDispRect)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->SetDispRect (hView, pDispRect);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.SetDispRect (m_hRender, hView, pDispRect);
}

VO_U32 CVideoRender::SetDispType (VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->SetDispType (nZoomMode, nRatio);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.SetDispType (m_hRender, nZoomMode, nRatio);
}

VO_U32 CVideoRender::Start(void)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->Start ();

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Start (m_hRender);
}

VO_U32 CVideoRender::Pause(void)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->Pause ();

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Pause (m_hRender);
}

VO_U32 CVideoRender::Stop(void)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->Stop ();

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Stop (m_hRender);
}

VO_U32 CVideoRender::Render (VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->Render (pVideoBuffer, nStart,  bWait);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Render (m_hRender, pVideoBuffer, nStart,  bWait);
}

VO_U32 CVideoRender::WaitDone (void)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->WaitDone ();

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.WaitDone (m_hRender);
}

VO_U32 CVideoRender::Redraw (void)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->Redraw ();

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.Redraw (m_hRender);
}

VO_U32 CVideoRender::SetCallBack (VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	VOLOGF ();

	m_pCallBack = pCallBack;
	m_pUserData = pUserData;

	if (m_pRender != NULL)
	{
		return m_pRender->SetCallBack (pCallBack, pUserData);
	}
	else
	{
		if (m_hRender == 0)
			return VO_ERR_WRONG_STATUS;

		return 	m_funRender.SetCallBack (m_hRender, pCallBack, pUserData);
	}
}

VO_U32 CVideoRender::GetVideoMemOP (VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->GetVideoMemOP (ppVideoMemOP);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.GetVideoMemOP (m_hRender, ppVideoMemOP);
}

void CVideoRender::SetLibOperator (VO_LIB_OPERATOR * pLibOP)
{
	m_pLibOP = pLibOP;

	SetParam (VO_PID_COMMON_LIBOP, m_pLibOP);
}

VO_U32 CVideoRender::SetParam(VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->SetParam (nID, pValue);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.SetParam (m_hRender, nID, pValue);
}

VO_U32 CVideoRender::GetParam(VO_U32 nID, VO_PTR pValue)
{
	VOLOGF ();

	if (m_pRender != NULL)
		return m_pRender->GetParam (nID, pValue);

	if (m_hRender == 0)
		return VO_ERR_WRONG_STATUS;

	return 	m_funRender.GetParam (m_hRender, nID, pValue);
}

VO_U32 CVideoRender::LoadLib (VO_HANDLE hInst)
{
	VOLOGF ();

	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;

	if (m_pConfig != NULL)
	{
		pDllFile = m_pConfig->GetItemText ((char*)"VideoRender", (char*)"RenderFile");
		pApiName = m_pConfig->GetItemText ((char*)"VideoRender", (char*)"RenderApi");
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
		vostrcpy (m_szDllFile, _T("voMMVR.Dll"));
	}

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
	else
	{
		vostrcpy (m_szAPIName, _T("voGetVideoRenderAPI"));
	}
#elif defined LINUX
	if (pDllFile != NULL)
	{
		vostrcpy (m_szDllFile, pDllFile);
		vostrcat (m_szDllFile, _T(".so"));
	}
	else
		vostrcpy (m_szDllFile, _T("voMMVR.so"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
	else
		vostrcpy (m_szAPIName, _T("voGetVideoRenderAPI"));
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
		return 0;

	VOGETVIDEORENDERAPI pAPI = (VOGETVIDEORENDERAPI) m_pAPIEntry;
	pAPI (&m_funRender, 0);

	return 1;
}
