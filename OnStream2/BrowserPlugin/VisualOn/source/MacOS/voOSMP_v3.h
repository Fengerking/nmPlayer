#import "VOOSMPType.h"
#import "voLog.h"
#import "VOCommonPlayer.h"
#import "VOCommonPlayerImpl.h"
#import "voJSON.h"
#import "VOOSMPSEIPicTiming.h"
#import "VOOSMPVerificationInfo.h"

#define JSON char*
typedef void (*GLOBALCALLBACKTOWEB)(void* pContext, int nEventId, int nParam1, int nParam2);


@interface CvoOSMP_v3 : NSObject<VOCommonPlayerDelegate>
{
@protected
  CALayer*                        m_pVideoLayer;
  bool                            m_bOpenFailed;
  bool                            m_bStopped;
  int                             m_nOpenFlag;
  char                            m_szOpenUrl[2048];
  GLOBALCALLBACKTOWEB             m_pCallbackToWeb;
  void*                           m_pContext;

  id<VOCommonPlayer>                  player;
  id<VOCommonPlayerControl>           m_pPlayerCtrl;
  id<VOCommonPlayerConfiguration>     m_pPlayerConf;
  id<VOCommonPlayerHTTPConfiguration> m_pPlayerHTTPConf;
  id<VOCommonPlayerAssetSelection>    m_pPlayerAssetSelection;
  id<VOCommonPlayerSubtitle>          m_pPlayerSubtitle;
  
}

// Structure Convert
+ (void)freeJSON:(JSON)json;
- (VOOSMPAnalyticsFilter*) parseAnalyticsFilter:(const char *)filter;
- (JSON) packAnalyticsInfo:(id<VOOSMPAnalyticsInfo>)info;
- (JSON) packSEIPicTiming:(id<VOOSMPSEIPicTiming>)picTiming;
- (VOOSMPVerificationInfo*) parseVerificationInfo:(JSON)info;
- (VOOSMPHTTPProxy*) parseHttpProxy:(JSON)proxy;
- (JSON) packAssetProperty:(id<VOOSMPAssetProperty>)prop;
- (JSON) packAssetIndex:(id<VOOSMPAssetIndex>)index;
- (VOOSMPOpenParam *) parseOpenParam:(JSON)openParam;

// Player Control
- (VO_OSMP_RETURN_CODE) initPlayer:(VO_OSMP_PLAYER_ENGINE)playEngineType initParam:(JSON)initParam;
- (VO_OSMP_RETURN_CODE) setView:(void*) view;
- (VO_OSMP_RETURN_CODE) setDisplayArea:(Rect)rect;

- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType openParam:(JSON)openParam;
- (VO_OSMP_RETURN_CODE) close;
- (VO_OSMP_RETURN_CODE) start;
- (VO_OSMP_RETURN_CODE) pause;
- (VO_OSMP_RETURN_CODE) stop;
- (long) setPosition:(long)msec;
- (long) getPosition;
- (long) getDuration;
- (VO_OSMP_RETURN_CODE) setVolume:(float)value;
- (VO_OSMP_RETURN_CODE) mute;
- (VO_OSMP_RETURN_CODE) unmute;
- (VO_OSMP_RETURN_CODE) suspend:(bool)keepAudio;
- (VO_OSMP_RETURN_CODE) resume;
- (VO_OSMP_RETURN_CODE) setVideoAspectRatio:(VO_OSMP_ASPECT_RATIO)ar;
- (VO_OSMP_RETURN_CODE) redrawVideo;
- (VO_OSMP_STATUS) getPlayerStatus;
- (bool) canBePaused;
- (bool) isLiveStreaming;
- (VO_OSMP_RETURN_CODE) startAnalyticsNotification:(int)interval filter:(const char *)filter;
- (VO_OSMP_RETURN_CODE) stopAnalyticsNotification;
- (JSON) getAnalytics:(const char *)filter;
- (VO_OSMP_RETURN_CODE) setScreenBrightness:(int)brightness;
- (int) getScreenBrightness;
- (VO_OSMP_RETURN_CODE) startSEINotification:(int)interval;
- (VO_OSMP_RETURN_CODE) stopSEINotification;
- (JSON) getSEIInfo:(long long)time flag:(VO_OSMP_SEI_INFO_FLAG)flag;


