#import "voOSMP_v3.h"

@implementation CvoOSMP_v3


@synthesize m_player = player;

- (id)init: (CALayer*)pVideoLayer CallbackToWeb:(GLOBALCALLBACKTOWEB)pCallbackToWeb Context:(void*)pContext
{
  id ret = [super init];
  if (ret == nil)
    return ret;
  
  m_bStopped = false;
  m_nOpenFlag = 0;
  m_bOpenFailed = false;
  m_pVideoLayer = pVideoLayer;
  m_pPlayerCtrl = nil;
  m_pPlayerConf = nil;
  m_pPlayerHTTPConf = nil;
  m_pPlayerAssetSelection = nil;
  m_pPlayerSubtitle = nil;
  m_pCallbackToWeb = pCallbackToWeb;
  m_pContext = pContext;

  memset(m_szOpenUrl, 0, sizeof(m_szOpenUrl));
  
  return ret;
}

- (void)dealloc
{
  m_pPlayerCtrl = nil;
  m_pPlayerConf = nil;
  m_pPlayerHTTPConf = nil;
  m_pPlayerAssetSelection = nil;
  m_pPlayerSubtitle = nil;
  self.m_player = nil;
  
  [super dealloc];
}


#pragma mark Structure Convert
+ (void)freeJSON:(JSON)json
{
  free(json);
}

- (VOOSMPAnalyticsFilter*) parseAnalyticsFilter:(const char *)filter
{
  int nLastTime = 0;
  int nSourceTime = 0;
  int nCodecTime = 0;
  int nRenderTime = 0;
  int nJitterTime = 0;
  
	voJSON * pJSON = voJSON_Parse(filter);
	int nSize = voJSON_GetArraySize(pJSON);
  for(int i = 0; i<nSize; ++i)
	{
		voJSON * pJSONItem = voJSON_GetArrayItem(pJSON, i);
		if (pJSONItem == NULL || pJSONItem->string == NULL || strlen(pJSONItem->string) <= 0)
			return nil;
		
		if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"lastTime"])
      nLastTime = atoi(pJSONItem->valuestring);
		else if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"sourceTime"])
      nSourceTime = atoi(pJSONItem->valuestring);
		else if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"codecTime"])
      nCodecTime = atoi(pJSONItem->valuestring);
		else if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"renderTime"])
      nRenderTime = atoi(pJSONItem->valuestring);
		else if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"jitter"])
      nJitterTime = atoi(pJSONItem->valuestring);
	}
  
  VOOSMPAnalyticsFilter* pFilter = [[[VOOSMPAnalyticsFilter alloc] init:nLastTime sourceTime:nSourceTime codecTime:nCodecTime renderTime:nRenderTime jitterTime:nJitterTime] autorelease];
  
  return pFilter;
}

- (JSON) packAnalyticsInfo:(id<VOOSMPAnalyticsInfo>)info
{
  voJSON *pJSON = voJSON_CreateObject();
	if (pJSON == NULL)
		return NULL;
  
	voJSON_AddNumberToObject(pJSON, "lastTime", info.lastTime);
	voJSON_AddNumberToObject(pJSON, "sourceDropNum", info.sourceDropNum);
	voJSON_AddNumberToObject(pJSON, "codecDropNum", info.codecDropNum);
	voJSON_AddNumberToObject(pJSON, "renderDropNum", info.renderDropNum);
	voJSON_AddNumberToObject(pJSON, "decodedNum", info.decodedNum);
	voJSON_AddNumberToObject(pJSON, "renderNum", info.renderNum);
	voJSON_AddNumberToObject(pJSON, "sourceTimeNum", info.sourceTimeNum);
	voJSON_AddNumberToObject(pJSON, "codecTimeNum", info.codecTimeNum);
	voJSON_AddNumberToObject(pJSON, "renderTimeNum", info.renderTimeNum);
	voJSON_AddNumberToObject(pJSON, "jitterNum", info.jitterNum);
	voJSON_AddNumberToObject(pJSON, "codecErrorsNum", info.codecErrorsNum);
	if (info.codecErrorsNum > 0)
		voJSON_AddItemToObject(pJSON, "codecErrors", voJSON_CreateIntArray(info.codecErrors, info.codecErrorsNum));
	else
		voJSON_AddStringToObject(pJSON, "codecErrors", "none errors");
	voJSON_AddNumberToObject(pJSON, "CPULoad", info.CPULoad);
	voJSON_AddNumberToObject(pJSON, "frequency", info.frequency);
	voJSON_AddNumberToObject(pJSON, "maxFrequency", info.maxFrequency);
	voJSON_AddNumberToObject(pJSON, "worstDecodeTime", info.worstDecodeTime);
	voJSON_AddNumberToObject(pJSON, "worstRenderTime", info.worstRenderTime);
	voJSON_AddNumberToObject(pJSON, "averageDecodeTime", info.averageDecodeTime);
	voJSON_AddNumberToObject(pJSON, "averageRenderTime", info.averageRenderTime);
	voJSON_AddNumberToObject(pJSON, "totalCPULoad", info.totalCPULoad);
  
	char* szValue = voJSON_Print(pJSON);
	voJSON_Delete(pJSON);

  return szValue;
}

