
#include "CBaseModule.h"
#include <tchar.h>

CBaseModule::CBaseModule(void)
:m_hDll(NULL)
{
}

CBaseModule::~CBaseModule(void)
{
	if (m_hDll)
	{
		FreeLibrary(m_hDll);
		m_hDll = NULL;
	}
}

HMODULE CBaseModule::LoadDll (const TCHAR * pDllFile)
{
	HMODULE hDll = NULL;
	HINSTANCE hInst = NULL;

	TCHAR szDll[MAX_PATH];
	TCHAR szPath[MAX_PATH];
	::GetModuleFileName (hInst, szPath, sizeof (szPath));
	TCHAR * pPos = _tcsrchr (szPath, _T('\\'));
	*(pPos + 1) = 0;

	_tcscpy (szDll, szPath);
	_tcscat (szDll, pDllFile);
	hDll = LoadLibrary (szDll);

	if (hDll == NULL)
		hDll = LoadLibrary (pDllFile);

	return hDll;
}
