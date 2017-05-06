#include "voVideoRender.h"
#include "CBaseVideoRender.h"

#ifdef _WIN32
#include <windows.h>
#include ".\Win32\CGDIVideoRender.h"
#elif defined LINUX
# if defined (__VOVR_FB__)
#   include "CFBVideoRender.h"
#elif defined(__VOVR_SURFACE__)
#    include "CSurfaceVRender.h"
#elif defined(__VOVR_V4L2__)
#    include "voV4L2Render.h"
# endif
#endif


#define CHECK_RENDER_POINT if (hRender == NULL)\
	return VO_ERR_INVALID_ARG | VO_INDEX_SNK_VIDEO;\
CBaseVideoRender * pRender = (CBaseVideoRender *)hRender;

VO_PTR		g_hvovrInst = NULL;

#if defined _WIN32 && !defined _LIB
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hvovrInst = hModule;
    return TRUE;
}
#endif // _WIN32


VO_U32 vovrInit (VO_HANDLE * phRender, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag)
{
	CBaseVideoRender * pRender = NULL;

#ifdef _WIN32
	pRender = new CGDIVideoRender (g_hvovrInst, hView, pMemOP);
#elif defined LINUX
#  if defined (__VOVR_FB__)
	pRender = new CFBVideoRender (g_hvovrInst, hView, pMemOP);
#elif defined(__VOVR_SURFACE__)
    pRender = new CSurfaceVRender (g_hvovrInst, hView, pMemOP);
#elif defined(__VOVR_V4L2__)
    pRender = new voV4L2Render (g_hvovrInst, hView, pMemOP);
#  endif // __VOVR_FB__

#endif

	if (pRender == NULL)
		return VO_ERR_OUTOF_MEMORY | VO_INDEX_SNK_VIDEO;

	*phRender = pRender;

	return VO_ERR_NONE;
}

VO_U32 vovrUninit (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	delete pRender;

	return VO_ERR_NONE;
}

VO_U32 vovrSetVideoInfo (VO_HANDLE hRender, VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	CHECK_RENDER_POINT

	return pRender->SetVideoInfo (nWidth, nHeight, nColor);
}

VO_U32 vovrSetDispRect (VO_HANDLE hRender, VO_PTR hView, VO_RECT * pDispRect)
{
	CHECK_RENDER_POINT

	return pRender->SetDispRect (hView, pDispRect);
}

VO_U32 vovrSetDispType (VO_HANDLE hRender, VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	CHECK_RENDER_POINT

	return pRender->SetDispType (nZoomMode, nRatio);
}

VO_U32 vovrStart (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Start ();
}

VO_U32 vovrPause (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Pause ();
}

VO_U32 vovrStop (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Stop ();
}

VO_U32 vovrRender (VO_HANDLE hRender, VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	CHECK_RENDER_POINT

	return pRender->Render (pVideoBuffer, nStart, bWait);
}

VO_U32 vovrWaitDone (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->WaitDone ();
}

VO_U32 vovrRedraw (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Redraw ();
}

VO_U32 vovrSetCallBack (VO_HANDLE hRender, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	CHECK_RENDER_POINT

	return pRender->SetCallBack (pCallBack, pUserData);
}

VO_U32 vovrGetVideoMemOP (VO_HANDLE hRender, VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	CHECK_RENDER_POINT

	return pRender->GetVideoMemOP (ppVideoMemOP);
}

VO_U32 vovrSetParam (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue)
{
	CHECK_RENDER_POINT

	return pRender->SetParam (nID, pValue);
}

VO_U32 vovrGetParam (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue)
{
	CHECK_RENDER_POINT

	return pRender->GetParam (nID, pValue);
}

VO_S32 voGetVideoRenderAPI (VO_VIDEO_RENDERAPI * pRender, VO_U32 uFlag)
{
	if (pRender == NULL)
		return VO_ERR_INVALID_ARG | VO_INDEX_SNK_VIDEO;

	pRender->Init = vovrInit;
	pRender->Uninit = vovrUninit;
	pRender->SetVideoInfo = vovrSetVideoInfo;
	pRender->SetDispRect = vovrSetDispRect;
	pRender->SetDispType = vovrSetDispType;
	pRender->Start = vovrStart;
	pRender->Pause = vovrPause;
	pRender->Stop = vovrStop;
	pRender->Render = vovrRender;
	pRender->WaitDone = vovrWaitDone;
	pRender->Redraw = vovrRedraw;
	pRender->SetCallBack = vovrSetCallBack;
	pRender->GetVideoMemOP = vovrGetVideoMemOP;
	pRender->SetParam = vovrSetParam;
	pRender->GetParam = vovrGetParam;

	return VO_ERR_NONE;
}
