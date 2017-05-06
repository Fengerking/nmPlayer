
#include "DllLoader.h"

CDllLoader::CDllLoader(void)
:m_hModule(NULL)
{
	memset(m_strApiName,0, sizeof(m_strApiName));
	memset(m_strDllFile,0, sizeof(m_strDllFile));

}

CDllLoader::~CDllLoader(void)
{
	Unload();
}

void CDllLoader::Unload()
{
	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}
void CDllLoader::Load()
{
// 	TCHAR szParserName[128];
// 	memset(szParserName, 0, 128);
// 
// _tcscpy(szParserName, _T("voVideoEditor.dll"));
	TCHAR szDllName[128];
	MultiByteToWideChar (CP_ACP, 0, m_strDllFile, -1, szDllName, sizeof (szDllName));
	m_hModule = ::LoadLibrary(szDllName);
	//char szApiName[128];
	//WideCharToMultiByte (CP_ACP, 0,  m_strApiName, -1, szApiName, 128, NULL, NULL);
	m_api = GetProcAddress(m_hModule, m_strApiName);

	
}