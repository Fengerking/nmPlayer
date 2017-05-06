/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __OnStreamEngine_H__
#define __OnStreamEngine_H__

#include <windows.h>
#include <tchar.h>

#include "voOnStreamType.h"
#include "voOnStreamEngine.h"

class OnStreamEngine
{
public:
	// Used to control the image drawing
	OnStreamEngine (int nPlayerType, void* pInitParam, int nInitParamFlag);
	~OnStreamEngine (void);

	int			Init(int nPlayerType, void* pInitParam, int nInitParamFlag);
	int			Uninit(void);
	int 		SetDataSource (void * pSource, int nFlag);
	int 		SetView(void* pView);
	int 		Run (void);
	int 		Pause (void);
	int 		Stop (void);
	int 		GetStatus (VOOSMP_STATUS * pStatus);
	int 		GetCurPos (int * pCurPos);
	int 		SetCurPos (int nCurPos);
	int			GetSubtitleSample(void* pHandle, voSubtitleInfo* pSample);
    int			SelectLanguage(void* pHandle, int nIndex);
    int			GetLanguage(void* pHandle, VOOSMP_SUBTITLE_LANGUAGE_INFO** ppLangInfo);    
    int			GetSEI(void* pHandle, VOOSMP_SEI_INFO* pInfo);
	int 		GetParam (int nID, void * pValue);
	int 		SetParam (int nID, void * pValue);

protected:
	int					LoadDll (void);

protected:
	HMODULE				m_hDll;
	void *				m_hPlay;
	HWND				m_hWnd;

	VOOSMP_RECT			m_rcDraw;

	VOOSMP_LIB_FUNC *	m_pLibop;

	voOnStreamEngnAPI	m_pPlayerAPI;
};

#endif // __OnStreamEngine_H__
