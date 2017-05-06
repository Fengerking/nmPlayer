	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COverlayUI.h

	Contains:	COverlayUI header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2011-01-30		JBF			Create file

*******************************************************************************/
#ifndef __COverlayUI_H__
#define __COverlayUI_H__

#ifdef _MAC_OS 
#include <unistd.h>
#include <fcntl.h>
#elif defined _WIN32
#include <windows.h>
#include <tchar.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#endif

#ifdef _WIN32
#include <windows.h>
#elif defined _LINUX
#include <dlfcn.h>
#elif defined _MAC_OS
#include <dlfcn.h>
#elif defined _IOS
#include <dlfcn.h>
#elif defined __SYMBIAN32__
#include <e32base.h>
#include <e32std.h>
#endif // _WIN32

#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "voString.h"
#include "..\voPluginUI\voPluginUI.h"

class COverlayUI
{
public:
	// Used to control the image drawing
	COverlayUI (void);
	virtual ~COverlayUI (void);

	int			Init (VO_PTCHAR pLibPath, VOPUI_INIT_PARAM * pParam);

	void *	GetView (void);
	int			ShowFullScreen (void);
	int			ShowControls (bool bShow);
	int			SetParam (int nID, void * pParam);
	int			GetParam (int nID, void * pParam);

protected:
	int			LoadModule (VO_PTCHAR path);
	void		Release();

#ifndef _WIN32
	void *				m_hDll;
#else
	HMODULE				m_hDll;
#endif

	void *				m_hUI;
	VOGETPLUGINUIAPI	m_fGetAPI;
	VOPUI_OVERLAY_API	m_OverlayAPI;
};

#endif // __COverlayUI_H__

