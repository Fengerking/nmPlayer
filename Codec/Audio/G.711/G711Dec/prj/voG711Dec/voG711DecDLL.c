#include <windows.h>

extern void* g_G711dec_inst;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_G711dec_inst = (void*)hModule;
    return TRUE;
}

