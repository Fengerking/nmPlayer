/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#include "afxwin.h"
#include "config.h"
#include "voScriptableObject.h"
#include "voOSFunc.h"
#include "voLog.h"


#define CORRECT_RET(x) { if (x < 0) x = -x; }

// for player control
static NPIdentifier     keyInit;
static NPIdentifier     keyOpen;
static NPIdentifier     keyClose;
static NPIdentifier     keyPlay;
static NPIdentifier     keyPause;
static NPIdentifier     keyStop;
static NPIdentifier     keySetPosition;
static NPIdentifier     keyGetPosition;
static NPIdentifier     keyGetDuration;
static NPIdentifier     keySetVolume;
static NPIdentifier     keyGetVolume;
static NPIdentifier     keyMute;
static NPIdentifier     keyUnMute;
static NPIdentifier     keySetVideoAspectRatio;
static NPIdentifier     keyGetPlayerStatus;
static NPIdentifier     keyCanBePaused;
static NPIdentifier		keyFullScreen;
static NPIdentifier		keyIsFullScreen;
static NPIdentifier		keyIsLiveStreaming;
static NPIdentifier		keyToggleOverlayUI;

//for player Configuration
static NPIdentifier     keySetLicenseContent;
static NPIdentifier     keySetPreAgreedLicense;
static NPIdentifier     keySetLicenseFilePath;
static NPIdentifier     keyEnableDeblock;
static NPIdentifier     keySetDeviceCapabilityByFile;
static NPIdentifier     keySetInitialBitrate;
static NPIdentifier     keyGetMinPosition;
static NPIdentifier     keyGetMaxPosition;
static NPIdentifier     keySetPresentationDelay;
static NPIdentifier     keySetAnewBufferingTime;
static NPIdentifier		keySetInitialBufferTime;
static NPIdentifier		keyEnableAudioEffect;
static NPIdentifier		keyEnableAudioStream;
static NPIdentifier		keySetDRMVerificationInfo;
static NPIdentifier		keyGetDRMUniqueIdentifier;
static NPIdentifier		keyGetDRMUniqueIndentifier;
static NPIdentifier		keySetDRMUniqueIdentifier;
static NPIdentifier		keySetDRMUniqueIndentifier;
static NPIdentifier		keySetDRMLibrary;
static NPIdentifier		keyShowCursor;
static NPIdentifier		keyEnableLiveStreamingDVRPosition;
static NPIdentifier		keyGetVideoDecodingBitrate;
static NPIdentifier		keyGetAudioDecodingBitrate;
static NPIdentifier		keyEnableSEI;
static NPIdentifier		keyStartSEINotification;
static NPIdentifier		keyStopSEINotification;
static NPIdentifier		keyGetSEIInfo;
static NPIdentifier		keyEnableCPUAdaptation;

//Player Asset Selection
static NPIdentifier     keyGetVideoCount;
static NPIdentifier     keyGetAudioCount;
static NPIdentifier     keyGetSubtitleCount;
static NPIdentifier     keySelectVideo;
static NPIdentifier     keySelectAudio;
static NPIdentifier     keySelectSubtitle;
static NPIdentifier     keyIsVideoAvailable;
static NPIdentifier     keyIsAudioAvailable;
static NPIdentifier     keyIsSubtitleAvailable;
static NPIdentifier     keyCommitSelection;
static NPIdentifier     keyClearSelection;
static NPIdentifier     keyGetVideoProperty;
static NPIdentifier     keyGetAudioProperty;
static NPIdentifier     keyGetSubtitleProperty;
static NPIdentifier     keyGetPlayingAsset;
static NPIdentifier     keyGetCurrentSelection;
// for subtitle
static NPIdentifier		keySetSubtitlePath;
static NPIdentifier		keyEnableSubtitle;
static NPIdentifier		keySetSubtitleFontColor;
static NPIdentifier		keySetSubtitleFontOpacity;
static NPIdentifier		keySetSubtitleFontSizeScale;
static NPIdentifier		keySetSubtitleFontBackgroundColor;
static NPIdentifier		keySetSubtitleFontBackgroundOpacity;
static NPIdentifier		keySetSubtitleWindowBackgroundColor;
static NPIdentifier		keySetSubtitleWindowBackgroundOpacity;
static NPIdentifier		keySetSubtitleFontItalic;
static NPIdentifier		keySetSubtitleFontBold;
static NPIdentifier		keySetSubtitleFontUnderline;
static NPIdentifier		keySetSubtitleFontName;
static NPIdentifier		keySetSubtitleFontEdgeType;
static NPIdentifier		keySetSubtitleFontEdgeColor;
static NPIdentifier		keySetSubtitleFontEdgeOpacity;
static NPIdentifier		keyResetSubtitleParameter;
static NPIdentifier		keySetHttpHeader;
static NPIdentifier		keySetHttpProxy;

static NPIdentifier     keyEnableAnalytics;
static NPIdentifier     keyStartAnalyticsNotification;
static NPIdentifier     keyStopAnalyticsNotification;
static NPIdentifier     keyGetAnalytics;
static NPIdentifier		keySetAudioPlaybackSpeed;

// for callback
static NPIdentifier		keyRegisterEventHandler;
static NPIdentifier		keyJSCallback; 

// property
static NPIdentifier		keyVersion;

extern TCHAR g_lpszPluginPath[MAX_PATH];

