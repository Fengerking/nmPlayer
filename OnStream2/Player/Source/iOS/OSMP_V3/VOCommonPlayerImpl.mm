
#import "VOCommonPlayerImpl.h"
#import "VOCommonPlayerImplement.h"

@interface VOCommonPlayerImpl ()

// Properties that don't need to be seen by the outside world.
@property (nonatomic, retain) id player;
@end

@implementation VOCommonPlayerImpl

@synthesize player = _player;

#pragma mark Control

- (id) init:(VO_OSMP_PLAYER_ENGINE)playerType initParam:(id)initParam initParamFlag:(int)initParamFlag
{
    if (nil != (self = [super init]))
    {
        _player = [[VOCommonPlayerImplement alloc] init:playerType initParam:initParam initParamFlag:initParamFlag];
        
        if (nil == _player) {
            [self release];
            return nil;
        }
    }
    
    return self;
}

- (id) init:(VO_OSMP_PLAYER_ENGINE)playerType initParam:(VOOSMPInitParam *)initParam
{
    if (nil != (self = [super init]))
    {
        _player = [[VOCommonPlayerImplement alloc] init:playerType initParam:initParam];
        
        if (nil == _player) {
            [self release];
            return nil;
        }
    }
    
    return self;
}

- (void) dealloc
{
    self.player = nil;

    [super dealloc];
}

-(VO_OSMP_RETURN_CODE) setView:(void*) view
{
    return [(VOCommonPlayerImplement *)_player setView:view];
}

-(VO_OSMP_RETURN_CODE) setDisplayArea:(Rect)rect
{
    return [(VOCommonPlayerImplement *)_player setDisplayArea:rect];
}

- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType initParam:(VOOSMPInitParam *)initParam initParamFlag:(int)initParamFlag
{
    return [(VOCommonPlayerImplement *)_player open:url flag:flag sourceType:sourceType initParam:initParam initParamFlag:initParamFlag];
}

- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType openParam:(VOOSMPOpenParam *)openParam
{
    return [(VOCommonPlayerImplement *)_player open:url flag:flag sourceType:sourceType openParam:openParam];
}

-(VO_OSMP_RETURN_CODE) close
{
    return [(VOCommonPlayerImplement *)_player close];
}

-(VO_OSMP_RETURN_CODE) start
{
    return [(VOCommonPlayerImplement *)_player start];
}

-(VO_OSMP_RETURN_CODE) pause
{
    return [(VOCommonPlayerImplement *)_player pause];
}

-(VO_OSMP_RETURN_CODE) stop
{
    return [(VOCommonPlayerImplement *)_player stop];
}

-(long) setPosition:(long)msec
{
    return [(VOCommonPlayerImplement *)_player setPosition:msec];
}

-(long) getPosition
{
    return [(VOCommonPlayerImplement *)_player getPosition];
}

-(long) getDuration
{
    return [(VOCommonPlayerImplement *)_player getDuration];
}

-(VO_OSMP_RETURN_CODE) setVolume:(float)value
{
    return [(VOCommonPlayerImplement *)_player setVolume:value];
}

- (VO_OSMP_RETURN_CODE) mute
{
    return [(VOCommonPlayerImplement *)_player mute];
}

- (VO_OSMP_RETURN_CODE) unmute
{
    return [(VOCommonPlayerImplement *)_player unmute];
}

- (VO_OSMP_RETURN_CODE) suspend:(bool)keepAudio
{
    return [(VOCommonPlayerImplement *)_player suspend:keepAudio];
}

-(VO_OSMP_RETURN_CODE) resume
{
    return [(VOCommonPlayerImplement *)_player resume];
}

- (VO_OSMP_RETURN_CODE) setVideoAspectRatio:(VO_OSMP_ASPECT_RATIO)ar
{
    return [(VOCommonPlayerImplement *)_player setVideoAspectRatio:ar];
}

- (VO_OSMP_RETURN_CODE) redrawVideo
{
    return [(VOCommonPlayerImplement *)_player redrawVideo];
}

-(VO_OSMP_STATUS) getPlayerStatus
{
    return [(VOCommonPlayerImplement *)_player getPlayerStatus];
}

-(bool) canBePaused
{
    return [(VOCommonPlayerImplement *)_player canBePaused];
}

