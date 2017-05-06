// dllmain.cpp : Implementation of DllMain.

#include <tchar.h>
#include <windows.h>
#include "voLog.h"

//#pragma data_seg("flag_data")
//int app_count = 0;
//#pragma data_seg()
//#pragma comment(linker,"/SECTION:flag_data,RWS")

HINSTANCE g_hInstance(NULL);
TCHAR g_lpszPluginPath[MAX_PATH];

// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (!g_hInstance) {
		g_hInstance = hInstance;

		::GetModuleFileName(g_hInstance, g_lpszPluginPath, MAX_PATH);
		TCHAR * pPos = _tcsrchr (g_lpszPluginPath, _T('\\'));
		if (pPos != NULL)
			*(pPos + 1) = 0;
	}

    return TRUE;
}