voScriptableObject::voScriptableObject(NPP npp) : m_pnpp(npp)
												, m_pplgn(NULL)
												, m_pOsmpEngn(NULL)
												, m_nOpenFlag(VO_OSMP_FLAG_SRC_OPEN_SYNC | VOOSMP_FLAG_SOURCE_URL)
                        , m_OpenType(VO_OSMP_SRC_AUTO_DETECT)
{
	// player control
	keyInit = NPN_GetStringIdentifier("init");
	keyOpen = NPN_GetStringIdentifier("open");
	keyClose = NPN_GetStringIdentifier("close");
	keyPlay = NPN_GetStringIdentifier("start");
	keyPause = NPN_GetStringIdentifier("pause");
	keyStop = NPN_GetStringIdentifier("stop");
	keySetPosition = NPN_GetStringIdentifier("setPosition");
	keyGetPosition = NPN_GetStringIdentifier("getPosition");
	keyGetDuration = NPN_GetStringIdentifier("getDuration");
	keySetVolume = NPN_GetStringIdentifier("setVolume");
	keyGetVolume = NPN_GetStringIdentifier("getVolume");
	keyMute = NPN_GetStringIdentifier("mute");
	keyUnMute = NPN_GetStringIdentifier("unmute");
	keySetVideoAspectRatio = NPN_GetStringIdentifier("setVideoAspectRatio");
	keyGetPlayerStatus = NPN_GetStringIdentifier("getPlayerStatus");
	keyCanBePaused = NPN_GetStringIdentifier("canBePaused");
	keyFullScreen = NPN_GetStringIdentifier("fullScreen");
	keyIsFullScreen = NPN_GetStringIdentifier("isFullScreen");
	keyIsLiveStreaming = NPN_GetStringIdentifier("isLiveStreaming");
	keyToggleOverlayUI = NPN_GetStringIdentifier("toggleOverlayUI");

	//Player Configuration
	keySetLicenseContent = NPN_GetStringIdentifier("setLicenseContent");
	keySetPreAgreedLicense = NPN_GetStringIdentifier("setPreAgreedLicense");
	keySetLicenseFilePath = NPN_GetStringIdentifier("setLicenseFilePath");
	keyEnableDeblock = NPN_GetStringIdentifier("enableDeblock");
	keySetDeviceCapabilityByFile = NPN_GetStringIdentifier("setDeviceCapabilityByFile");
	keySetInitialBitrate = NPN_GetStringIdentifier("setInitialBitrate");
	keyGetMinPosition = NPN_GetStringIdentifier("getMinPosition");
	keyGetMaxPosition = NPN_GetStringIdentifier("getMaxPosition");
	keySetPresentationDelay = NPN_GetStringIdentifier("setPresentationDelay");
	keySetAnewBufferingTime = NPN_GetStringIdentifier("setAnewBufferingTime");
	keySetInitialBufferTime = NPN_GetStringIdentifier("setInitialBufferTime");
	keyEnableAudioEffect = NPN_GetStringIdentifier("enableAudioEffect");
	keyEnableAudioStream = NPN_GetStringIdentifier("enableAudioStream");
	keySetDRMVerificationInfo = NPN_GetStringIdentifier("setDRMVerificationInfo");
	keyGetDRMUniqueIdentifier = NPN_GetStringIdentifier("getDRMUniqueIdentifier");
	keyGetDRMUniqueIndentifier = NPN_GetStringIdentifier("getDRMUniqueIndentifier");
	keySetDRMUniqueIdentifier = NPN_GetStringIdentifier("setDRMUniqueIdentifier");
	keySetDRMUniqueIndentifier = NPN_GetStringIdentifier("setDRMUniqueIndentifier");
	keySetDRMLibrary = NPN_GetStringIdentifier("setDRMLibrary");
	keyShowCursor = NPN_GetStringIdentifier("showCursor");
	keyEnableLiveStreamingDVRPosition = NPN_GetStringIdentifier("enableLiveStreamingDVRPosition");
	keyGetVideoDecodingBitrate = NPN_GetStringIdentifier("getVideoDecodingBitrate");
	keyGetAudioDecodingBitrate = NPN_GetStringIdentifier("getAudioDecodingBitrate");
	keyEnableSEI = NPN_GetStringIdentifier("enableSEI");
	keyStartSEINotification = NPN_GetStringIdentifier("startSEINotification");
	keyStopSEINotification = NPN_GetStringIdentifier("stopSEINotification");
	keyGetSEIInfo = NPN_GetStringIdentifier("getSEIInfo");
	keyEnableCPUAdaptation = NPN_GetStringIdentifier("enableCPUAdaptation");

	//Player Asset Selection
	keyGetVideoCount = NPN_GetStringIdentifier("getVideoCount");
	keyGetAudioCount = NPN_GetStringIdentifier("getAudioCount");
	keyGetSubtitleCount = NPN_GetStringIdentifier("getSubtitleCount");
	keySelectVideo = NPN_GetStringIdentifier("selectVideo");
	keySelectAudio = NPN_GetStringIdentifier("selectAudio");
	keySelectSubtitle = NPN_GetStringIdentifier("selectSubtitle");
	keyIsVideoAvailable = NPN_GetStringIdentifier("isVideoAvailable");
	keyIsAudioAvailable = NPN_GetStringIdentifier("isAudioAvailable");
	keyIsSubtitleAvailable = NPN_GetStringIdentifier("isSubtitleAvailable");
	keyCommitSelection = NPN_GetStringIdentifier("commitSelection");
	keyClearSelection = NPN_GetStringIdentifier("clearSelection");
	keyGetVideoProperty = NPN_GetStringIdentifier("getVideoProperty");
	keyGetAudioProperty = NPN_GetStringIdentifier("getAudioProperty");
	keyGetSubtitleProperty = NPN_GetStringIdentifier("getSubtitleProperty");
	keyGetPlayingAsset = NPN_GetStringIdentifier("getPlayingAsset");
	keyGetCurrentSelection = NPN_GetStringIdentifier("getCurrentSelection");

	// for subtitle relatives
	keySetSubtitlePath = NPN_GetStringIdentifier("setSubtitlePath");
	keyEnableSubtitle = NPN_GetStringIdentifier("enableSubtitle");
	keySetSubtitleFontColor = NPN_GetStringIdentifier("setSubtitleFontColor");
	keySetSubtitleFontOpacity = NPN_GetStringIdentifier("setSubtitleFontOpacity");
	keySetSubtitleFontSizeScale = NPN_GetStringIdentifier("setSubtitleFontSizeScale");
	keySetSubtitleFontBackgroundColor = NPN_GetStringIdentifier("setSubtitleFontBackgroundColor");
	keySetSubtitleFontBackgroundOpacity = NPN_GetStringIdentifier("setSubtitleFontBackgroundOpacity");
	keySetSubtitleWindowBackgroundColor = NPN_GetStringIdentifier("setSubtitleWindowBackgroundColor");
	keySetSubtitleWindowBackgroundOpacity = NPN_GetStringIdentifier("setSubtitleWindowBackgroundOpacity");
	keySetSubtitleFontItalic = NPN_GetStringIdentifier("setSubtitleFontItalic");
	keySetSubtitleFontBold = NPN_GetStringIdentifier("setSubtitleFontBold");
	keySetSubtitleFontUnderline = NPN_GetStringIdentifier("setSubtitleFontUnderline");
	keySetSubtitleFontName = NPN_GetStringIdentifier("setSubtitleFontName");
	keySetSubtitleFontEdgeType = NPN_GetStringIdentifier("setSubtitleFontEdgeType");
	keySetSubtitleFontEdgeType = NPN_GetStringIdentifier("setSubtitleFontEdgeColor");
	keySetSubtitleFontEdgeOpacity = NPN_GetStringIdentifier("setSubtitleFontEdgeOpacity");
	keyResetSubtitleParameter = NPN_GetStringIdentifier("resetSubtitleParameter");
	keySetHttpHeader = NPN_GetStringIdentifier("setHTTPHeader");
	keySetHttpProxy = NPN_GetStringIdentifier("setHTTPProxy");
	keyEnableAnalytics = NPN_GetStringIdentifier("enableAnalytics");
	keyStartAnalyticsNotification = NPN_GetStringIdentifier("startAnalyticsNotification");
	keyStopAnalyticsNotification = NPN_GetStringIdentifier("stopAnalyticsNotification");
	keyGetAnalytics = NPN_GetStringIdentifier("getAnalytics");
	keySetAudioPlaybackSpeed = NPN_GetStringIdentifier("setAudioPlaybackSpeed");
	
	keyRegisterEventHandler = NPN_GetStringIdentifier("registerEventHandler");
	keyVersion = NPN_GetStringIdentifier("version");

	if (npp)
		m_pplgn = (voNPPlugin*)npp->pdata;

	memset(m_szURL, 0, sizeof(TCHAR)*BIG_BUF_LEN);
}

