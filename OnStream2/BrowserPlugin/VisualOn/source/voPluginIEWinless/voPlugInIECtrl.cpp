// voPlugInIECtrl.cpp : Implementation of the CvoPlugInIECtrl ActiveX Control class.

#include "stdafx.h"
#include "voPlugInIE.h"
#include "voPlugInIECtrl.h"
#include "voPlugInIEPropPage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CvoPlugInIECtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CvoPlugInIECtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CvoPlugInIECtrl, COleControl)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "open", dispidopen, open, VT_UI4, VTS_BSTR VTS_UI4 VTS_UI4 VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "start", dispidstart, start, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "pause", dispidpause, pause, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getDuration", dispidgetDuration, getDuration, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "close", dispidclose, close, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "stop", dispidstop, stop, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setPosition", dispidsetPosition, setPosition, VT_I4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getPosition", dispidgetPosition, getPosition, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getPlayerStatus", dispidgetPlayerStatus, getPlayerStatus, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "canBePaused", dispidcanBePaused, canBePaused, VT_BOOL, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setVolume", dispidsetVolume, setVolume, VT_UI4, VTS_R4 VTS_R4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "FullScreen", dispidFullScreen, FullScreen, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "mute", dispidmute, mute, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "unmute", dispidunmute, unmute, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setVideoAspectRatio", dispidsetVideoAspectRatio, setVideoAspectRatio, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setLicenseContent", dispidsetLicenseContent, setLicenseContent, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setPreAgreedLicense", dispidsetPreAgreedLicense, setPreAgreedLicense, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setLicenseFilePath", dispidsetLicenseFilePath, setLicenseFilePath, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableDeblock", dispidenableDeblock, enableDeblock, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDeviceCapabilityByFile", dispidsetDeviceCapabilityByFile, setDeviceCapabilityByFile, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setInitialBitrate", dispidsetInitialBitrate, setInitialBitrate, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getMinPosition", dispidgetMinPosition, getMinPosition, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getMaxPosition", dispidgetMaxPosition, getMaxPosition, VT_I4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getVideoCount", dispidgetVideoCount, getVideoCount, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getAudioCount", dispidgetAudioCount, getAudioCount, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getSubtitleCount", dispidgetSubtitleCount, getSubtitleCount, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "selectVideo", dispidselectVideo, selectVideo, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "selectAudio", dispidselectAudio, selectAudio, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "selectSubtitle", dispidselectSubtitle, selectSubtitle, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "isVideoAvailable", dispidisVideoAvailable, isVideoAvailable, VT_BOOL, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "isAudioAvailable", dispidisAudioAvailable, isAudioAvailable, VT_BOOL, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "isSubtitleAvailable", dispidisSubtitleAvailable, isSubtitleAvailable, VT_BOOL, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "commitSelection", dispidcommitSelection, commitSelection, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "clearSelection", dispidclearSelection, clearSelection, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getVideoProperty", dispidgetVideoProperty, getVideoProperty, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getAudioProperty", dispidgetAudioProperty, getAudioProperty, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getSubtitleProperty", dispidgetSubtitleProperty, getSubtitleProperty, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getPlayingAsset", dispidgetPlayingAsset, getPlayingAsset, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getVolume", dispidgetVolume, getVolume, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitlePath", dispidsetSubtitlePath, setSubtitlePath, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableSubtitle", dispidenableSubtitle, enableSubtitle, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontColor", dispidsetSubtitleFontColor, setSubtitleFontColor, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontOpacity", dispidsetSubtitleFontOpacity, setSubtitleFontOpacity, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontSizeScale", dispidsetSubtitleFontSizeScale, setSubtitleFontSizeScale, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontBackgroundColor", dispidsetSubtitleFontBackgroundColor, setSubtitleFontBackgroundColor, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontBackgroundOpacity", dispidsetSubtitleFontBackgroundOpacity, setSubtitleFontBackgroundOpacity, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleWindowBackgroundColor", dispidsetSubtitleWindowBackgroundColor, setSubtitleWindowBackgroundColor, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleWindowBackgroundOpacity", dispidsetSubtitleWindowBackgroundOpacity, setSubtitleWindowBackgroundOpacity, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontItalic", dispidsetSubtitleFontItalic, setSubtitleFontItalic, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontBold", dispidsetSubtitleFontBold, setSubtitleFontBold, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontUnderline", dispidsetSubtitleFontUnderline, setSubtitleFontUnderline, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontName", dispidsetSubtitleFontName, setSubtitleFontName, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontEdgeType", dispidsetSubtitleFontEdgeType, setSubtitleFontEdgeType, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontEdgeOpacity", dispidsetSubtitleFontEdgeOpacity, setSubtitleFontEdgeOpacity, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "resetSubtitleParameter", dispidresetSubtitleParameter, resetSubtitleParameter, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setSubtitleFontEdgeColor", dispidsetSubtitleFontEdgeColor, setSubtitleFontEdgeColor, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "registerEventHandler", dispidregisterEventHandler, registerEventHandler, VT_UI4, VTS_DISPATCH)
  DISP_PROPERTY_ID(CvoPlugInIECtrl, "version", dispidVersion, m_Version, VT_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setHTTPHeader", dispidsetHTTPHeader, setHTTPHeader, VT_UI4, VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setHTTPProxy", dispidsetHTTPProxy, setHTTPProxy, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "EnableAnalytics", dispidEnableAnalytics, EnableAnalytics, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "StartAnalyticsNotification", dispidStartAnalyticsNotification, StartAnalyticsNotification, VT_UI4, VTS_I4 VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "StopAnalyticsNotification", dispidStopAnalyticsNotification, StopAnalyticsNotification, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "GetAnalytics", dispidGetAnalytics, GetAnalytics, VT_BSTR, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setAudioPlaybackSpeed", dispidsetAudioPlaybackSpeed, setAudioPlaybackSpeed, VT_UI4, VTS_R4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "isLiveStreaming", dispidisLiveStreaming, isLiveStreaming, VT_BOOL, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "isFullScreen", dispidisFullScreen, isFullScreen, VT_BOOL, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setPresentationDelay", dispidsetPresentationDelay, setPresentationDelay, VT_UI8, VTS_UI8)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "toggleOverlayUI", dispidtoggleOverlayUI, toggleOverlayUI, VT_UI8, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getCurrentSelection", dispidgetCurrentSelection, getCurrentSelection, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setAnewBufferingTime", dispidsetAnewBufferingTime, setAnewBufferingTime, VT_UI8, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableAudioStream", dispidenableAudioStream, enableAudioStream, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableAudioEffect", dispidenableAudioEffect, enableAudioEffect, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDRMVerificationInfo", dispidsetDRMVerificationInfo, setDRMVerificationInfo, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getDRMUniqueIndentifier", dispidgetDRMUniqueIndentifier, getDRMUniqueIndentifier, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDRMUniqueIndentifier", dispidsetDRMUniqueIndentifier, setDRMUniqueIndentifier, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDRMLibrary", dispidsetDRMLibrary, setDRMLibrary, VT_UI4, VTS_BSTR VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getParameter", dispidgetParameter, getParameter, VT_UI4, VTS_UI4 VTS_PUI1)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setParameter", dispidsetParameter, setParameter, VT_UI4, VTS_UI4 VTS_PUI1)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "init", dispidinit, init, VT_UI4, VTS_UI4 VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setView", dispidsetView, setView, VT_UI4, VTS_PUI1)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDisaplayArea", dispidsetDisaplayArea, setDisaplayArea, VT_UI4, VTS_PUI1)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "suspend", dispidsuspend, suspend, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "resume", dispidresume, resume, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setOnEventHandler", dispidsetOnEventHandler, setOnEventHandler, VT_UI4, VTS_DISPATCH)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableLiveStreamingDVRPosition", dispidenableLiveStreamingDVRPosition, enableLiveStreamingDVRPosition, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getAudioDecodingBitrate", dispidgetAudioDecodingBitrate, getAudioDecodingBitrate, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getVideoDecodingBitrate", dispidgetVideoDecodingBitrate, getVideoDecodingBitrate, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDRMUniqueIdentifier", dispidsetDRMUniqueIdentifier, setDRMUniqueIdentifier, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getDRMUniqueIdentifier", dispidgetDRMUniqueIdentifier, getDRMUniqueIdentifier, VT_BSTR, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setInitialBufferTime", dispidsetInitialBufferTime, setInitialBufferTime, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableSEI", dispidenableSEI, enableSEI, VT_UI4, VTS_UI4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "startSEINotification", dispidstartSEINotification, startSEINotification, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "stopSEINotification", dispidstopSEINotification, stopSEINotification, VT_UI4, VTS_NONE)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getSEIInfo", dispidgetSEIInfo, getSEIInfo, VT_BSTR, VTS_I4 VTS_UI4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableCPUAdaptation", dispidenableCPUAdaptation, enableCPUAdaptation, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setBitrateThreshold", dispidsetBitrateThreshold, setBitrateThreshold, VT_UI4, VTS_I4 VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "updateSourceURL", dispidupdateSourceURL, updateSourceURL, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "enableDolbyLibrary", dispidenableDolbyLibrary, enableDolbyLibrary, VT_UI4, VTS_BOOL)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "getVersion", dispidgetVersion, getVersion, VT_BSTR, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setHTTPRetryTimeout", dispidsetHTTPRetryTimeout, setHTTPRetryTimeout, VT_UI4, VTS_I4)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDefaultAudioLanguage", dispidsetDefaultAudioLanguage, setDefaultAudioLanguage, VT_UI4, VTS_BSTR)
  DISP_FUNCTION_ID(CvoPlugInIECtrl, "setDefaultSubtitleLanguage", dispidsetDefaultSubtitleLanguage, setDefaultSubtitleLanguage, VT_UI4, VTS_BSTR)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CvoPlugInIECtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CvoPlugInIECtrl, 1)
	PROPPAGEID(CvoPlugInIEPropPage::guid)
