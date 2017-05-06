#include <windows.h>

extern void* voMPEG4DechInst;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	voMPEG4DechInst = (void*)hModule;
    return TRUE;
}