voScriptableObject::~voScriptableObject() {}

char* voScriptableObject::getStrVal(NPVariant& val)
{
	NPString str = NPVARIANT_TO_STRING(val);
	memset(m_szval, 0, BIG_BUF_LEN);
	if (str.UTF8Length > BIG_BUF_LEN) {
		VOLOGI("input len: %d", str.UTF8Length);
		return NULL;
	}

	memcpy(m_szval, str.UTF8Characters, str.UTF8Length);
	return m_szval;
}

INT32 voScriptableObject::getIntVal(NPVariant& val) {

	INT32 nval = 0;
	if (NPVARIANT_IS_INT32(val))
		nval = NPVARIANT_TO_INT32(val);
	else if (NPVARIANT_IS_DOUBLE(val)) 
		nval = (INT32)NPVARIANT_TO_DOUBLE(val);
	else if (NPVARIANT_IS_STRING(val)) {
		NPString str = NPVARIANT_TO_STRING(val);
		memset(m_szval, 0, BIG_BUF_LEN);
		if (str.UTF8Length > BIG_BUF_LEN) {
			VOLOGI("input len: %d", str.UTF8Length);
			return 0;
		}

		memcpy(m_szval, str.UTF8Characters, str.UTF8Length);
		nval = atoi(m_szval);
	}

	return nval;
}

int voScriptableObject::getVersion(LPTSTR lpszVersion, const int nlen) {
	VOOSMP_MODULE_VERSION version;
	version.pszVersion = NULL;
	version.nModuleType = VOOSMP_MODULE_TYPE_SDK;
	int nRet = COSMPEngnWrap::GetVersion(&version, g_lpszPluginPath);

	if(nRet!=0 || !version.pszVersion)
		return 0;

	_tcscpy(lpszVersion, (TCHAR*)version.pszVersion);
	return _tcslen(lpszVersion);
}

bool voScriptableObject::getBoolVal(NPVariant& val) {
	bool bv = NPVARIANT_TO_BOOLEAN(val);
	return bv;
}

bool voScriptableObject::HasMethod(NPIdentifier name)
{

	return (  name == keyInit 
			|| name == keyOpen
			|| name == keyClose
			|| name == keyPlay
			|| name == keyPause
			|| name == keyStop
			|| name == keySetPosition
			|| name == keyGetPosition
			|| name == keyGetDuration
			|| name == keySetVolume
			|| name == keyGetVolume
			|| name == keyMute
			|| name == keyUnMute
			|| name == keySetVideoAspectRatio
			|| name == keyGetPlayerStatus
			|| name == keyCanBePaused
			|| name == keyFullScreen
			|| name == keyIsFullScreen
			|| name == keyIsLiveStreaming
			|| name == keyToggleOverlayUI
			|| name == keySetLicenseContent
			|| name == keySetPreAgreedLicense
			|| name == keySetLicenseFilePath
			|| name == keyEnableDeblock
			|| name == keySetDeviceCapabilityByFile
			|| name == keySetInitialBitrate
			|| name == keyGetMinPosition
			|| name == keyGetMaxPosition
			|| name == keySetPresentationDelay
			|| name == keySetAnewBufferingTime
			|| name == keySetInitialBufferTime
			|| name == keyEnableAudioEffect
			|| name == keyEnableAudioStream
			|| name == keySetDRMVerificationInfo
			|| name == keyGetDRMUniqueIdentifier
			|| name == keyGetDRMUniqueIndentifier
			|| name == keySetDRMUniqueIdentifier
			|| name == keySetDRMUniqueIndentifier
			|| name == keySetDRMLibrary
			|| name == keyShowCursor
			|| name == keyEnableLiveStreamingDVRPosition
			|| name == keyGetVideoDecodingBitrate
			|| name == keyGetAudioDecodingBitrate
			|| name == keyEnableSEI
			|| name == keyStartSEINotification
			|| name == keyStopSEINotification
			|| name == keyGetSEIInfo
			|| name == keyEnableCPUAdaptation
			|| name == keyGetVideoCount
			|| name == keyGetAudioCount
			|| name == keyGetSubtitleCount
			|| name == keySelectVideo
			|| name == keySelectAudio
			|| name == keySelectSubtitle
			|| name == keyIsVideoAvailable
			|| name == keyIsAudioAvailable
			|| name == keyIsSubtitleAvailable
			|| name == keyCommitSelection
			|| name == keyClearSelection
			|| name == keyGetVideoProperty
			|| name == keyGetAudioProperty
			|| name == keyGetSubtitleProperty
			|| name == keyGetPlayingAsset
			|| name == keyGetCurrentSelection
			|| name == keySetSubtitlePath
			|| name == keyEnableSubtitle
			|| name == keySetSubtitleFontColor
			|| name == keySetSubtitleFontOpacity
			|| name == keySetSubtitleFontSizeScale
			|| name == keySetSubtitleFontBackgroundColor
			|| name == keySetSubtitleFontBackgroundOpacity
			|| name == keySetSubtitleWindowBackgroundColor
			|| name == keySetSubtitleWindowBackgroundOpacity
			|| name == keySetSubtitleFontItalic
			|| name == keySetSubtitleFontBold
			|| name == keySetSubtitleFontUnderline
			|| name == keySetSubtitleFontName
			|| name == keySetSubtitleFontEdgeType
			|| name == keySetSubtitleFontEdgeColor
			|| name == keySetSubtitleFontEdgeOpacity
			|| name == keyResetSubtitleParameter
			|| name == keySetHttpHeader
			|| name == keySetHttpProxy
			|| name == keyEnableAnalytics
			|| name == keyStartAnalyticsNotification
			|| name == keyStopAnalyticsNotification
			|| name == keyGetAnalytics
			|| name == keySetAudioPlaybackSpeed
			|| name == keyRegisterEventHandler
			);
}