END_PROPPAGEIDS(CvoPlugInIECtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CvoPlugInIECtrl, "VOPLUGINIE.voPlugInIECtrl.1",
	0x1381487f, 0x4e33, 0x4e67, 0xa7, 0x5e, 0x71, 0xbe, 0xa4, 0xaa, 0x8a, 0x3c)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CvoPlugInIECtrl, _tlid, _wVerMajor, _wVerMinor)

BEGIN_INTERFACE_MAP(CvoPlugInIECtrl, COleControl )
  INTERFACE_PART(CvoPlugInIECtrl, IID_IObjectSafety, MyObjSafe)
END_INTERFACE_MAP()

// Interface IDs

const IID BASED_CODE IID_DvoPlugInIE =
		{ 0x86E0D131, 0x83F8, 0x4591, { 0x99, 0x60, 0xEC, 0x88, 0x68, 0x5A, 0xF6, 0x3A } };
const IID BASED_CODE IID_DvoPlugInIEEvents =
		{ 0x31327542, 0x88D4, 0x4A48, { 0x84, 0xCF, 0x93, 0xD7, 0x6C, 0xC5, 0xA4, 0x46 } };



// Control type information

static const DWORD BASED_CODE _dwvoPlugInIEOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	//OLEMISC_IGNOREACTIVATEWHENVISIBLE |  //make ole windowless object valid 
	//OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CvoPlugInIECtrl, IDS_VOPLUGINIE, _dwvoPlugInIEOleMisc)