- (JSON) packSEIPicTiming:(id<VOOSMPSEIPicTiming>)picTiming
{
  voJSON *pJSON = voJSON_CreateObject();
	if (pJSON == NULL)
		return NULL;
  
//	voJSON_AddNumberToObject(pJSON, "lastTime", info.lastTime);
  return NULL;
}

- (VOOSMPVerificationInfo*) parseVerificationInfo:(JSON)info
{
  int nDataFlag = 0;
  NSString* pVerificationString = 0;
  
	voJSON * pJSON = voJSON_Parse(info);
	int nSize = voJSON_GetArraySize(pJSON);
  for(int i = 0; i<nSize; ++i)
	{
		voJSON * pJSONItem = voJSON_GetArrayItem(pJSON, i);
		if (pJSONItem == NULL || pJSONItem->string == NULL || strlen(pJSONItem->string) <= 0)
			return nil;
		
		if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"dataFlag"])
      nDataFlag = atoi(pJSONItem->valuestring);
		else if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"verificationString"])
      pVerificationString = [NSString stringWithUTF8String:pJSONItem->valuestring];
	}
  
  VOOSMPVerificationInfo* pInfo = [[[VOOSMPVerificationInfo alloc] init:(VO_OSMP_SRC_VERIFICATION_FLAG)nDataFlag verificationString:pVerificationString] autorelease];
  
  return pInfo;
}

- (VOOSMPHTTPProxy*) parseHttpProxy:(JSON)proxy
{
  int nProxyPort = 0;
  NSString* pProxyHost = 0;
  
	voJSON * pJSON = voJSON_Parse(proxy);
	int nSize = voJSON_GetArraySize(pJSON);
  for(int i = 0; i<nSize; ++i)
	{
		voJSON * pJSONItem = voJSON_GetArrayItem(pJSON, i);
		if (pJSONItem == NULL || pJSONItem->string == NULL || strlen(pJSONItem->string) <= 0)
			return nil;
		
		if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"proxyHost"])
      pProxyHost = [NSString stringWithUTF8String:pJSONItem->valuestring];
		else if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"proxyPort"])
      nProxyPort = atoi(pJSONItem->valuestring);
	}
  
  VOOSMPHTTPProxy* pProxy = [[[VOOSMPHTTPProxy alloc] init:pProxyHost proxyPort:nProxyPort] autorelease];
  
  return pProxy;
}

- (JSON) packAssetProperty:(id<VOOSMPAssetProperty>)prop
{
  voJSON *pJSON = voJSON_CreateObject();
	if (pJSON == NULL)
		return NULL;
  
  int nPropCount = [prop getPropertyCount];
  
	voJSON_AddNumberToObject(pJSON, "propertyCount", nPropCount);
  
  for (int i=0; i<nPropCount; i++)
  {
    voJSON_AddStringToObject(pJSON, [[prop getKey:i] UTF8String], [(NSString*)[prop getValue:i] UTF8String]);
  }
  
	char* szValue = voJSON_Print(pJSON);
	voJSON_Delete(pJSON);
  
  return szValue;
  
}

