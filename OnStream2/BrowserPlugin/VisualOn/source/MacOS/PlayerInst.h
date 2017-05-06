#import "npapiBase.h"
#import "voOSMP_v3.h"
#import <IOKit/pwr_mgt/IOPMLib.h>

#define BIG_BUF_LEN 2048

class PlayerInst : public npapiBase
{
public:
  PlayerInst(NPNetscapeFuncs* pBrowserFuncs, NPObject* pNPObj);
  ~PlayerInst();
  
  NPError nppNew(NPMIMEType pluginType, NPP pInstance, uint16_t nMode, int16_t nArgc, char* szArgn[], char* szArgv[], NPSavedData* pSaved);
  NPError nppDestroy(NPP pInstance, NPSavedData** ppSaved);
  NPError nppSetWindow(NPP pInstance, NPWindow* pWindow);
  int16_t nppHandleEvent(NPP pInstance, void* pEvent);
  NPError nppGetValue(NPP pInstance, NPPVariable variable, void *pValue);

  static void GlobalCallbackToWeb(void* pContext, int nEventId, int nParam1, int nParam2);
  void CallbackToWeb(int nEventId, int nParam1, int nParam2);

protected:
  bool npHasMethod(NPObject *pNPObj, NPIdentifier name);
  bool npInvoke(NPObject *pNPObj, NPIdentifier name, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult);
  bool npHasProperty(NPObject * pNPObj, NPIdentifier name);
  bool npGetProperty(NPObject *pNPObj, NPIdentifier name, NPVariant *pResult);

protected:
  void InitIdentifier();
  char* getStrVal(NPVariant val);
  int32_t getIntVal(NPVariant val);
  bool getBoolVal(NPVariant val);
  float getFloatVal(NPVariant val);
  char* copyStringToNPNMem(const char*pVal);
  void freeStr(char* pVal);

protected:
  int             m_width;
  int             m_height;
  CALayer*        m_pLayer;
  CALayer*        m_pVideoLayer;
  NSTimer*        m_timer;
  NPP             m_pInstance;
  bool            m_bLicenseSet;
  CvoOSMP_v3*     m_pOSMP;
  float           m_fVolume;
  bool            m_bFullScreen;
  IOPMAssertionID m_assertionID;

  
  NPObject*       m_pJSCallbackOjbect;
  
  // Player Control
  NPIdentifier    keyInit;
  NPIdentifier    keyOpen;
  NPIdentifier    keyClose;
  NPIdentifier    keyStart;
  NPIdentifier    keyPause;
  NPIdentifier    keyStop;
  NPIdentifier    keySetPosition;
  NPIdentifier    keyGetPosition;
  NPIdentifier    keyGetDuration;
  NPIdentifier    keySetVolume;
  NPIdentifier    keyGetVolume;
  NPIdentifier    keyMute;
  NPIdentifier    keyUnMute;
//  NPIdentifier    keySuspend;
//  NPIdentifier    keyResume;
  NPIdentifier    keySetVideoAspectRatio;
//  NPIdentifier    keyRedrawVideo
  NPIdentifier    keyGetPlayerStatus;
  NPIdentifier    keyCanBePaused;
  NPIdentifier    keyIsLiveStreaming;
  NPIdentifier    keyStartAnalyticsNotification;
  NPIdentifier    keyStopAnalyticsNotification;
  NPIdentifier    keyGetAnalytics;
//  NPIdentifier    keySetScreenBrightness;
//  NPIdentifier    keyGetScreenBrightness;
  NPIdentifier    keyStartSEINotification;
  NPIdentifier    keyStopSEINotification;
  NPIdentifier    keyGetSEIInfo;

