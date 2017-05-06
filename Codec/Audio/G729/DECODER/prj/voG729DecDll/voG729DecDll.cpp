// voG729DecDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#ifdef _WIN32
#include "windows.h"
#include "g729dec_api.h"

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hG729DecInst = hModule;
    return TRUE;
}

#endif

