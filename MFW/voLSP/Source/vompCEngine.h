	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		vompCEngine.h

	Contains:	vompCEngine header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/
#ifndef __vompCEngine_H__
#define __vompCEngine_H__

#include "vompAPI.h"
#include "voType.h"

#include "volspAPI.h"

class vompCEngine
{
public:
	vompCEngine ();
	virtual ~vompCEngine (void);
	virtual int         init();
	virtual int         initVomp();

	virtual int 		SetDataSource (void * pSource, int nFlag, int nMode);
	virtual int 		SendBuffer (VO_BUFFER_INPUT_TYPE eInputType, VOMP_BUFFERTYPE * pBuffer);
	virtual int 		Run (void);
	virtual int 		Pause (void);
	virtual int 		Stop (void);
	virtual int 		Flush (void);
	virtual int 		GetStatus (VOMP_STATUS * pStatus);
	virtual int 		GetDuration (int * pDuration);
	virtual int 		GetCurPos (int * pCurPos);
	virtual int 		SetCurPos (int nCurPos);
	virtual int 		GetParam (int nID, void * pValue);
	virtual int 		SetParam (int nID, void * pValue);

	static int			OnListener (void * pUserData, int nID, void * pParam1, void * pParam2);
	virtual int			HandleEvent (int nID, void * pParam1, void * pParam2);

	virtual int			SetDrawRect(VOMP_RECT* pRect);
	
protected:
	int					LoadDll (void);

protected:
	VO_CHAR			m_szDllFile[256];
	VO_PTR				m_hDll;

	void *				m_hPlay;

	VOMPINIT			m_fInit;
	VOMPSETDATASOURCE	m_fSetSource;
	VOMPSENDBUFFER		m_fSendBuffer;
	VOMPRUN				m_fRun;
	VOMPPAUSE			m_fPause;
	VOMPSTOP			m_fStop;
	VOMPFLUSH			m_fFlush;
	VOMPGETSTATUS		m_fGetStatus;
	VOMPGETDURATION		m_fGetDuration;
	VOMPGETCURPOS		m_fGetPos;
	VOMPSETCURPOS		m_fSetPos;
	VOMPGETPARAM		m_fGetParam;
	VOMPSETPARAM		m_fSetParam;
	VOMPUNINIT			m_fUninit;
};

#endif // __vompCEngine_H__

