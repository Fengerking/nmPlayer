	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		source.cpp

	Contains:	get source api cpp file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#ifdef _WIN32
#  include <windows.h>
#  include <tchar.h>
#endif // _WIN32

#include "voOMXWrap.h"
#include "CBaseCompWrap.h"

#define VOME_CHECK_HANDLE if (hDec == NULL)\
	return VO_ERR_INVALID_ARG;\
CBaseCompWrap * pWrap = (CBaseCompWrap *)hDec;


VO_PTR	g_hInst = NULL;
#if defined LOG_TAG
#undef LOG_TAG
#define LOG_TAG "OMXCompWrap"
#endif

#include "voLog.h"
#include "voinfo.h"

#ifdef _WIN32
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = (VO_PTR) hModule;
    return TRUE;
}
#endif // _WIN32

VO_U32 OMXWrapVideoDecInit(VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	CBaseCompWrap * pCompWrap = new CBaseCompWrap (VO_COMPWRAP_CODEC_VideoDec, vType, pUserData);
	if (pCompWrap == NULL)
		return VO_ERR_OUTOF_MEMORY;

	*phDec = pCompWrap;

	return VO_ERR_NONE;
}

VO_U32 OMXWrapVideoDecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	VOME_CHECK_HANDLE

	return pWrap->vdSetInputData (pInput);
}

VO_U32 OMXWrapVideoDecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
	VOME_CHECK_HANDLE

	return pWrap->vdGetOutputData (pOutBuffer, pOutInfo);
}

VO_U32 OMXWrapVideoDecUninit(VO_HANDLE hDec)
{
	VOME_CHECK_HANDLE

	VOINFO("gonna delete wrapper: %p", pWrap);
	delete pWrap;

	return VO_ERR_NONE;
}

VO_U32 OMXWrapAudioDecInit(VO_HANDLE * phDec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
	CBaseCompWrap * pCompWrap = new CBaseCompWrap (VO_COMPWRAP_CODEC_AudioDec, vType, pUserData);
	if (pCompWrap == NULL)
		return VO_ERR_OUTOF_MEMORY;

	*phDec = pCompWrap;

	return VO_ERR_NONE;
}

VO_U32 OMXWrapAudioDecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
	VOME_CHECK_HANDLE

	return pWrap->vdSetInputData (pInput);
}

VO_U32 OMXWrapAudioDecGetOutputData(VO_HANDLE hDec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	VOME_CHECK_HANDLE

	return pWrap->adGetOutputData (pOutBuffer, pOutInfo);}


VO_U32 OMXWrapAudioDecUninit(VO_HANDLE hDec)
{
	VOME_CHECK_HANDLE

	delete pWrap;

	return VO_ERR_NONE;
}

VO_U32 OMXWrapSetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	VOME_CHECK_HANDLE

	return pWrap->SetParam(uParamID, pData);
}

VO_U32 OMXWrapGetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
	VOME_CHECK_HANDLE

	return pWrap->GetParam (uParamID, pData);
}

VO_S32 voGetOMXWrapVideoDecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	pDecHandle->Init = OMXWrapVideoDecInit;
	pDecHandle->SetInputData = OMXWrapVideoDecSetInputData;
	pDecHandle->GetOutputData = OMXWrapVideoDecGetOutputData;
	pDecHandle->Uninit = OMXWrapVideoDecUninit;

	pDecHandle->SetParam = OMXWrapSetParam;
	pDecHandle->GetParam = OMXWrapGetParam;

	return VO_ERR_NONE;
}

VO_S32 voGetOMXWrapAudioDecAPI (VO_AUDIO_CODECAPI * pDecHandle, VO_U32 uFlag)
{
	pDecHandle->Init = OMXWrapAudioDecInit;
	pDecHandle->SetInputData = OMXWrapAudioDecSetInputData;
	pDecHandle->GetOutputData = OMXWrapAudioDecGetOutputData;
	pDecHandle->Uninit = OMXWrapAudioDecUninit;

	pDecHandle->SetParam = OMXWrapSetParam;
	pDecHandle->GetParam = OMXWrapGetParam;

	return VO_ERR_NONE;
}

VO_U32  OMXWrapVideoRndInit(VO_HANDLE * phRender, VO_PTR hView, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag)
{
	CBaseCompWrap * pCompWrap = new CBaseCompWrap (VO_COMPWRAP_RENDER_VIDEO, nFlag, NULL);
	if (pCompWrap == NULL)
		return VO_ERR_OUTOF_MEMORY;

	*phRender = pCompWrap;

	return VO_ERR_NONE;
}

