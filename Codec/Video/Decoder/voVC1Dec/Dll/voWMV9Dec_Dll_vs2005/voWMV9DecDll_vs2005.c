#include <windows.h>

extern void* g_VC1_hModule;	//for CheckLcs lib.

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{  
	g_VC1_hModule = hModule;
    return TRUE;
}
