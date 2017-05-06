#pragma once
// voPlugInIECtrl.h : Declaration of the CvoPlugInIECtrl ActiveX Control class.

#include "stdafx.h"
#include <ObjSafe.h>
#include "voThread.h"
#include "voLog.h"
#include "voCMutex.h"
#include "..\Common\CMsgQueue.h"
#include "..\Common\COSMPEngnWrap.h"

#define WM_USERDEFNOTIFY    	WM_USER+101 //user notify message
#define WM_NOTIFY_CALLBACK      WM_USER+102 
#define WM_CHANGE_TO_DDDRAW 	WM_USER+103
#define	TIMER_CHANGE_DDDRAW		101

// CvoPlugInIECtrl : See voPlugInIECtrl.cpp for implementation.

class CvoPlugInIECtrl : public COleControl
{
	DECLARE_DYNCREATE(CvoPlugInIECtrl)

  RECT                  m_drawRect;

  voCMutex				      m_mtNotify;
  HWND                  m_hwndBrowser;
  bool                  m_bExitMsgLoop;
  voThreadHandle			  m_hMsgLoop;
  CMsgQueue				      m_msgQueue;
  BOOL                  m_bNeedClearMsgQueue;
  voCMutex				      m_mtMsgQueue;

  IStream*              m_pStream;
  CComDispatchDriver    m_pDispatch;

  INT					          m_nStatus;   // 0: loaded/closed   1: openning   2: opened/prepared   3: playing   4: paused   5: stopped


  HWND                  m_hwndRenderMsg;//specially to process render event to notify plugin call ondraw

  BOOL                  m_bDDRraw;

  BOOL                  m_bHybridMode;

// Constructor
public:
	CvoPlugInIECtrl();

  void          notify(int nID, void * pParam1, void * pParam2);
  void          NotifyCallbackJS(VOCBMSG &msg);

  int				    msgHandlerLoop();
  static int		msgHandlerProc(VO_PTR pParam);

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();

  //Process msg
  static LRESULT CALLBACK msgWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Implementation
protected:
	~CvoPlugInIECtrl();

