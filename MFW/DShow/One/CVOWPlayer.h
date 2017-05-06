	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWPlayer.h

	Contains:	CVOWPlayer header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __CVOWPlayer_H__
#define __CVOWPlayer_H__

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif // _WIN32

#include "voMMPlay.h"
#include "voString.h"

class CVOWPlayer
{
public:
	// Used to control the image drawing
	CVOWPlayer (void);
	virtual ~CVOWPlayer (void);

	VO_U32		Init (VO_VOMM_INITPARAM * pParam, VOMMPlayCallBack pCallBack, VO_PTR pUserData);
	VO_U32		SetView (VO_PTR hView, VO_RECT * pRect);

	VO_U32 		Create (VO_PTR pURL, VO_U32 nType);
	VO_U32 		Run (void);
	VO_U32 		Pause (void);
	VO_U32 		Stop (void);
	VO_U32 		GetDuration (void);
	VO_S32 		GetCurPos (void);
	VO_U32 		SetCurPos (VO_S32 nPos);
	VO_U32 		SetParam (VO_U32 nID, VO_PTR pValue);
	VO_U32 		GetParam (VO_U32 nID, VO_PTR pValue);

protected:
	VO_U32		LoadDll (void);
	VO_U32		Release (void);

protected:
	VO_TCHAR	m_szDllFile[256];
#ifdef _WIN32
	HMODULE		m_hDll;
#elif defined LINUX
	VO_PTR m_hDll;
#endif

	VOMM_PLAYAPI	m_playFunc;
	VO_HANDLE		m_hPlay;

};

#endif // __CVOWPlayer_H__
