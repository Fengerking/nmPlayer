// voMPEG2DecDLL_vs2005.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <windows.h>
#include "voMpeg2Decoder.h"

BOOL APIENTRY DllMain(HANDLE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	g_hMPEG2DecInst = hModule;
    return TRUE;
}

