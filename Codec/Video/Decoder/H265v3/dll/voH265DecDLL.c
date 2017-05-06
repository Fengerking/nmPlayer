#include <windows.h>

extern void* g_h265dec_inst;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_h265dec_inst = (void*)hModule;
    return TRUE;
}

