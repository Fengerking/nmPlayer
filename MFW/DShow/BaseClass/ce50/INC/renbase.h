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

#ifndef __RENBASE__
#define __RENBASE__

#include <renqual.h>

// Forward class declarations

class CBaseRenderer;
class CBaseVideoRenderer;
class CRendererInputPin;

// This is our input pin class that channels calls to the renderer

class CRendererInputPin : public CBaseInputPin
{
protected:

    CBaseRenderer *m_pRenderer;

public:

    CRendererInputPin(CBaseRenderer *pRenderer,
                      HRESULT *phr,
                      LPCWSTR Name);

    // Overriden from the base pin classes

    HRESULT BreakConnect();
    HRESULT CompleteConnect(IPin *pReceivePin);
    HRESULT SetMediaType(const CMediaType *pmt);
    HRESULT CheckMediaType(const CMediaType *pmt);
    HRESULT Active();
    HRESULT Inactive();

    // Add rendering behaviour to interface functions

    STDMETHODIMP QueryId(LPWSTR *Id);
    STDMETHODIMP EndOfStream();
    STDMETHODIMP BeginFlush();
    STDMETHODIMP EndFlush();
    STDMETHODIMP Receive(IMediaSample *pMediaSample);

    STDMETHODIMP NewSegment(
                    REFERENCE_TIME tStart,
                    REFERENCE_TIME tStop,
                    double dRate);

    // Helper
    IMemAllocator inline *Allocator() const
    {
        return m_pAllocator;
    }
};

// Main renderer class that handles synchronisation and state changes

class CBaseRenderer : public CBaseFilter
{
protected:

    friend class CRendererInputPin;

    friend void CALLBACK EndOfStreamTimer(UINT uID,      // Timer identifier
                                          UINT uMsg,     // Not currently used
                                          DWORD_PTR dwUser,  // User information
                                          DWORD_PTR dw1,     // Windows reserved
                                          DWORD_PTR dw2);    // Is also reserved

    CRendererPosPassThru *m_pPosition;  // Media seeking pass by object
    CAMEvent m_RenderEvent;             // Used to signal timer events
    CAMEvent m_ThreadSignal;            // Signalled to release worker thread
    CAMEvent m_evComplete;              // Signalled when state complete
    BOOL m_bAbort;                      // Stop us from rendering more data
    BOOL m_bStreaming;                  // Are we currently streaming
    DWORD_PTR m_dwAdvise;                   // Timer advise cookie
    IMediaSample *m_pMediaSample;       // Current image media sample
    BOOL m_bEOS;                        // Any more samples in the stream
    BOOL m_bEOSDelivered;               // Have we delivered an EC_COMPLETE
    CRendererInputPin *m_pInputPin;     // Our renderer input pin object
    CCritSec m_InterfaceLock;           // Critical section for interfaces
    CCritSec m_RendererLock;            // Controls access to internals
    IQualityControl * m_pQSink;         // QualityControl sink
    BOOL m_bRepaintStatus;              // Can we signal an EC_REPAINT
    //  Avoid some deadlocks by tracking filter during stop
    volatile BOOL  m_bInReceive;        // Inside Receive between PrepareReceive
                                        // And actually processing the sample
    REFERENCE_TIME m_SignalTime;        // Time when we signal EC_COMPLETE
    UINT m_EndOfStreamTimer;            // Used to signal end of stream

    BOOL m_bFirstFrameRendered;         // Used to indicate if a frame was already rendered or not

public:

    CBaseRenderer(REFCLSID RenderClass, // CLSID for this renderer
                  TCHAR *pName,         // Debug ONLY description
                  LPUNKNOWN pUnk,       // Aggregated owner object
                  HRESULT *phr);        // General OLE return code

    ~CBaseRenderer();

    // Overriden to say what interfaces we support and where

    virtual HRESULT GetMediaPositionInterface(REFIID riid,void **ppv);
    STDMETHODIMP NonDelegatingQueryInterface(REFIID, void **);

    virtual HRESULT SourceThreadCanWait(BOOL bCanWait);

#ifdef DEBUG
    // Debug only dump of the renderer state
    void DisplayRendererState();
#endif
    virtual HRESULT WaitForRenderTime();
    virtual HRESULT CompleteStateChange(FILTER_STATE OldState);

    // Return internal information about this filter

    BOOL IsEndOfStream() { return m_bEOS; };
    BOOL IsEndOfStreamDelivered() { return m_bEOSDelivered; };
    BOOL IsStreaming() { return m_bStreaming; };
    void SetAbortSignal(BOOL bAbort) { m_bAbort = bAbort; };
    virtual void OnReceiveFirstSample(IMediaSample *pMediaSample) { };
    CAMEvent *GetRenderEvent() { return &m_RenderEvent; };

    // Permit access to the transition state

    void Ready() { m_evComplete.Set(); };
    void NotReady() { m_evComplete.Reset(); };
    BOOL CheckReady() { return m_evComplete.Check(); };