- (bool) isLiveStreaming
{
    return [(VOCommonPlayerImplement *)_player isLiveStreaming];
}

- (VO_OSMP_RETURN_CODE) stopAnalyticsNotification
{
    return [(VOCommonPlayerImplement *)_player stopAnalyticsNotification];
}

- (id<VOOSMPAnalyticsInfo>) getAnalytics:(VOOSMPAnalyticsFilter *)filter
{
    return [(VOCommonPlayerImplement *)_player getAnalytics:filter];
}

- (VO_OSMP_RETURN_CODE) setOnEventDelegate:(id <VOCommonPlayerDelegate>)delegate
{
    return [(VOCommonPlayerImplement *)_player setOnEventDelegate:delegate];
}

- (VO_OSMP_RETURN_CODE) setScreenBrightness:(int)brightness
{
    return [(VOCommonPlayerImplement *)_player setScreenBrightness:brightness];
}

- (int) getScreenBrightness
{
    return [(VOCommonPlayerImplement *)_player getScreenBrightness];
}

- (VO_OSMP_RETURN_CODE) startSEINotification:(int)interval
{
    return [(VOCommonPlayerImplement *)_player startSEINotification:interval];
}

- (VO_OSMP_RETURN_CODE) stopSEINotification
{
    return [(VOCommonPlayerImplement *)_player stopSEINotification];
}

- (id) getSEIInfo:(long long)time flag:(VO_OSMP_SEI_INFO_FLAG)flag
{
    return [(VOCommonPlayerImplement *)_player getSEIInfo:time flag:flag];
}

- (id<VOOSMPImageData>) captureVideoImage
{
    return [(VOCommonPlayerImplement *)_player captureVideoImage];
}

- (VO_OSMP_RETURN_CODE) updateSourceURL:(NSString *)url
{
    return [(VOCommonPlayerImplement *)_player updateSourceURL:url];
}

#pragma mark Configuration

- (VO_OSMP_RETURN_CODE) setLicenseContent:(NSData *)data
{
    return [(VOCommonPlayerImplement *)_player setLicenseContent:data];
}

-(VO_OSMP_RETURN_CODE) setLicenseFilePath:(NSString *)path
{
    return [(VOCommonPlayerImplement *)_player setLicenseFilePath:path];
}

-(VO_OSMP_RETURN_CODE) setPreAgreedLicense:(NSString *)str
{
    return [(VOCommonPlayerImplement *)_player setPreAgreedLicense:str];
}

-(VO_OSMP_RETURN_CODE) enableDeblock:(bool)value
{
    return [(VOCommonPlayerImplement *)_player enableDeblock:value];
}

-(VO_OSMP_RETURN_CODE) setDeviceCapabilityByFile:(NSString *)filename
{
    return [(VOCommonPlayerImplement *)_player setDeviceCapabilityByFile:filename];
}

-(VO_OSMP_RETURN_CODE) setInitialBitrate:(int) bitrate
{
    return [(VOCommonPlayerImplement *)_player setInitialBitrate:bitrate];
}

-(VO_OSMP_PLAYER_ENGINE) getPlayerType
{
    return [(VOCommonPlayerImplement *)_player getPlayerType];
}

- (VO_OSMP_RETURN_CODE) getParameter:(int)paramID pObj:(void *)pObj
{
    return [(VOCommonPlayerImplement *)_player getParameter:paramID pObj:pObj];
}

- (VO_OSMP_RETURN_CODE) setParameter:(int)paramID pObj:(void *)pObj
{
    return [(VOCommonPlayerImplement *)_player setParameter:paramID pObj:pObj];
}

- (VO_OSMP_RETURN_CODE) setZoomMode:(VO_OSMP_ZOOM_MODE)mode rect:(Rect)rect
{
    return [(VOCommonPlayerImplement *)_player setZoomMode:mode rect:rect];
}

- (VO_OSMP_RETURN_CODE) enableLiveStreamingDVRPosition:(bool)enable
{
    return [(VOCommonPlayerImplement *)_player enableLiveStreamingDVRPosition:enable];
}

- (long) getMinPosition
{
    return [(VOCommonPlayerImplement *)_player getMinPosition];
}

- (long) getMaxPosition
{
    return [(VOCommonPlayerImplement *)_player getMaxPosition];
}

