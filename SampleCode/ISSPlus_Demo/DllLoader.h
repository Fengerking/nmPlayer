#pragma once

#include <afxwin.h>
class CDllLoader
{
public:
	CDllLoader(void);
	virtual ~CDllLoader(void);

protected:
	virtual void Load();
	virtual void Unload();
	HMODULE m_hModule;
	char m_strApiName[128];
	char m_strDllFile[128];

	FARPROC  m_api;	
};
