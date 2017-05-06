// voPlugInIECtrl.cpp : Implementation of the CvoPlugInIECtrl ActiveX Control class.

#include "stdafx.h"

#include <Mshtml.h>
#include <shlwapi.h>

#include "voPlugInIE.h"
#include "voPlugInIECtrl.h"
#include "voPlugInIEPropPage.h"

#include "voFile.h"
#include "CBaseConfig.h"
#include "voOSFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CvoPlugInIECtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CvoPlugInIECtrl, COleControl)
	ON_MESSAGE(OCM_COMMAND, &CvoPlugInIECtrl::OnOcmCommand)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_DESTROY()
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
	0xccf8affb, 0xdb3f, 0x4cb6, 0xbc, 0xd7, 0xd4, 0x13, 0x79, 0xab, 0x6d, 0x70)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CvoPlugInIECtrl, _tlid, _wVerMajor, _wVerMinor)

BEGIN_INTERFACE_MAP(CvoPlugInIECtrl, COleControl )
  INTERFACE_PART(CvoPlugInIECtrl, IID_IObjectSafety, MyObjSafe)
END_INTERFACE_MAP()

// Interface IDs

const IID BASED_CODE IID_DvoPlugInIE =
		{ 0x48B7F877, 0x8D2B, 0x4516, { 0xA4, 0x6B, 0xD5, 0x14, 0x3C, 0xEB, 0xFC, 0xDC } };
const IID BASED_CODE IID_DvoPlugInIEEvents =
		{ 0x89CB9BC4, 0x5F55, 0x430E, { 0x99, 0x6C, 0x34, 0xA4, 0x3D, 0x41, 0xD3, 0x44 } };



// Control type information

static const DWORD BASED_CODE _dwvoPlugInIEOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
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

  // TODO:  Add your specialized creation code here
  InitOSMPEngnWrap();
}



// CvoPlugInIECtrl::~CvoPlugInIECtrl - Destructor

CvoPlugInIECtrl::~CvoPlugInIECtrl()
{
	// TODO: Cleanup your control's instance data here.
	VOLOGI("destruct to delete engine.");
	UnInitOSMPEngnWrap();
}



// CvoPlugInIECtrl::OnDraw - Drawing function

void CvoPlugInIECtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;
	pdc->FillSolidRect(rcBounds,RGB_DEFAULT_BKG);

	//DoSuperclassPaint(pdc, rcBounds);
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


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;
	return dwFlags;
}



// CvoPlugInIECtrl::OnResetState - Reset control to default state

void CvoPlugInIECtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CvoPlugInIECtrl::PreCreateWindow - Modify parameters for CreateWindowEx

BOOL CvoPlugInIECtrl::PreCreateWindow(CREATESTRUCT& cs)
{
  VOLOGI("Window created");
	cs.lpszClass = _T("STATIC");
	return COleControl::PreCreateWindow(cs);
}



// CvoPlugInIECtrl::IsSubclassedControl - This is a subclassed control

BOOL CvoPlugInIECtrl::IsSubclassedControl()
{
	return TRUE;
}



// CvoPlugInIECtrl::OnOcmCommand - Handle command messages

LRESULT CvoPlugInIECtrl::OnOcmCommand(WPARAM wParam, LPARAM lParam)
{
#ifdef _WIN32
	WORD wNotifyCode = HIWORD(wParam);
#else
	WORD wNotifyCode = HIWORD(lParam);
#endif

	// TODO: Switch on wNotifyCode here.

	return 0;
}



// CvoPlugInIECtrl message handlers

int CvoPlugInIECtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  VOLOGI("NewPlugin Start!");
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

    VOLOGI("the browser window is %d", m_hWnd);

	// TODO:  Add your specialized creation code here
    InitOSMPEngnWrap();//the browser atl window is ready now

	return 0;
}

void CvoPlugInIECtrl::OnDestroy()
{
	COleControl::OnDestroy();

    // TODO: Add your message handler code here	
	VOLOGI("here to delete engine.");
    UnInitOSMPEngnWrap();
}

void CvoPlugInIECtrl::OnClose(DWORD dwSaveOption)
{
    COleControl::OnClose(dwSaveOption);

    VOLOGI("here to close engine.");
    UnInitOSMPEngnWrap();
}

LRESULT CvoPlugInIECtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (NULL!=m_hwndBrowser && m_hwndBrowser!=GetSafeHwnd())
	{
		VOLOGI("new window has been created!");
	}

	// TODO: Add your specialized code here and/or call the base class
	switch (message)
	{
    case WM_NOTIFY_CALLBACK:
        {
            procNotifyCallBackMsg();
        }
        return 0;
	case WM_SIZE://fix bug:[Window]IE-->Video doesn't adjust itself well while IE is zoomed https://sh.visualon.com/node/30244
		if (m_pOSMPEngnWrap && FALSE == m_pOSMPEngnWrap->IsFullScreen())
		{
			m_pOSMPEngnWrap->postMessage2UI(WM_SIZE, wParam, lParam);
		}    
		break;

	case WM_MOVE:
		{
			VOLOGI("moving....");
		}
		break;

	case WM_SYSKEYDOWN:
		{
			switch(wParam)
			{
			}
			VOLOGI("sys key: 0x%02x, 0x%02x", wParam, lParam);
		}
		break;

	case WM_KEYDOWN:
		{
			VOLOGI("COMMON key: 0x%02x, 0x%02x", wParam, lParam);
		}
		break;

	case WM_CLOSE:
		{
			VOLOGI("here to do close...");
		}
		break;
	default:
		break;
	}

	return COleControl::WindowProc(message, wParam, lParam);
}
