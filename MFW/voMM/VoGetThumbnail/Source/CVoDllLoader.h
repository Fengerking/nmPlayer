#pragma once
# include <map>
# include <string>
using namespace std;

class CVoDllLoader
{
public:
	CVoDllLoader(void);
	~CVoDllLoader(void);

	map<wstring, HMODULE> m_mapDll;

	HMODULE GetDllHandle(wstring strDllName);
	void SetDllHandle(wstring strDllName, HMODULE hDll);

	static CVoDllLoader* g_dllLoader;
protected:
	//HMODULE Load(wstring strDllName);

};