// Player Configuration
- (VO_OSMP_RETURN_CODE) setLicenseContent:(NSData *)data;
- (VO_OSMP_RETURN_CODE) setLicenseFilePath:(NSString *)path;
- (VO_OSMP_RETURN_CODE) setPreAgreedLicense:(NSString *)str;
- (VO_OSMP_RETURN_CODE) enableDeblock:(bool) value;
- (VO_OSMP_RETURN_CODE) setDeviceCapabilityByFile:(NSString *)filename;
- (VO_OSMP_RETURN_CODE) setInitialBitrate:(int) bitrate;
- (VO_OSMP_PLAYER_ENGINE) getPlayerType;
- (VO_OSMP_RETURN_CODE) getParameter:(int)paramID pObj:(void *)pObj;
- (VO_OSMP_RETURN_CODE) setParameter:(int)paramID pObj:(void *)pObj;
- (VO_OSMP_RETURN_CODE) setZoomMode:(VO_OSMP_ZOOM_MODE)mode rect:(Rect)rect;
- (VO_OSMP_RETURN_CODE) enableLiveStreamingDVRPosition:(bool)enable;
- (long) getMinPosition;
- (long) getMaxPosition;
- (VO_OSMP_RETURN_CODE) setDRMLibrary:(NSString *)libName libApiName:(NSString *)libApiName;
- (VO_OSMP_RETURN_CODE) enableAnalytics:(int)cacheTime;
- (VO_OSMP_RETURN_CODE) setDRMAdapter:(void *)adapter;
- (VO_OSMP_RETURN_CODE) setDRMVerificationInfoWithStructure:(VOOSMPVerificationInfo*)info;
- (VO_OSMP_RETURN_CODE) setDRMVerificationInfo:(JSON)info;
- (NSString *) getDRMUniqueIdentifier;
- (VO_OSMP_RETURN_CODE) setDRMUniqueIdentifier:(NSString *)string;
- (VO_OSMP_RETURN_CODE) setInitialBufferTime:(int)time;
- (VO_OSMP_RETURN_CODE) setAnewBufferingTime:(int)time;
- (VO_OSMP_RETURN_CODE) setMaxBufferTime:(int)time;
- (VO_OSMP_RETURN_CODE) enableAudioStream:(bool) value;
- (VO_OSMP_RETURN_CODE) enableVideoStream:(bool) value;
- (VO_OSMP_RETURN_CODE) enableLowLatencyVideo:(bool) value;
- (VO_OSMP_RETURN_CODE) enableAudioEffect:(bool) value;
- (VO_OSMP_RETURN_CODE) enablePCMOutput:(bool) value;
- (VO_OSMP_RETURN_CODE) setAudioPlaybackSpeed:(float)speed;
- (NSString*) getVersion:(VO_OSMP_MODULE_TYPE)module;
- (VO_OSMP_RETURN_CODE) setPresentationDelay:(int)time;
- (VO_OSMP_RETURN_CODE) enableSEI:(VO_OSMP_SEI_INFO_FLAG)flag;
- (VO_OSMP_RETURN_CODE) setPDConnectionRetryCount:(int)times;
- (VO_OSMP_RETURN_CODE) enableCPUAdaptation:(bool)value;


// Player HTTP Configuration
- (VO_OSMP_RETURN_CODE) setHTTPVerificationInfo:(JSON)info;
- (VO_OSMP_RETURN_CODE) setHTTPHeader:(NSString *)headerName headerValue:(NSString *)headerValue;
- (VO_OSMP_RETURN_CODE) setHTTPProxy:(JSON)proxy;


// Player Asset Selection
- (int) getVideoCount;
- (int) getAudioCount;
- (int) getSubtitleCount;
- (VO_OSMP_RETURN_CODE) selectVideo:(int)index;
- (VO_OSMP_RETURN_CODE) selectAudio:(int)index;
- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index;
- (bool) isVideoAvailable:(int)index;
- (bool) isAudioAvailable:(int)index;
- (bool) isSubtitleAvailable:(int)index;
- (VO_OSMP_RETURN_CODE) commitSelection;
- (VO_OSMP_RETURN_CODE) clearSelection;
- (JSON) getVideoProperty:(int)index;
- (JSON) getAudioProperty:(int)index;
- (JSON) getSubtitleProperty:(int)index;
- (JSON) getPlayingAsset;
- (JSON) getCurrentSelection;


// Player Subtitle
- (VO_OSMP_RETURN_CODE) setSubtitlePath:(NSString *)filePath;
- (VO_OSMP_RETURN_CODE) enableSubtitle:(bool)value;
- (VO_OSMP_RETURN_CODE) setSubtitleFontColor:(int)color;
- (VO_OSMP_RETURN_CODE) setSubtitleFontOpacity:(int)alpha;
- (VO_OSMP_RETURN_CODE) setSubtitleFontSizeScale:(int)scale;
- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundColor:(int)color;
- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundOpacity:(int)alpha;
- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundColor:(int)color;
- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundOpacity:(int)alpha;
- (VO_OSMP_RETURN_CODE) setSubtitleFontItalic:(bool)enable;
- (VO_OSMP_RETURN_CODE) setSubtitleFontBold:(bool)enable;
- (VO_OSMP_RETURN_CODE) setSubtitleFontUnderline:(bool)enable;
- (VO_OSMP_RETURN_CODE) setSubtitleFontName:(NSString *)name;
- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeType:(int)type;
- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeColor:(int)color;
- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeOpacity:(int)alpha;
- (VO_OSMP_RETURN_CODE) resetSubtitleParameter;




- (id)init: (CALayer*)pVideoLayer CallbackToWeb:(GLOBALCALLBACKTOWEB)pCallbackToWeb Context:(void*)pContext;
- (VO_OSMP_RETURN_CODE) onVOEvent:(VO_OSMP_CB_EVENT_ID)nID param1:(int)param1 param2:(int)param2 pObj:(void *)pObj;
- (void)dealloc;

@property (nonatomic, retain) id<VOCommonPlayer> m_player;

@end
