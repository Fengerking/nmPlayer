// voPlugInIE.cpp : Implementation of CvoPlugInIEApp and DLL registration.

#include "stdafx.h"
#include "voPlugInIE.h"

#include "voLog.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CvoPlugInIEApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xE13FA279, 0x2F8C, 0x429F, { 0x87, 0x3B, 0xAE, 0x7, 0xB3, 0xC4, 0x74, 0x58 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CvoPlugInIEApp::InitInstance - DLL initialization

BOOL CvoPlugInIEApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}


    TCHAR szAppPath[MAX_PATH] = _T("");
    GetModuleFileName(AfxGetInstanceHandle(),szAppPath, MAX_PATH);
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

      VOLOGUNINIT();

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

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

	return NOERROR;
}
