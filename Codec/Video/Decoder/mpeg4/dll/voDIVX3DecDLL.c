#include <windows.h>

extern void* voDIVX3DechInst;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	voDIVX3DechInst = (void*)hModule;
    return TRUE;
}

