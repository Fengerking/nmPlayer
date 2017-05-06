// voPlugInIE.cpp : Implementation of CvoPlugInIEApp and DLL registration.

#include "stdafx.h"
#include "voPlugInIE.h"
#include "comcat.h"
#include "Objsafe.h"

#include "voLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const GUID CDECL CLSID_SafeItem =
{ 0xA23308FB, 0x10C, 0x4035, { 0x9E, 0x2D, 0x27, 0xE3, 0xA7, 0x5, 0x71, 0xA7 } };

CvoPlugInIEApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x3F09DD6A, 0xA436, 0x4023, { 0x9E, 0xFA, 0x7D, 0x7E, 0xE, 0x7, 0xFF, 0x16 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

HINSTANCE g_hInstance(NULL);

// CvoPlugInIEApp::InitInstance - DLL initialization

BOOL CvoPlugInIEApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	g_hInstance = AfxGetInstanceHandle();

    TCHAR szAppPath[MAX_PATH] = _T("");
    GetModuleFileName(g_hInstance,szAppPath, MAX_PATH);
    VO_PTCHAR pPos = vostrrchr(szAppPath, _T('/'));
    if (pPos == NULL)
        pPos = vostrrchr(szAppPath, _T('\\'));
    VO_S32 nPos = pPos - szAppPath;
    szAppPath[nPos+1] = _T('\0');

    VOLOGINIT(szAppPath);

	return bInit;
}



// CvoPlugInIEApp::ExitInstance - DLL termination

int CvoPlugInIEApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

    VOLOGI("ExitInstance");

    VOLOGUNINIT();

	return COleControlModule::ExitInstance();
}

// Create a component type
HRESULT CreateComponentCategory(CATID catid, WCHAR* catDescription)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (FAILED(hr))
		return hr;

	// Make sure the HKCR\Component Categories\{..catid...}
	// key is registered.
	CATEGORYINFO catinfo;
	catinfo.catid = catid;
	catinfo.lcid = 0x0409 ; // english

	// Make sure the provided description is not too long.
	// Only copy the first 127 characters if it is.
	int len = wcslen(catDescription);
	if (len>127)
		len = 127;
	wcsncpy(catinfo.szDescription, catDescription, len);
	// Make sure the description is null terminated.
	catinfo.szDescription[len] = '\0';

	if (pcr) {
		hr = pcr->RegisterCategories(1, &catinfo);
		pcr->Release();
	}

	return hr;
}

// register a component type
HRESULT RegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	// Register your component categories information.
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;
	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Register this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		if (pcr != NULL)
			hr = pcr->RegisterClassImplCategories(clsid, 1, rgcatid);
	}
	if (pcr != NULL)
		pcr->Release();
	return hr;
}

// unregister  a component type
HRESULT UnRegisterCLSIDInCategory(REFCLSID clsid, CATID catid)
{
	ICatRegister* pcr = NULL ;
	HRESULT hr = S_OK ;

	hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, 
		NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
	if (SUCCEEDED(hr))
	{
		// Unregister this category as being "implemented" by the class.
		CATID rgcatid[1] ;
		rgcatid[0] = catid;
		if (pcr != NULL)
			hr = pcr->UnRegisterClassImplCategories(clsid, 1, rgcatid);
	}

	if (pcr != NULL)
		pcr->Release();

	return hr;
}

// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	//add start
	HRESULT hr;

	// Mark the control safe for initialization.
	// Create a init component
	hr = CreateComponentCategory(CATID_SafeForInitializing,
		L"Controls safely initializable from persistent data!");
	if (FAILED(hr))
		return hr;
	// register a init component
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	// Mark the control's scripting safe for initialization.
	//  Create a safe scripting component
	hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely scriptable!");
	if (FAILED(hr))
		return hr;
	// register a safe scripting component
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	HRESULT hr;
	// unregister a init component.
	hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;
	// unregister a safe script component
	hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;


	return NOERROR;
}
