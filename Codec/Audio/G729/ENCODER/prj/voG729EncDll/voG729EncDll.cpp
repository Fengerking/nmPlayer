// voG729EncDll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#ifdef _WIN32
#include <windows.h>
#include "g729_api.h"
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hG729EncInst = hModule;
    return TRUE;
}
#endif