    virtual int GetPinCount();
    virtual CBasePin *GetPin(int n);
    FILTER_STATE GetRealState();
    void SendRepaint();
    void SendNotifyWindow(IPin *pPin,HWND hwnd);
    virtual BOOL OnDisplayChange();
    void SetRepaintStatus(BOOL bRepaint);

    // Override the filter and pin interface functions

    STDMETHODIMP Stop();
    STDMETHODIMP Pause();
    STDMETHODIMP Run(REFERENCE_TIME StartTime);
    STDMETHODIMP GetState(DWORD dwMSecs,FILTER_STATE *State);
    STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin);

    virtual void OnNewSegment(REFERENCE_TIME tStart,
                              REFERENCE_TIME tStop,
                              double dRate);

    // These are available for a quality management implementation

    virtual void OnRenderStart(IMediaSample *pMediaSample);
    virtual void OnRenderEnd(IMediaSample *pMediaSample);
    virtual HRESULT OnStartStreaming() { return NOERROR; };
    virtual HRESULT OnStopStreaming() { return NOERROR; };
    virtual void OnWaitStart() { };
    virtual void OnWaitEnd() { };
    virtual void PrepareRender() { };

#ifdef PERF
    REFERENCE_TIME m_trRenderStart; // Just before we started drawing
                                    // Set in OnRenderStart, Used in OnRenderEnd
    int m_idBaseStamp;              // MSR_id for frame time stamp
    int m_idBaseRenderTime;         // MSR_id for true wait time
    int m_idBaseAccuracy;           // MSR_id for time frame is late (int)
#endif

    // Quality management implementation for scheduling rendering

    virtual BOOL ScheduleSample(IMediaSample *pMediaSample);
    virtual HRESULT GetSampleTimes(IMediaSample *pMediaSample,
                                   REFERENCE_TIME *pStartTime,
                                   REFERENCE_TIME *pEndTime);

    virtual HRESULT ShouldDrawSampleNow(IMediaSample *pMediaSample,
                                        REFERENCE_TIME *ptrStart,
                                        REFERENCE_TIME *ptrEnd);

    // Lots of end of stream complexities

    void TimerCallback();
    void ResetEndOfStreamTimer();
    HRESULT NotifyEndOfStream();
    virtual HRESULT SendEndOfStream();
    virtual HRESULT ResetEndOfStream();
    virtual HRESULT EndOfStream();

    // Rendering is based around the clock

    virtual void SignalTimerFired();
    virtual HRESULT CancelNotification();
    virtual HRESULT ClearPendingSample();

    // Called when the filter changes state

    virtual HRESULT Active();
    virtual HRESULT Inactive();
    virtual HRESULT StartStreaming();
    virtual HRESULT StopStreaming();
    virtual HRESULT BeginFlush();
    virtual HRESULT EndFlush();

    // Deal with connections and type changes

    virtual HRESULT BreakConnect();
    virtual HRESULT SetMediaType(const CMediaType *pmt);
    virtual HRESULT CompleteConnect(IPin *pReceivePin);

    // These look after the handling of data samples

    virtual HRESULT PrepareReceive(IMediaSample *pMediaSample);
    virtual HRESULT Receive(IMediaSample *pMediaSample);
    virtual BOOL HaveCurrentSample();
    virtual IMediaSample *GetCurrentSample();
    virtual HRESULT Render(IMediaSample *pMediaSample);

    // Derived classes MUST override these
    virtual HRESULT DoRenderSample(IMediaSample *pMediaSample) PURE;
    virtual HRESULT CheckMediaType(const CMediaType *) PURE;

    virtual void WaitForReceiveToComplete();
};


// CBaseVideoRenderer is a renderer class (see its ancestor class) and
// it handles scheduling of media samples so that they are drawn at the
// correct time by the reference clock.  It implements a degradation
// strategy.  Possible degradation modes are:
//    Drop frames here (only useful if the drawing takes significant time)
//    Signal supplier (upstream) to drop some frame(s) - i.e. one-off skip.
//    Signal supplier to change the frame rate - i.e. ongoing skipping.
//    Or any combination of the above.
// In order to determine what's useful to try we need to know what's going
// on.  This is done by timing various operations (including the supplier).
// This timing is done by using timeGetTime as it is accurate enough and
// usually cheaper than calling the reference clock.  It also tells the
// truth if there is an audio break and the reference clock stops.
// We provide a number of public entry points (named OnXxxStart, OnXxxEnd)
// which the rest of the renderer calls at significant moments.  These do
// the timing.
class CBaseVideoRenderer : public CBaseRenderer,         // Base renderer class
                           public CVideoRendererQuality, // Quality management
                           public IQualityControl
{
public:


    CBaseVideoRenderer(REFCLSID RenderClass, // CLSID for this renderer
                       TCHAR *pName,         // Debug ONLY description
                       LPUNKNOWN pUnk,       // Aggregated owner object
                       HRESULT *phr);        // General OLE return code

    ~CBaseVideoRenderer();

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

#endif // __RENBASE__

