//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of your Microsoft Windows CE
// Source Alliance Program license form.  If you did not accept the terms of
// such a license, you are not authorized to use this source code.
//
//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//
//--------------------------------------------------------------------------;

// Generic ActiveX base renderer class, December 1995

#ifndef __RENBASEASYNC__
#define __RENBASEASYNC__

#include <renbase.h>

// Forward class declarations

class CBaseRendererAsync;
class CBaseVideoRendererAsync;

//
// A sample that is queued in the RendererQueue, holding all samples that need to be processed.
// Just a single sample is scheduled at a time (the head of the queue).
//
class CRendererSample
{
    friend class CRendererSampleQueue;

public:
    CRendererSample (IMediaSample *pSample)
    {
        m_pMediaSample = pSample;
        m_pMediaSample->AddRef();
        m_bIsScheduled = FALSE;
        m_pNext = NULL;
    }

    ~CRendererSample()
    {
        ASSERT (m_pMediaSample  &&  m_pNext == NULL);
        m_pMediaSample->Release();
    }

    BOOL IsScheduled () { return m_bIsScheduled; }

    void SetScheduled (BOOL bIsScheduled) { m_bIsScheduled = bIsScheduled; }

    IMediaSample *MediaSample () { return m_pMediaSample; }


protected:
    BOOL             m_bIsScheduled;
    CRendererSample *m_pNext;
    IMediaSample    *m_pMediaSample;
};


//
// A queue of renderer sample objects
//
class CRendererSampleQueue
{
public:
    CRendererSampleQueue()
    {
        m_NEntries = 0;
        m_pHead = m_pTail = NULL;
    }

    virtual ~CRendererSampleQueue()
    {
        Flush();
    }

    void Flush (void)
    {
        CRendererSample *pRendererSample;
        while (Remove (&pRendererSample) == S_OK)
        {
            delete pRendererSample;
        }
    }

    HRESULT Insert(CRendererSample *pRendererSample)
    {
        CAutoLock lock(&m_Lock);

        ASSERT (pRendererSample);
        if (!pRendererSample)
        {
            return E_INVALIDARG;
        }

        if (m_pTail)
        {
            m_pTail->m_pNext = pRendererSample;
        }
        m_pTail = pRendererSample;
        m_pTail->m_pNext = NULL;
        if (!m_pHead)
        {
            m_pHead = m_pTail;
        }
        ++m_NEntries;

        return S_OK;
    }

    HRESULT Remove(CRendererSample **pRendererSample)
    {
        if (!pRendererSample)
            return E_INVALIDARG;

        *pRendererSample = m_pHead;
        if (m_pHead)
        {
            m_pHead = m_pHead->m_pNext;
            if (!m_pHead)
            {
                m_pTail = NULL;
            }
            (*pRendererSample)->m_pNext = NULL;
            --m_NEntries;
            ASSERT (m_NEntries >= 0);
        }
        return (*pRendererSample) ? S_OK : S_FALSE;
    }

    HRESULT Peek(CRendererSample **pRendererSample)
    {
        if (!pRendererSample)
            return E_INVALIDARG;

        *pRendererSample = m_pHead;
        return (*pRendererSample) ? S_OK : S_FALSE;
    }

protected:
    CCritSec         m_Lock;
    CRendererSample *m_pHead, *m_pTail;
    LONG             m_NEntries;
};


//
// Main renderer class that handles synchronisation and state changes
// Spawns a thread that takes care of scheduling/processing samples. The
// Receive method just queues samples for later processing
//
class CBaseRendererAsync : public CBaseRenderer, protected CAMThread
{
    friend class CRendererInputPin;
public:
    CBaseRendererAsync(REFCLSID RenderClass, // CLSID for this renderer
                       TCHAR *pName,         // Debug ONLY description
                       LPUNKNOWN pUnk,       // Aggregated owner object
                       HRESULT *phr);        // General OLE return code

    ~CBaseRendererAsync();
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,void **ppv);

    virtual HRESULT SourceThreadCanWait(BOOL bCanWait);

#ifdef DEBUG
    // Debug only dump of the renderer state
    void DisplayRendererState();
