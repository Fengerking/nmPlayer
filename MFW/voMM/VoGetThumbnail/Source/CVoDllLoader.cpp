#include "windows.h"
#include "CVoDllLoader.h"
#include "voString.h"
CVoDllLoader* CVoDllLoader::g_dllLoader = NULL;

CVoDllLoader::CVoDllLoader(void)
{
	g_dllLoader = this;
}

CVoDllLoader::~CVoDllLoader(void)
{
	map<wstring, HMODULE>::iterator  iter;
	for(iter = m_mapDll.begin(); iter != m_mapDll.end(); iter++)
	{
		HMODULE hDll = iter->second;
		if (hDll != NULL)
			FreeLibrary (hDll);
	}
	//m_mapDll.
	//m_mapDll.

}

HMODULE CVoDllLoader::GetDllHandle(wstring strDllName)
{
	HMODULE hDll = NULL;
	map<wstring, HMODULE>::iterator  iter;
	iter = m_mapDll.find(strDllName);

	if(iter != m_mapDll.end())
		hDll = iter->second;
	//else
	//{
	//	hDll = Load(strDllName);
	//	m_mapDll.insert(pair<>)
	//}

	return hDll;
}

void CVoDllLoader::SetDllHandle(wstring strDllName, HMODULE hDll)
{
	map<wstring, HMODULE>::iterator  iter;
	iter = m_mapDll.find(strDllName);

	if(iter == m_mapDll.end())
		m_mapDll.insert(pair<wstring, HMODULE>(strDllName,hDll));;
}
//HMODULE CVoDllLoader::Load(wstring strDllName)
//{
//	HMODULE hDll = NULL;

	//VO_TCHAR szDll[MAX_PATH];
	//VO_TCHAR szPath[MAX_PATH];

	//GetModuleFileName (NULL, szPath, sizeof (szPath));
	//VO_TCHAR * pPos = vostrrchr (szPath, _T('\\'));
	//*(pPos + 1) = 0;

	//vostrcpy (szDll, szPath);
	//vostrcat (szDll,  strDllName.c_str());
	//hDll = LoadLibrary (szDll);

	//if (hDll == NULL)
	//{
	//	vostrcpy (szDll, szPath);
	//	vostrcat (szDll, _T("Dll\\"));
	//	vostrcat (szDll, strDllName.c_str());
	//	hDll = LoadLibrary (szDll);
	//}

	//if (hDll == NULL)
	//{
	//	hDll = LoadLibrary (strDllName.c_str());
	//}

//	return hDll;
//
//
//}