- (VO_OSMP_RETURN_CODE) setDRMLibrary:(NSString *)libName libApiName:(NSString *)libApiName
{
    return [(VOCommonPlayerImplement *)_player setDRMLibrary:libName libApiName:libApiName];
}

- (VO_OSMP_RETURN_CODE) enableBitrateAdaptation:(bool) value
{
    return VO_OSMP_ERR_IMPLEMENT;//[(VOCommonPlayerImplement *)_player enableBitrateAdaptation:value];
}

- (VO_OSMP_RETURN_CODE) setDRMAdapter:(void *)adapter
{
    return [(VOCommonPlayerImplement *)_player setDRMAdapter:adapter];
}

- (VO_OSMP_RETURN_CODE) setDRMVerificationInfo:(VOOSMPVerificationInfo *)info
{
    return [(VOCommonPlayerImplement *)_player setDRMVerificationInfo:info];
}

- (NSString *) getDRMUniqueIdentifier
{
    return [(VOCommonPlayerImplement *)_player getDRMUniqueIdentifier];
}

- (VO_OSMP_RETURN_CODE) setDRMUniqueIdentifier:(NSString *)string
{
    return [(VOCommonPlayerImplement *)_player setDRMUniqueIdentifier:string];
}

- (NSString *) getDRMUniqueIndentifier
{
    return [(VOCommonPlayerImplement *)_player getDRMUniqueIndentifier];
}

- (VO_OSMP_RETURN_CODE) setDRMUniqueIndentifier:(NSString *)string
{
    return [(VOCommonPlayerImplement *)_player setDRMUniqueIndentifier:string];
}

- (VO_OSMP_RETURN_CODE) setInitialBufferTime:(int)time
{
    return [(VOCommonPlayerImplement *)_player setInitialBufferTime:time];
}

- (VO_OSMP_RETURN_CODE) setAnewBufferingTime:(int)time
{
    return [(VOCommonPlayerImplement *)_player setAnewBufferingTime:time];
}

- (VO_OSMP_RETURN_CODE) setPlaybackBufferingTime:(int)time
{
    return [(VOCommonPlayerImplement *)_player setPlaybackBufferingTime:time];
}

- (VO_OSMP_RETURN_CODE) setMaxBufferTime:(int)time
{
    return [(VOCommonPlayerImplement *)_player setMaxBufferTime:time];
}

- (VO_OSMP_RETURN_CODE) enableAudioStream:(bool) value
{
    return [(VOCommonPlayerImplement *)_player enableAudioStream:value];
}

- (VO_OSMP_RETURN_CODE) enableVideoStream:(bool) value
{
    return [(VOCommonPlayerImplement *)_player enableVideoStream:value];
}

- (VO_OSMP_RETURN_CODE) enableLowLatencyVideo:(bool) value
{
    return [(VOCommonPlayerImplement *)_player enableLowLatencyVideo:value];
}

- (VO_OSMP_RETURN_CODE) enableAudioEffect:(bool) value
{
    return [(VOCommonPlayerImplement *)_player enableAudioEffect:value];
}

- (VO_OSMP_RETURN_CODE) enablePCMOutput:(bool) value
{
    return [(VOCommonPlayerImplement *)_player enablePCMOutput:value];
}

- (VO_OSMP_RETURN_CODE) setAudioPlaybackSpeed:(float)speed
{
    return [(VOCommonPlayerImplement *)_player setAudioPlaybackSpeed:speed];
}

- (NSString*) getVersion:(VO_OSMP_MODULE_TYPE)module
{
    return [(VOCommonPlayerImplement *)_player getVersion:module];
}

- (VO_OSMP_RETURN_CODE) setPresentationDelay:(int)time
{
    return [(VOCommonPlayerImplement *)_player setPresentationDelay:time];
}

- (VO_OSMP_RETURN_CODE) enableSEI:(VO_OSMP_SEI_INFO_FLAG)flag
{
    return [(VOCommonPlayerImplement *)_player enableSEI:flag];
}

- (VO_OSMP_RETURN_CODE) setPDConnectionRetryCount:(int)times
{
    return [(VOCommonPlayerImplement *)_player setPDConnectionRetryCount:times];
}

- (NSArray *) getVideoDecodingBitrate
{
    return [(VOCommonPlayerImplement *)_player getVideoDecodingBitrate];
}

