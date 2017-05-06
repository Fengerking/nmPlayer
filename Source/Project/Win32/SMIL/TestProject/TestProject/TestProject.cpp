// TestProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include "vosmil.h"
//#include "volog.h"
typedef VO_S32 (*ptr_voGetSMILAPI)(VO_SMIL_API* pHandle);

#define _VOLOG_ERROR
int _tmain(int argc, _TCHAR* argv[])
{
	HMODULE h = 0;
	h = LoadLibrary( _T("voSMILFR.dll") );
	VO_HANDLE hSMILhandle = 0; 
	ptr_voGetSMILAPI func = (ptr_voGetSMILAPI)GetProcAddress( h , "voGetSMILAPI" );

	VO_SMIL_API api;
	func( &api );

	VO_U32 size = 100*1024;
	BYTE	buffer[100*1024] = {0};
	VO_U32 result = 0;
	VO_PTR pSmilStruct = NULL;

	FILE*    pBufferIn = fopen("1.xml", "rb");//CBSi_SMIL.xml
	api.Init(&hSMILhandle,INIT_CBS_FLAG,NULL);
	result = fread(buffer, 1, size, pBufferIn);
	api.Parse(hSMILhandle, buffer, result, &pSmilStruct);
	VO_CBS_SMIL_VIDEO* tmp = (VO_CBS_SMIL_VIDEO*)pSmilStruct;
	api.Uninit(hSMILhandle);

	fclose(pBufferIn);


	return 0;
}


