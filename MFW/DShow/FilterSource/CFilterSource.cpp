#include <initguid.h>
#include "CFilterSource.h"
#include "FilterSource_Guids.h"
#include <tchar.h>
#include "CBaseFileParser.h"
#include "CBaseStream.h"
#include "CBaseTrack.h"
#include "CMediaSeeking.h"
#include "cmnFile.h"
#include "cmnMemory.h"
#include "voLog.h"


extern VO_FILE_OPERATOR		g_fileOP;
extern VO_MEM_OPERATOR		g_memOP;

CFilterSource::CFilterSource(LPUNKNOWN lpunk, HRESULT *phr)
 : CSource(NAME("File Source"), lpunk, CLSID_FilterSource)
 , mpFileParser(NULL)
 , mbIsURL(false)
{
	cmnMemFillPointer(0);
	cmnFileFillPointer();

	mpSeeking = new CMediaSeeking(TEXT("vo Seeking") , lpunk , phr , &mcsSeeker);
	mpSeeking->AddRef();
}

CFilterSource::~CFilterSource(void)
{
	if(mpFileParser != NULL)
	{
		delete mpFileParser;
		mpFileParser = NULL;
	}

	if(mpSeeking != NULL)
	{
		mpSeeking->Release();
		mpSeeking = NULL;
	}
}

STDMETHODIMP CFilterSource::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	if(riid == IID_IFileSourceFilter)
	{
		return GetInterface((IFileSourceFilter *) this, ppv);
	}
	else if(riid == IID_IMediaSeeking)
	{
		*ppv = mpSeeking;
		mpSeeking->AddRef();
	}
	else if(riid == IID_IAMNetworkStatus && mbIsURL)
	{
	}
	else
		return CSource::NonDelegatingQueryInterface(riid , ppv);
}

int       CFilterSource::GetPinCount(void)
{
	int nCount = CSource::GetPinCount();
	return CSource::GetPinCount();
}

CBasePin *CFilterSource::GetPin(int n)
{
	return CSource::GetPin(n);
}

STDMETHODIMP CFilterSource::Stop()
{ 
	VOLOGI("&&& Source Filter Stop");
	HRESULT hr = CSource::Stop();
	return hr;
}

STDMETHODIMP CFilterSource::Pause()
{
	VOLOGI("&&& Source Filter Pause");
	HRESULT hr = CSource::Pause();
	return hr;
}

STDMETHODIMP CFilterSource::Run(REFERENCE_TIME tStart)
{
	VOLOGI("&&& Source Filter Run");
	HRESULT hr = CSource::Run(tStart);
	return hr;
}

// handle IFileSourceFilter interface
STDMETHODIMP CFilterSource::Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE *pmt)
{
#ifdef _UNICODE
	_tcscpy(mszSourceFile, pszFileName);
#else
	WideCharToMultiByte(CP_ACP, 0, pszFileName, -1, m_szSourceFile, sizeof (mszSourceFile), NULL, NULL);
#endif //_UNICODE

	mbIsURL = IsURL(mszSourceFile);

	if(mpFileParser == NULL)
	{
		mpFileParser = new CBaseFileParser(&g_fileOP , &g_memOP , NULL);
		if(mpFileParser == NULL)
			return E_OUTOFMEMORY;

		if(mpSeeking != NULL)
		{
			mpSeeking->SetFileParser(mpFileParser);
		}
	}

	HRESULT hr =  mpFileParser->Load(mszSourceFile);
	if(hr == S_FALSE )
		return S_OK;
	else if(hr != S_OK)
		return hr;

	LONGLONG llDuration = 0;
	int nStreamCount = mpFileParser->GetTrackCount();
	for (int i = 0 ; i < nStreamCount ; i++)
	{
		CBaseTrack *pTrack = mpFileParser->GetTrack(i);

		CBaseStream *pStream = NULL	;
		if(pTrack->GetTrackType() == VO_SOURCE_TT_VIDEO || pTrack->GetTrackType() == VO_SOURCE_TT_RTSP_VIDEO)
		{
			pStream = new CBaseStream(pTrack , &hr , this , L"Video" , &g_memOP , true);
			mpSeeking->SetVideoStream(pStream);
		}
		else if(pTrack->GetTrackType() == VO_SOURCE_TT_AUDIO || pTrack->GetTrackType() == VO_SOURCE_TT_RTSP_AUDIO)
		{
			pStream = new  CBaseStream(pTrack , &hr , this , L"audio" , &g_memOP , false);
			mpSeeking->SetAudioStream(pStream);
		}
		else
		{
			TCHAR pName[50];
			ZeroMemory(pName , sizeof(TCHAR) * 50);
			swprintf(pName , _T("Track%d") , i);
			pStream = new CBaseStream(pTrack , &hr , this , pName , &g_memOP , false);
		}

		if(llDuration < pStream->GetDuration())
			llDuration = pStream->GetDuration();
	}

	mpSeeking->SetDuration(llDuration);

	return S_OK;
}