bool voScriptableObject::HasProperty(NPIdentifier name)
{
   return (name == keyVersion);
}

bool voScriptableObject::GetProperty(NPIdentifier name, NPVariant *result)
{
	if (name == keyVersion) {
		TCHAR lpszVer[128];
		memset(lpszVer, 0, sizeof(TCHAR)*128);
		int size = getVersion(lpszVer, 128);
		if (size > 0 && size < 128 - 1) {
			char* ver = (char*)NPN_MemAlloc(size + 1);
			SecureZeroMemory(ver, size + 1);
			WideCharToMultiByte(CP_UTF8, 0, lpszVer, _tcslen(lpszVer), ver, size + 1, NULL, NULL);
			STRINGZ_TO_NPVARIANT(ver, *result);
		}

		VOLOGI("get version from m_pnpp: %p", m_pnpp);
	}

	return true;
}

bool voScriptableObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argc, NPVariant *result)
{
	//VOLOGI("here do invoke!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	if (m_pOsmpEngn  ==  NULL) {

		// make Initiating-engine done here to avoid the 'Calling Method on NPObject!' error in Firefox
		// because if this initializing action is implemented in NPP_SetWindow, then the later would cost too much before returning, 
		// then the subsequent returned-up-to-JS scriptable object becomes invalid
		// David @ 2013-06-19
		if (!m_pplgn->isEngnStarted()) {
			m_pplgn->startEngn();
			m_pOsmpEngn = m_pplgn->getStrmEngn();
		}
	}


	if (name == keyInit) {

		DOUBLE_TO_NPVARIANT(0, *result);
		return true;

	} else if (name == keyOpen) {

		if (args != NULL && argc == 4) {
			NPVariant npval = args[0];
			getStrVal(npval);

			unsigned int ret = 0;
			if (strcmp(m_szval,"") == 0) {

				INT32_TO_NPVARIANT(ret, *result);
				return true;
			}
			
			VOLOGI("open link: %s", m_szval);
			MultiByteToWideChar(CP_UTF8, 0, m_szval, -1, m_szURL, sizeof(m_szURL));
			
			NPVariant npflag = args[1];
			m_nOpenFlag = getIntVal(npflag);
			VOLOGI("open flag: 0x%02x", m_nOpenFlag);
			NPVariant nptype = args[2];
			m_OpenType = getIntVal(nptype);
			m_pplgn->setStatus(1);   // openning
			ret = (unsigned int)m_pOsmpEngn->Open(m_szURL, (VO_OSMP_SRC_FLAG)m_nOpenFlag, (VO_OSMP_SRC_FORMAT)m_OpenType, 0);
			DOUBLE_TO_NPVARIANT(ret, *result);
			if (ret == (unsigned int)VO_OSMP_ERR_NONE)
				m_pplgn->setStatus(2);  // opened or prepared

			return true;
		}
	} else if (name == keyClose) {

		unsigned int ret = (unsigned int)m_pOsmpEngn->Close();
		DOUBLE_TO_NPVARIANT(ret, *result);
		m_pplgn->setStatus(0);
		return true;
	} else if (name == keyPlay) {

		int stat = m_pplgn->getStatus();
		// if stopped, then reopen, otherwise if playing back live streaming, it cannot playback from the begin
		if (stat == 5) {
			m_pplgn->setStatus(1);
			unsigned int ret = (unsigned int)m_pOsmpEngn->Open(m_szURL, (VO_OSMP_SRC_FLAG)m_nOpenFlag, (VO_OSMP_SRC_FORMAT)m_OpenType, 0);
			if (ret == (unsigned int)VO_OSMP_ERR_NONE)
				m_pplgn->setStatus(2);

			if (m_pplgn->isWindowless()) {
				DOUBLE_TO_NPVARIANT(0, *result);
				return true;
			}
		}

		unsigned int ret = (unsigned int)m_pOsmpEngn->Start();
		DOUBLE_TO_NPVARIANT(ret, *result);
		if (stat)  // if closed, just do nothing, else change status
			m_pplgn->setStatus(3);

		return true;
	} else if (name == keyPause) {

		// if closed or stopped, do nothing 
		int stat = m_pplgn->getStatus();
		if (stat == 0 || stat == 5) {
			INT32_TO_NPVARIANT(0, *result);
			return true;
		}

		unsigned int ret = (unsigned int)m_pOsmpEngn->Pause();
		DOUBLE_TO_NPVARIANT(ret, *result);
		m_pplgn->setStatus(4);
		return true;
	} else if (name == keyStop) {
		// if closed, do nothing 
		if (m_pplgn->getStatus() == 0) {
			INT32_TO_NPVARIANT(0, *result);
			return true;
		}

		unsigned int ret = (unsigned int)m_pOsmpEngn->Stop();
		DOUBLE_TO_NPVARIANT(ret, *result);
		m_pplgn->setStatus(5);

		return true;
	} else if (name == keySetPosition) {

		if (args != NULL && argc == 1)
		{
			NPVariant nppos = args[0];
			int npos = getIntVal(nppos);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetPosition(npos);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyGetPosition) {

		int nPos = m_pOsmpEngn->GetPosition();
		INT32_TO_NPVARIANT(nPos, *result);
		return true;
	} else if (name == keyGetDuration) {
	
		int nDuration = 0;
		if (m_pplgn->getStatus() < 2) {
			INT32_TO_NPVARIANT(nDuration, *result);
			return true;
		}
		nDuration = m_pOsmpEngn->GetDuration();
		INT32_TO_NPVARIANT(nDuration, *result);
		return true;
	} else if	( name == keySetVolume) {

		if (args != NULL && argc == 2)
		{

			NPVariant lv = args[0];
			NPVariant rv = args[1];
			float flv, frv;
			flv = frv = 0.0f;

			if (NPVARIANT_IS_INT32(lv)) {
				flv = (float)NPVARIANT_TO_INT32(lv);
				frv = (float)NPVARIANT_TO_INT32(rv);
			} else if (NPVARIANT_IS_DOUBLE(lv)) {
				flv = (float)NPVARIANT_TO_DOUBLE(lv);
				frv = (float)NPVARIANT_TO_DOUBLE(rv);
			} else if (NPVARIANT_IS_STRING(lv)) {
				NPString slv = NPVARIANT_TO_STRING(lv);
				NPString srv = NPVARIANT_TO_STRING(rv);
				char vol[32];
				memset(vol,0,32);
				strncpy(vol, slv.UTF8Characters, slv.UTF8Length);
				flv = (float)atof(vol);

				memset(vol,0,32);
				strncpy(vol, srv.UTF8Characters, srv.UTF8Length);
				frv = (float)atof(vol);
			}
			
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetVolume(flv, frv);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyGetVolume) {

		int vol = m_pOsmpEngn->GetVolume();
		INT32_TO_NPVARIANT(vol, *result);
		return true;

	} else if (name == keyMute) {

		unsigned int ret = (unsigned int)m_pOsmpEngn->Mute();
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;

	} else if (name == keyUnMute) {

		unsigned int ret = (unsigned int)m_pOsmpEngn->UnMute();
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;

	} else if (name == keySetVideoAspectRatio) {
		
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int var = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetVideoAspectRatio((VO_OSMP_ASPECT_RATIO)var);
			//if paused, ensure the video image change instantly as to the input aspect ratio
			//if (4 == m_pplgn->getStatus()) {
			//
			//	if (m_pplgn->isWindowless())
			//		m_pplgn->notifyBrowserToDraw();
			//	else
			//		m_pOsmpEngn->Redraw();
			//}

			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyGetPlayerStatus) {

		VO_OSMP_STATUS stat = m_pOsmpEngn->GetPlayerStatus(); 
		INT32_TO_NPVARIANT(stat, *result);
		return true;

	} else if (name == keyCanBePaused) {

		BOOL cbp = m_pOsmpEngn->CanBePaused();
		BOOLEAN_TO_NPVARIANT(cbp, *result);
		return true;
	} else if (name == keyFullScreen) {

		unsigned int ret = (unsigned int)m_pOsmpEngn->FullScreen();
		DOUBLE_TO_NPVARIANT(ret, *result);

		// to make the browser window active again after returning from fullscreen for FF - david @ 2013-05-07
		BOOL bfull = m_pOsmpEngn->IsFullScreen();
		if (!bfull) {
			if (m_pplgn && !m_pplgn->isWindowless()) {
				PostMessage(GetParent(GetParent(m_pplgn->getWindow())), WM_PAINT, 0, 0);
				SetActiveWindow(GetParent(GetParent(m_pplgn->getWindow())));
			}
		}

		return true;
	} else if ( name == keySetLicenseContent) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			unsigned char lic[LIC_CONTENT_LEN];
			NPString str = NPVARIANT_TO_STRING(args0);
			memset(lic, 0, LIC_CONTENT_LEN);
			memcpy(lic, str.UTF8Characters, str.UTF8Length);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetLicenseContent((LPCTSTR)lic);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keySetPreAgreedLicense) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			NPString str = NPVARIANT_TO_STRING(args0);
			char licc[BIG_BUF_LEN];
			memset(licc, 0, BIG_BUF_LEN);
			strncpy(licc, str.UTF8Characters, str.UTF8Length);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetPreAgreedLicense(licc);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keySetLicenseFilePath) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			getStrVal(args0);
			TCHAR licpath[BIG_BUF_LEN];
			MultiByteToWideChar(CP_UTF8, 0, m_szval, -1, licpath, sizeof(licpath));
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetLicenseFilePath(licpath);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keyEnableDeblock) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool enable = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableDeblock(enable);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keySetDeviceCapabilityByFile) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			getStrVal(args0);
			TCHAR capfile[BIG_BUF_LEN];
			MultiByteToWideChar(CP_UTF8, 0, m_szval, -1, capfile, sizeof(capfile));
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetDeviceCapabilityByFile(capfile);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keySetInitialBitrate) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int ib = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetInitialBitrate(ib);	
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keyGetMinPosition) {

		INT32_TO_NPVARIANT(m_pOsmpEngn->GetMinPosition(), *result);
		return true;
	} else if ( name == keyGetMaxPosition) {

		INT32_TO_NPVARIANT(m_pOsmpEngn->GetMaxPosition(), *result);
		return true;
	} else if ( name == keySetPresentationDelay) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nt = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetPresentationDelay(nt);
			
			DOUBLE_TO_NPVARIANT(ret, *result);
		}
		return true;
	} else if (name == keySetAnewBufferingTime) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int bt = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetAnewBufferingTime(bt);

			DOUBLE_TO_NPVARIANT(ret, *result);
		}
		return true;	
	} else if (name == keySetInitialBufferTime) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int bt = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetInitialBufferTime(bt);

			DOUBLE_TO_NPVARIANT(ret, *result);
		}
		return true;
	} else if (name == keyEnableAudioEffect) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool enable = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableAudioEffect(enable);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyEnableAudioStream) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool enable = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableAudioStream(enable);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetDRMVerificationInfo) {

		if (args != NULL && argc == 1) {

			NPVariant args0 = args[0];
			NPString vi = NPVARIANT_TO_STRING(args0);
			char drmvi[MAX_PATH];
			memset(drmvi, 0, MAX_PATH);
			if (vi.UTF8Length)
				strncpy(drmvi, vi.UTF8Characters, vi.UTF8Length);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetDRMVerificationInfo(drmvi, vi.UTF8Length);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyGetDRMUniqueIdentifier || name == keyGetDRMUniqueIndentifier) {

		int len = 0;
		char* pid = m_pOsmpEngn->GetDRMUniqueIndentifier();
		if (NULL != pid)
			len = strlen(pid);

		char* pval = (char*)NPN_MemAlloc(len + 1);
		if (NULL == pval)
			return false;

		SecureZeroMemory(pval, len + 1);
		if (len)
			strncpy(pval, pid, len);

		STRINGZ_TO_NPVARIANT(pval, *result);
		return true;

	} else if (name == keySetDRMUniqueIdentifier || name == keySetDRMUniqueIndentifier) {

		if (args != NULL && argc == 1) {

			NPVariant args0 = args[0];
			NPString id = NPVARIANT_TO_STRING(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetDRMUniqueIndentifier((char*)id.UTF8Characters);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetDRMLibrary) {

		if (args != NULL && argc == 2) {
			NPVariant args0 = args[0];
			NPString libname = NPVARIANT_TO_STRING(args0);
			char szlibname[MAX_PATH];
			memset(szlibname, 0, MAX_PATH);
			strncpy(szlibname, libname.UTF8Characters, libname.UTF8Length);

			NPVariant args1 = args[1];
			NPString apiname = NPVARIANT_TO_STRING(args1);
			char szapi[MAX_PATH];
			memset(szapi, 0, MAX_PATH);
			strncpy(szapi, apiname.UTF8Characters, apiname.UTF8Length);

			unsigned int ret = (unsigned int)m_pOsmpEngn->SetDRMLibrary(szlibname, szapi);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}

	} else if (name == keyShowCursor){

		//if (args != NULL && argc == 1) {
		//	NPVariant args0 = args[0];
		//	bool enable = getBoolVal(args0);
		//	m_pOsmpEngn->ShowCursor(enable);
		//	DOUBLE_TO_NPVARIANT(0, *result);
		//}
		return true;
	} else if (name == keyEnableLiveStreamingDVRPosition) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool enable = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableLiveStreamingDVRPosition(enable);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyGetVideoDecodingBitrate) {

		char* pbr = (char*)m_pOsmpEngn->GetVideoDecodingBitrate();
		if (pbr == NULL)
			return false;

		int len = strlen(pbr);
		char* pval = (char*)NPN_MemAlloc(len + 1);
		if (NULL == pval)
			return false;

		SecureZeroMemory(pval, len + 1);
		memcpy(pval, pbr, len);
		STRINGZ_TO_NPVARIANT(pval, *result);
		return true;
	} else if (name == keyGetAudioDecodingBitrate) {

		char* pbr = (char*)m_pOsmpEngn->GetAudioDecodingBitrate();
		if (pbr == NULL)
			return false;

		int len = strlen(pbr);
		char* pval = (char*)NPN_MemAlloc(len + 1);
		if (NULL == pval)
			return false;

		SecureZeroMemory(pval, len + 1);
		memcpy(pval, pbr, len);
		STRINGZ_TO_NPVARIANT(pval, *result);
		return true;
	} else if (name == keyEnableSEI) {

		if (args != NULL && argc == 1)
		{
			NPVariant npe = args[0];
			int ne = getIntVal(npe);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableSEI((VO_OSMP_SEI_INFO_FLAG)ne);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyStartSEINotification) {
		if (args != NULL && argc == 1)
		{
			NPVariant npi = args[0];
			int ni = getIntVal(npi);
			unsigned int ret = (unsigned int)m_pOsmpEngn->StartSEINotification(ni);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyStopSEINotification) {
		unsigned int ret = (unsigned int)m_pOsmpEngn->StopSEINotification();
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;
	} else if (name == keyGetSEIInfo) {
		
		if (args != NULL && argc == 2)
		{
			NPVariant npt = args[0];
			LONG nt = getIntVal(npt);

			NPVariant npf = args[1];
			VO_OSMP_SEI_INFO_FLAG sif = (VO_OSMP_SEI_INFO_FLAG)getIntVal(npf);
			LPCTSTR si = (LPCTSTR)m_pOsmpEngn->GetSEIInfo(nt, sif);
			int len = WideCharToMultiByte(CP_UTF8, 0, si, -1, NULL, 0, NULL, NULL) + 1;

			char* pval = (char*)NPN_MemAlloc(len);
			if (NULL == pval)
				return false;

			SecureZeroMemory(pval, len);
			WideCharToMultiByte(CP_UTF8, 0, si, _tcslen(si), pval, len, NULL, NULL);
			STRINGZ_TO_NPVARIANT(pval, *result);
			return true;
		}

	} else if (name == keyEnableCPUAdaptation) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool enable = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableCPUAdaptation(enable);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keyGetVideoCount) {

		INT32_TO_NPVARIANT(m_pOsmpEngn->GetVideoCount(), *result);
		return true;
	} else if ( name == keyGetAudioCount) {

		INT32_TO_NPVARIANT(m_pOsmpEngn->GetAudioCount(), *result);
		return true;
	} else if ( name == keyGetSubtitleCount) {

		INT32_TO_NPVARIANT(m_pOsmpEngn->GetSubtitleCount(), *result);
		return true;
	} else if ( name == keySelectVideo) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int vi = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SelectVideo(vi);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keySelectAudio) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int ai = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SelectAudio(ai);
			
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keySelectSubtitle) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int si = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SelectSubtitle(si);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if ( name == keyIsVideoAvailable) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int vi = getIntVal(args0);
			BOOL available = m_pOsmpEngn->IsVideoAvailable(vi);
			BOOLEAN_TO_NPVARIANT(available, *result);
			return true;
		}
	} else if ( name == keyIsAudioAvailable) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int ai = getIntVal(args0);
			BOOL available = m_pOsmpEngn->IsAudioAvailable(ai);
			BOOLEAN_TO_NPVARIANT(available, *result);
			return true;
		}
	} else if ( name == keyIsSubtitleAvailable) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int si = getIntVal(args0);
			BOOL available = m_pOsmpEngn->IsSubtitleAvailable(si);
			BOOLEAN_TO_NPVARIANT(available, *result);
			return true;
		}
	} else if ( name == keyCommitSelection) {

		unsigned int ret = (int)m_pOsmpEngn->CommitSelection();
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;

	} else if ( name == keyClearSelection) {

		unsigned int ret = (unsigned int)m_pOsmpEngn->ClearSelection();
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;

	} else if ( name == keyGetVideoProperty) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int index = getIntVal(args0);
			LPCTSTR prop = m_pOsmpEngn->GetVideoProperty(index);
			int len = WideCharToMultiByte(CP_UTF8, 0, prop, -1, NULL, 0, NULL, NULL) + 1;
