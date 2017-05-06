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
#endif // _WIN32

#include "voIndex.h"
#include "voString.h"
#include "CvoBaseObject.h"

typedef VO_S32 (VO_API * VOMMGETAPI) (VO_PTR pFunc, VO_U32 uFlag);

class CDllLoad : public CvoBaseObject
{
public:
	// Used to control the image drawing
	CDllLoad (void);
	virtual ~CDllLoad (void);

	virtual void		SetLibOperator (VO_LIB_OPERATOR * pLibOP) {m_pLibOP = pLibOP;}
	virtual void		SetWorkPath (VO_TCHAR * pWorkPath) {m_pWorkPath = pWorkPath;}

	virtual VO_U32		LoadLib (VO_HANDLE hInst);
	virtual VO_U32		FreeLib (void);

public:
	VO_TCHAR		m_szDllFile[256];
	VO_TCHAR		m_szAPIName[128];
	VO_PTR			m_pAPIEntry;

#ifdef _WIN32
	HMODULE			m_hDll;
#else
	VO_PTR			m_hDll;
#endif // _WIN32

	VO_LIB_OPERATOR *	m_pLibOP;
	VO_TCHAR *			m_pWorkPath;
};

#endif // __CDllLoad_H__