// CvoPlugInIECtrl::CvoPlugInIECtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CvoPlugInIECtrl

BOOL CvoPlugInIECtrl::CvoPlugInIECtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VOPLUGINIE,
			IDB_VOPLUGINIE,
			afxRegApartmentThreading,
			_dwvoPlugInIEOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}

// CvoPlugInIECtrl::CvoPlugInIECtrl - Constructor

CvoPlugInIECtrl::CvoPlugInIECtrl()
{
	InitializeIIDs(&IID_DvoPlugInIE, &IID_DvoPlugInIEEvents);
	// TODO: Initialize your control's instance data here.

  InitMembersValues();

  m_bDDRraw = FALSE;
  m_hwndRenderMsg = NULL;

  CreateNotifyRenderWnd();

  if(NULL == m_pOSMPEngnWrap)
    InitOSMPEngnWrap();

  m_bHybridMode = IsEnableHybridMode();
}



// CvoPlugInIECtrl::~CvoPlugInIECtrl - Destructor

CvoPlugInIECtrl::~CvoPlugInIECtrl()
{
	// TODO: Cleanup your control's instance data here.
	VOLOGI("destruct to delete engine.");

	m_nStatus = 0;
  ::SendMessage(m_hwndRenderMsg,WM_CLOSE,NULL,NULL);
  ::DestroyWindow(m_hwndRenderMsg);

  memset(&m_drawRect,0,sizeof(RECT));

  UnInitOSMPEngnWrap();
}



