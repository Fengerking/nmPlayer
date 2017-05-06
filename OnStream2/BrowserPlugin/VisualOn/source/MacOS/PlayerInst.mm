#import "PlayerInst.h"

PlayerInst::PlayerInst(NPNetscapeFuncs* pBrowserFuncs, NPObject* pNPObj)
: npapiBase(pBrowserFuncs, pNPObj)
{
  m_pBrowserFuncs = pBrowserFuncs;
  m_width = 0;
  m_height = 0;
  m_pLayer = [[CALayer layer] retain];
  m_pVideoLayer = [[CALayer alloc] init];
  [m_pLayer addSublayer:m_pVideoLayer];
  m_timer = 0;
  m_pOSMP = 0;
  m_pJSCallbackOjbect = 0;
  m_bLicenseSet = false;
  m_fVolume = 1.0;
  m_bFullScreen = false;
  m_assertionID = 0;
  InitIdentifier();
}

PlayerInst::~PlayerInst()
{
  if (m_pJSCallbackOjbect)
    m_pBrowserFuncs->releaseobject(m_pJSCallbackOjbect);
  
  if (m_pVideoLayer)
    [m_pVideoLayer release];
  
  if (m_pLayer)
    [m_pLayer release];
  
  if (m_pOSMP)
    [m_pOSMP release];
}

void PlayerInst::InitIdentifier()
{
  // Player Control
  keyInit = m_pBrowserFuncs->getstringidentifier("init");
  keyOpen = m_pBrowserFuncs->getstringidentifier("open");
  keyClose = m_pBrowserFuncs->getstringidentifier("close");
  keyStart = m_pBrowserFuncs->getstringidentifier("start");
  keyPause = m_pBrowserFuncs->getstringidentifier("pause");
  keyStop = m_pBrowserFuncs->getstringidentifier("stop");
  keySetPosition = m_pBrowserFuncs->getstringidentifier("setPosition");
  keyGetPosition = m_pBrowserFuncs->getstringidentifier("getPosition");
  keyGetDuration = m_pBrowserFuncs->getstringidentifier("getDuration");
  keySetVolume = m_pBrowserFuncs->getstringidentifier("setVolume");
  keyGetVolume = m_pBrowserFuncs->getstringidentifier("getVolume");
  keyMute = m_pBrowserFuncs->getstringidentifier("mute");
  keyUnMute = m_pBrowserFuncs->getstringidentifier("unmute");
//  keySuspend = m_pBrowserFuncs->getstringidentifier("suspend");
//  keyResume = m_pBrowserFuncs->getstringidentifier("resume");
  keySetVideoAspectRatio = m_pBrowserFuncs->getstringidentifier("setVideoAspectRatio");
//  keyRedrawVideo = m_pBrowserFuncs->getstringidentifier("redrawVideo");
  keyGetPlayerStatus = m_pBrowserFuncs->getstringidentifier("getPlayerStatus");
  keyCanBePaused = m_pBrowserFuncs->getstringidentifier("canBePaused");
  keyIsLiveStreaming = m_pBrowserFuncs->getstringidentifier("isLiveStreaming");
  keyStartAnalyticsNotification = m_pBrowserFuncs->getstringidentifier("startAnalyticsNotification");
  keyStopAnalyticsNotification = m_pBrowserFuncs->getstringidentifier("stopAnalyticsNotification");
  keyGetAnalytics = m_pBrowserFuncs->getstringidentifier("getAnalytics");
//  keySetScreenBrightness = m_pBrowserFuncs->getstringidentifier("setScreenBrightness");
//  keyGetScreenBrightness = m_pBrowserFuncs->getstringidentifier("getScreenBrightness");
  keyStartSEINotification = m_pBrowserFuncs->getstringidentifier("startSEINotification");
  keyStopSEINotification = m_pBrowserFuncs->getstringidentifier("stopSEINotification");
  keyGetSEIInfo = m_pBrowserFuncs->getstringidentifier("getSEIInfo");

  // Player Configuration
  keySetLicenseContent = m_pBrowserFuncs->getstringidentifier("setLicenseContent");
  keySetLicenseFilePath = m_pBrowserFuncs->getstringidentifier("setLicenseFilePath");
  keySetPreAgreedLicense = m_pBrowserFuncs->getstringidentifier("setPreAgreedLicense");
  keyEnableDeblock = m_pBrowserFuncs->getstringidentifier("enableDeblock");
  keySetDeviceCapabilityByFile = m_pBrowserFuncs->getstringidentifier("setDeviceCapabilityByFile");
  keySetInitialBitrate = m_pBrowserFuncs->getstringidentifier("setInitialBitrate");
//  keyGetPlayerType = m_pBrowserFuncs->getstringidentifier("getPlayerType");
//  keyGetParameter = m_pBrowserFuncs->getstringidentifier("getParameter");
//  keySetParameter = m_pBrowserFuncs->getstringidentifier("setParameter");
//  keySetZoomMode = m_pBrowserFuncs->getstringidentifier("setZoomMode");
  keyEnableLiveStreamingDVRPosition = m_pBrowserFuncs->getstringidentifier("enableLiveStreamingDVRPosition");
  keyGetMinPosition = m_pBrowserFuncs->getstringidentifier("getMinPosition");
  keyGetMaxPosition = m_pBrowserFuncs->getstringidentifier("getMaxPosition");
  keyEnableAnalytics = m_pBrowserFuncs->getstringidentifier("enableAnalytics");
//  keySetDRMAdapter = m_pBrowserFuncs->getstringidentifier("setDRMAdapter");
  keySetDRMVerificationInfo = m_pBrowserFuncs->getstringidentifier("setDRMVerificationInfo");
  keyGetDRMUniqueIdentifier = m_pBrowserFuncs->getstringidentifier("getDRMUniqueIdentifier");
  keySetDRMUniqueIdentifier = m_pBrowserFuncs->getstringidentifier("setDRMUniqueIdentifier");
//  keySetInitialBufferTime = m_pBrowserFuncs->getstringidentifier("setInitialBufferTime");
  keySetAnewBufferingTime = m_pBrowserFuncs->getstringidentifier("setAnewBufferingTime");
//  keySetMaxBufferTime = m_pBrowserFuncs->getstringidentifier("setMaxBufferTime");
  keyEnableAudioStream = m_pBrowserFuncs->getstringidentifier("enableAudioStream");
//  keyEnableVideoStream = m_pBrowserFuncs->getstringidentifier("enableVideoStream");
//  keyEnableLowLatencyVideo = m_pBrowserFuncs->getstringidentifier("enableLowLatencyVideo");
  keyEnableAudioEffect = m_pBrowserFuncs->getstringidentifier("enableAudioEffect");
//  keyEnablePCMOutput = m_pBrowserFuncs->getstringidentifier("enablePCMOutput");
  keySetAudioPlaybackSpeed = m_pBrowserFuncs->getstringidentifier("setAudioPlaybackSpeed");
  keyGetVersion = m_pBrowserFuncs->getstringidentifier("getVersion");
  keySetPresentationDelay = m_pBrowserFuncs->getstringidentifier("setPresentationDelay");
  keyEnableSEI = m_pBrowserFuncs->getstringidentifier("enableSEI");
//  keySetPDConnectionRetryCount = m_pBrowserFuncs->getstringidentifier("setPDConnectionRetryCount");
  keyEnableCPUAdaptation = m_pBrowserFuncs->getstringidentifier("enableCPUAdaptation");
  
  // Player HTTP Configuration
//  keySetHTTPVerificationInfo = m_pBrowserFuncs->getstringidentifier("setHTTPVerificationInfo");
  keySetHttpHeader = m_pBrowserFuncs->getstringidentifier("setHttpHeader");
  keySetHttpProxy = m_pBrowserFuncs->getstringidentifier("setHttpProxy");

  //Player Asset Selection
  keyGetVideoCount = m_pBrowserFuncs->getstringidentifier("getVideoCount");
  keyGetAudioCount = m_pBrowserFuncs->getstringidentifier("getAudioCount");
  keyGetSubtitleCount = m_pBrowserFuncs->getstringidentifier("getSubtitleCount");
  keySelectVideo = m_pBrowserFuncs->getstringidentifier("selectVideo");
  keySelectAudio = m_pBrowserFuncs->getstringidentifier("selectAudio");
  keySelectSubtitle = m_pBrowserFuncs->getstringidentifier("selectSubtitle");
  keyIsVideoAvailable = m_pBrowserFuncs->getstringidentifier("isVideoAvailable");
  keyIsAudioAvailable = m_pBrowserFuncs->getstringidentifier("isAudioAvailable");
  keyIsSubtitleAvailable = m_pBrowserFuncs->getstringidentifier("isSubtitleAvailable");
  keyCommitSelection = m_pBrowserFuncs->getstringidentifier("commitSelection");
  keyClearSelection = m_pBrowserFuncs->getstringidentifier("clearSelection");
  keyGetVideoProperty = m_pBrowserFuncs->getstringidentifier("getVideoProperty");
  keyGetAudioProperty = m_pBrowserFuncs->getstringidentifier("getAudioProperty");
  keyGetSubtitleProperty = m_pBrowserFuncs->getstringidentifier("getSubtitleProperty");
  keyGetPlayingAsset = m_pBrowserFuncs->getstringidentifier("getPlayingAsset");
  keyGetCurrentSelection = m_pBrowserFuncs->getstringidentifier("getCurrentSelection");
  
  // for subtitle relatives
  keySetSubtitlePath = m_pBrowserFuncs->getstringidentifier("setSubtitlePath");
  keyEnableSubtitle = m_pBrowserFuncs->getstringidentifier("enableSubtitle");
  keySetSubtitleFontColor = m_pBrowserFuncs->getstringidentifier("setSubtitleFontColor");
  keySetSubtitleFontOpacity = m_pBrowserFuncs->getstringidentifier("setSubtitleFontOpacity");
  keySetSubtitleFontSizeScale = m_pBrowserFuncs->getstringidentifier("setSubtitleFontSizeScale");
  keySetSubtitleFontBackgroundColor = m_pBrowserFuncs->getstringidentifier("setSubtitleFontBackgroundColor");
  keySetSubtitleFontBackgroundOpacity = m_pBrowserFuncs->getstringidentifier("setSubtitleFontBackgroundOpacity");
  keySetSubtitleWindowBackgroundColor = m_pBrowserFuncs->getstringidentifier("setSubtitleWindowBackgroundColor");
  keySetSubtitleWindowBackgroundOpacity = m_pBrowserFuncs->getstringidentifier("setSubtitleWindowBackgroundOpacity");
  keySetSubtitleFontItalic = m_pBrowserFuncs->getstringidentifier("setSubtitleFontItalic");
  keySetSubtitleFontBold = m_pBrowserFuncs->getstringidentifier("setSubtitleFontBold");
  keySetSubtitleFontUnderline = m_pBrowserFuncs->getstringidentifier("setSubtitleFontUnderline");
  keySetSubtitleFontName = m_pBrowserFuncs->getstringidentifier("setSubtitleFontName");
  keySetSubtitleFontEdgeType = m_pBrowserFuncs->getstringidentifier("setSubtitleFontEdgeType");
  keySetSubtitleFontEdgeColor = m_pBrowserFuncs->getstringidentifier("setSubtitleFontEdgeColor");
  keySetSubtitleFontEdgeOpacity = m_pBrowserFuncs->getstringidentifier("setSubtitleFontEdgeOpacity");
  keyResetSubtitleParameter = m_pBrowserFuncs->getstringidentifier("resetSubtitleParameter");
  
  // Unknown
  keySetDRMLibrary = m_pBrowserFuncs->getstringidentifier("setDRMLibrary");
  keyShowCursor = m_pBrowserFuncs->getstringidentifier("showCursor");
  keyFullScreen = m_pBrowserFuncs->getstringidentifier("fullScreen");
  keyIsFullScreen = m_pBrowserFuncs->getstringidentifier("isFullScreen");
  keyToggleOverlayUI = m_pBrowserFuncs->getstringidentifier("toggleOverlayUI");

  // Callback
  keyRegisterEventHandler = m_pBrowserFuncs->getstringidentifier("registerEventHandler");
  
  // Property
  keyVersion = m_pBrowserFuncs->getstringidentifier("version");
}