- (JSON) packAssetIndex:(id<VOOSMPAssetIndex>)index
{
  voJSON *pJSON = voJSON_CreateObject();
	if (pJSON == NULL)
		return NULL;
  
	voJSON_AddNumberToObject(pJSON, "videoIndex", index.getVideoIndex);
	voJSON_AddNumberToObject(pJSON, "audioIndex", index.getAudioIndex);
	voJSON_AddNumberToObject(pJSON, "subtitleIndex", index.getSubtitleIndex);
  
	char* szValue = voJSON_Print(pJSON);
	voJSON_Delete(pJSON);
  
  return szValue;
}

- (VOOSMPOpenParam *) parseOpenParam:(JSON)openParam
{
  int nFileSize = 0;
	voJSON * pJSON = voJSON_Parse(openParam);
	int nSize = voJSON_GetArraySize(pJSON);
  for(int i = 0; i<nSize; ++i)
	{
		voJSON * pJSONItem = voJSON_GetArrayItem(pJSON, i);
		if (pJSONItem == NULL || pJSONItem->string == NULL || strlen(pJSONItem->string) <= 0)
			return nil;
		
		if ([[NSString stringWithUTF8String:pJSONItem->string] caseInsensitiveCompare:@"fileSize"])
      nFileSize = atoi(pJSONItem->valuestring);
	}
  
  VOOSMPOpenParam* pOpenParam = [[[VOOSMPOpenParam alloc] init] autorelease];
  [pOpenParam setFileSize:nFileSize];
  
  return pOpenParam;
}

#pragma mark Player Controller

- (VO_OSMP_RETURN_CODE) initPlayer:(VO_OSMP_PLAYER_ENGINE)playEngineType initParam:(JSON)initParam;
{
  VOOSMPInitParam* pInitParam = [[VOOSMPInitParam alloc] init];
  NSString* strPath = [NSHomeDirectory() stringByAppendingString:@"/Library/Internet Plug-Ins/voBrowserPlugin.plugin/Contents/Resources/"];
  [pInitParam setLibraryPath:strPath];
  
  self.m_player = [[VOCommonPlayerImpl alloc] init:playEngineType initParam:pInitParam];
  [pInitParam release];
  if (self.m_player == nil)
    return VO_OSMP_ERR_UNKNOWN;

  [self.m_player release];
  
  m_pPlayerCtrl           = self.m_player;
  m_pPlayerConf           = self.m_player;
  m_pPlayerHTTPConf       = self.m_player;
  m_pPlayerAssetSelection = self.m_player;
  m_pPlayerSubtitle       = self.m_player;
  
  [m_pPlayerCtrl setOnEventDelegate:self];
  
  return VO_OSMP_ERR_NONE;
}

- (VO_OSMP_RETURN_CODE) setView:(void*) view
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl setView: view];
}

- (VO_OSMP_RETURN_CODE) setDisplayArea:(Rect)rect
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl setDisplayArea: rect];
}

- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType openParam:(JSON)openParam;
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  VOOSMPOpenParam* pOpenParam = [self parseOpenParam:openParam];
  if (pOpenParam == nil)
    return VO_OSMP_SRC_ERR_ERROR_DATA;

  VO_OSMP_RETURN_CODE ret = [m_pPlayerCtrl open:url flag:flag sourceType:sourceType openParam:pOpenParam];
  
  return ret;
}

- (VO_OSMP_RETURN_CODE) close
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl close];
}

- (VO_OSMP_RETURN_CODE) start
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl start];
}

- (VO_OSMP_RETURN_CODE) pause
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl pause];
}

- (VO_OSMP_RETURN_CODE) stop
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl stop];
}

- (long) setPosition:(long)msec
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl setPosition:msec];
}

- (long) getPosition
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl getPosition];
}

- (long) getDuration
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl getDuration];
}

- (VO_OSMP_RETURN_CODE) setVolume:(float)value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl setVolume:value];
}

- (VO_OSMP_RETURN_CODE) mute
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl mute];
}

- (VO_OSMP_RETURN_CODE) unmute
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl unmute];
}

- (VO_OSMP_RETURN_CODE) suspend:(bool)keepAudio
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl suspend:keepAudio];
}

- (VO_OSMP_RETURN_CODE) resume
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl resume];
}

- (VO_OSMP_RETURN_CODE) setVideoAspectRatio:(VO_OSMP_ASPECT_RATIO)ar
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl setVideoAspectRatio:ar];
}

- (VO_OSMP_RETURN_CODE) redrawVideo
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl redrawVideo];
}