// CvoPlugInIECtrl::OnDraw - Drawing function

void CvoPlugInIECtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

  if(m_bDDRraw)
  {
    VOLOGI("OnDraw when in DDDraw");

    if(m_pOSMPEngnWrap)
      m_pOSMPEngnWrap->ToggleOverlayUI(TRUE);
  }

  if(NULL == m_pOSMPEngnWrap || m_hwndBrowser==NULL)
    InitOSMPEngnWrap();

  if(NULL == m_pOSMPEngnWrap)
    return;

  if(rcBounds != m_drawRect)
  {
    VOLOGI("rect changed!!!! New %d %d %d %d, Old %d %d %d %d", 
      rcBounds.left, rcBounds.right, rcBounds.top, rcBounds.bottom, 
      m_drawRect.left, m_drawRect.right, m_drawRect.top, m_drawRect.bottom);
    pdc->FillSolidRect(rcBounds,RGB_DEFAULT_BKG);
    m_drawRect = rcBounds;
    m_pOSMPEngnWrap->updateViewRegion(m_drawRect);
  }

  if (m_nStatus != 3 && m_nStatus != 4)  
	return;

  m_pOSMPEngnWrap->SetDC(pdc->GetSafeHdc());

  m_pOSMPEngnWrap->Redraw();

  if (m_bDDRraw)
  {
    if(m_pOSMPEngnWrap)
      m_pOSMPEngnWrap->ToggleOverlayUI(FALSE);
  }

	if (!IsOptimizedDraw())
	{
		// The container does not support optimized drawing.

		// TODO: if you selected any GDI objects into the device context *pdc,
		//		restore the previously-selected objects here.
	}
}


// CvoPlugInIECtrl::DoPropExchange - Persistence support

void CvoPlugInIECtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CvoPlugInIECtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CvoPlugInIECtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control can activate without creating a window.
	// TODO: when writing the control's message handlers, avoid using
	//		the m_hWnd member variable without first checking that its
	//		value is non-NULL.
	dwFlags |= windowlessActivate;

	// The control can receive mouse notifications when inactive.
	// TODO: if you write handlers for WM_SETCURSOR and WM_MOUSEMOVE,
	//		avoid using the m_hWnd member variable without first
	//		checking that its value is non-NULL.
	dwFlags |= pointerInactive;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
//	dwFlags |= canOptimizeDraw;
	return dwFlags;
}



// CvoPlugInIECtrl::OnResetState - Reset control to default state

void CvoPlugInIECtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}