  // Player Configuration
  NPIdentifier    keySetLicenseContent;
  NPIdentifier    keySetLicenseFilePath;
  NPIdentifier    keySetPreAgreedLicense;
  NPIdentifier    keyEnableDeblock;
  NPIdentifier    keySetDeviceCapabilityByFile;
  NPIdentifier    keySetInitialBitrate;
//  NPIdentifier    keyGetPlayerType;
//  NPIdentifier    keyGetParameter;
//  NPIdentifier    keySetParameter;
//  NPIdentifier    keySetZoomMode;
  NPIdentifier    keyEnableLiveStreamingDVRPosition;
  NPIdentifier    keyGetMinPosition;
  NPIdentifier    keyGetMaxPosition;
  NPIdentifier    keyEnableAnalytics;
//  NPIdentifier    keySetDRMAdapter;
  NPIdentifier    keySetDRMVerificationInfo;
  NPIdentifier    keyGetDRMUniqueIdentifier;
  NPIdentifier    keySetDRMUniqueIdentifier;
//  NPIdentifier    keySetInitialBufferTime;
  NPIdentifier    keySetAnewBufferingTime;
//  NPIdentifier    keySetMaxBufferTime;
  NPIdentifier    keyEnableAudioStream;
//  NPIdentifier    keyEnableVideoStream;
//  NPIdentifier    keyEnableLowLatencyVideo;
  NPIdentifier    keyEnableAudioEffect;
//  NPIdentifier    keyEnablePCMOutput;
  NPIdentifier    keySetAudioPlaybackSpeed;
  NPIdentifier    keyGetVersion;
  NPIdentifier    keySetPresentationDelay;
  NPIdentifier    keyEnableSEI;
//  NPIdentifier    keySetPDConnectionRetryCount;
  NPIdentifier    keyEnableCPUAdaptation;

  
  // Player HTTP Configuration
//  NPIdentifier    keySetHTTPVerificationInfo;
  NPIdentifier    keySetHttpHeader;
  NPIdentifier    keySetHttpProxy;

  // Player Asset Selection
  NPIdentifier    keyGetVideoCount;
  NPIdentifier    keyGetAudioCount;
  NPIdentifier    keyGetSubtitleCount;
  NPIdentifier    keySelectVideo;
  NPIdentifier    keySelectAudio;
  NPIdentifier    keySelectSubtitle;
  NPIdentifier    keyIsVideoAvailable;
  NPIdentifier    keyIsAudioAvailable;
  NPIdentifier    keyIsSubtitleAvailable;
  NPIdentifier    keyCommitSelection;
  NPIdentifier    keyClearSelection;
  NPIdentifier    keyGetVideoProperty;
  NPIdentifier    keyGetAudioProperty;
  NPIdentifier    keyGetSubtitleProperty;
  NPIdentifier    keyGetPlayingAsset;
  NPIdentifier    keyGetCurrentSelection;

  // Player Subtitle
  NPIdentifier    keySetSubtitlePath;
  NPIdentifier    keyEnableSubtitle;
  NPIdentifier    keySetSubtitleFontColor;
  NPIdentifier    keySetSubtitleFontOpacity;
  NPIdentifier    keySetSubtitleFontSizeScale;
  NPIdentifier    keySetSubtitleFontBackgroundColor;
  NPIdentifier    keySetSubtitleFontBackgroundOpacity;
  NPIdentifier    keySetSubtitleWindowBackgroundColor;
  NPIdentifier    keySetSubtitleWindowBackgroundOpacity;
  NPIdentifier    keySetSubtitleFontItalic;
  NPIdentifier    keySetSubtitleFontBold;
  NPIdentifier    keySetSubtitleFontUnderline;
  NPIdentifier    keySetSubtitleFontName;
  NPIdentifier    keySetSubtitleFontEdgeType;
  NPIdentifier    keySetSubtitleFontEdgeColor;
  NPIdentifier    keySetSubtitleFontEdgeOpacity;
  NPIdentifier    keyResetSubtitleParameter;
  

  
  // System
  NPIdentifier    keyRegisterEventHandler;
  NPIdentifier    keySetDRMLibrary;
  NPIdentifier    keyShowCursor;
  NPIdentifier    keyFullScreen;
  NPIdentifier    keyIsFullScreen;
  NPIdentifier    keyToggleOverlayUI;
  
  // Property
  NPIdentifier    keyVersion;
};
