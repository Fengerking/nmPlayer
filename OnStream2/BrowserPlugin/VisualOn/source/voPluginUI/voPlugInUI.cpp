#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "voPlugInUI.h"

#include "CPlugInUIWnd.h"

HINSTANCE		g_hInst = NULL;

#ifdef _WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	g_hInst = (HINSTANCE)hModule;
    return TRUE;
}
#endif // _WIN32

int voPlugInUI_Init (VOPUI_INIT_PARAM * pParam, void ** phHandle)
{
	*phHandle = NULL;

	if (pParam == NULL)
		return -1;

	if (pParam->hInst == NULL)
		pParam->hInst = g_hInst;


	CPlugInUIWnd * pWnd = new CPlugInUIWnd (pParam);
	if (pWnd == NULL)
		return -1;

	*phHandle = pWnd;

	return 0;
}

int voPlugInUI_Uninit (void * hHandle)
{
	CPlugInUIWnd * pWnd = (CPlugInUIWnd *)hHandle;
	if (pWnd) 
		delete pWnd;

	pWnd = NULL;
	return 0;
}

void * voPlugInUI_GetView (void * hHandle)
{
	CPlugInUIWnd * pWnd = (CPlugInUIWnd *)hHandle;
	if (pWnd == NULL)
		return NULL;

	return pWnd->GetView ();
}

int voPlugInUI_ShowFullScreen (void * hHandle)
{
	CPlugInUIWnd * pWnd = (CPlugInUIWnd *)hHandle;
	if (pWnd == NULL)
		return -1;

	return pWnd->ShowFullScreen ();
}

int voPlugInUI_ShowControls (void * hHandle, bool bShow)
{
	CPlugInUIWnd * pWnd = (CPlugInUIWnd *)hHandle;
	if (pWnd == NULL)
		return -1;

	return pWnd->ShowControls (bShow);
}

int voPlugInUI_SetParam (void * hHandle, int nID, void * pParam)
{
	CPlugInUIWnd * pWnd = (CPlugInUIWnd *)hHandle;
	if (pWnd == NULL)
		return -1;

	return pWnd->SetParam (nID, pParam);
}

int voPlugInUI_GetParam (void * hHandle, int nID, void * pParam)
{
	CPlugInUIWnd * pWnd = (CPlugInUIWnd *)hHandle;
	if (pWnd == NULL)
		return -1;

	return pWnd->GetParam (nID, pParam);
}


int voGetPlugInUIAPI (VOPUI_OVERLAY_API * pAPI, int uFlag)
{
	if (pAPI == NULL)
		return -1;

	pAPI->Init = voPlugInUI_Init;
	pAPI->Uninit = voPlugInUI_Uninit;
	pAPI->GetView = voPlugInUI_GetView;
	pAPI->ShowFullScreen = voPlugInUI_ShowFullScreen;
	pAPI->ShowControls = voPlugInUI_ShowControls;
	pAPI->GetParam = voPlugInUI_GetParam;
	pAPI->SetParam = voPlugInUI_SetParam;

	return 0;
}

