// dradecdll.cpp : Defines the entry point for the DLL application.
//

#include <windows.h>
#include <commctrl.h>
#include "voChHdle.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    g_hDRADecInst = hModule;
	return TRUE;
}

