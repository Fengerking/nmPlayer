#include "voAudioRender.h"
#include "CBaseAudioRender.h"

#ifdef _WIN32
#include <windows.h>
#include ".\Win32\CWaveOutRender.h"
#elif defined __VOAR_ALSA__
#include "CALSARender.h"
#elif defined __VOAR_MSM__
#include "vomsmrender.h"
#elif defined _LINUX_ANDROID
#include "CAudioTrackRender.h"
#endif // _WIN32

#define CHECK_RENDER_POINT if (hRender == NULL)\
	return VO_ERR_INVALID_ARG | VO_INDEX_SNK_AUDIO;\
CBaseAudioRender * pRender = (CBaseAudioRender *)hRender;

VO_PTR		g_hvoarInst = NULL;

#if defined _WIN32 && !defined _LIB
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hvoarInst = hModule;
    return TRUE;
}
#endif // _WIN32

VO_U32 voarInit (VO_HANDLE * phRender, VO_MEM_OPERATOR * pMemOP, VO_U32 nFlag)
{
	CBaseAudioRender * pRender = NULL;

#ifdef _WIN32
	pRender = new CWaveOutRender (g_hvoarInst, pMemOP);
#elif defined __VOAR_ALSA__
	pRender = new CALSARender(NULL, pMemOP);
#elif defined __VOAR_MSM__
	pRender = new vomsmrender(NULL, pMemOP);
#elif defined _LINUX_ANDROID
	pRender = new CAudioTrackRender (NULL, pMemOP);	
#endif

	if (pRender == NULL)
		return VO_ERR_OUTOF_MEMORY | VO_INDEX_SNK_AUDIO;

	*phRender = pRender;

	return VO_ERR_NONE;
}

VO_U32 voarUninit (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	delete pRender;

	return VO_ERR_NONE;
}

VO_U32 voarSetFormat (VO_HANDLE hRender, VO_AUDIO_FORMAT * pFormat)
{
	CHECK_RENDER_POINT

	return pRender->SetFormat (pFormat);;
}

VO_U32 voarStart (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Start ();;
}

VO_U32 voarPause (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Pause ();;
}

VO_U32 voarStop (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Stop ();;
}

VO_U32 voarRender (VO_HANDLE hRender, VO_PBYTE pBuffer, VO_U32 nSize, VO_U64 nStart, VO_BOOL bWait)
{
	CHECK_RENDER_POINT

	return pRender->Render (pBuffer, nSize, nStart, bWait);;
}

VO_U32 voarFlush (VO_HANDLE hRender)
{
	CHECK_RENDER_POINT

	return pRender->Flush ();;
}

VO_U32 voarGetPlayingTime (VO_HANDLE hRender, VO_S64	* pPlayingTime)
{
	CHECK_RENDER_POINT

	return pRender->GetPlayingTime (pPlayingTime);;
}

VO_U32 voarGetBufferTime (VO_HANDLE hRender, VO_S32	* pBufferTime)
{
	CHECK_RENDER_POINT

	return pRender->GetBufferTime (pBufferTime);;
}

VO_U32 voarSetCallBack (VO_HANDLE hRender, VOAUDIOCALLBACKPROC pCallBack, VO_PTR pUserData)
{
	CHECK_RENDER_POINT

	return pRender->SetCallBack (pCallBack, pUserData);
}

VO_U32 voarSetParam (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue)
{
	CHECK_RENDER_POINT

	return pRender->SetParam (nID, pValue);;
}

VO_U32 voarGetParam (VO_HANDLE hRender, VO_U32 nID, VO_PTR pValue)
{
	CHECK_RENDER_POINT

	return pRender->SetParam (nID, pValue);;
}

VO_S32 voGetAudioRenderAPI (VO_AUDIO_RENDERAPI * pRender, VO_U32 uFlag)
{
	if (pRender == NULL)
		return VO_ERR_INVALID_ARG;

	pRender->Init = voarInit;
	pRender->Uninit = voarUninit;
	pRender->SetFormat = voarSetFormat;

	pRender->Start = voarStart;
	pRender->Pause = voarPause;
	pRender->Stop = voarStop;
	pRender->Render = voarRender;
	pRender->Flush = voarFlush;

	pRender->GetBufferTime = voarGetBufferTime;
	pRender->GetPlayingTime = voarGetPlayingTime;

	pRender->SetCallBack = voarSetCallBack;

	pRender->GetParam = voarGetParam;
	pRender->SetParam = voarSetParam;
	
	return VO_ERR_NONE;
}