#endif
    virtual HRESULT WaitForRenderTime();

    virtual BOOL OnDisplayChange();

    // Override the filter and pin interface functions
    STDMETHODIMP Stop();
    STDMETHODIMP Pause();
    STDMETHODIMP Run(REFERENCE_TIME StartTime);

    virtual void SignalTimerFired();

    // Called when the filter changes state

    virtual HRESULT Inactive();
    virtual HRESULT StartStreaming();
    virtual HRESULT StopStreaming();
    virtual HRESULT BeginFlush();
    virtual HRESULT EndFlush();

    // Deal with connections and type changes

    virtual HRESULT BreakConnect();

    // These look after the handling of data samples

    virtual HRESULT PrepareReceive(IMediaSample *pMediaSample);
    virtual HRESULT Receive(IMediaSample *pMediaSample);
    virtual BOOL HaveCurrentSample();
    virtual BOOL ScheduleSample(IMediaSample *pMediaSample);

    // Derived classes MUST override these
    virtual HRESULT DoRenderSample(IMediaSample *pMediaSample) PURE;
    virtual HRESULT CheckMediaType(const CMediaType *) PURE;

    virtual void WaitForReceiveToComplete();

protected:
    CAMEvent m_ScheduleEvent;             // Used to signal that an object needs to be scheduled
    CRendererSample *m_pScheduledSample;  // Sample that is currently scheduled with the m_dwAdvise cookie
    CRendererSampleQueue m_RendererQueue; // Contains the samples that need to be rendered
    BOOL m_bFirstSample;                  // set if this is the first sample after a pause

    enum Command { CMD_RUN, CMD_STOP, CMD_EXIT };

    // type-corrected overrides of communication funcs
    Command GetRequest() {
    	return (Command) CAMThread::GetRequest();
    };

    BOOL CheckRequest(Command * pCom) {
	    return CAMThread::CheckRequest( (DWORD *) pCom);
    };

    // commands we can give the thread
    HRESULT RunThread();
    HRESULT StopThread();
    HRESULT KillThread();   
    DWORD   ThreadProc();

    // schedule & render samples available in m_RendererQueue
    HRESULT ScheduleQueuedSample (void);
    HRESULT ProcessQueuedSample (void);
};


// CBaseVideoRendererAsync is a renderer class (see its async ancestor class) and 
// CVideoRendererQuality class for quality management
class CBaseVideoRendererAsync : public CBaseRendererAsync,   // Base renderer class
                                public CVideoRendererQuality // Quality management
{
public:
    CBaseVideoRendererAsync(REFCLSID RenderClass, // CLSID for this renderer
                       TCHAR *pName,         // Debug ONLY description
                       LPUNKNOWN pUnk,       // Aggregated owner object
                       HRESULT *phr);        // General OLE return code

    ~CBaseVideoRendererAsync();

    BOOL ScheduleSample(IMediaSample *pMediaSample);
    virtual HRESULT ShouldDrawSampleNow(IMediaSample *pMediaSample, REFERENCE_TIME *ptrStart, REFERENCE_TIME *ptrEnd);

    virtual HRESULT SendQuality(REFERENCE_TIME trLate, REFERENCE_TIME trRealStream);
    STDMETHODIMP JoinFilterGraph(IFilterGraph * pGraph, LPCWSTR pName);

    // IQualityControl methods - Notify allows audio-video throttling
    STDMETHODIMP SetSink( IQualityControl * piqc);
    STDMETHODIMP Notify( IBaseFilter * pSelf, Quality q);

    // These provide a full video quality management implementation

    virtual void OnRenderStart(IMediaSample *pMediaSample) { CVideoRendererQuality::OnRenderStartQual (pMediaSample); }
    virtual void OnRenderEnd(IMediaSample *pMediaSample) { CVideoRendererQuality::OnRenderEndQual (pMediaSample); }
    virtual HRESULT OnStartStreaming() { return CVideoRendererQuality::OnStartStreamingQual (m_State); }
    virtual HRESULT OnStopStreaming(){ return CVideoRendererQuality::OnStopStreamingQual (m_State); }
    virtual void OnWaitStart() { CVideoRendererQuality::OnWaitStartQual (); }
    virtual void OnWaitEnd() { CVideoRendererQuality::OnWaitEndQual (); }

    virtual void PrepareRender();

    // IQualProp property page support
    STDMETHODIMP get_FramesDroppedInRenderer(int *cFramesDropped);
    STDMETHODIMP get_FramesDrawn(int *pcFramesDrawn);
    STDMETHODIMP get_AvgFrameRate(int *piAvgFrameRate);
    STDMETHODIMP get_Jitter(int *piJitter);
    STDMETHODIMP get_AvgSyncOffset(int *piAvg);
    STDMETHODIMP get_DevSyncOffset(int *piDev);
    HRESULT GetStdDev(int nSamples, int *piResult, LONGLONG llSumSq, LONGLONG iTot);

    // Implement an IUnknown interface and expose IQualProp
    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid,VOID **ppv);
};

#endif // __RENBASEASYNC__
