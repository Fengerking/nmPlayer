#include <windows.h>

extern void* g_WMV_hModule;	//for CheckLcs lib.

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{  
	g_WMV_hModule = hModule;
    return TRUE;
}