- (VO_OSMP_STATUS) getPlayerStatus
{
  if (self.m_player == nil)
    return VO_OSMP_STATUS_MAX;
  
  return [m_pPlayerCtrl getPlayerStatus];
}

- (bool) canBePaused
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl canBePaused];
}

- (bool) isLiveStreaming
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl isLiveStreaming];
}

- (VO_OSMP_RETURN_CODE) startAnalyticsNotification:(int)interval filter:(const char *)filter
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  VOOSMPAnalyticsFilter* pAF = [self parseAnalyticsFilter:filter];
  if (pAF == nil)
    return VO_OSMP_SRC_ERR_ERROR_DATA;
  
  VO_OSMP_RETURN_CODE ret = [m_pPlayerCtrl startAnalyticsNotification:interval filter:pAF];
  
  return ret;
}

- (VO_OSMP_RETURN_CODE) stopAnalyticsNotification
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl stopAnalyticsNotification];
}

- (JSON) getAnalytics:(const char *)filter
{
  if (self.m_player == nil)
    return nil;
  
  VOOSMPAnalyticsFilter* pAF = [self parseAnalyticsFilter:filter];
  if (pAF == nil)
    return NULL;
  
  id<VOOSMPAnalyticsInfo> info = [m_pPlayerCtrl getAnalytics:pAF];
  
  JSON pRet = [self packAnalyticsInfo:info];
  
  return pRet;
}

- (VO_OSMP_RETURN_CODE) setScreenBrightness:(int)brightness
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl setScreenBrightness:brightness];
}

- (int) getScreenBrightness
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl getScreenBrightness];
}

- (VO_OSMP_RETURN_CODE) startSEINotification:(int)interval
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl startSEINotification:interval];
}

- (VO_OSMP_RETURN_CODE) stopSEINotification
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerCtrl stopSEINotification];
}

- (JSON) getSEIInfo:(long long)time flag:(VO_OSMP_SEI_INFO_FLAG)flag
{
  if (self.m_player == nil)
    return nil;
  
  id ret = [m_pPlayerCtrl getSEIInfo:time flag:flag];
  
  if (flag == VO_OSMP_FLAG_SEI_PIC_TIMING)
  {
    id<VOOSMPSEIPicTiming> picTiming = ret;
    JSON json = [self packSEIPicTiming:picTiming];
    return json;
  }
  else
  {
    return nil;
  }
}


#pragma mark Player Configuration
- (VO_OSMP_RETURN_CODE) setLicenseContent:(NSData *)data
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setLicenseContent:data];
}

- (VO_OSMP_RETURN_CODE) setLicenseFilePath:(NSString *)path
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setLicenseFilePath:path];
}

- (VO_OSMP_RETURN_CODE) setPreAgreedLicense:(NSString *)str
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setPreAgreedLicense:str];
}

- (VO_OSMP_RETURN_CODE) enableDeblock:(bool) value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableDeblock:value];
}

- (VO_OSMP_RETURN_CODE) setDeviceCapabilityByFile:(NSString *)filename
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setDeviceCapabilityByFile:filename];
}

- (VO_OSMP_RETURN_CODE) setInitialBitrate:(int) bitrate
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setInitialBitrate:bitrate];
}

- (VO_OSMP_PLAYER_ENGINE) getPlayerType
{
  if (self.m_player == nil)
    return VO_OSMP_PLAYER_ENGINE_MAX;
  
  return [m_pPlayerConf getPlayerType];
}

- (VO_OSMP_RETURN_CODE) getParameter:(int)paramID pObj:(void *)pObj
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf getParameter:paramID pObj:pObj];
}

- (VO_OSMP_RETURN_CODE) setParameter:(int)paramID pObj:(void *)pObj
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setParameter:paramID pObj:pObj];
}

- (VO_OSMP_RETURN_CODE) setZoomMode:(VO_OSMP_ZOOM_MODE)mode rect:(Rect)rect
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setZoomMode:mode rect:rect];
}

- (VO_OSMP_RETURN_CODE) enableLiveStreamingDVRPosition:(bool)enable
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableLiveStreamingDVRPosition:enable];
}

- (long) getMinPosition
{
  if (self.m_player == nil)
    return -1;
  
  return [m_pPlayerConf getMinPosition];
}

