#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "voMMPlay.h"
#include "voGetVideoThumbnail.h"
#include "CBaseConfig.h"
#include "CVoDllLoader.h"

#include "CDumpPlayFile.h"

#define CHECK_PLAY_POINT if (hPlay == NULL)\
	return VO_ERR_INVALID_ARG;\
CBasePlay * pPlay = (CBasePlay *)hPlay;

VO_PTR	g_hvommPlayInst = NULL;
CVoDllLoader dllLoader;

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hvommPlayInst = hModule;
    return TRUE;
}
#endif // _WIN32


VO_U32 vommPlayInit (VO_HANDLE * phPlay)
{
	CBasePlay * pPlay = NULL;

	pPlay = new CPlayFile (g_hvommPlayInst, NULL, NULL);

	//if (pPlay == NULL)
	//{
	//	delete pConfig;
	//	return VO_ERR_OUTOF_MEMORY;
	//}

	//pPlay->SetConfig (pConfig);

	*phPlay = pPlay;

	return VO_ERR_NONE;
}

VO_U32 vommPlayUninit (VO_HANDLE hPlay)
{
	//CHECK_PLAY_POINT
	CBasePlay * pPlay = (CBasePlay *)hPlay;

	delete pPlay;

	return VO_ERR_NONE;
}


//VO_U32 vommPlayCreate (VO_HANDLE hPlay, VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength)
//{
//	CHECK_PLAY_POINT
//
//	return pPlay->Create (pSource, nType, nOffset, nLength);
//}

VO_U32 vommPlayGetThumbnail (VO_HANDLE hPlay, VO_PTR pSource, VO_U32 nWidth, VO_U32 nHeight, VO_PTR pBitmap)
{
	//CHECK_PLAY_POINT
	CPlayFile* pFile = (CPlayFile*) hPlay;
	pFile->Create(pSource, VO_VOMM_CREATE_SOURCE_URL, NULL,NULL, nWidth, nHeight, pBitmap);

	return 0;//pPlay->GetCurPos (pPos);
}

VO_S32 vommGetPlayAPI (VOMM_PLAYAPI * pPlay, VO_U32 uFlag)
{

	return VO_ERR_NONE;
}
VO_S32 voGetThumbnailAPI (VO_THUMBNAILAPI * pGetThumbnail)
{
	if (pGetThumbnail == NULL)
		return VO_ERR_INVALID_ARG;

	pGetThumbnail->Init = vommPlayInit;
	pGetThumbnail->Uninit = vommPlayUninit;
	pGetThumbnail->GetThumbnail = vommPlayGetThumbnail;
	//pPlay->SetParam = vommPlaySetParam;
	//pPlay->GetParam = vommPlayGetParam;
	return VO_ERR_NONE;
}