- (NSArray *) getAudioDecodingBitrate
{
    return [(VOCommonPlayerImplement *)_player getAudioDecodingBitrate];
}

- (VO_OSMP_RETURN_CODE) enableCPUAdaptation:(bool)value
{
    return [(VOCommonPlayerImplement *)_player enableCPUAdaptation:value];
}

- (VO_OSMP_RETURN_CODE) setBitrateThreshold:(int)upper lower:(int)lower
{
    return [(VOCommonPlayerImplement *)_player setBitrateThreshold:upper lower:lower];
}

- (VO_OSMP_RETURN_CODE) setSegmentDownloadRetryCount:(int) times
{
    return [(VOCommonPlayerImplement *)_player setSegmentDownloadRetryCount:times];
}

- (VO_OSMP_RETURN_CODE) enableDolbyLibrary:(bool) value
{
    return [(VOCommonPlayerImplement*)_player enableDolbyLibrary:value];
}



#pragma mark subtitle
- (VO_OSMP_RETURN_CODE) setSubtitlePath:(NSString *)filePath
{
    return [(VOCommonPlayerImplement *)_player setSubtitlePath:filePath];
}

- (VO_OSMP_RETURN_CODE) enableSubtitle:(bool)value
{
    return [(VOCommonPlayerImplement *)_player enableSubtitle:value];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontColor:(int)color
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontOpacity:(int)alpha
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontSizeScale:(int)scale
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontSizeScale:scale];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundColor:(int)color
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontBackgroundColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundOpacity:(int)alpha
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontBackgroundOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundColor:(int)color
{
    return [(VOCommonPlayerImplement *)_player setSubtitleWindowBackgroundColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundOpacity:(int)alpha
{
    return [(VOCommonPlayerImplement *)_player setSubtitleWindowBackgroundOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontItalic:(bool)enable
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontItalic:enable];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBold:(bool)enable
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontBold:enable];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontUnderline:(bool)enable
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontUnderline:enable];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontName:(NSString *)name
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontName:name];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeType:(int)type
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontEdgeType:type];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeColor:(int)color
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontEdgeColor:color];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeOpacity:(int)alpha
{
    return [(VOCommonPlayerImplement *)_player setSubtitleFontEdgeOpacity:alpha];
}

- (VO_OSMP_RETURN_CODE) resetSubtitleParameter
{
    return [(VOCommonPlayerImplement *)_player resetSubtitleParameter];
}

- (VO_OSMP_RETURN_CODE) previewSubtitle:(NSString *)sampleText view:(id)view
{
    return [(VOCommonPlayerImplement *)_player previewSubtitle:sampleText view:view];
}

#pragma mark Asset selection
- (int) getVideoCount
{
    return [(VOCommonPlayerImplement *)_player getVideoCount];
}

- (int) getAudioCount
{
    return [(VOCommonPlayerImplement *)_player getAudioCount];
}

- (int) getSubtitleCount
{
    return [(VOCommonPlayerImplement *)_player getSubtitleCount];
}

- (VO_OSMP_RETURN_CODE) selectVideo:(int)index
{
    return [(VOCommonPlayerImplement *)_player selectVideo:index];
}

- (VO_OSMP_RETURN_CODE) selectAudio:(int)index
{
    return [(VOCommonPlayerImplement *)_player selectAudio:index];
}

- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index
{
    return [(VOCommonPlayerImplement *)_player selectSubtitle:index];
}

- (bool) isVideoAvailable:(int)index
{
    return [(VOCommonPlayerImplement *)_player isVideoAvailable:index];
}

- (bool) isAudioAvailable:(int)index
{
    return [(VOCommonPlayerImplement *)_player isAudioAvailable:index];
}

- (bool) isSubtitleAvailable:(int)index
{
    return [(VOCommonPlayerImplement *)_player isSubtitleAvailable:index];
}

- (VO_OSMP_RETURN_CODE) commitSelection
{
    return [(VOCommonPlayerImplement *)_player commitSelection];
}

- (VO_OSMP_RETURN_CODE) clearSelection
{
    return [(VOCommonPlayerImplement *)_player clearSelection];
}

- (id<VOOSMPAssetProperty>) getVideoProperty:(int)index
{
    return [(VOCommonPlayerImplement *)_player getVideoProperty:index];
}

