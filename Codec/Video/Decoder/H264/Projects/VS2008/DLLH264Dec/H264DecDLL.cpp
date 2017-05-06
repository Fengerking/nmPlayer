// H264DecDLL.cpp : Defines the entry point for the DLL application.
#include <windows.h>
extern void* hWin32Module;
//..\..\..\..\..\..\..\vorelease\bin\win32\IPP\MainProfile\voH264Dec.dll
BOOL APIENTRY DllMain( HANDLE hModule, 
													 DWORD  ul_reason_for_call, 
													 LPVOID lpReserved
													 )
{
	//switch (ul_reason_for_call)

	// {

	//     case DLL_PROCESS_ATTACH:

	//     case DLL_THREAD_ATTACH:

	//     case DLL_THREAD_DETACH:

	//     case DLL_PROCESS_DETACH:

	//break;
	//}

	hWin32Module = (HINSTANCE) hModule;
	return TRUE;
}
void vo (int i)
{

}