	DECLARE_OLECREATE_EX(CvoPlugInIECtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CvoPlugInIECtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CvoPlugInIECtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CvoPlugInIECtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

  DECLARE_INTERFACE_MAP()
  BEGIN_INTERFACE_PART(MyObjSafe, IObjectSafety)
    STDMETHOD_(HRESULT, GetInterfaceSafetyOptions) (
    REFIID riid,
    DWORD __RPC_FAR *pdwSupportedOptions,
    DWORD __RPC_FAR *pdwEnabledOptions
    );
    STDMETHOD_(HRESULT, SetInterfaceSafetyOptions) (
      REFIID riid,
      DWORD dwOptionSetMask,
      DWORD dwEnabledOptions
      );
  END_INTERFACE_PART(MyObjSafe);

private:
    void InitMembersValues();

    COSMPEngnWrap * m_pOSMPEngnWrap;
    COSMPEngnWrap *GetOSMPEngnWrap();
    BOOL InitOSMPEngnWrap();
    void UnInitOSMPEngnWrap();

    BOOL IsWindowLess(){return TRUE;};

    void GetVersion(LPTSTR szVersion, int iStrLength, int moduleType);

    HWND GetBrowserHwnd();

    void ParseToMsg(OUT VOCBMSG &cbMsg,IN int nID,IN void * pParam1,IN void * pParam2);

    void procNotifyCallBackMsg();

    BOOL CreateNotifyRenderWnd();


    BOOL IsEnableHybridMode();

  // Dispatch and event IDs
public:
	enum {
        dispidsetDefaultSubtitleLanguage = 112L,
        dispidsetDefaultAudioLanguage = 111L,
        dispidsetHTTPRetryTimeout = 110L,
        dispidgetVersion = 109L,
        dispidenableDolbyLibrary = 108L,
        dispidupdateSourceURL = 107L,
        dispidsetBitrateThreshold = 106L,
    dispidenableCPUAdaptation = 105L,
    dispidgetSEIInfo = 104L,
    dispidstopSEINotification = 103L,
    dispidstartSEINotification = 102L,
    dispidenableSEI = 101L,
    dispidsetInitialBufferTime = 100L,
    dispidgetDRMUniqueIdentifier = 99L,
    dispidsetDRMUniqueIdentifier = 98L,
    dispidgetVideoDecodingBitrate = 97L,
    dispidgetAudioDecodingBitrate = 96L,
    dispidenableLiveStreamingDVRPosition = 95L,
    dispidsetOnEventHandler = 94L,
    dispidresume = 93L,
    dispidsuspend = 92L,
    dispidsetDisaplayArea = 91L,
    dispidsetView = 90L,
    dispidinit = 89L,
    dispidsetParameter = 88L,
    dispidgetParameter = 87L,
    dispidsetDRMLibrary = 86L,
    dispidsetDRMUniqueIndentifier = 84L,
    dispidgetDRMUniqueIndentifier = 83L,
    dispidsetDRMVerificationInfo = 82L,
    dispidenableAudioEffect = 81L,
    dispidenableAudioStream = 80L,
    dispidsetAnewBufferingTime = 79L,
    dispidgetCurrentSelection = 78L,
    dispidtoggleOverlayUI = 77L,
    dispidsetPresentationDelay = 76L,
    dispidisFullScreen = 75L,
    dispidisLiveStreaming = 74L,
    dispidsetAudioPlaybackSpeed = 72L,
    dispidGetAnalytics = 71L,
    dispidStopAnalyticsNotification = 70L,
    dispidStartAnalyticsNotification = 69L,
    dispidEnableAnalytics = 68L,
    dispidsetHTTPProxy = 67L,
    dispidsetHTTPHeader = 66L,
    dispidVersion = 10,
    dispidregisterEventHandler0 = 65L,
    dispidsetSubtitleFontEdgeColor = 64L,
    dispidresetSubtitleParameter = 63L,
    dispidsetSubtitleFontEdgeOpacity = 62L,
    dispidsetSubtitleFontEdgeType = 61L,
    dispidsetSubtitleFontName = 60L,
    dispidsetSubtitleFontUnderline = 59L,
    dispidsetSubtitleFontBold = 58L,
    dispidsetSubtitleFontItalic = 57L,
    dispidsetSubtitleWindowBackgroundOpacity = 56L,
    dispidsetSubtitleWindowBackgroundColor = 55L,
    dispidsetSubtitleFontBackgroundOpacity = 54L,
    dispidsetSubtitleFontBackgroundColor = 53L,
    dispidsetSubtitleFontSizeScale = 52L,
    dispidsetSubtitleFontOpacity = 51L,
    dispidsetSubtitleFontColor = 50L,
    dispidenableSubtitle = 49L,
    dispidsetSubtitlePath = 48L,
    dispidgetVolume = 47L,
      dispidTestfun = 46L,
      dispidgetPlayingAsset = 45L,
      dispidgetSubtitleProperty = 44L,
      dispidgetAudioProperty = 43L,
      dispidgetVideoProperty = 42L,
      dispidclearSelection = 41L,
      dispidcommitSelection = 40L,
      dispidisSubtitleAvailable = 39L,
      dispidisAudioAvailable = 38L,
      dispidisVideoAvailable = 37L,
      dispidselectSubtitle = 36L,
      dispidselectAudio = 35L,
      dispidselectVideo = 34L,
      dispidgetSubtitleCount = 33L,
      dispidgetAudioCount = 32L,
      dispidgetVideoCount = 31L,
      dispidgetMaxPosition = 30L,
      dispidgetMinPosition = 29L,
      dispidsetInitialBitrate = 28L,
      dispidsetDeviceCapabilityByFile = 27L,
      dispidenableDeblock = 26L,
      dispidsetLicenseFilePath = 25L,
      dispidsetPreAgreedLicense = 24L,
      dispidsetLicenseContent = 23L,
      dispidsetVideoAspectRatio = 22L,
      dispidunmute = 14L,
      dispidmute = 13L,
      dispidGetVersion = 21L,
      dispidregisterEventHandler = 20L,
      dispidDestroy = 19L,
      dispidFullScreen = 18L,
      dispidsetVolume = 17L,
      dispidGetCodecErrors = 16L,
      dispidcanBePaused = 12L,
      dispidgetPlayerStatus = 9L,
      dispidgetPosition = 8L,
      dispidsetPosition = 7L,
      dispidgetDuration = 6L,
      dispidstop = 5L,      
      dispidpause = 4L,
      dispidstart = 3L,
      dispidclose = 2L,
      dispidopen = 1L
  };

protected:
  CString m_Version;

    //Player Control
    ULONG open(LPCTSTR cstrUrl, ULONG flag, ULONG sourceType, LPCTSTR openParam);
    ULONG close(void);
    ULONG start(void);
    ULONG pause(void);    
    ULONG stop(void);
    LONG setPosition(LONG lMSec);
    LONG getPosition(void);
    LONG getDuration(void);
    ULONG setVolume(FLOAT fLeftVol, FLOAT fRightVol);
    ULONG mute(void);
    ULONG unmute(void);
    ULONG setVideoAspectRatio(ULONG ar);
    ULONG getPlayerStatus(void);
    VARIANT_BOOL canBePaused(void);

    //Player Configuration
    ULONG setLicenseContent(LPCTSTR data);
    ULONG setPreAgreedLicense(LPCTSTR str);
    ULONG setLicenseFilePath(LPCTSTR path);
    ULONG enableDeblock(VARIANT_BOOL bEnable);
    ULONG setDeviceCapabilityByFile(LPCTSTR filePath);
    ULONG setInitialBitrate(ULONG bitrate);
    LONG getMinPosition(void);
    LONG getMaxPosition(void);

    //Player Asset Selection
    ULONG getVideoCount(void);
    ULONG getAudioCount(void);
    ULONG getSubtitleCount(void);
    ULONG selectVideo(ULONG index);
    ULONG selectAudio(ULONG index);
    ULONG selectSubtitle(ULONG index);
    VARIANT_BOOL isVideoAvailable(ULONG index);
    VARIANT_BOOL isAudioAvailable(ULONG index);
    VARIANT_BOOL isSubtitleAvailable(ULONG index);
    ULONG commitSelection(void);
    ULONG clearSelection(void);
    BSTR getVideoProperty(ULONG index);
    BSTR getAudioProperty(ULONG index);
    BSTR getSubtitleProperty(ULONG index);
    BSTR getPlayingAsset();

    ULONG FullScreen(void);
    ULONG registerEventHandler(IDispatch* pDispatch);
    ULONG getVolume();

    ULONG setSubtitlePath(LPCTSTR filePath);
    ULONG enableSubtitle(VARIANT_BOOL value);
    ULONG setSubtitleFontColor(ULONG color);
    ULONG setSubtitleFontOpacity(ULONG alpha);
    ULONG setSubtitleFontSizeScale(ULONG scale);
    ULONG setSubtitleFontBackgroundColor(ULONG color);
    ULONG setSubtitleFontBackgroundOpacity(ULONG alpha);
    ULONG setSubtitleWindowBackgroundColor(ULONG color);
    ULONG setSubtitleWindowBackgroundOpacity(ULONG alpha);
    ULONG setSubtitleFontItalic(VARIANT_BOOL enable);
    ULONG setSubtitleFontBold(VARIANT_BOOL enable);
    ULONG setSubtitleFontUnderline(VARIANT_BOOL enable);
    ULONG setSubtitleFontName(LPCTSTR name);
    ULONG setSubtitleFontEdgeType(ULONG type);
    ULONG setSubtitleFontEdgeOpacity(ULONG type);
    ULONG resetSubtitleParameter(void);
    ULONG setSubtitleFontEdgeColor(ULONG color);
    ULONG setHTTPHeader(LPCTSTR headerName, LPCTSTR headerValue);
    ULONG setHTTPProxy(LPCTSTR proxy);
    ULONG EnableAnalytics(LONG cacheTime);
    ULONG StartAnalyticsNotification(LONG interval, LPCTSTR filter);
    ULONG StopAnalyticsNotification(void);
    BSTR GetAnalytics(LPCTSTR filter);

    ULONG setAudioPlaybackSpeed(FLOAT fSpeed);
    VARIANT_BOOL isLiveStreaming(void);
    VARIANT_BOOL isFullScreen(void);
    ULONGLONG setPresentationDelay(ULONGLONG time);
    ULONGLONG toggleOverlayUI(VARIANT_BOOL shown);
    BSTR getCurrentSelection(void);
    ULONGLONG setAnewBufferingTime(LONG milliseconds);
    ULONG enableAudioStream(VARIANT_BOOL bEnable);
    ULONG enableAudioEffect(VARIANT_BOOL bEnable);
    ULONG setDRMVerificationInfo(LPCTSTR info);
    BSTR  getDRMUniqueIndentifier(void);
    ULONG setDRMUniqueIndentifier(LPCTSTR pid);
    ULONG setDRMLibrary(LPCTSTR libName, LPCTSTR libApiName);
    ULONG getParameter(ULONG nID, BYTE* pParam);
    ULONG setParameter(ULONG nID, BYTE* pParam);
    ULONG init(ULONG engineType, LPCTSTR initParam);
    ULONG setView(BYTE* pView);
    ULONG setDisaplayArea(BYTE* pArea);
    ULONG suspend(void);
    ULONG resume(void);
    ULONG setOnEventHandler(IDispatch* pDispatch);
    ULONG enableLiveStreamingDVRPosition(VARIANT_BOOL enable);
    BSTR getAudioDecodingBitrate(void);
    BSTR getVideoDecodingBitrate(void);
    ULONG setDRMUniqueIdentifier(LPCTSTR pid);
    BSTR getDRMUniqueIdentifier(void);
    ULONG setInitialBufferTime(LONG time);
    ULONG enableSEI(ULONG flag);
    ULONG startSEINotification(LONG interval);
    ULONG stopSEINotification(void);
    BSTR getSEIInfo(LONG time, ULONG flag);
    ULONG enableCPUAdaptation(VARIANT_BOOL bEnable);
    ULONG setBitrateThreshold(LONG upper, LONG lower);
    ULONG updateSourceURL(LPCTSTR url);
    ULONG enableDolbyLibrary(VARIANT_BOOL bEnable);
    BSTR getVersion(LONG moduleType);
    ULONG setHTTPRetryTimeout(LONG iRetryTime);
    ULONG setDefaultAudioLanguage(LPCTSTR type);
    ULONG setDefaultSubtitleLanguage(LPCTSTR type);
};

