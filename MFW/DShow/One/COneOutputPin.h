	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		COneOutputPin.h

	Contains:	COneOutputPin header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-12-01		JBF			Create file

*******************************************************************************/
#ifndef __COneOutputPin_H__
#define __COneOutputPin_H__

class CFilterOneSource;

class COneOutputPin : public CBasePin
{
public:
	// Constructor and destructor
	COneOutputPin(TCHAR *pObjName, CBaseFilter * pSource, CCritSec * pCSFile, HRESULT *phr, LPCWSTR pPinName);
	virtual ~COneOutputPin();

	DECLARE_IUNKNOWN;

	// Override to expose IMediaPosition
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppvoid);
	STDMETHODIMP Connect(IPin * pReceivePin, const AM_MEDIA_TYPE *pmt);

	// returns the preferred formats for a pin
	virtual HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);
	virtual HRESULT CheckConnect(IPin * pPin);

	// Check that we can support an output type
	HRESULT CheckMediaType(const CMediaType *pmt);
	HRESULT SetMediaType(const CMediaType *pmt);

	// Overriden to create and destroy output pins
	HRESULT CompleteConnect(IPin *pReceivePin);
	HRESULT BreakConnect();

	HRESULT Active(void);    // Starts up the worker thread
	HRESULT Inactive(void);  // Exits the worker thread.

	virtual STDMETHODIMP BeginFlush( void);
	virtual STDMETHODIMP EndFlush( void);

protected:

};

#endif //__COneOutputPin_H__