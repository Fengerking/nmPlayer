#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "voMMPlay.h"
#include "CBaseConfig.h"

#include "CDumpPlayFile.h"

#define CHECK_PLAY_POINT if (hPlay == NULL)\
	return VO_ERR_INVALID_ARG;\
CBasePlay * pPlay = (CBasePlay *)hPlay;

VO_PTR	g_hvommPlayInst = NULL;

#if defined _WIN32 && !defined _LIB
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hvommPlayInst = hModule;
    return TRUE;
}
#endif // _WIN32


VO_U32 vommPlayInit (VO_HANDLE * phPlay, VO_VOMM_INITPARAM * pParam)
{
	CBasePlay * pPlay = NULL;

	VO_MEM_OPERATOR *	pMemOP = NULL;
	VO_FILE_OPERATOR *	pFileOP = NULL;
	if (pParam != NULL)
	{
		pMemOP = pParam->pMemOP;
		pFileOP = pParam->pFileOP;
	}

	CBaseConfig * pConfig = new CBaseConfig ();
	if (pConfig == NULL)
		return VO_ERR_OUTOF_MEMORY;

	pConfig->Open ((TCHAR*)(_T("vowplay.cfg")), pFileOP);

	if (pConfig->GetItemText ((char*)"vowPlay", (char*)"DumpLogFile") != NULL)
		pPlay = new CDumpPlayFile (g_hvommPlayInst, pMemOP, pFileOP);
	else
		pPlay = new CPlayFile (g_hvommPlayInst, pMemOP, pFileOP);

	if (pPlay == NULL)
	{
		delete pConfig;
		return VO_ERR_OUTOF_MEMORY;
	}

	pPlay->SetConfig (pConfig);
	pPlay->SetParam (VO_PID_COMMON_LIBOP, pParam->pLibOP);
	pPlay->SetDrmCB (pParam->pDrmCB);

	*phPlay = pPlay;

	return VO_ERR_NONE;
}

VO_U32 vommPlayUninit (VO_HANDLE hPlay)
{
	CHECK_PLAY_POINT

	delete pPlay;

	return VO_ERR_NONE;
}

VO_U32 vommPlaySetCallBack (VO_HANDLE hPlay, VOMMPlayCallBack pCallBack, VO_PTR pUserData)
{
	CHECK_PLAY_POINT

	return pPlay->SetCallBack (pCallBack, pUserData);
}


VO_U32 vommPlaySetViewInfo (VO_HANDLE hPlay, VO_PTR hView, VO_RECT * pRect)
{
	CHECK_PLAY_POINT

	return pPlay->SetViewInfo (hView, pRect);
}

VO_U32 vommPlayCreate (VO_HANDLE hPlay, VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
{
	CHECK_PLAY_POINT

	return pPlay->Create (pSource, nType, nOffset, nLength);
}

VO_U32 vommPlayRun (VO_HANDLE hPlay)
{
	CHECK_PLAY_POINT

	return pPlay->Run ();
}

VO_U32 vommPlayPause (VO_HANDLE hPlay)
{
	CHECK_PLAY_POINT

	return pPlay->Pause ();
}

VO_U32 vommPlayStop (VO_HANDLE hPlay)
{
	CHECK_PLAY_POINT

	return pPlay->Stop ();
}

VO_U32 vommPlayGetDuration (VO_HANDLE hPlay, VO_U32 * pDuration)
{
	CHECK_PLAY_POINT

	return pPlay->GetDuration(pDuration);
}

VO_U32 vommPlayGetCurPos (VO_HANDLE hPlay, VO_S32 * pPos)
{
	CHECK_PLAY_POINT

	return pPlay->GetCurPos (pPos);
}

VO_U32 vommPlaySetCurPos (VO_HANDLE hPlay, VO_S32 nPos)
{
	CHECK_PLAY_POINT

	return pPlay->SetCurPos (nPos);
}

VO_U32 vommPlaySetParam (VO_HANDLE hPlay, VO_U32 nID, VO_PTR pValue)
{
	CHECK_PLAY_POINT

	return pPlay->SetParam (nID, pValue);
}

VO_U32 vommPlayGetParam (VO_HANDLE hPlay, VO_U32 nID, VO_PTR pValue)
{
	CHECK_PLAY_POINT

	return pPlay->GetParam (nID, pValue);
}

VO_S32 vommGetPlayAPI (VOMM_PLAYAPI * pPlay, VO_U32 uFlag)
{
	if (pPlay == NULL)
		return VO_ERR_INVALID_ARG;

	pPlay->Init = vommPlayInit;
	pPlay->Uninit = vommPlayUninit;
	pPlay->SetCallBack = vommPlaySetCallBack;
	pPlay->SetViewInfo = vommPlaySetViewInfo;
	pPlay->Create = vommPlayCreate;
	pPlay->Run = vommPlayRun;
	pPlay->Pause = vommPlayPause;
	pPlay->Stop = vommPlayStop;
	pPlay->GetDuration = vommPlayGetDuration;
	pPlay->GetCurPos = vommPlayGetCurPos;
	pPlay->SetCurPos = vommPlaySetCurPos;
	pPlay->SetParam = vommPlaySetParam;
	pPlay->GetParam = vommPlayGetParam;

	return VO_ERR_NONE;
}
