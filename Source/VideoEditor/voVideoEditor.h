// voVideoEditor.h : main header file for the voVideoEditor DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CvoVideoEditorApp
// See voVideoEditor.cpp for the implementation of this class
//

class CvoVideoEditorApp : public CWinApp
{
public:
	CvoVideoEditorApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
