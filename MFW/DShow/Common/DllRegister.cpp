	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		DllRegister.cpp

	Contains:	DllRegister file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-03-23		JBF			Create file

*******************************************************************************/
#include <streams.h>

STDAPI DllRegisterServer()
{
#ifdef _WIN32_WINNT
	return AMovieDllRegisterServer2(TRUE);
#else
	return AMovieDllRegisterServer2(TRUE);
#endif //_WIN32_WINNT
}

STDAPI DllUnregisterServer()
{
#ifdef _WIN32_WINNT
	return AMovieDllRegisterServer2(FALSE);
#else
	return AMovieDllUnregisterServer();
#endif //_WIN32_WINNT
}


//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}