#if 1
			char* pval = (char*)NPN_MemAlloc(len);
			if (NULL == pval)
				return false;

			SecureZeroMemory(pval, len);
			WideCharToMultiByte(CP_UTF8, 0, prop, _tcslen(prop), pval, len, NULL, NULL);
			STRINGZ_TO_NPVARIANT(pval, *result);
#else
			if (len > m_nPropLen || len <= 2) {
				//if (m_pVPBuf)
				//	NPN_MemFree((void*)m_pVPBuf);
				//m_nPropLen = len;
				//m_pVPBuf = (char*)NPN_MemAlloc(len); 
				//if (NULL == m_pVPBuf)
					return false;
			} 
			 
			SecureZeroMemory(m_pVPBuf, m_nPropLen);
			int wlen = _tcslen(prop);
			WideCharToMultiByte(CP_UTF8, 0, prop, wlen, m_pVPBuf, m_nPropLen, NULL, NULL);
			STRINGN_TO_NPVARIANT(m_pVPBuf, len, *result);
#endif	
			return true;
		}
	} else if ( name == keyGetAudioProperty) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int index = getIntVal(args0);
			LPCTSTR prop = m_pOsmpEngn->GetAudioProperty(index);
			int len = WideCharToMultiByte(CP_UTF8, 0, prop, -1, NULL, 0, NULL, NULL) + 1;
			char* pval = (char*)NPN_MemAlloc(len);
			if (NULL == pval)
				return false;

			SecureZeroMemory(pval, len);
			WideCharToMultiByte(CP_UTF8, 0, prop, _tcslen(prop), pval, len, NULL, NULL);
			STRINGZ_TO_NPVARIANT(pval, *result);
			return true;
		}
	} else if ( name == keyGetSubtitleProperty) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int index = getIntVal(args0);
			LPCTSTR prop = m_pOsmpEngn->GetSubtitleProperty(index);
			int len = WideCharToMultiByte(CP_UTF8, 0, prop, -1, NULL, 0, NULL, NULL) + 1;
			char* pval = (char*)NPN_MemAlloc(len);
			if (NULL == pval)
				return false;

			SecureZeroMemory(pval, len);
			WideCharToMultiByte(CP_UTF8, 0, prop, _tcslen(prop), pval, len, NULL, NULL);
			STRINGZ_TO_NPVARIANT(pval, *result);
			return true;
		}
	} else if (name == keyGetPlayingAsset) {

		LPCTSTR pa = m_pOsmpEngn->GetPlayingAsset();
		int len = WideCharToMultiByte(CP_UTF8, 0, pa, -1, NULL, 0, NULL, NULL);
		char* pval = (char*)NPN_MemAlloc(len);
		if (NULL == pval)
			return false;

		SecureZeroMemory(pval, len);
		WideCharToMultiByte(CP_UTF8, 0, pa, _tcslen(pa), pval, len, NULL, NULL);
		STRINGZ_TO_NPVARIANT(pval, *result);
		return true;

	} else if (name == keyGetCurrentSelection) {

		LPCTSTR pa = m_pOsmpEngn->GetCurrentSelection();
		int len = WideCharToMultiByte(CP_UTF8, 0, pa, -1, NULL, 0, NULL, NULL);
		char* pval = (char*)NPN_MemAlloc(len);
		if (NULL == pval)
			return false;

		SecureZeroMemory(pval, len);
		WideCharToMultiByte(CP_UTF8, 0, pa, _tcslen(pa), pval, len, NULL, NULL);
		STRINGZ_TO_NPVARIANT(pval, *result);
		return true;

	} else if (name == keySetSubtitlePath) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			getStrVal(args0);
			TCHAR szval[BIG_BUF_LEN];
			MultiByteToWideChar(CP_UTF8, 0, m_szval, -1, szval, sizeof(szval));
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitlePath(szval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyEnableSubtitle) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool bval = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableSubtitle(bval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontColor) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			DWORD dwval = (DWORD)getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontColor(dwval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontOpacity) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nval = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontOpacity(nval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontSizeScale) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nval = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontSizeScale(nval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontBackgroundColor) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			DWORD dwval = (DWORD)getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontBackgroundColor(dwval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontBackgroundOpacity) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nval = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontBackgroundOpacity(nval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleWindowBackgroundColor) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			DWORD dwval = (DWORD)getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleWindowBackgroundColor(dwval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleWindowBackgroundOpacity) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nval = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleWindowBackgroundOpacity(nval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontItalic) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool bval = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontItalic(bval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontBold) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool bval = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontBold(bval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontUnderline) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			bool bval = getBoolVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontUnderline(bval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontName) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			getStrVal(args0);
			TCHAR szval[BIG_BUF_LEN];
			MultiByteToWideChar(CP_UTF8, 0, m_szval, -1, szval, sizeof(szval));
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontName(szval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontEdgeType) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nval = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontEdgeType(nval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontEdgeColor) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			DWORD dwval = (DWORD)getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontEdgeColor(dwval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetSubtitleFontEdgeOpacity) {

		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int nval = getIntVal(args0);
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetSubtitleFontEdgeOpacity(nval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyResetSubtitleParameter) {
		
		unsigned int ret = (unsigned int)m_pOsmpEngn->ResetSubtitleParameter();
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;
	} else if (name == keySetHttpHeader) {
		if (args != NULL && argc == 2) {
			NPVariant args0 = args[0];
			getStrVal(args0);
			char headerType[16];
			strncpy(headerType, m_szval, strlen(m_szval));

			NPVariant args1 = args[1];
			getStrVal(args1);

			unsigned int ret = (unsigned int)m_pOsmpEngn->SetHTTPHeader(headerType, m_szval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keySetHttpProxy) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			getStrVal(args0);

			TCHAR szval[BIG_BUF_LEN];
			MultiByteToWideChar(CP_UTF8, 0, m_szval, -1, szval, sizeof(szval));
			unsigned int ret = (unsigned int)m_pOsmpEngn->SetHTTPProxy(szval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyEnableAnalytics) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			int cachetime = getIntVal(args0);

			unsigned int ret = (unsigned int)m_pOsmpEngn->EnableAnalytics(cachetime);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyStartAnalyticsNotification) {
		if (args != NULL && argc == 2) {
			NPVariant args0 = args[0];
			int interval = getIntVal(args0);

			NPVariant args1 = args[1];
			getStrVal(args1);

			unsigned int ret = (unsigned int)m_pOsmpEngn->StartAnalyticsNotification(interval, m_szval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}
	} else if (name == keyStopAnalyticsNotification) {

		unsigned int ret = (unsigned int)m_pOsmpEngn->StopAnalyticsNotification();
		
		DOUBLE_TO_NPVARIANT(ret, *result);
		return true;

	} else if (name == keyGetAnalytics) {
		if (args != NULL && argc == 1) {
			NPVariant args0 = args[0];
			getStrVal(args0);

			const char* pan = m_pOsmpEngn->GetAnalytics(m_szval);
			int len = strlen(pan);
			if (len == 0)
				return false;

			len++;
			char* pval = (char*)NPN_MemAlloc(len);
			if (NULL == pval)
				return false;

			SecureZeroMemory(pval, len);
			memcpy(pval, (char*)pan, len - 1);
			STRINGZ_TO_NPVARIANT(pval, *result);
			return true;
		}	
	} else if (name == keySetAudioPlaybackSpeed) { 

		if (args != NULL && argc == 1) {
			
			NPVariant val = args[0];

			FLOAT fval =0;
			if (NPVARIANT_IS_INT32(val))
				fval = (FLOAT)NPVARIANT_TO_INT32(val);
			else if (NPVARIANT_IS_DOUBLE(val)) 
				fval = (FLOAT)NPVARIANT_TO_DOUBLE(val);
			else if (NPVARIANT_IS_STRING(val)) {
				NPString str = NPVARIANT_TO_STRING(val);
				memset(m_szval, 0, BIG_BUF_LEN);
				if (str.UTF8Length > BIG_BUF_LEN) {
					VOLOGI("input len: %d", str.UTF8Length);
					return 0;
				}

				memcpy(m_szval, str.UTF8Characters, str.UTF8Length);
				fval = (FLOAT)atof(m_szval);
			}

			unsigned int ret = (unsigned int)m_pOsmpEngn->SetAudioPlaybackSpeed(fval);
			DOUBLE_TO_NPVARIANT(ret, *result);
			return true;
		}

	} else if (name == keyIsFullScreen) {
		
		BOOLEAN_TO_NPVARIANT(m_pOsmpEngn->IsFullScreen(), *result);
		return true;

	} else if (name == keyIsLiveStreaming) {

		BOOLEAN_TO_NPVARIANT(m_pOsmpEngn->IsLiveStreaming(), *result);
		return true;
	} else if (name == keyToggleOverlayUI) {

		if (args != NULL && argc == 1) {
			
			NPVariant val = args[0];
			BOOL shown = false;
			if (NPVARIANT_IS_INT32(val))
				shown = (BOOL)NPVARIANT_TO_INT32(val);
			else if (NPVARIANT_IS_BOOLEAN(val))
				shown = (BOOL)NPVARIANT_TO_BOOLEAN(val);

			unsigned int ret = (unsigned int)m_pOsmpEngn->ToggleOverlayUI(shown);
			DOUBLE_TO_NPVARIANT(ret, *result);
		}

		return true;
	} else if (name == keyRegisterEventHandler) {

		VOLOGI("registerEventHandler on NPP: %p", m_pnpp);
		if (args != NULL && argc == 1) {
			NPVariant npval = args[0];
			m_pplgn->setJSCallbackObject(npval);
			INT32_TO_NPVARIANT(0, *result);
			return true;
		}
	}
	
	DOUBLE_TO_NPVARIANT(-1, *result);
	return false;
} 

bool voScriptableObject::InvokeDefault(const NPVariant *args, uint32_t argc, NPVariant *result)
{
  // STRINGZ_TO_NPVARIANT(strdup("default method return val"), *result);
  return true;
}

NPObject* voScriptableObject::_Allocate(NPP instance, NPClass* npclass)
{
	NPObject* po = (NPObject*)(new voScriptableObject(instance));
	VOLOGI("come in NPP %p with new scriptable object %p, rc: %d", instance, po, po->referenceCount);
	return po;
}

void voScriptableObject::_Deallocate(NPObject *npobj)
{
	VOLOGI("gonna delete scriptable object %p", npobj);
	if (npobj) {
		delete (voScriptableObject *)npobj;
	}
	npobj = NULL;
}

void voScriptableObject::_Invalidate(NPObject *npobj)
{
	if (npobj)
		((voScriptableObject *)npobj)->Invalidate();
}

bool voScriptableObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->HasMethod(name);
}

bool voScriptableObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argc, NPVariant *result)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->Invoke(name, args, argc, result);
}

bool voScriptableObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argc, NPVariant *result)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->InvokeDefault(args, argc, result);
}

bool voScriptableObject::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	if (npobj == NULL)
		return false;
	return ((voScriptableObject *)npobj)->HasProperty(name);
}

bool voScriptableObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->GetProperty(name, result);
}

bool voScriptableObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->SetProperty(name, value);
}

bool voScriptableObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->RemoveProperty(name);
}

bool voScriptableObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	if (npobj == NULL)
		return false;

	return ((voScriptableObject *)npobj)->Enumerate(identifier, count);
}


bool voScriptableObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argc, NPVariant *result)
{
	if (npobj == NULL)
		return false;

  return ((voScriptableObject *)npobj)->Construct(args, argc, result);
}