NPError PlayerInst::nppNew(NPMIMEType pluginType, NPP pInstance, uint16_t nMode, int16_t nArgCount, char* szArgn[], char* szArgv[], NPSavedData* pSaved)
{
  // Check if NPNVsupportsCoreAnimationBool is supported
  NPBool supportsCA = false;
  if (m_pBrowserFuncs->getvalue(pInstance, NPNVsupportsCoreAnimationBool, &supportsCA) == NPERR_NO_ERROR && supportsCA)
  {
    m_pBrowserFuncs->setvalue(pInstance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreAnimation);
  } else {
    NSLog(@"[voBrowserPlugin] NPNVsupportsCoreAnimationBool drawing model not supported, can't create a plugin instance.");
    
    // Check if NPNVsupportsInvalidatingCoreAnimationBool is supported
    NPBool supportsICA = false;
    if (m_pBrowserFuncs->getvalue(pInstance, NPNVsupportsInvalidatingCoreAnimationBool, &supportsICA) == NPERR_NO_ERROR && supportsICA)
    {
      m_pBrowserFuncs->setvalue(pInstance, NPPVpluginDrawingModel, (void*)NPDrawingModelInvalidatingCoreAnimation);
    } else {
      NSLog(@"[voBrowserPlugin] InvalidatingCoreAnimation drawing model not supported, can't create a plugin instance.");
      return NPERR_INCOMPATIBLE_VERSION_ERROR;
    }
  }

  
  NPBool supportsCocoaEvents = false;
  if (m_pBrowserFuncs->getvalue(pInstance, NPNVsupportsCocoaBool, &supportsCocoaEvents) == NPERR_NO_ERROR && supportsCocoaEvents) {
    m_pBrowserFuncs->setvalue(pInstance, NPPVpluginEventModel, (void*)NPEventModelCocoa);
  } else {
    NSLog(@"[voBrowserPlugin] Cocoa event model not supported, can't create a plugin instance.");
    return NPERR_INCOMPATIBLE_VERSION_ERROR;
  }
  
  m_pInstance = pInstance;
  
  return NPERR_NO_ERROR;
}

NPError PlayerInst::nppDestroy(NPP pInstance, NPSavedData** ppSaved)
{
  if (m_pOSMP)
  {
    [m_pOSMP stop];
    [m_pOSMP close];
  }
  
  if (m_assertionID != 0)
  {
    IOPMAssertionRelease(m_assertionID);
    m_assertionID = 0;
  }

  return NPERR_NO_ERROR;
}

NPError PlayerInst::nppSetWindow(NPP pInstance, NPWindow* pWindow)
{
  if (!pWindow)
    return NPERR_NO_ERROR;
  
  m_width = pWindow->width;
  m_height = pWindow->height;
  
  NSLog(@"nppSetWindow");
  
  if (m_pVideoLayer)
  {
    [m_pVideoLayer setBounds:CGRectMake(0, 0, m_width, m_height)];
    [m_pVideoLayer setFrame:CGRectMake(0, 0, m_width, m_height)];
  }
  
  return NPERR_NO_ERROR;
}

int16_t PlayerInst::nppHandleEvent(NPP pInstance, void* pEvent)
{
  NPCocoaEvent* pCocoaEvent = (NPCocoaEvent*)pEvent;
  
  if (pCocoaEvent)
  {
    switch (pCocoaEvent->type) {
      case NPCocoaEventMouseDown:
        break;
      case NPCocoaEventMouseUp:
         break;
      case NPCocoaEventMouseMoved:
        break;
      case NPCocoaEventMouseEntered:
        break;
      case NPCocoaEventMouseExited:
        break;
      case NPCocoaEventMouseDragged:
        break;
      default:
        break;
    }
  }
  return 0;
}

