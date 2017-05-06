	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COneInputPin.h

	Contains:	COneInputPin header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-12-02		JBF			Create file

*******************************************************************************/

#ifndef __COneInputPin_H__
#define __COneInputPin_H__

class CFilterOneRender;

class COneInputPin : public CBaseInputPin
{
public:
	COneInputPin(TCHAR * pObjectName,             // Object string description
				CFilterOneRender * pRenderer,     // Used to delegate locking
				CCritSec * pInterfaceLock,       // Main critical section
				HRESULT * phr,                   // OLE failure return code
				LPCWSTR pPinName);              // This pins identification

	virtual ~COneInputPin (void);

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppvoid);

	HRESULT CheckMediaType (const CMediaType * pmt);
	HRESULT CompleteConnect (IPin * pReceivePin);
	HRESULT BreakConnect ();

	// switch the pin to inactive state - may already be inactive
	virtual HRESULT Inactive(void);

protected:
	CFilterOneRender *	m_pRender;
};

#endif // __COneInputPin_H__