BOOL CvoPlugInIECtrl::CreateNotifyRenderWnd()
{
  WNDCLASS wcex;
  wcex.style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= (WNDPROC)msgWindowProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= AfxGetInstanceHandle();
  wcex.hIcon   = NULL;
  wcex.hCursor  = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName	= (LPCTSTR)NULL;
  wcex.lpszClassName	= _T("PluginProRenderWindow");

  int iRet = RegisterClass(&wcex);
  
  m_hwndRenderMsg = CreateWindow(_T("PluginProRenderWindow"), _T("PluginProRenderWindow"), WS_MINIMIZE,
    0, 0, 10, 10, NULL, NULL, AfxGetInstanceHandle(), NULL);
  if (m_hwndRenderMsg == NULL)
    return FALSE;

  ::SetWindowLong (m_hwndRenderMsg, GWL_USERDATA, (LONG)this);
  ::SetWindowPos(m_hwndRenderMsg,NULL,0,0,0,0,SWP_HIDEWINDOW);

  return TRUE;
}

LRESULT CALLBACK CvoPlugInIECtrl::msgWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  CvoPlugInIECtrl * pPlugin = (CvoPlugInIECtrl *)GetWindowLong (hwnd, GWL_USERDATA);
  if (pPlugin == NULL)
    return(::DefWindowProc(hwnd, uMsg, wParam, lParam));
  else
    return pPlugin->OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

LRESULT CvoPlugInIECtrl::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;

  switch (uMsg)
  {
  case WM_NOTIFY_CALLBACK:
      {
          procNotifyCallBackMsg();
      }
      return 0;
  case WM_USERDEFNOTIFY:
    {
      if(m_bDDRraw)
        return 0;

      Refresh();      
      return 0;
    }
  case WM_CHANGE_TO_DDDRAW:
    {
      ::SetTimer(m_hwndRenderMsg,TIMER_CHANGE_DDDRAW,100,NULL);
      return 0;
    }
  case WM_TIMER:
    {
      if (wParam == TIMER_CHANGE_DDDRAW)
      {
        ::KillTimer(m_hwndRenderMsg,TIMER_CHANGE_DDDRAW);

        int nRC = 0;
        if(m_pOSMPEngnWrap)
          nRC = m_pOSMPEngnWrap->ToggleOverlayUI(FALSE);

        //if (m_pOSMPEngnWrap->GetIsCreateDDdrawFailed())
        //{
        //  m_bDDRraw = FALSE;
        //}
        
        return 0;
      }
    }
    break;
  default:
    break;
  }

  return	::DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//1.Win8 2.pluginSetting.ini
BOOL CvoPlugInIECtrl::IsEnableHybridMode()
{
  //firstly, if this os is win8
  {
      if (m_pOSMPEngnWrap && m_pOSMPEngnWrap->GetOSMPPlayer())
    {
          if(m_pOSMPEngnWrap->GetOSMPPlayer()->IsWin8OS())
      {
        VOLOGI("Win8 is not need switch render type.");
        return FALSE;
      }
    }
  }

  //secondly, get value from pluginSetting.ini
  {
    TCHAR szAppPath[MAX_PATH] = _T("");
    GetModuleFileName(AfxGetInstanceHandle(),szAppPath, MAX_PATH);
    VO_PTCHAR pPos = vostrrchr(szAppPath, _T('/'));
    if (pPos == NULL)
      pPos = vostrrchr(szAppPath, _T('\\'));
    VO_S32 nPos = pPos - szAppPath;
    szAppPath[nPos+1] = _T('\0');

    TCHAR szIniFile[MAX_PATH] = _T("");
    _tcscpy(szIniFile,szAppPath);
    _tcscat(szIniFile,PLUGIN_SETTING_INI_FILE);

    TCHAR szTemp[10] = _T("");
    GetPrivateProfileString(_T("Render"),_T("HybridMode"), _T("0"), szTemp, 10, szIniFile);

    int iHybridMode =  _ttoi(szTemp);
    VOLOGI("get HybridMode is %d", iHybridMode);
    if (iHybridMode>=1)
    {
      return TRUE;
    }
  }

  return FALSE;
}
