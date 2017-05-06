// voQCELPDecdll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#if 0
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
      )
{
    return TRUE;
}
#else

#ifdef _WIN32
#include <windows.h>
#include "qcelp.h"

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    g_hQCELPDecInst = hModule;
    return TRUE;
}
#endif // _WIN32
#endif //#if 0

