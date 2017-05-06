#pragma once

#include <Windows.h>

class CBaseModule
{
public:
	CBaseModule(void);
	virtual ~CBaseModule(void);

protected:
	virtual bool GetFunc()=0;

protected:
	HMODULE LoadDll(const TCHAR * pDllFile);

private:
	HMODULE				m_hDll;
};
