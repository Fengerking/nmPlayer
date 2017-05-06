// ISSControl.h : main header file for the ISSControl DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CISSControlApp
// See ISSControl.cpp for the implementation of this class
//

class CISSControlApp : public CWinApp
{
public:
	CISSControlApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
