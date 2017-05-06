	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDllLoad.h

	Contains:	CDllLoad header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CDllLoad_H__
#define __CDllLoad_H__

#ifdef _WIN32
#include <windows.h>
#elif defined _LINUX
#include <dlfcn.h>
#include "vodlfcn.h"
#elif defined _MAC_OS
#include <dlfcn.h>
#elif defined _IOS
#include <dlfcn.h>
#elif defined __SYMBIAN32__
#include <e32base.h>
#include <e32std.h>
#endif // _WIN32

#include "voIndex.h"
#include "voString.h"
#include "CvoBaseObject.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef VO_S32 (VO_API * VOMMGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

class CDllLoad : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CDllLoad (void);
	virtual ~CDllLoad (void);

	virtual void		SetLibOperator (VO_LIB_OPERATOR * pLibOP) {m_pLibOP = pLibOP;}
	virtual void		SetWorkPath (VO_TCHAR * pWorkPath) {m_pWorkPath = pWorkPath;}
	virtual void		SetCPUNumber (int nNum) {m_nCPUNum = nNum;}
	virtual void		SetCPUVersion (int nVer);

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	virtual VO_U32		FreeLib (void);

protected:
	virtual VO_PTR		LoadVerLib (VO_TCHAR * pFile);
public:
	VO_TCHAR		m_szDllFile[256];
	VO_TCHAR		m_szAPIName[128];
	VO_PTR			m_pAPIEntry;
	VO_U32			m_nCPUNum;

#ifdef _WIN32
	HMODULE			m_hDll;
#elif defined _LINUX
	VO_PTR			m_hDll;
#elif defined _IOS || defined _MAC_OS
	VO_PTR			m_hDll;		
#elif defined __SYMBIAN32__
	RLibrary		m_cLibrary;
	VO_PTR			m_hDll;
#endif // _WIN32

	VO_LIB_OPERATOR *	m_pLibOP;
	VO_TCHAR *			m_pWorkPath;
	VO_TCHAR 			m_szVer[32];
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CDllLoad_H__