- (long) getMaxPosition
{
  if (self.m_player == nil)
    return -1;
  
  return [m_pPlayerConf getMaxPosition];
}

- (VO_OSMP_RETURN_CODE) setDRMLibrary:(NSString *)libName libApiName:(NSString *)libApiName
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setDRMLibrary:libName libApiName:libApiName];
}

- (VO_OSMP_RETURN_CODE) enableAnalytics:(int)cacheTime
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableAnalytics:cacheTime];
}

- (VO_OSMP_RETURN_CODE) setDRMAdapter:(void *)adapter
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setDRMAdapter:adapter];
}

- (VO_OSMP_RETURN_CODE) setDRMVerificationInfoWithStructure:(VOOSMPVerificationInfo*)info
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setDRMVerificationInfo:info];
}

- (VO_OSMP_RETURN_CODE) setDRMVerificationInfo:(JSON)info
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  VOOSMPVerificationInfo* pInfo = [self parseVerificationInfo:info];
  if (pInfo == nil)
    return VO_OSMP_SRC_ERR_ERROR_DATA;

  VO_OSMP_RETURN_CODE ret = [m_pPlayerConf setDRMVerificationInfo:pInfo];
  
  return ret;
}

- (NSString *) getDRMUniqueIdentifier
{
  if (self.m_player == nil)
    return nil;
  
  return [m_pPlayerConf getDRMUniqueIdentifier];
}

- (VO_OSMP_RETURN_CODE) setDRMUniqueIdentifier:(NSString *)string
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setDRMUniqueIdentifier:string];
}

- (VO_OSMP_RETURN_CODE) setInitialBufferTime:(int)time
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setInitialBufferTime:time];
}

- (VO_OSMP_RETURN_CODE) setAnewBufferingTime:(int)time
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setAnewBufferingTime:time];
}

- (VO_OSMP_RETURN_CODE) setMaxBufferTime:(int)time
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setMaxBufferTime:time];
}

- (VO_OSMP_RETURN_CODE) enableAudioStream:(bool) value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableAudioStream:value];
}

- (VO_OSMP_RETURN_CODE) enableVideoStream:(bool) value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableVideoStream:value];
}

- (VO_OSMP_RETURN_CODE) enableLowLatencyVideo:(bool) value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableLowLatencyVideo:value];
}

- (VO_OSMP_RETURN_CODE) enableAudioEffect:(bool) value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableAudioEffect:value];
}

- (VO_OSMP_RETURN_CODE) enablePCMOutput:(bool) value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enablePCMOutput:value];
}

- (VO_OSMP_RETURN_CODE) setAudioPlaybackSpeed:(float)speed
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setAudioPlaybackSpeed:speed];
}

- (NSString*) getVersion:(VO_OSMP_MODULE_TYPE)module
{
  if (self.m_player == nil)
    return nil;
  
  return [m_pPlayerConf getVersion:module];
}

- (VO_OSMP_RETURN_CODE) setPresentationDelay:(int)time
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setPresentationDelay:time];
}

- (VO_OSMP_RETURN_CODE) enableSEI:(VO_OSMP_SEI_INFO_FLAG)flag
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableSEI:flag];
}

- (VO_OSMP_RETURN_CODE) setPDConnectionRetryCount:(int)times
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf setPDConnectionRetryCount:times];
}

- (VO_OSMP_RETURN_CODE) enableCPUAdaptation:(bool)value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerConf enableCPUAdaptation:value];
}


#pragma mark Player HTTP Configuration
- (VO_OSMP_RETURN_CODE) setHTTPVerificationInfo:(JSON)info
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  VOOSMPVerificationInfo* pInfo = [self parseVerificationInfo:info];
  if (pInfo == nil)
    return VO_OSMP_SRC_ERR_ERROR_DATA;

  VO_OSMP_RETURN_CODE ret = [m_pPlayerHTTPConf setHTTPVerificationInfo:pInfo];
  
  return ret;
}

- (VO_OSMP_RETURN_CODE) setHTTPHeader:(NSString *)headerName headerValue:(NSString *)headerValue
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerHTTPConf setHTTPHeader:headerName headerValue:headerValue];
}