- (id<VOOSMPAssetProperty>) getAudioProperty:(int)index
{
    return [(VOCommonPlayerImplement *)_player getAudioProperty:index];
}

- (id<VOOSMPAssetProperty>) getSubtitleProperty:(int)index
{
    return [(VOCommonPlayerImplement *)_player getSubtitleProperty:index];
}

- (id<VOOSMPAssetIndex>) getPlayingAsset
{
    return [(VOCommonPlayerImplement *)_player getPlayingAsset];
}

- (id<VOOSMPAssetIndex>) getCurrentSelection
{
    return [(VOCommonPlayerImplement *)_player getCurrentSelection];
}

- (VO_OSMP_RETURN_CODE) setDefaultAudioLanguage:(NSString*)type
{
    return [(VOCommonPlayerImplement *)_player setDefaultAudioLanguage:type];
}

- (VO_OSMP_RETURN_CODE) setDefaultSubtitleLanguage:(NSString*)type
{
    return [(VOCommonPlayerImplement *)_player setDefaultSubtitleLanguage:type];
}

#pragma mark RTSP
- (VO_OSMP_RETURN_CODE) setRTSPConnectionType:(VO_OSMP_RTSP_CONNECTION_TYPE)type
{
    return [(VOCommonPlayerImplement *)_player setRTSPConnectionType:type];
}

- (VO_OSMP_RETURN_CODE) setRTSPConnectionPort:(VOOSMPRTSPPort *)portNum
{
    return [(VOCommonPlayerImplement *)_player setRTSPConnectionPort:portNum];
}

- (id<VOOSMPRTSPStatistics>) getRTSPStatistics
{
    return [(VOCommonPlayerImplement *)_player getRTSPStatistics];
}

- (VO_OSMP_RETURN_CODE) enableRTSPOverHTTP:(bool)enable
{
    return [(VOCommonPlayerImplement *)_player enableRTSPOverHTTP:enable];
}

- (VO_OSMP_RETURN_CODE) setRTSPOverHTTPConnectionPort:(int)portNum
{
    return [(VOCommonPlayerImplement *)_player setRTSPOverHTTPConnectionPort:portNum];
}

#pragma mark HTTP
- (VO_OSMP_RETURN_CODE) setHTTPVerificationInfo:(VOOSMPVerificationInfo *)info
{
    return [(VOCommonPlayerImplement *)_player setHTTPVerificationInfo:info];
}

- (VO_OSMP_RETURN_CODE) setHTTPHeader:(NSString *)headerName headerValue:(NSString *)headerValue
{
    return [(VOCommonPlayerImplement *)_player setHTTPHeader:headerName headerValue:headerValue];
}

- (VO_OSMP_RETURN_CODE) setHTTPProxy:(VOOSMPHTTPProxy *)proxy
{
    return [(VOCommonPlayerImplement *)_player setHTTPProxy:proxy];
}

- (VO_OSMP_RETURN_CODE) setHTTPRetryTimeout:(int)time
{
    return [(VOCommonPlayerImplement *)_player setHTTPRetryTimeout:time];
}

#pragma mark Analytics
- (VO_OSMP_RETURN_CODE) enableAnalytics:(int)cacheTime
{
    return [(VOCommonPlayerImplement *)_player enableAnalytics:cacheTime];
}

- (VO_OSMP_RETURN_CODE) startAnalyticsNotification:(int)interval filter:(VOOSMPAnalyticsFilter *)filter
{
    return [(VOCommonPlayerImplement *)_player startAnalyticsNotification:interval filter:filter];
}

#pragma mark device info
- (int) getNumberOfCores
{
    return [(VOCommonPlayerImplement *)_player getNumberOfCores];
}

- (bool) hasNeon
{
    return [(VOCommonPlayerImplement *)_player hasNeon];
}

- (long) getMaxCPUFrequency
{
    return [(VOCommonPlayerImplement *)_player getMaxCPUFrequency];
}

- (VO_OSMP_DEVICE_INFO_OS_TYPE) getOSType
{
    return [(VOCommonPlayerImplement *)_player getOSType];
}

- (NSString *) getOSVersion
{
    return [(VOCommonPlayerImplement *)_player getOSVersion];
}

- (NSString *) getDeviceModel
{
    return [(VOCommonPlayerImplement *)_player getDeviceModel];
}

@end
