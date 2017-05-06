	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CFilterOneRender.h

	Contains:	CFilterOneRender header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2007-08-14		JBF			Create file

*******************************************************************************/
#ifndef __CFilterOneRender_H__
#define __CFilterOneRender_H__

#include "IFilterOne.h"
#include "CVideoWindow.h"
#include "COneSeeking.h"

#include "CVOWPlayer.h"

class COneInputPin;

class CFilterOneRender : public CBaseFilter
					   , public IFilterOne
{
public:
	DECLARE_IUNKNOWN;

	// Constructor
	CFilterOneRender(LPUNKNOWN pUnk, HRESULT *phr);
	virtual ~CFilterOneRender (void);

	// Basic COM - used here to reveal our property interface.
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	// Pin enumeration
	CBasePin * GetPin(int n);
	int GetPinCount();

	// Open and close the file as necessary
	STDMETHODIMP Run(REFERENCE_TIME tStart);
	STDMETHODIMP Pause();
	STDMETHODIMP Stop();

	// IFilterOne interface
	STDMETHODIMP SetParam(int nID, int nValue);
	STDMETHODIMP GetParam(int nID, int * pValue);

	//Normal function
	HRESULT		OpenSource (TCHAR * pSource);
	HRESULT		Close (void);
	HRESULT		Resize (void);

	CVOWPlayer *	GetPlayer (void) {return m_pPlayer;}

protected:
	CCritSec			m_csRender;
	COneInputPin *		m_pInput;
	CVideoWindow *		m_pWndVideo;
	COneSeeking *		m_pSeeking;

	CVOWPlayer *		m_pPlayer;
	BOOL				m_bVideo;

public:
	static VO_U32		vowPlayCallBackProc (VO_PTR pUserData, VO_U32 nID, VO_PTR pValue1, VO_PTR pValue2);

#ifdef _THREAD_IDEL
protected:
	static	VO_U32	PlaybackIdleThreadProc (VO_PTR pParam);
	virtual VO_U32	PlaybackIdleThreadLoop (void);

	VO_BOOL			m_bIdleStop;
	HANDLE			m_hThread;
#endif // _THREAD_IDEL

};

#endif //__CFilterOneRender_H__