STDMETHODIMP CFilterSource::GetCurFile(LPOLESTR *ppszFileName, AM_MEDIA_TYPE *pmt)
{
	int nLen = _tcslen (mszSourceFile) + 1;

	*ppszFileName = (LPOLESTR) CoTaskMemAlloc (nLen * sizeof (WCHAR));
	memset (*ppszFileName, 0, nLen * sizeof (WCHAR));

#ifdef _UNICODE
	wcscpy (*ppszFileName, mszSourceFile);
#else
	MultiByteToWideChar (CP_ACP, 0,mszSourceFile, -1, *ppszFileName, nLen);
#endif //_UNICODE

	

	return S_OK;
}

bool	CFilterSource::IsURL(TCHAR *pStr)
{
	if (!vostrncmp (pStr, _T("RTSP://"), 6))
		return true;
	else if (!vostrncmp (pStr, _T("rtsp://"), 6))
		return true;
	else if (!vostrncmp (pStr, _T("MMS://"), 5))
		return true;
	else if (!vostrncmp (pStr, _T("mms://"), 5))
		return true;
	else if (!vostrncmp (pStr, _T("HTTP://"), 6))
		return true;
	else if (!vostrncmp (pStr, _T("http://"), 6))
		return true;

	return false;
}

STDMETHODIMP CFilterSource::get_ReceivedPackets(long * pReceivedPackets)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::get_RecoveredPackets(long * pRecoveredPackets)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::get_LostPackets( long * pLostPackets)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::get_ReceptionQuality(long * pReceptionQuality)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::get_BufferingCount(long * pBufferingCount)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::get_IsBroadcast( VARIANT_BOOL * pIsBroadcast)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::get_BufferingProgress(long * pBufferingProgress)
{
	return E_NOTIMPL;
}

STDMETHODIMP CFilterSource::GetTypeInfoCount(UINT * pctinfo)
{
	return m_basedisp.GetTypeInfoCount(pctinfo);
}

STDMETHODIMP CFilterSource::GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo ** pptinfo)
{
	return m_basedisp.GetTypeInfo(IID_IMediaEvent, itinfo, lcid, pptinfo);
}

STDMETHODIMP CFilterSource::GetIDsOfNames(REFIID riid, OLECHAR  ** rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid)
{
	return m_basedisp.GetIDsOfNames(IID_IMediaEvent, rgszNames, cNames, lcid, rgdispid);
}

STDMETHODIMP CFilterSource::Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams,
					VARIANT * pvarResult,EXCEPINFO * pexcepinfo, UINT * puArgErr)
{
	// this parameter is a dead leftover from an earlier interface
	//if (IID_NULL != riid)
	//	return DISP_E_UNKNOWNINTERFACE;

	ITypeInfo * pti;
	HRESULT hr = GetTypeInfo(0, lcid, &pti);

	if (FAILED(hr))
		return hr;

	hr = pti->Invoke((IMediaEvent *)this, dispidMember, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
	pti->Release();

	return hr;
}


