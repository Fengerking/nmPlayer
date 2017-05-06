// voAMRWBPDecDLL.cpp : Defines the entry point for the DLL application.
//
#include "voAMRWPDecID.h"
#include "stdafx.h"
#include <windows.h>
#include <commctrl.h>
#include "voChHdle.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
#ifdef LCHECK 
    g_hAMRWBPDecInst = hModule;
#endif
	return TRUE;
}

