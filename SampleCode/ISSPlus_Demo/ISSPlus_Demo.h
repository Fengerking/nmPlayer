
// ISSPlus_Demo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CISSPlus_DemoApp:
// See ISSPlus_Demo.cpp for the implementation of this class
//

class CISSPlus_DemoApp : public CWinAppEx
{
public:
	CISSPlus_DemoApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CISSPlus_DemoApp theApp;