NPError PlayerInst::nppGetValue(NPP pInstance, NPPVariable variable, void *pValue)
{
  static const unsigned WKNVExpectsNonretainedLayer = 74657;
  NPError rv = NPERR_NO_ERROR;
  
  if(pInstance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;
  
  switch (variable)
  {
    case NPPVpluginScriptableNPObject:
      if (m_pNPObj)
      {
        m_pBrowserFuncs->retainobject(m_pNPObj);
        *(NPObject **)pValue = m_pNPObj;
      }
      else
      {
        return NPERR_INVALID_INSTANCE_ERROR;
      }
      break;
    case NPPVpluginCoreAnimationLayer:
    {
      NSLog(@"NPPVpluginCoreAnimationLayer");
      m_pLayer.autoresizingMask = kCALayerWidthSizable | kCALayerHeightSizable;
      m_pLayer.needsDisplayOnBoundsChange = YES;
      NSBundle* pMainBundle = [NSBundle mainBundle];
      
      // Check the undocumented WKNVExpectsNonretainedLayer added to WebKit2 June 4.
      // Use the supported response as definitive. Not supported as of Firefox 7.0.0.
      NPBool expectsNonretainedLayer = false;
      if (m_pBrowserFuncs->getvalue(pInstance, (NPNVariable)WKNVExpectsNonretainedLayer, &expectsNonretainedLayer) == NPERR_NO_ERROR)
        *(CALayer**) pValue = (expectsNonretainedLayer) ? m_pLayer : [m_pLayer retain];
      // Need to only retain when Safari is run in 32bit mode and is the host process. Backwards compatiblity.
      // When Safari is run in 64bit mode, the plugin is sandboxed in another process. According to curr spec.
      // Also, allow the application to specify this behavior in their main bundle plist file, in case it's
      // an application linking to WebKit in 32bit mode, but it's not called Safari
      else if (NSOrderedSame == [[pMainBundle bundleIdentifier] compare:@"com.apple.Safari"] || [pMainBundle objectForInfoDictionaryKey:@"voBrowserPluginCALayerRetain"])
        *(CALayer**) pValue = [m_pLayer retain];
      else
        *(CALayer**) pValue = m_pLayer;
    }
      break;
    default:
      rv = NPERR_GENERIC_ERROR;
  }
  
  return rv;
}

bool PlayerInst::npHasProperty(NPObject * pNPObj, NPIdentifier name)
{
  return (name == keyVersion);
}

bool PlayerInst::npGetProperty(NPObject *pNPObj, NPIdentifier name, NPVariant *pResult)
{
  if (name == keyVersion)
  {
    NSLog(@"versionProperty+++");
    CvoOSMP_v3* pOSMP = [[CvoOSMP_v3 alloc] init:m_pVideoLayer CallbackToWeb:GlobalCallbackToWeb Context:this];
    if (pOSMP)
    {
      int ret = [pOSMP initPlayer:VO_OSMP_VOME2_PLAYER initParam:(char*)""];

      if (ret == 0)
      {
        NSString* strVer = [pOSMP getVersion:VO_OSMP_MODULE_TYPE_SDK];
        [pOSMP release];
        if (!strVer)
        {
          NSLog(@"versionProperty--- Return: NULL");
          return false;
        }
        char* pVal = copyStringToNPNMem([strVer UTF8String]);
        if (!pVal)
        {
          NSLog(@"versionProperty--- Return: NULL (Convert Error)");
          return false;
        }
        NSLog(@"versionProperty--- Return: %s", pVal);
        STRINGZ_TO_NPVARIANT(pVal, *pResult);
        return true;
      }
      else
      {
        [pOSMP release];
        NSLog(@"versionProperty--- Return: NULL (initPlayer Error)");
      }
    }
    else
    {
      NSLog(@"versionProperty--- Return: NULL (alloc Error)");
    }
  }
  
  return false;
}

bool PlayerInst::npHasMethod(NPObject *pNPObj, NPIdentifier name)
{
  return (// Player Control
          name == keyInit
          || name == keyOpen
          || name == keyClose
          || name == keyStart
          || name == keyPause
          || name == keyStop
          || name == keySetPosition
          || name == keyGetPosition
          || name == keyGetDuration
          || name == keySetVolume
          || name == keyGetVolume
          || name == keyMute
          || name == keyUnMute
//          || name == keySuspend
//          || name == keyResume
          || name == keySetVideoAspectRatio
//          || name == keyRedrawVideo
          || name == keyGetPlayerStatus
          || name == keyCanBePaused
          || name == keyIsLiveStreaming
          || name == keyStartAnalyticsNotification
          || name == keyStopAnalyticsNotification
          || name == keyGetAnalytics
//          || name == keySetScreenBrightness
//          || name == keyGetScreenBrightness
          || name == keyStartSEINotification
          || name == keyStopSEINotification
          || name == keyGetSEIInfo

          
          // Player Configuration
          || name == keySetLicenseContent
          || name == keySetLicenseFilePath
          || name == keySetPreAgreedLicense
          || name == keyEnableDeblock
          || name == keySetDeviceCapabilityByFile
          || name == keySetInitialBitrate
//          || name == keyGetPlayerType
//          || name == keyGetParameter
//          || name == keySetParameter
//          || name == keySetZoomMode
          || name == keyEnableLiveStreamingDVRPosition
          || name == keyGetMinPosition
          || name == keyGetMaxPosition
          || name == keySetDRMLibrary
          || name == keyEnableAnalytics
//          || name == keySetDRMAdapter
          || name == keySetDRMVerificationInfo
          || name == keyGetDRMUniqueIdentifier
          || name == keySetDRMUniqueIdentifier
//          || name == keySetInitialBufferTime
          || name == keySetAnewBufferingTime
//          || name == keySetMaxBufferTime
          || name == keyEnableAudioStream
//          || name == keyEnableVideoStream
//          || name == keyEnableLowLatencyVideo
          || name == keyEnableAudioEffect
//          || name == keyEnablePCMOutput
          || name == keySetAudioPlaybackSpeed
          || name == keyGetVersion
          || name == keySetPresentationDelay
          || name == keyEnableSEI
//          || name == keySetPDConnectionRetryCount
          || name == keyEnableCPUAdaptation
          
          
          // Player HTTP Configuration
//          || name == keySetHTTPVerificationInfo
          || name == keySetHttpHeader
          || name == keySetHttpProxy
         
          
          // Player Asset Selection
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
          
          
          // Player Subtitle
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
          
          
          // System
          || name == keyRegisterEventHandler
          || name == keyShowCursor
          || name == keyFullScreen
          || name == keyIsFullScreen
          || name == keyToggleOverlayUI
          
          // Property
          || name == keyVersion
      );
}

char* PlayerInst::getStrVal(NPVariant val)
{
  char* pVal = (char*)malloc(NPVARIANT_TO_STRING(val).UTF8Length + 1);
  memcpy(pVal, NPVARIANT_TO_STRING(val).UTF8Characters, NPVARIANT_TO_STRING(val).UTF8Length);
  pVal[NPVARIANT_TO_STRING(val).UTF8Length] = 0;
  return pVal;
}

void PlayerInst::freeStr(char* pVal)
{
  if (pVal)
    free(pVal);
}

int32_t PlayerInst::getIntVal(NPVariant val)
{
  int32_t nVal = 0;
  if (NPVARIANT_IS_INT32(val))
    nVal = NPVARIANT_TO_INT32(val);
  else if (NPVARIANT_IS_DOUBLE(val))
    nVal = (int32_t)NPVARIANT_TO_DOUBLE(val);
  else if (NPVARIANT_IS_STRING(val))
  {
    char* pVal = getStrVal(val);
    nVal = atoi(pVal);
    freeStr(pVal);
  }
  
  return nVal;
}

float PlayerInst::getFloatVal(NPVariant val)
{
  float fVal = 0;
  if (NPVARIANT_IS_INT32(val))
    fVal = (float)NPVARIANT_TO_INT32(val);
  else if (NPVARIANT_IS_DOUBLE(val))
    fVal = (float)NPVARIANT_TO_DOUBLE(val);
  else if (NPVARIANT_IS_STRING(val))
  {
    char* pVal = getStrVal(val);
    fVal = (float)atof(pVal);
    freeStr(pVal);
  }

  return fVal;
}

bool PlayerInst::getBoolVal(NPVariant val)
{
  return NPVARIANT_TO_BOOLEAN(val);
}

char* PlayerInst::copyStringToNPNMem(const char*pVal)
{
  int nLen = strlen(pVal);
  if (nLen == 0)
    return 0;
  
  
  char* pNPNVal = (char*)m_pBrowserFuncs->memalloc(nLen + 1);
  if (!pNPNVal)
    return 0;
  
  memcpy(pNPNVal, pVal, nLen);
  pNPNVal[nLen] = 0;
  
  return pNPNVal;
}

void PlayerInst::GlobalCallbackToWeb(void* pContext, int nEventId, int nParam1, int nParam2)
{
  PlayerInst* pInst = (PlayerInst*)pContext;
  pInst->CallbackToWeb(nEventId, nParam1, nParam2);
}

void PlayerInst::CallbackToWeb(int nEventId, int nParam1, int nParam2)
{
  NPVariant returnedvalue;
  NPVariant npParam[3];
  
  DOUBLE_TO_NPVARIANT(nEventId, npParam[0]);
  DOUBLE_TO_NPVARIANT(nParam1, npParam[1]);
  DOUBLE_TO_NPVARIANT(nParam2, npParam[2]);
  
  m_pBrowserFuncs->invokeDefault(m_pInstance, m_pJSCallbackOjbect, npParam, 3, &returnedvalue);
}

bool PlayerInst::npInvoke(NPObject *pNPObj, NPIdentifier name, const NPVariant *pArgs, uint32_t nArgCount, NPVariant *pResult)
{
  if (!m_pOSMP)
  {
    NSLog(@"alloc OSMP: videoLayer:0x%0.8X", (unsigned int)m_pVideoLayer);
    m_pOSMP = [[CvoOSMP_v3 alloc] init:m_pVideoLayer CallbackToWeb:GlobalCallbackToWeb Context:this];
  }
//  
//  if (name == keyTest)
//  {
//    NPObject* window = NULL;
//    m_pBrowserFuncs->getvalue(m_pInstance, NPNVWindowNPObject, &window);
//    
//    NPVariant testvalue;
//    NPIdentifier key_testvalue =  m_pBrowserFuncs->getstringidentifier("testvalue");
//    m_pBrowserFuncs->getproperty(m_pInstance, window, key_testvalue, &testvalue);
//    double dblTestValue = NPVARIANT_TO_DOUBLE(testvalue);
//    dblTestValue ++;
//    DOUBLE_TO_NPVARIANT(dblTestValue, testvalue);
//    m_pBrowserFuncs->setproperty(m_pInstance, window, key_testvalue, &testvalue);
//    
//    NPVariant param;
//    STRINGZ_TO_NPVARIANT("THIS IS A TEST.", param);
//    
//    NPIdentifier key_testfunc =  m_pBrowserFuncs->getstringidentifier("testcallbackfunc");
//    NPVariant returnedvalue;
//    m_pBrowserFuncs->invoke(m_pInstance, window, key_testfunc, &param, 1, &returnedvalue);
//    
//    NPIdentifier key_returnedvalue =  m_pBrowserFuncs->getstringidentifier("returnedvalue");
//    m_pBrowserFuncs->setproperty(m_pInstance, window, key_returnedvalue, &returnedvalue);
//  }
//
  // Player Control
  if (name == keyInit)
  {
    NSLog(@"init+++");
    if (pArgs != NULL && nArgCount == 2)
    {
      int nEngineType = getIntVal(pArgs[0]);      
      char* pInitParam = getStrVal(pArgs[1]);
      NSLog(@"init Param: EngineType=%d, InitParam=%s", nEngineType, pInitParam);
      
      int ret = [m_pOSMP initPlayer:(VO_OSMP_PLAYER_ENGINE)nEngineType initParam:pInitParam];
      freeStr(pInitParam);
      NSLog(@"init--- Return: 0x%0.8X", ret);
      if (ret == 0)
      {
        NSLog(@"setView+++");
        ret = [m_pOSMP setView:m_pVideoLayer];
        NSLog(@"setView--- Return: 0x%0.8X", ret);
      }
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      
      
      return true;
    }
    NSLog(@"init--- Param error");
  }
  else if (name == keyOpen)
  {
    NSLog(@"open+++");
    if (pArgs != NULL && nArgCount == 4)
    {
      char* pUrl = getStrVal(pArgs[0]);
      int nOpenFlag = getIntVal(pArgs[1]);
      int nSrcType = getIntVal(pArgs[2]);
      char* pOpenParam = getStrVal(pArgs[3]);
      NSLog(@"open Param: URL=%s, OpenFlag=%d, SourceType=%d, OpenParam=%s", pUrl, nOpenFlag, nSrcType, pOpenParam);

      int ret = 0;
      if (strlen(pUrl) == 0)
      {
        freeStr(pUrl);
        freeStr(pOpenParam);
        NSLog(@"open--- Return: 0x%0.8X", ret);
        DOUBLE_TO_NPVARIANT(ret, *pResult);
        return true;
      }
      
      if (!m_bLicenseSet)
      {
        NSLog(@"setLicenseFilePath+++");
        NSString* strPath = [NSHomeDirectory() stringByAppendingString:@"/Library/Internet Plug-Ins/voBrowserPlugin.plugin/Contents/Resources/voVidDec.dat"];
        
        NSLog(@"setLicenseFilePath Param: Path=%@", strPath);
        ret = [m_pOSMP setLicenseFilePath:strPath];
        if (ret != 0)
        {
          NSLog(@"setLicenseFilePath--- Return: 0x%0.8X", ret);
          DOUBLE_TO_NPVARIANT(ret, *pResult);
          return true;
        }
        NSLog(@"setLicenseFilePath--- Return: 0x%0.8X", ret);
        m_bLicenseSet = true;
      }
      
      ret = [m_pOSMP open:[NSString stringWithUTF8String:pUrl] flag:(VO_OSMP_SRC_FLAG)nOpenFlag sourceType:(VO_OSMP_SRC_FORMAT)nSrcType openParam:pOpenParam];
      freeStr(pUrl);
      freeStr(pOpenParam);
      NSLog(@"open--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      
      return true;
    }
    NSLog(@"open--- Param error");
  }
  else if (name == keyClose)
  {
    NSLog(@"close+++");
    int ret = [m_pOSMP close];
    NSLog(@"close--- Return: 0x%0.8X", ret);
    if (m_assertionID != 0)
    {
      IOPMAssertionRelease(m_assertionID);
      m_assertionID = 0;
    }
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyStart)
  {
    NSLog(@"start+++");
    int ret = [m_pOSMP start];
    NSLog(@"start--- Return: 0x%0.8X", ret);
    
    if (m_assertionID == 0)
    {
      CFStringRef reasonForActivity = CFSTR("Play video");
      IOPMAssertionCreateWithName(kIOPMAssertionTypePreventUserIdleDisplaySleep, kIOPMAssertionLevelOn, reasonForActivity, &m_assertionID);
    }

    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyPause)
  {
    NSLog(@"pause+++");
    int ret = [m_pOSMP pause];
    NSLog(@"pause--- Return: 0x%0.8X", ret);
    if (m_assertionID != 0)
    {
      IOPMAssertionRelease(m_assertionID);
      m_assertionID = 0;
    }
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyStop)
  {
    NSLog(@"stop+++");
    int ret = [m_pOSMP stop];
    NSLog(@"stop--- Return: 0x%0.8X", ret);
    if (m_assertionID != 0)
    {
      IOPMAssertionRelease(m_assertionID);
      m_assertionID = 0;
    }
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keySetPosition)
  {
    NSLog(@"setPosition+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nPos = getIntVal(pArgs[0]);
      NSLog(@"setPosition Param: Pos=%d", nPos);
      int ret = [m_pOSMP setPosition:nPos];
      NSLog(@"setPosition--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setPosition--- Param error");
  }
  else if (name == keyGetPosition)
  {
    NSLog(@"getPosition+++");
    int nPos = [m_pOSMP getPosition];
    NSLog(@"getPosition--- Return: 0x%0.8X", nPos);
    DOUBLE_TO_NPVARIANT(nPos, *pResult);
    return true;
  }
  else if (name == keyGetDuration)
  {
    NSLog(@"getDuration+++");
    int nDuration = [m_pOSMP getDuration];
    NSLog(@"getDuration--- Return: 0x%0.8X", nDuration);
    DOUBLE_TO_NPVARIANT(nDuration, *pResult);
    return true;
  }
  else if (name == keySetVolume)
  {
    NSLog(@"setVolume+++");
    if (pArgs != NULL && nArgCount == 2)
    {
      float fltLeft = getFloatVal(pArgs[0]);
      float fltRight = getFloatVal(pArgs[1]);
      NSLog(@"setVolume Param: Left=%f, Right=%f", fltLeft, fltRight);
      fltLeft = fltLeft / (float)100;
      int ret = [m_pOSMP setVolume:fltLeft];
      NSLog(@"setVolume--- Return: 0x%0.8X", ret);
      if (ret == 0)
        m_fVolume = fltLeft;
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setVolume--- Param error");
  }
  else if (name == keyGetVolume)
  {
    DOUBLE_TO_NPVARIANT(m_fVolume * 100, *pResult);
    return true;
    
  }
  else if (name == keyMute)
  {
    NSLog(@"mute+++");
    int ret = [m_pOSMP mute];
    NSLog(@"mute--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyUnMute)
  {
    NSLog(@"unmute+++");
    int ret = [m_pOSMP unmute];
    NSLog(@"unmute--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;    
  }
//  else if (name == keySuspend)
//  {
//  }
//  else if (name == keyResume)
//  {
//  }
  else if (name == keySetVideoAspectRatio)
  {
    NSLog(@"setVideoAspectRatio+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nAR = getIntVal(pArgs[0]);
      NSLog(@"setVideoAspectRatio Param: Ratio=%d", nAR);
      int ret = [m_pOSMP setVideoAspectRatio:(VO_OSMP_ASPECT_RATIO)nAR];
      NSLog(@"setVideoAspectRatio--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setVideoAspectRatio--- Param error");
  }
//  else if (name == keyRedrawVideo)
//  {
//  }
  else if (name == keyGetPlayerStatus)
  {
    //NSLog(@"getPlayerStatus+++");
    VO_OSMP_STATUS stat = [m_pOSMP getPlayerStatus];
    //NSLog(@"getPlayerStatus--- Return: %d", stat);
    DOUBLE_TO_NPVARIANT(stat, *pResult);
    return true;
  }
  else if (name == keyCanBePaused)
  {
    NSLog(@"canBePaused+++");
    bool bCanBePaused = [m_pOSMP canBePaused];
    NSLog(@"canBePaused--- Return: %d", bCanBePaused);
    BOOLEAN_TO_NPVARIANT(bCanBePaused, *pResult);
    return true;
  }
  else if (name == keyIsLiveStreaming)
  {
    NSLog(@"isLiveStreaming+++");
    bool bIsLiveStreaming = [m_pOSMP isLiveStreaming];
    NSLog(@"isLiveStreaming--- Return: %d", bIsLiveStreaming);
    BOOLEAN_TO_NPVARIANT(bIsLiveStreaming, *pResult);
    return true;
  }
  else if (name == keyStartAnalyticsNotification)
  {
    NSLog(@"startAnalyticsNotification+++");
    if (pArgs != NULL && nArgCount == 2)
    {
      int nInterval = getIntVal(pArgs[0]);
      char* pFilter = getStrVal(pArgs[1]);
      NSLog(@"startAnalyticsNotification Param: Interval=%d, Filter=%s", nInterval, pFilter);
      
      int ret = [m_pOSMP startAnalyticsNotification:nInterval filter:pFilter];
      freeStr(pFilter);
      NSLog(@"startAnalyticsNotification--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"startAnalyticsNotification--- Param error");
  }
  else if (name == keyStopAnalyticsNotification)
  {
    NSLog(@"stopAnalyticsNotification+++");
    int ret = [m_pOSMP stopAnalyticsNotification];
    NSLog(@"stopAnalyticsNotification--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyGetAnalytics)
  {
    NSLog(@"getAnalytics+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      char* pFilter = getStrVal(pArgs[0]);
      NSLog(@"getAnalytics Param: Filter=%s", pFilter);
      
      JSON pRet = [m_pOSMP getAnalytics:pFilter];      
      if (!pRet)
      {
        freeStr(pFilter);
        NSLog(@"getAnalytics--- Return: NULL");
        return false;
      }
      char* pVal = copyStringToNPNMem(pRet);
      [CvoOSMP_v3 freeJSON:pRet];
      if (!pVal)
      {
        freeStr(pFilter);
        NSLog(@"getAnalytics--- Return: NULL (Convert Error)");
        return false;
      }

      freeStr(pFilter);
      NSLog(@"getAnalytics--- Return: %s", pVal);
      STRINGZ_TO_NPVARIANT(pVal, *pResult);
      return true;
    }
    NSLog(@"getAnalytics--- Param error");
  }
//  else if (name == keySetScreenBrightness)
//  {
//  }
//  else if (name == keyGetScreenBrightness)
//  {
//  }
  else if (name == keyStartSEINotification)
  {
    NSLog(@"startSEINotification+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int interval = getIntVal(pArgs[0]);
      NSLog(@"startSEINotification Param: Interval=%d", interval);
      int ret = [m_pOSMP startSEINotification:interval];
      NSLog(@"startSEINotification--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"startSEINotification--- Param error");
  }
  else if (name == keyStopSEINotification)
  {
    NSLog(@"stopSEINotification+++");
    int ret = [m_pOSMP stopSEINotification];
    NSLog(@"stopSEINotification--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyGetSEIInfo)
  {
    NSLog(@"getSEIInfo+++");
    if (pArgs != NULL && nArgCount == 2)
    {
      int tm = getIntVal(pArgs[0]);
      int flag = getIntVal(pArgs[1]);
      NSLog(@"getSEIInfo Param: Time=%d, Flag=%d", tm, flag);
      
      JSON pRet = [m_pOSMP getSEIInfo:tm flag:(VO_OSMP_SEI_INFO_FLAG)flag];
      if (!pRet)
      {
        NSLog(@"getSEIInfo--- Return: NULL");
        return false;
      }
      char* pVal = copyStringToNPNMem(pRet);
      [CvoOSMP_v3 freeJSON:pRet];
      if (!pVal)
      {
        NSLog(@"getSEIInfo--- Return: NULL (Convert Error)");
        return false;
      }
      
      NSLog(@"getSEIInfo--- Return: %s", pVal);
      STRINGZ_TO_NPVARIANT(pVal, *pResult);
      return true;
    }
    NSLog(@"getSEIInfo--- Param error");
  }
  

  // Player Configuration
  else if (name == keySetLicenseContent)
  {
    NSLog(@"setLicenseContent+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      NSData* pData = [NSData dataWithBytes:NPVARIANT_TO_STRING(pArgs[0]).UTF8Characters length:NPVARIANT_TO_STRING(pArgs[0]).UTF8Length];
      int ret = [m_pOSMP setLicenseContent:pData];
      NSLog(@"setLicenseContent--- Return: 0x%0.8X", ret);
      if (ret == 0)
        m_bLicenseSet = true;
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setLicenseContent--- Param error");
  }
  else if (name == keySetLicenseFilePath)
  {
    NSLog(@"setLicenseFilePath+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      char* pLicPath = getStrVal(pArgs[0]);
      NSLog(@"setLicenseFilePath Param: LicPath=%s", pLicPath);
      int ret = [m_pOSMP setLicenseFilePath:[NSString stringWithUTF8String:pLicPath]];
      freeStr(pLicPath);
      NSLog(@"setLicenseFilePath--- Return: 0x%0.8X", ret);
      if (ret == 0)
        m_bLicenseSet = true;
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setLicenseFilePath--- Param error");
  }
  else if (name == keySetPreAgreedLicense)
  {
    NSLog(@"setPreAgreedLicense+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      char* pString = getStrVal(pArgs[0]);
      NSLog(@"setPreAgreedLicense Param: String=%s", pString);
      int ret = [m_pOSMP setPreAgreedLicense:[NSString stringWithUTF8String:pString]];
      freeStr(pString);
      NSLog(@"setPreAgreedLicense--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setPreAgreedLicense--- Param error");
  }
  else if (name == keyEnableDeblock)
  {
    NSLog(@"enableDeblock+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bEnable = getBoolVal(pArgs[0]);
      NSLog(@"enableDeblock Param: Enable=%d", bEnable);
      int ret = [m_pOSMP enableDeblock:bEnable];
      NSLog(@"enableDeblock--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableDeblock--- Param error");
  }
  else if (name == keySetDeviceCapabilityByFile)
  {
    NSLog(@"setDeviceCapabilityByFile+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      char* pCapFile = getStrVal(pArgs[0]);
      NSLog(@"setDeviceCapabilityByFile Param: File=%s", pCapFile);
      int ret = [m_pOSMP setDeviceCapabilityByFile:[NSString stringWithUTF8String:pCapFile]];
      freeStr(pCapFile);
      NSLog(@"setDeviceCapabilityByFile--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setDeviceCapabilityByFile--- Param error");
  }
  else if (name == keySetInitialBitrate)
  {
    NSLog(@"setInitialBitrate+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIB = getIntVal(pArgs[0]);
      NSLog(@"setInitialBitrate Param: Bitrate=%d", nIB);
      int ret = [m_pOSMP setInitialBitrate:nIB];
      NSLog(@"setInitialBitrate--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setInitialBitrate--- Param error");
  }
//  else if (name == keyGetPlayerType)
//  {
//  }
//  else if (name == keyGetParameter)
//  {
//  }
//  else if (name == keySetParameter)
//  {
//  }
//  else if (name == keySetZoomMode)
//  {
//  }
  else if (name == keyEnableLiveStreamingDVRPosition)
  {
    NSLog(@"enableLiveStreamingDVRPosition+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bEnable = getBoolVal(pArgs[0]);
      NSLog(@"enableLiveStreamingDVRPosition Param: Enable=%d", bEnable);
      int ret = [m_pOSMP enableLiveStreamingDVRPosition:bEnable];
      NSLog(@"enableLiveStreamingDVRPosition--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableLiveStreamingDVRPosition--- Param error");
  }
  else if (name == keyGetMinPosition)
  {
    NSLog(@"getMinPosition+++");
    int nPos = [m_pOSMP getMinPosition];
    NSLog(@"getMinPosition--- Return: %d", nPos);
    DOUBLE_TO_NPVARIANT(nPos, *pResult);
    return true;
  }
  else if (name == keyGetMaxPosition)
  {
    NSLog(@"getMaxPosition+++");
    int nPos = [m_pOSMP getMaxPosition];
    NSLog(@"getMaxPosition--- Return: %d", nPos);
    DOUBLE_TO_NPVARIANT(nPos, *pResult);
    return true;
  }
  else if (name == keySetDRMLibrary)
  {
    NSLog(@"setDRMLibrary+++");
    char* pLibName = getStrVal(pArgs[0]);
    char* pLibAPIName = getStrVal(pArgs[1]);
    NSLog(@"setDRMLibrary Param: LibName=%s, LibAPIName=%s", pLibName, pLibAPIName);
    int ret = [m_pOSMP setDRMLibrary:[NSString stringWithUTF8String:pLibName] libApiName:[NSString stringWithUTF8String:pLibAPIName]];
    NSLog(@"setDRMLibrary--- Return: 0x%0.8X", ret);

    if (ret == 0 &&
        [[NSString stringWithUTF8String:pLibName] caseInsensitiveCompare:@"voDRM_Verimatrix_AES128"] == NSOrderedSame &&
        [[NSString stringWithUTF8String:pLibAPIName] caseInsensitiveCompare:@"voGetDRMAPI"] == NSOrderedSame)
    {
      NSLog(@"setDRMVerificationInfoWithStructure+++");
      NSString* plistPath = [NSHomeDirectory() stringByAppendingString:@"/Library/Internet Plug-Ins/voBrowserPlugin.plugin/Contents/Info.plist"];
      NSMutableDictionary *plistDict = [[[NSMutableDictionary alloc] initWithContentsOfFile:plistPath] autorelease];
      NSNumber* infoType = [plistDict objectForKey:@"DRMVerificationInfoType"];
      NSString* infoString = [plistDict objectForKey:@"DRMVerificationInfoString"];

      VOOSMPVerificationInfo *info = [[[VOOSMPVerificationInfo alloc] init:(VO_OSMP_SRC_VERIFICATION_FLAG)[infoType longValue] verificationString:infoString] autorelease];
      int ret = [m_pOSMP setDRMVerificationInfoWithStructure:info];
      if (ret != 0)
      {
        NSLog(@"setDRMVerificationInfoWithStructure--- Return: 0x%0.8X", ret);
        freeStr(pLibName);
        freeStr(pLibAPIName);
        DOUBLE_TO_NPVARIANT(ret, *pResult);
        return true;
      }
      NSLog(@"setDRMVerificationInfoWithStructure---");
    }
    freeStr(pLibName);
    freeStr(pLibAPIName);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyEnableAnalytics)
  {
    NSLog(@"enableAnalytics+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nCacheTime = getIntVal(pArgs[0]);
      NSLog(@"enableAnalytics Param: CacheTime=%d", nCacheTime);
      int ret = [m_pOSMP enableAnalytics:nCacheTime];
      NSLog(@"enableAnalytics--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableAnalytics--- Param error");
  }
//  else if (name == keySetDRMAdapter)
//  {
//  }
  else if (name == keySetDRMVerificationInfo)
  {
    NSLog(@"setDRMVerificationInfo+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      JSON pInfo = getStrVal(pArgs[0]);
      NSLog(@"setDRMVerificationInfo Param: Info=%s", pInfo);
      int ret = [m_pOSMP setDRMVerificationInfo:pInfo];
      free(pInfo);
      NSLog(@"setDRMVerificationInfo--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setDRMVerificationInfo--- Param error");
  }
  else if (name == keyGetDRMUniqueIdentifier)
  {
    NSLog(@"getDRMUniqueIdentifier+++");
    NSString* strId = [m_pOSMP getDRMUniqueIdentifier];
    if (!strId)
    {
      NSLog(@"getDRMUniqueIdentifier--- Return: NULL");
      return false;
    }
    char* pVal = copyStringToNPNMem([strId UTF8String]);
    if (!pVal)
    {
      NSLog(@"getDRMUniqueIdentifier--- Return: NULL (Convert Error)");
      return false;
    }

    NSLog(@"getDRMUniqueIdentifier--- Return: %s", pVal);
    STRINGZ_TO_NPVARIANT(pVal, *pResult);
    return true;
  }
  else if (name == keySetDRMUniqueIdentifier)
  {
    NSLog(@"setDRMUniqueIdentifier+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      char* pId = getStrVal(pArgs[0]);
      NSLog(@"setDRMUniqueIdentifier Param: String=%s", pId);
      int ret = [m_pOSMP setDRMUniqueIdentifier:[NSString stringWithUTF8String:pId]];
      freeStr(pId);
      NSLog(@"setDRMUniqueIdentifier--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setDRMUniqueIdentifier--- Param error");
  }
//  else if (name == keySetInitialBufferTime)
//  {
//  }
  else if (name == keySetAnewBufferingTime)
  {
    NSLog(@"setAnewBufferingTime+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nTime = getIntVal(pArgs[0]);
      NSLog(@"setAnewBufferingTime Param: Time=%d", nTime);
      int ret = [m_pOSMP setAnewBufferingTime:nTime];
      NSLog(@"setAnewBufferingTime--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setAnewBufferingTime--- Param error");
  }
//  else if (name == keySetMaxBufferTime)
//  {
//  }
  else if (name == keyEnableAudioStream)
  {
    NSLog(@"enableAudioStream+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bEnable = getBoolVal(pArgs[0]);
      NSLog(@"enableAudioStream Param: Enable=%d", bEnable);
      int ret = [m_pOSMP enableAudioStream:bEnable];
      NSLog(@"enableAudioStream--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableAudioStream--- Param error");
  }
//  else if (name == keyEnableVideoStream)
//  {
//  }
//  else if (name == keyEnableLowLatencyVideo)
//  {
//  }
  else if (name == keyEnableAudioEffect)
  {
    NSLog(@"enableAudioEffect+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bEnable = getBoolVal(pArgs[0]);
      NSLog(@"enableAudioEffect Param: Enable=%d", bEnable);
      int ret = [m_pOSMP enableAudioEffect:bEnable];
      NSLog(@"enableAudioEffect--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableAudioEffect--- Param error");
  }
//  else if (name == keyEnablePCMOutput)
//  {
//  }
  else if (name == keySetAudioPlaybackSpeed)
  {
    NSLog(@"setAudioPlaybackSpeed+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      float fSpeed = getFloatVal(pArgs[0]);
      NSLog(@"setAudioPlaybackSpeed Param: Speed=%f", fSpeed);
      int ret = [m_pOSMP setAudioPlaybackSpeed:fSpeed];
      NSLog(@"setAudioPlaybackSpeed--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setAudioPlaybackSpeed--- Param error");
  }
  else if (name == keyGetVersion)
  {
    NSLog(@"getVersion+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nModule = getIntVal(pArgs[0]);
      NSLog(@"getVersion Param: Module=%d", nModule);
      NSString* strVer = [m_pOSMP getVersion:(VO_OSMP_MODULE_TYPE)nModule];
      if (!strVer)
      {
        NSLog(@"getVersion--- Return: NULL");
        return false;
      }
      char* pVal = copyStringToNPNMem([strVer UTF8String]);
      if (!pVal)
      {
        NSLog(@"getVersion--- Return: NULL (Convert Error)");
        return false;
      }
      NSLog(@"getVersion--- Return: %s", pVal);
      STRINGZ_TO_NPVARIANT(pVal, *pResult);
      return true;
    }
    NSLog(@"getVersion--- Param error");
  }
  else if (name == keySetPresentationDelay)
  {
    NSLog(@"setPresentationDelay+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nDelay = getIntVal(pArgs[0]);
      NSLog(@"setPresentationDelay Param: Delay=%d", nDelay);
      int ret = [m_pOSMP setPresentationDelay:nDelay];
      NSLog(@"setPresentationDelay--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setPresentationDelay--- Param error");
  }
  else if (name == keyEnableSEI)
  {
    NSLog(@"enableSEI+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int flag = getIntVal(pArgs[0]);
      NSLog(@"enableSEI Param: Flag=%d", flag);
      int ret = [m_pOSMP enableSEI:(VO_OSMP_SEI_INFO_FLAG)flag];
      NSLog(@"enableSEI--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableSEI--- Param error");
  }
//  else if (name == keySetPDConnectionRetryCount)
//  {
//  }
  else if (name == keyEnableCPUAdaptation)
  {
    NSLog(@"enableCPUAdaptation+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      BOOL bValue = getBoolVal(pArgs[0]);
      NSLog(@"enableCPUAdaptation Param: Value=%s", bValue ? "TRUE" : "FALSE");
      int ret = [m_pOSMP enableCPUAdaptation:bValue];
      NSLog(@"enableCPUAdaptation--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableCPUAdaptation--- Param error");
  }

  
  // Player HTTP Configuration
//  else if (name == keySetHTTPVerificationInfo)
//  {
//  }
  else if (name == keySetHttpHeader)
  {
    NSLog(@"setHttpHeader+++");
    if (pArgs != NULL && nArgCount == 2)
    {
      char* pHeader = getStrVal(pArgs[0]);
      char* pValue = getStrVal(pArgs[1]);
      NSLog(@"setHttpHeader Param: Header=%s, Value=%s", pHeader, pValue);
      
      int ret = [m_pOSMP setHTTPHeader:[NSString stringWithUTF8String:pHeader] headerValue:[NSString stringWithUTF8String:pValue]];
      freeStr(pHeader);
      freeStr(pValue);
      NSLog(@"setHttpHeader--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setHttpHeader--- Param error");
  }
  else if (name == keySetHttpProxy)
  {
    NSLog(@"setHttpProxy+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      JSON pProxy = getStrVal(pArgs[0]);
      NSLog(@"setHttpProxy Param: Proxy=%s", pProxy);
      int ret = [m_pOSMP setHTTPProxy:pProxy];
      freeStr(pProxy);
      NSLog(@"setHttpProxy--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setHttpProxy--- Param error");
  }
  
  // Player Asset Selection
  else if (name == keyGetVideoCount)
  {
    NSLog(@"getVideoCount+++");
    int nCount = [m_pOSMP getVideoCount];
    NSLog(@"getVideoCount--- Return: %d", nCount);
    DOUBLE_TO_NPVARIANT(nCount, *pResult);
    return true;
  }
  else if (name == keyGetAudioCount)
  {
    NSLog(@"getAudioCount+++");
    int nCount = [m_pOSMP getAudioCount];
    NSLog(@"getAudioCount--- Return: %d", nCount);
    DOUBLE_TO_NPVARIANT(nCount, *pResult);
    return true;
  }
  else if (name == keyGetSubtitleCount)
  {
    NSLog(@"getSubtitleCount+++");
    int nCount = [m_pOSMP getSubtitleCount];
    NSLog(@"getSubtitleCount--- Return: %d", nCount);
    DOUBLE_TO_NPVARIANT(nCount, *pResult);
    return true;
  }
  else if (name == keySelectVideo)
  {
    NSLog(@"selectVideo+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"selectVideo Param: Index=%d", nIndex);
      int ret = [m_pOSMP selectVideo:nIndex];
      NSLog(@"selectVideo--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"selectVideo--- Param error");
  }
  else if (name == keySelectAudio)
  {
    NSLog(@"selectAudio+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"selectAudio Param: Index=%d", nIndex);
      int ret = [m_pOSMP selectAudio:nIndex];
      NSLog(@"selectAudio--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"selectAudio--- Param error");
  }
  else if (name == keySelectSubtitle)
  {
    NSLog(@"selectSubtitle+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"selectSubtitle Param: Index=%d", nIndex);
      int ret = [m_pOSMP selectSubtitle:nIndex];
      NSLog(@"selectSubtitle--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"selectSubtitle--- Param error");
  }
  else if (name == keyIsVideoAvailable)
  {
    NSLog(@"isVideoAvailable+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"isVideoAvailable Param: Index=%d", nIndex);
      bool bAvailable = [m_pOSMP isVideoAvailable:nIndex];
      NSLog(@"isVideoAvailable--- Return: %d", bAvailable);
      BOOLEAN_TO_NPVARIANT(bAvailable, *pResult);
      return true;
    }
    NSLog(@"isVideoAvailable--- Param error");
  }
  else if (name == keyIsAudioAvailable)
  {
    NSLog(@"isAudioAvailable+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"isAudioAvailable Param: Index=%d", nIndex);
      bool bAvailable = [m_pOSMP isAudioAvailable:nIndex];
      NSLog(@"isAudioAvailable--- Return: %d", bAvailable);
      BOOLEAN_TO_NPVARIANT(bAvailable, *pResult);
      return true;
    }
    NSLog(@"isAudioAvailable--- Param error");
  }
  else if (name == keyIsSubtitleAvailable)
  {
    NSLog(@"isSubtitleAvailable+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"isSubtitleAvailable Param: Index=%d", nIndex);
      bool bAvailable = [m_pOSMP isSubtitleAvailable:nIndex];
      NSLog(@"isSubtitleAvailable--- Return: %d", bAvailable);
      BOOLEAN_TO_NPVARIANT(bAvailable, *pResult);
      return true;
    }
    NSLog(@"isSubtitleAvailable--- Param error");
  }
  else if (name == keyCommitSelection)
  {
    NSLog(@"commitSelection+++");
    int ret = [m_pOSMP commitSelection];
    NSLog(@"commitSelection--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyClearSelection)
  {
    NSLog(@"clearSelection+++");
    int ret = [m_pOSMP clearSelection];
    NSLog(@"clearSelection--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  else if (name == keyGetVideoProperty)
  {
    NSLog(@"getVideoProperty+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"getVideoProperty Param: Index=%d", nIndex);
      JSON pProp = [m_pOSMP getVideoProperty:nIndex];
      if (!pProp)
      {
        NSLog(@"getVideoProperty--- Return: NULL");
        return false;
      }
      char* pVal = copyStringToNPNMem(pProp);
      [CvoOSMP_v3 freeJSON:pProp];
      if (!pVal)
      {
        NSLog(@"getVideoProperty--- Return: NULL (Convert Error)");
        return false;
      }
      NSLog(@"getVideoProperty--- Return: %s", pVal);
      
      STRINGZ_TO_NPVARIANT(pVal, *pResult);
      return true;
    }
    NSLog(@"getVideoProperty--- Param error");
  }
  else if (name == keyGetAudioProperty)
  {
    NSLog(@"getAudioProperty+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"getAudioProperty Param: Index=%d", nIndex);
      JSON pProp = [m_pOSMP getAudioProperty:nIndex];
      if (!pProp)
      {
        NSLog(@"getAudioProperty--- Return: NULL");
        return false;
      }
      char* pVal = copyStringToNPNMem(pProp);
      [CvoOSMP_v3 freeJSON:pProp];
      if (!pVal)
      {
        NSLog(@"getAudioProperty--- Return: NULL (Convert Error)");
        return false;
      }
      NSLog(@"getAudioProperty--- Return: %s", pVal);
      
      STRINGZ_TO_NPVARIANT(pVal, *pResult);
      return true;
    }
    NSLog(@"getAudioProperty--- Param error");
  }
  else if (name == keyGetSubtitleProperty)
  {
    NSLog(@"getSubtitleProperty+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nIndex = getIntVal(pArgs[0]);
      NSLog(@"getSubtitleProperty Param: Index=%d", nIndex);
      JSON pProp = [m_pOSMP getSubtitleProperty:nIndex];
      if (!pProp)
      {
        NSLog(@"getSubtitleProperty--- Return: NULL");
        return false;
      }
      char* pVal = copyStringToNPNMem(pProp);
      [CvoOSMP_v3 freeJSON:pProp];
      if (!pVal)
      {
        NSLog(@"getSubtitleProperty--- Return: NULL (Convert Error)");
        return false;
      }
      NSLog(@"getSubtitleProperty--- Return: %s", pVal);
      
      STRINGZ_TO_NPVARIANT(pVal, *pResult);
      return true;
    }
    NSLog(@"getSubtitleProperty--- Param error");
  }
  else if (name == keyGetPlayingAsset)
  {
    NSLog(@"getPlayingAsset+++");
    JSON pAsset = [m_pOSMP getPlayingAsset];
    if (!pAsset)
    {
      NSLog(@"getPlayingAsset--- Return: NULL");
      return false;
    }
    char* pVal = copyStringToNPNMem(pAsset);
    [CvoOSMP_v3 freeJSON:pAsset];
    if (!pVal)
    {
      NSLog(@"getPlayingAsset--- Return: NULL (Convert Error)");
      return false;
    }
    NSLog(@"getPlayingAsset--- Return: %s", pVal);
    
    STRINGZ_TO_NPVARIANT(pVal, *pResult);
    return true;
  }
  else if (name == keyGetCurrentSelection)
  {
    NSLog(@"getCurrentSelection+++");
    JSON pCurSel = [m_pOSMP getCurrentSelection];
    if (!pCurSel)
    {
      NSLog(@"getCurrentSelection--- Return: NULL");
      return false;
    }
    char* pVal = copyStringToNPNMem(pCurSel);
    [CvoOSMP_v3 freeJSON:pCurSel];
    if (!pVal)
    {
      NSLog(@"getCurrentSelection--- Return: NULL (Convert Error)");
      return false;
    }
    NSLog(@"getCurrentSelection--- Return: %s", pVal);
    
    STRINGZ_TO_NPVARIANT(pVal, *pResult);
    return true;
  }
  
  
  // Player Subtitle
  else if (name == keySetSubtitlePath)
  {
    NSLog(@"setSubtitlePath+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      JSON pPath = getStrVal(pArgs[0]);
      NSLog(@"setSubtitlePath Param: Path=%s", pPath);
      int ret = [m_pOSMP setSubtitlePath:[NSString stringWithUTF8String:pPath]];
      freeStr(pPath);
      NSLog(@"setSubtitlePath--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitlePath--- Param error");
  }
  else if (name == keyEnableSubtitle)
  {
    NSLog(@"enableSubtitle+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bEnable = getBoolVal(pArgs[0]);
      NSLog(@"enableSubtitle Param: Enable=%d", bEnable);
      int ret = [m_pOSMP enableSubtitle:bEnable];
      NSLog(@"enableSubtitle--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"enableSubtitle--- Param error");
  }
  else if (name == keySetSubtitleFontColor)
  {
    NSLog(@"setSubtitleFontColor+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nColor = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontColor Param: Color=%d", nColor);
      int ret = [m_pOSMP setSubtitleFontColor:nColor];
      NSLog(@"setSubtitleFontColor--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontColor--- Param error");
  }
  else if (name == keySetSubtitleFontOpacity)
  {
    NSLog(@"setSubtitleFontOpacity+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nOpacity = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontOpacity Param: Opacity=%d", nOpacity);
      int ret = [m_pOSMP setSubtitleFontOpacity:nOpacity];
      NSLog(@"setSubtitleFontOpacity--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontOpacity--- Param error");
  }
  else if (name == keySetSubtitleFontSizeScale)
  {
    NSLog(@"setSubtitleFontSizeScale+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nScale = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontSizeScale Param: Scale=%d", nScale);
      int ret = [m_pOSMP setSubtitleFontSizeScale:nScale];
      NSLog(@"setSubtitleFontSizeScale--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontSizeScale--- Param error");
  }
  else if (name == keySetSubtitleFontBackgroundColor)
  {
    NSLog(@"setSubtitleFontBackgroundColor+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nColor = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontBackgroundColor Param: Color=%d", nColor);
      int ret = [m_pOSMP setSubtitleFontBackgroundColor:nColor];
      NSLog(@"setSubtitleFontBackgroundColor--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontBackgroundColor--- Param error");
  }
  else if (name == keySetSubtitleFontBackgroundOpacity)
  {
    NSLog(@"setSubtitleFontBackgroundOpacity+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nOpacity = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontBackgroundOpacity Param: Opacity=%d", nOpacity);
      int ret = [m_pOSMP setSubtitleFontBackgroundOpacity:nOpacity];
      NSLog(@"setSubtitleFontBackgroundOpacity--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontBackgroundOpacity--- Param error");
  }
  else if (name == keySetSubtitleWindowBackgroundColor)
  {
    NSLog(@"setSubtitleWindowBackgroundColor+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nColor = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleWindowBackgroundColor Param: Color=%d", nColor);
      int ret = [m_pOSMP setSubtitleWindowBackgroundColor:nColor];
      NSLog(@"setSubtitleWindowBackgroundColor--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleWindowBackgroundColor--- Param error");
  }
  else if (name == keySetSubtitleWindowBackgroundOpacity)
  {
    NSLog(@"setSubtitleWindowBackgroundOpacity+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nOpacity = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleWindowBackgroundOpacity Param: Opacity=%d", nOpacity);
      int ret = [m_pOSMP setSubtitleWindowBackgroundOpacity:nOpacity];
      NSLog(@"setSubtitleWindowBackgroundOpacity--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleWindowBackgroundOpacity--- Param error");
  }
  else if (name == keySetSubtitleFontItalic)
  {
    NSLog(@"setSubtitleFontItalic+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bItalic = getBoolVal(pArgs[0]);
      NSLog(@"setSubtitleFontItalic Param: Italic=%d", bItalic);
      int ret = [m_pOSMP setSubtitleFontItalic:bItalic];
      NSLog(@"setSubtitleFontItalic--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontItalic--- Param error");
  }
  else if (name == keySetSubtitleFontBold)
  {
    NSLog(@"setSubtitleFontBold+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bBold = getBoolVal(pArgs[0]);
      NSLog(@"setSubtitleFontBold Param: Bold=%d", bBold);
      int ret = [m_pOSMP setSubtitleFontBold:bBold];
      NSLog(@"setSubtitleFontBold--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontBold--- Param error");
  }
  else if (name == keySetSubtitleFontUnderline)
  {
    NSLog(@"setSubtitleFontUnderline+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      bool bUnderline = getBoolVal(pArgs[0]);
      NSLog(@"setSubtitleFontUnderline Param: Underline=%d", bUnderline);
      int ret = [m_pOSMP setSubtitleFontUnderline:bUnderline];
      NSLog(@"setSubtitleFontUnderline--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontUnderline--- Param error");
  }
  else if (name == keySetSubtitleFontName)
  {
    NSLog(@"setSubtitleFontName+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      JSON pName = getStrVal(pArgs[0]);
      if (!pName)
      {
        NSLog(@"setSubtitleFontName--- Param error (NULL)");
        return false;
      }
      NSLog(@"setSubtitleFontName Param: Name=%s", pName);
      int ret = [m_pOSMP setSubtitleFontName:[NSString stringWithUTF8String:pName]];
      freeStr(pName);
      NSLog(@"setSubtitleFontName--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontName--- Param error");
  }
  else if (name == keySetSubtitleFontEdgeType)
  {
    NSLog(@"setSubtitleFontEdgeType+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nType = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontEdgeType Param: Type=%d", nType);
      int ret = [m_pOSMP setSubtitleFontEdgeType:nType];
      NSLog(@"setSubtitleFontEdgeType--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontEdgeType--- Param error");
  }
  else if (name == keySetSubtitleFontEdgeColor)
  {
    NSLog(@"setSubtitleFontEdgeColor+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nColor = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontEdgeColor Param: Color=%d", nColor);
      int ret = [m_pOSMP setSubtitleFontEdgeColor:nColor];
      NSLog(@"setSubtitleFontEdgeColor--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontEdgeColor--- Param error");
  }
  else if (name == keySetSubtitleFontEdgeOpacity)
  {
    NSLog(@"setSubtitleFontEdgeOpacity+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      int nOpacity = getIntVal(pArgs[0]);
      NSLog(@"setSubtitleFontEdgeOpacity Param: Opacity=%d", nOpacity);
      int ret = [m_pOSMP setSubtitleFontEdgeOpacity:nOpacity];
      NSLog(@"setSubtitleFontEdgeOpacity--- Return: 0x%0.8X", ret);
      DOUBLE_TO_NPVARIANT(ret, *pResult);
      return true;
    }
    NSLog(@"setSubtitleFontEdgeOpacity--- Param error");
  }
  else if (name == keyResetSubtitleParameter)
  {
    NSLog(@"resetSubtitleParameter+++");
    int ret = [m_pOSMP resetSubtitleParameter];
    NSLog(@"resetSubtitleParameter--- Return: 0x%0.8X", ret);
    DOUBLE_TO_NPVARIANT(ret, *pResult);
    return true;
  }
  
  // System
  else if (name == keyRegisterEventHandler)
  {
    NSLog(@"registerEventHandler+++");
    if (pArgs != NULL && nArgCount == 1)
    {
      m_pJSCallbackOjbect = NPVARIANT_TO_OBJECT(pArgs[0]);
      NSLog(@"registerEventHandler Param: JSCallbackOjbect=0x%0.8X", (unsigned int)m_pJSCallbackOjbect);
      if (m_pJSCallbackOjbect)
        m_pBrowserFuncs->retainobject(m_pJSCallbackOjbect);
      NSLog(@"registerEventHandler--- Return: 0x%0.8X", 0);
      DOUBLE_TO_NPVARIANT(0, *pResult);
      return true;
    }
  }
  else if (name == keyShowCursor)
  {
    NSLog(@"showCursor+++");
    NSLog(@"showCursor--- Return: 0x%0.8X", 0);
    DOUBLE_TO_NPVARIANT(0, *pResult);
    return true;
  }
  else if (name == keyFullScreen)
  {
    NSLog(@"fullScreen+++");
    m_bFullScreen = !m_bFullScreen;
    NSLog(@"fullScreen---");
    DOUBLE_TO_NPVARIANT(0, *pResult);
    return true;
  }
  else if (name == keyIsFullScreen)
  {
    NSLog(@"isFullScreen+++");
    NSLog(@"isFullScreen---");
    DOUBLE_TO_NPVARIANT(m_bFullScreen, *pResult);
    return true;
  }
  else if (name == keyToggleOverlayUI)
  {
    NSLog(@"toggleOverlayUI+++");
    NSLog(@"toggleOverlayUI--- Return: 0x%0.8X", 0);
    DOUBLE_TO_NPVARIANT(0, *pResult);
    return true;
  }
  
  DOUBLE_TO_NPVARIANT(-1, *pResult);
  return false;
}