- (VO_OSMP_RETURN_CODE) setHTTPProxy:(JSON)proxy
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  VOOSMPHTTPProxy* pProxy = [self parseHttpProxy:proxy];
  if (pProxy == nil)
    return VO_OSMP_SRC_ERR_ERROR_DATA;

  VO_OSMP_RETURN_CODE ret = [m_pPlayerHTTPConf setHTTPProxy:pProxy];
  
  return ret;
}

#pragma mark Player Asset Selection
- (int) getVideoCount
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection getVideoCount];
}

- (int) getAudioCount
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection getAudioCount];
}

- (int) getSubtitleCount
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection getSubtitleCount];
}

- (VO_OSMP_RETURN_CODE) selectVideo:(int)index
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection selectVideo:index];
}

- (VO_OSMP_RETURN_CODE) selectAudio:(int)index
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection selectAudio:index];
}

- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection selectSubtitle:index];
}

- (bool) isVideoAvailable:(int)index
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection isVideoAvailable:index];
}

- (bool) isAudioAvailable:(int)index
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection isAudioAvailable:index];
}

- (bool) isSubtitleAvailable:(int)index
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection isSubtitleAvailable:index];
}

- (VO_OSMP_RETURN_CODE) commitSelection
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection commitSelection];
}

- (VO_OSMP_RETURN_CODE) clearSelection
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerAssetSelection clearSelection];
}

- (JSON) getVideoProperty:(int)index
{
  if (self.m_player == nil)
    return nil;
  
  id<VOOSMPAssetProperty> prop = [m_pPlayerAssetSelection getVideoProperty:index];
  JSON json = [self packAssetProperty:prop];
  
  return json;
}

- (JSON) getAudioProperty:(int)index
{
  if (self.m_player == nil)
    return nil;
  
  id<VOOSMPAssetProperty> prop = [m_pPlayerAssetSelection getAudioProperty:index];
  JSON json = [self packAssetProperty:prop];
  
  return json;
}

- (JSON) getSubtitleProperty:(int)index
{
  if (self.m_player == nil)
    return nil;
  
  id<VOOSMPAssetProperty> prop = [m_pPlayerAssetSelection getSubtitleProperty:index];
  JSON json = [self packAssetProperty:prop];
  
  return json;
}

- (JSON) getPlayingAsset
{
  if (self.m_player == nil)
    return nil;
  
  id<VOOSMPAssetIndex> index = [m_pPlayerAssetSelection getPlayingAsset];
  JSON json = [self packAssetIndex:index];
  
  return json;
}

- (JSON) getCurrentSelection
{
  if (self.m_player == nil)
    return nil;
  
  id<VOOSMPAssetIndex> index = [m_pPlayerAssetSelection getCurrentSelection];
  JSON json = [self packAssetIndex:index];
  
  return json;
}


#pragma mark Player Subtitle
- (VO_OSMP_RETURN_CODE) setSubtitlePath:(NSString *)filePath
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitlePath:filePath];
}

- (VO_OSMP_RETURN_CODE) enableSubtitle:(bool)value
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle enableSubtitle:value];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontColor:(int)color
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontOpacity:(int)alpha
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontSizeScale:(int)scale
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontSizeScale:scale];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundColor:(int)color
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontBackgroundColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundOpacity:(int)alpha
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontBackgroundOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundColor:(int)color
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleWindowBackgroundColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundOpacity:(int)alpha
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleWindowBackgroundOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontItalic:(bool)enable
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontItalic:enable];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBold:(bool)enable
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontBold:enable];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontUnderline:(bool)enable
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontUnderline:enable];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontName:(NSString *)name
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontName:name];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeType:(int)type
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontEdgeType:type];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeColor:(int)color
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontEdgeColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeOpacity:(int)alpha
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle setSubtitleFontEdgeOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) resetSubtitleParameter
{
  if (self.m_player == nil)
    return VO_OSMP_SRC_ERR_SRC_UNINITIALIZE;
  
  return [m_pPlayerSubtitle resetSubtitleParameter];
}

- (VO_OSMP_RETURN_CODE) onVOEvent:(VO_OSMP_CB_EVENT_ID)nID param1:(int)param1 param2:(int)param2 pObj:(void *)pObj
{
  m_pCallbackToWeb(m_pContext, nID, param1, param2);
  return VO_OSMP_ERR_NONE;
}


@end