VO_U32  OMXWrapVideoRndUninit(VO_HANDLE hRender)
{
	if(hRender == NULL)
		return VO_ERR_INVALID_ARG; 

	CBaseCompWrap * pCompWrap = (CBaseCompWrap *)hRender;

	delete pCompWrap;
	pCompWrap = NULL;

	return VO_ERR_NONE;
}

VO_U32  OMXWrapVideoRndSetVideoInfo(VO_HANDLE hRender, VO_U32 nWidth, VO_U32 nHeight, VO_IV_COLORTYPE nColor)
{
	if(hRender == NULL)
		return VO_ERR_INVALID_ARG; 

	CBaseCompWrap * pCompWrap = (CBaseCompWrap *)hRender;
	pCompWrap->vrSetVideoInfo(nWidth , nHeight , nColor);

	return VO_ERR_NONE;
}
VO_U32	OMXWrapVideoRndSetDispRect(VO_HANDLE hRender, VO_PTR hView, VO_RECT * pDispRect)
{
	return VO_ERR_NONE;
}

VO_U32	OMXWrapVideoRndSetDispType(VO_HANDLE hRender, VO_IV_ZOOM_MODE nZoomMode, VO_IV_ASPECT_RATIO nRatio)
{
	return VO_ERR_NONE;
}

VO_U32	OMXWrapVideoRndStart(VO_HANDLE hRender)
{
	return OMXWrapSetParam(hRender , VO_PID_COMMON_START , NULL);
}

VO_U32	OMXWrapVideoRndPause(VO_HANDLE hRender)
{
	return OMXWrapSetParam(hRender , VO_PID_COMMON_PAUSE , NULL);
}

VO_U32  OMXWrapVideoRndStop(VO_HANDLE hRender)
{
	return OMXWrapSetParam(hRender , VO_PID_COMMON_STOP , NULL);
}

VO_U32  OMXWrapVideoRndRender(VO_HANDLE hRender, VO_VIDEO_BUFFER * pVideoBuffer, VO_S64 nStart, VO_BOOL bWait)
{
	if(hRender == NULL)
		return VO_ERR_INVALID_ARG; 

	CBaseCompWrap * pCompWrap = (CBaseCompWrap *)hRender;
	pCompWrap->vrSetInputData(pVideoBuffer);

	return VO_ERR_NONE;
}

VO_U32	OMXWrapVideoRndWaitDone(VO_HANDLE hRender)
{
	return VO_ERR_NONE;
}

VO_U32	OMXWrapVideoRndRedraw(VO_HANDLE hRender)
{
	return VO_ERR_NONE;
}

VO_U32	OMXWrapVideoRndSetCallBack(VO_HANDLE hRender, VOVIDEOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	return VO_ERR_NONE;
}

VO_U32  OMXWrapVideoRndGetVideoMemOP(VO_HANDLE hRender, VO_MEM_VIDEO_OPERATOR ** ppVideoMemOP)
{
	return VO_ERR_NONE;
}

VO_U32	OMXWrapVideoRndSetParam(VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue)
{
	return OMXWrapSetParam(hRender , nID , pValue);
}

VO_U32  OMXWrapVideoRndGetParam(VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue)
{
	return OMXWrapGetParam(hRender , nID , pValue);
}

VO_S32 voGetOMXWrapVideoRenderAPI (VO_VIDEO_RENDERAPI * pRenderHandle, VO_U32 uFlag)
{
	pRenderHandle->Init			= OMXWrapVideoRndInit;
	pRenderHandle->Uninit		= OMXWrapVideoRndUninit;
	pRenderHandle->SetVideoInfo = OMXWrapVideoRndSetVideoInfo;
	pRenderHandle->SetDispRect  = OMXWrapVideoRndSetDispRect;
	pRenderHandle->SetDispType	= OMXWrapVideoRndSetDispType;
	pRenderHandle->Start		= OMXWrapVideoRndStart;
	pRenderHandle->Pause		= OMXWrapVideoRndPause;
	pRenderHandle->Stop			= OMXWrapVideoRndStop;
	pRenderHandle->Render		= OMXWrapVideoRndRender;
	pRenderHandle->WaitDone		= OMXWrapVideoRndWaitDone;
	pRenderHandle->Redraw		= OMXWrapVideoRndRedraw;
	pRenderHandle->SetCallBack	= OMXWrapVideoRndSetCallBack;
	pRenderHandle->GetVideoMemOP= OMXWrapVideoRndGetVideoMemOP;
	pRenderHandle->SetParam		= OMXWrapVideoRndSetParam;
	pRenderHandle->GetParam		= OMXWrapVideoRndGetParam;

	return VO_ERR_NONE;
}
