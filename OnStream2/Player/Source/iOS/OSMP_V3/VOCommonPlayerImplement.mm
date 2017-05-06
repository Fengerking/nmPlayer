
#ifdef _IOS
#import <MediaPlayer/MediaPlayer.h>
#import <mach/machine.h>
#import <UIKit/UIKit.h>
#endif

#import "VOCommonPlayerImplement.h"

#import "VOOSMPAnalyticsInfoImpl.h"
#import "VOCommonPlayerDelegate.h"
#import "VOOSMPAssetInfo.h"
#import "VOOSMPChunkInfoImpl.h"
#import "VOOSMPRTSPStatisticsImpl.h"
#import "voHalInfo.h"
#import "VOOSMPPCMBufferImpl.h"
#import "VOOSMPHTTPDownloadFailureImpl.h"
#import "VOOSMPSEIPicTimingImpl.h"
#import "VOOSMPImageDataImpl.h"
#import "voOSEvent.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef enum {
    VO_SELECT_VIDEO,
    VO_SELECT_AUDIO,
    VO_SELECT_SUBTITLE,
}VO_SELECT_TYPE;

const static float VOLUME_MAX = 1.0;
const static int VOLUME_SCALE = 100;

@interface VOCommonPlayerImplement ()

// Properties that don't need to be seen by the outside world.
@property (nonatomic, retain) NSTimer *timer;

- (void) onMainThreadVOEvent:(voOSEvent *)event;
- (VO_OSMP_RETURN_CODE) onVOEventlistener:(VO_OSMP_CB_EVENT_ID)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
- (VO_OSMP_RETURN_CODE) onVOSyncEventlistener:(unsigned int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2;
- (id<VOOSMPAssetProperty>) getTrackProperty:(int)index type:(VO_SELECT_TYPE)type;
- (void) createTimer:(float)interval;
- (VO_OSMP_RETURN_CODE) updateSubtitleSetting;
- (VO_OSMP_RETURN_CODE) setVerificationInfo:(VOOSMPVerificationInfo *)info paramID:(int)paramID;
- (VO_OSMP_RETURN_CODE) enableByID:(int) nID value:(bool)value;
@end

@implementation VOCommonPlayerImplement

@synthesize timer = _timer;

VO_OSMP_RETURN_CODE voEventCallback_NewMP_ns(void * pUserData, VO_OSMP_CB_EVENT_ID nID, void * pParam1, void * pParam2)
{
    if (pUserData == nil)
    {
        return VO_OSMP_ERR_POINTER;
    }
    
    VOCommonPlayerImplement * pSelf = (VOCommonPlayerImplement *)pUserData;
    
    VO_OSMP_RETURN_CODE result = [pSelf onVOEventlistener:nID withParam1:pParam1 withParam2:pParam2];
    return result;
}

int voEventOnRequest_NewMP_ns(void * pUserData, int nID, void * pParam1, void * pParam2)
{
    if (pUserData == nil)
    {
        return VOOSMP_ERR_Pointer;
    }
    
    VOCommonPlayerImplement * pSelf = (VOCommonPlayerImplement *)pUserData;

    VO_OSMP_RETURN_CODE result = [pSelf onVOSyncEventlistener:nID withParam1:pParam1 withParam2:pParam2];
    
    return result;
}

- (VO_OSMP_RETURN_CODE) onVOSyncEventlistener:(unsigned int)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2
{
    if (nil == _playerDelegate) {
        return VO_OSMP_ERR_NONE;
    }
    
    if (![_playerDelegate respondsToSelector:@selector(onVOSyncEvent:param1:param2:pObj:)]) {
        return VO_OSMP_ERR_NONE;
    }
    
    VO_OSMP_RETURN_CODE nRet = VO_OSMP_ERR_NONE;
    int nParam1 = 0;
    int nParam2 = 0;
    void* pObj = NULL;
    VO_OSMP_CB_SYNC_EVENT_ID nEventID = (VO_OSMP_CB_SYNC_EVENT_ID)nID;;
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    if (VOOSMP_SRC_CB_Authentication_Request == nID) {
        
        if (NULL == pParam1) {
            [pool release];
            return VO_OSMP_ERR_POINTER;
        }
        
        VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE type = *(VOOSMP_SRC_AUTHENTICATION_REQUEST_TYPE *)pParam1;
        if (VOOSMP_SRC_AUTHENTICATION_DRM_REQUEST_SERVER_INFO == type) {
            nEventID = VO_OSMP_SRC_CB_SYNC_AUTHENTICATION_DRM_SERVER_INFO;
        }
        else {
            [pool release];
            return VO_OSMP_ERR_NONE;
        }
    }
    else if (VOOSMP_SRC_CB_IO_HTTP_Start_Download == nID) {
        if (NULL != pParam1) {
            pObj = [NSString stringWithFormat:@"%s", (unsigned char*)pParam1];
        }
    }
    else if (VOOSMP_SRC_CB_IO_HTTP_Download_Failed == nID) {
        
        VOOSMPHTTPDownloadFailureImpl* info = [VOOSMPHTTPDownloadFailureImpl alloc];
        info = [[info init] autorelease];
        
        if (NULL != pParam1) {
            info.URL = [NSString stringWithFormat:@"%s", (unsigned char*)pParam1];
        }
        
        if (NULL != pParam2) {
            VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION *description = (VOOSMP_SRC_IO_FAILED_REASON_DESCRIPTION *)pParam2;
            
            info.reason = (VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON)(description->reason);
            
            if ((NULL != description->pszResponse) && (0 < description->nResponseSize)) {
                info.response = [NSString stringWithFormat:@"%s", description->pszResponse];
            }
        }
        
        pObj = info;
    }
    else {
        nEventID = (VO_OSMP_CB_SYNC_EVENT_ID)nID;
    }
    
    nRet = [_playerDelegate onVOSyncEvent:nEventID param1:nParam1 param2:nParam2 pObj:pObj];
    
    [pool release];
    
    return nRet;
}

- (VO_OSMP_RETURN_CODE) onVOEventlistener:(VO_OSMP_CB_EVENT_ID)nID withParam1:(void*)pParam1 withParam2:(void*)pParam2
{
    if (nil == _playerDelegate) {
        return VO_OSMP_ERR_NONE;
    }
    
    VO_OSMP_RETURN_CODE nRet = VO_OSMP_ERR_NONE;
    int nParam1 = 0;
    int nParam2 = 0;
    id pObj = NULL;
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    if (VOOSMP_CB_MediaTypeChanged == nID) {
        nID = VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO;
        nParam1 = VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE;
        nParam2 = *(int *)pParam1;
    }
    else if ((VO_OSMP_CB_SRC_BUFFER_TIME  == nID)
            || (VO_OSMP_CB_VIDEO_SIZE_CHANGED == nID)
            || (VO_OSMP_SRC_CB_SEEK_COMPLETE == nID)
            ) {
        
        if (NULL != pParam1) {
            nParam1 = *(int *)pParam1;
        }
        if (NULL != pParam2) {
            nParam2 = *(int *)pParam2;
        }
    }
    else if ((VO_OSMP_CB_VIDEO_ASPECT_RATIO == nID)
             || (VO_OSMP_CB_DEBLOCK == nID)
             || (VO_OSMP_CB_HW_DECODER_STATUS == nID)
             || (VO_OSMP_CB_BLUETOOTHHANDSET_CONNECTION == nID)
             || (VO_OSMP_SRC_CB_DOWNLOAD_STATUS == nID)
             || (VO_OSMP_SRC_CB_BA_HAPPENED == nID)
             || (VO_OSMP_SRC_CB_OPEN_FINISHED == nID)
             || (VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR == nID)
             || (VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING == nID)
             || (VO_OSMP_SRC_CB_RTSP_ERROR == nID)
             || (VO_OSMP_SRC_CB_PD_DOWNLOAD_POSITION == nID)
             || (VO_OSMP_SRC_CB_PD_BUFFERING_PERCENT == nID)
             ) {
        
        if (NULL != pParam1) {
            nParam1 = *(int *)pParam1;
        }
    }
    else if (VO_OSMP_CB_AUTHENTICATION_RESPONSE == nID) {
                 
        if (NULL != pParam1) {
            pObj = [[[NSString alloc] initWithFormat:@"%s",(char *)pParam1] autorelease];
        }
    }
    else if (VO_OSMP_CB_ANALYTICS_INFO == nID) {
        if (NULL != pParam1) {
            pObj = (id<VOOSMPAnalyticsInfo>)pParam1;
        }
    }
    else if (VO_OSMP_SRC_CB_CUSTOMER_TAG == nID) {
        if (NULL != pParam1) {
            nParam1 = *(int *)pParam1;
            
            if ((VO_OSMP_SRC_CUSTOMERTAGID_TIMEDTAG == nParam1) && (NULL != pParam2)) {
                int nValue = *(int *)pParam2;
                VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG *timeTag = (VOOSMP_SRC_CUSTOMERTAG_TIMEDTAG *)nValue;
                nParam2 = timeTag->ullTimeStamp;
                pObj = [NSData dataWithBytes:timeTag->pData length:timeTag->uSize];
            }
        }
    }
    else if (VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO == nID) {
        if (NULL != pParam1) {
            nParam1 = *(int *)pParam1;
            
            if (NULL != pParam2) {
                switch (nParam1) {
                    case VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_BITRATE_CHANGE:
                    case VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_MEDIATYPE_CHANGE:
                    case VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_PROGRAM_TYPE:
                        nParam2 = *(int *)pParam2;
                        break;
                    case VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_BEGINDOWNLOAD:
                    case VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DROPPED:
                    case VO_OSMP_SRC_ADAPTIVE_STREAMING_INFO_EVENT_CHUNK_DOWNLOADOK:
                    {
                        VOOSMP_SRC_CHUNK_INFO * chunkInfo = (VOOSMP_SRC_CHUNK_INFO *)pParam2;
                        //pObj = [[VOOSMPChunkInfoImpl alloc] init:chunkInfo];
                        VOOSMPChunkInfoImpl * info = [VOOSMPChunkInfoImpl alloc];
                        pObj = [[info init:chunkInfo] autorelease];
                    }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else if (VO_OSMP_CB_PCM_OUTPUT == nID) {
        if (NULL != pParam1) {
            VOOSMPPCMBufferImpl *info = [VOOSMPPCMBufferImpl alloc];
            pObj = [[info init:(VOOSMP_PCMBUFFER *)pParam1] autorelease];
        }
    }
    else if (VO_OSMP_CB_SEI_INFO == nID) {
        if (NULL != pParam1) {
            VOOSMP_SEI_INFO *pSEI = (VOOSMP_SEI_INFO *)pParam1;
            nParam1 = pSEI->nFlag;
            
            if(pSEI->nFlag == VOOSMP_FLAG_SEI_PIC_TIMING)
            {
                VOOSMPSEIPicTimingImpl* info = [VOOSMPSEIPicTimingImpl alloc];
                pObj = [[info init:pSEI] autorelease];
            }
            else if(pSEI->nFlag == VOOSMP_FLAG_SEI_USER_DATA_UNREGISTERED)
            {
                VOLOGI("Recieve VO_OSMP_SEI_INFO_USER_DATA_UNREGISTERED");
                VOOSMPSEIUserDataUnregisteredImpl* info = [VOOSMPSEIUserDataUnregisteredImpl alloc];
                pObj = [[info init:(VOOSMP_SEI_USER_DATA_UNREGISTERED*)pSEI->pInfo] autorelease];
            }
        }
    }
    
    switch (nID) {
        case VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_INFO:
        case VO_OSMP_SRC_CB_ADAPTIVE_STREAMING_ERROR:
        case VO_OSMP_SRC_CB_ADAPTIVE_STREAM_WARNING:
            // All onVOEvent will be post in sub thread
            // break;
        default:
        {
            if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
                voOSEvent *pEvent = [[voOSEvent alloc] init:nID param1:nParam1 param2:nParam2 obj:pObj];
                [self performSelectorOnMainThread:@selector(onMainThreadVOEvent:) withObject:pEvent waitUntilDone:NO];
                [pEvent release];
                
                [pool release];
                return VO_OSMP_ERR_NONE;
            }
        }
            break;
    }
    
    nRet = [_playerDelegate onVOEvent:(VO_OSMP_CB_EVENT_ID)nID param1:nParam1 param2:nParam2 pObj:pObj];
    
    [pool release];
    
    return nRet;
}

- (void) onMainThreadVOEvent:(voOSEvent *)event
{
    if ((nil == _playerDelegate) || (nil == event)) {
        return;
    }
    
    int nID = event.eventID;
    int nParam1 = event.param1;
    int nParam2 = event.param2;
    id pObj = event.obj;
    
    [_playerDelegate onVOEvent:(VO_OSMP_CB_EVENT_ID)nID param1:nParam1 param2:nParam2 pObj:pObj];
}

#pragma mark Control
- (id) init:(VO_OSMP_PLAYER_ENGINE)playerType initParam:(id)initParam initParamFlag:(int)initParamFlag
{
    if (nil != (self = [super init]))
    {
        _enableDVRPosition = true;
        
        _player = new voOSMediaPlayer();
        
        if (NULL == _player) {
            [self release];
            return nil;
        }
        
        VOOSMP_INIT_PARAM cParam;
        memset(&cParam, 0, sizeof(cParam));
        
        if (VOOSMP_ERR_None != _player->Init(playerType, &cParam, initParamFlag)) {
            [self release];
            return nil;
        }
        
        _volumeValue = VOLUME_MAX * VOLUME_SCALE;
        
        _playerDelegate = nil;
        
        self.timer = nil;
        
        memset(&_subtitleSetting, 0, sizeof(_subtitleSetting));
        
        VOOSMP_LISTENERINFO _listenerInfo;
        memset(&_listenerInfo, 0, sizeof(_listenerInfo));
        _listenerInfo.pListener = (VOOSMPListener)voEventCallback_NewMP_ns;
        _listenerInfo.pUserData = self;
        _player->SetParam(VOOSMP_PID_LISTENER, &_listenerInfo);
        
        VOOSMP_LISTENERINFO cOnRequestInfo;
        memset(&cOnRequestInfo, 0, sizeof(cOnRequestInfo));
        cOnRequestInfo.pListener = voEventOnRequest_NewMP_ns;
        cOnRequestInfo.pUserData = self;
        _player->SetParam(VOOSMP_PID_ONREQUEST_LISTENER, &cOnRequestInfo);
        
        [self enableByID:VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE value:true];
        [self enableByID:VOOSMP_PID_AUDIO_EFFECT_ENABLE value:false];
    }
    
    return self;
}

- (id) init:(VO_OSMP_PLAYER_ENGINE)playEngineType initParam:(VOOSMPInitParam *)initParam
{
    if (nil != (self = [super init]))
    {
        _enableDVRPosition = true;
        
        _player = new voOSMediaPlayer();
        
        if (NULL == _player) {
            [self release];
            return nil;
        }
        
        int initParamFlag = VOOSMP_FLAG_INIT_NOUSE;
        
        VOOSMP_INIT_PARAM cParam;
        memset(&cParam, 0, sizeof(cParam));
        
        if(nil != initParam)
        {
            NSString* path = [initParam getLibraryPath];
            
            if (path)
            {
                initParamFlag |= VOOSMP_FLAG_INIT_WORKING_PATH;
                cParam.pWorkingPath = (void*)[path UTF8String];
            }
        }
        
        if (VOOSMP_ERR_None != _player->Init(playEngineType, &cParam, initParamFlag)) {
            [self release];
            return nil;
        }
        
        _volumeValue = VOLUME_MAX * VOLUME_SCALE;
        
        _playerDelegate = nil;
        
        self.timer = nil;
        
        memset(&_subtitleSetting, 0, sizeof(_subtitleSetting));

        VOOSMP_LISTENERINFO _listenerInfo;
        memset(&_listenerInfo, 0, sizeof(_listenerInfo));
        _listenerInfo.pListener = (VOOSMPListener)voEventCallback_NewMP_ns;
        _listenerInfo.pUserData = self;
        _player->SetParam(VOOSMP_PID_LISTENER, &_listenerInfo);
        
        VOOSMP_LISTENERINFO cOnRequestInfo;
        memset(&cOnRequestInfo, 0, sizeof(cOnRequestInfo));
        cOnRequestInfo.pListener = voEventOnRequest_NewMP_ns;
        cOnRequestInfo.pUserData = self;
        _player->SetParam(VOOSMP_PID_ONREQUEST_LISTENER, &cOnRequestInfo);
        
        [self enableByID:VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE value:true];
        [self enableByID:VOOSMP_PID_AUDIO_EFFECT_ENABLE value:false];
    }
    
    return self;
}

- (void) dealloc
{
    [self.timer invalidate];
	self.timer = nil;
    
    if (NULL != _player) {
        delete _player;
        _player = NULL;
    }

    [super dealloc];
}

-(VO_OSMP_RETURN_CODE) setView:(void*) view
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetView(view));
}

-(VO_OSMP_RETURN_CODE) setDisplayArea:(Rect)rect
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    VOOSMP_RECT cDraw;
    cDraw.nBottom = rect.bottom;
    cDraw.nLeft = rect.left;
    cDraw.nRight = rect.right;
    cDraw.nTop = rect.top;
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_DRAW_RECT, &cDraw));
}

- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType initParam:(VOOSMPInitParam *)initParam initParamFlag:(int)initParamFlag
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == url) {
        return VO_OSMP_ERR_POINTER;
    }
    
    int nFlag = VOOSMP_FLAG_SOURCE_URL | VOOSMP_FLAG_SOURCE_READBUFFER | flag;
    
    VOOSMP_INIT_PARAM cParam;
    memset(&cParam, 0, sizeof(cParam));
    
    if (nil != initParam) {
        if (0 != (VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE & initParamFlag)) {
            cParam.llFileSize = initParam.fileSize;
        }
    }
    
    char szModule[] = "voSourceIO";
    char szAPI[] = "voGetSourceIOAPI";
    
    int newSourceType = sourceType;
    
    if ([url hasPrefix:@"file://localhost/"] || [url hasPrefix:@"/var"]) {
        cParam.pszIOFileName = szModule;
        cParam.pszIOApiName = szAPI;
        
        initParamFlag = initParamFlag | VOOSMP_FLAG_INIT_IO_FILE_NAME;
        initParamFlag = initParamFlag | VOOSMP_FLAG_INIT_IO_API_NAME;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Open((void *)[url UTF8String], nFlag, newSourceType, &cParam, initParamFlag));
}

- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(VO_OSMP_SRC_FLAG)flag sourceType:(VO_OSMP_SRC_FORMAT)sourceType openParam:(VOOSMPOpenParam *)openParam
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == url) {
        return VO_OSMP_ERR_POINTER;
    }
    
    int openParamFlag = VOOSMP_FLAG_INIT_NOUSE;
    int nFlag = VOOSMP_FLAG_SOURCE_URL | VOOSMP_FLAG_SOURCE_READBUFFER | flag;
    
    VOOSMP_INIT_PARAM cParam;
    memset(&cParam, 0, sizeof(cParam));
    
    if (nil != openParam) {
        if (0 < openParam.fileSize) {
            cParam.llFileSize = openParam.fileSize;
            openParamFlag = openParamFlag | VOOSMP_FLAG_INIT_ACTUAL_FILE_SIZE;
        }
    }
    
    char szModule[] = "voSourceIO";
    char szAPI[] = "voGetSourceIOAPI";
    
    int newSourceType = sourceType;
    
    if ([url hasPrefix:@"file://localhost/"] || [url hasPrefix:@"/var"]) {
        cParam.pszIOFileName = szModule;
        cParam.pszIOApiName = szAPI;
        
        openParamFlag = openParamFlag | VOOSMP_FLAG_INIT_IO_FILE_NAME;
        openParamFlag = openParamFlag | VOOSMP_FLAG_INIT_IO_API_NAME;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Open((void *)[url UTF8String], nFlag, newSourceType, &cParam, openParamFlag));
}

-(VO_OSMP_RETURN_CODE) close
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Close());
}

-(VO_OSMP_RETURN_CODE) start
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Run());
}

-(VO_OSMP_RETURN_CODE) pause
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Pause());
}

-(VO_OSMP_RETURN_CODE) stop
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    [self.timer invalidate];
	self.timer = nil;
    
    return (VO_OSMP_RETURN_CODE)(_player->Stop());
}

-(long) setPosition:(long)msec
{
    if (NULL == _player) {
        return -1;
    }
    
    //Jim tag: DX requirement
    if(_enableDVRPosition && _player->IsLive())
    {
        VOOSMP_SRC_DVRINFO cInfo;
        memset(&cInfo, 0, sizeof(cInfo));
        
        int nRet = _player->GetParam(VOOSMP_SRC_PID_DVRINFO, &cInfo);
        if (VOOSMP_ERR_None != nRet) {
            return -1;
        }
        
        long adjustPos = msec + cInfo.uLiveTime;
        VOLOGI("[DVR]set pos %ld, pos %ld, max %lld", adjustPos, msec, cInfo.uLiveTime);
        return _player->SetPos(adjustPos);
    }
    
    return _player->SetPos(msec);
}

-(long) getPosition
{
    if (NULL == _player) {
        return -1;
    }
    
    if (_enableDVRPosition && (_player->IsLive()))
    {
        VOOSMP_SRC_DVRINFO cInfo;
        memset(&cInfo, 0, sizeof(cInfo));
        
        int nRet = _player->GetParam(VOOSMP_SRC_PID_DVRINFO, &cInfo);
        if (VOOSMP_ERR_None == nRet) {
            return _player->GetPos() - cInfo.uLiveTime;
        }
    }
    
    return _player->GetPos();
}

-(long) getDuration
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    long long duration = 0;
    if (VO_OSMP_ERR_NONE != _player->GetDuration(&duration)) {
        return -1;
    }
    
    return duration;
}

-(VO_OSMP_RETURN_CODE) setVolume:(float)value
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    // Don't need to protect at this layer. float is not precise
//    if ((0 > value) || (VOLUME_MAX < value)) {
//        return VO_OSMP_ERR_UNKNOWN;
//    }
    
//#ifdef _IOS
//    [[MPMusicPlayerController applicationMusicPlayer] setVolume:value];
//#endif
    
    _volumeValue = value * VOLUME_SCALE;
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_AUDIO_VOLUME, &_volumeValue));
}

- (VO_OSMP_RETURN_CODE) mute
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int iValue = 0;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_AUDIO_VOLUME, &iValue));
}

- (VO_OSMP_RETURN_CODE) unmute
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int iValue = _volumeValue;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_AUDIO_VOLUME, &iValue));
}

- (VO_OSMP_RETURN_CODE) suspend:(bool)keepAudio
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Suspend(keepAudio));
}

-(VO_OSMP_RETURN_CODE) resume
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->Resume());
}

- (VO_OSMP_RETURN_CODE) setVideoAspectRatio:(VO_OSMP_ASPECT_RATIO)ar
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_VIDEO_ASPECT_RATIO, &ar));
}

- (VO_OSMP_RETURN_CODE) redrawVideo
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_VIDEO_REDRAW, NULL));
}

-(VO_OSMP_STATUS) getPlayerStatus
{
    if (NULL == _player) {
        return VO_OSMP_STATUS_INITIALIZING;
    }
    
    VOOSMP_STATUS status = VOOSMP_STATUS_INIT;
    if (VOOSMP_ERR_None != (_player->GetParam(VOOSMP_PID_STATUS, &status))) {
        status = VOOSMP_STATUS_INIT;
    }
    
    return (VO_OSMP_STATUS)status;
}

-(bool) canBePaused
{
    return true;
//    if (NULL == _player) {
//        return false;
//    }
//    
//    if (0 < [self getDuration]) {
//        return true;
//    }
//        
//    return false;
}

- (bool) isLiveStreaming
{
    if (NULL == _player) {
        return false;
    }
    
    return _player->IsLive();
}

- (VO_OSMP_RETURN_CODE) stopAnalyticsNotification
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    [self.timer invalidate];
	self.timer = nil;
    
    int iOn = 0;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF, &iOn));
}

- (id<VOOSMPAnalyticsInfo>) getAnalytics:(VOOSMPAnalyticsFilter *)filter
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMP_PERFORMANCE_DATA cData;
    memset(&cData, 0, sizeof(cData));
    
    if (nil == filter) {
        cData.nLastTime = filter.lastTime;
        cData.nSourceTimeNum = filter.sourceTime;
        cData.nCodecTimeNum = filter.codecTime;
        cData.nRenderTimeNum = filter.renderTime;
        cData.nJitterNum = filter.jitterTime;
    }
    
    int nRet = (_player->GetParam(VOOSMP_PID_VIDEO_PERFORMANCE_OPTION, &cData));
    
    if (VOOSMP_ERR_None != nRet) {
        return nil;
    }
    
    VOOSMPAnalyticsInfoImpl* obj = [[[VOOSMPAnalyticsInfoImpl alloc] init:&cData] autorelease];
    
    return obj;
}

- (VO_OSMP_RETURN_CODE) setOnEventDelegate:(id <VOCommonPlayerDelegate>)delegate
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _playerDelegate = delegate;
    return VO_OSMP_ERR_NONE;
}

- (VO_OSMP_RETURN_CODE) setScreenBrightness:(int)brightness
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_SCREEN_BRIGHTNESS_VALUE, &brightness));
}

- (int) getScreenBrightness
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    int brightness = 0;
    
    if (VOOSMP_ERR_None != _player->GetParam(VOOSMP_PID_SCREEN_BRIGHTNESS_VALUE, &brightness))
    {
        brightness = -1;
    }
    
    return brightness;
}

- (VO_OSMP_RETURN_CODE) startSEINotification:(int)interval
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_SEI_EVENT_INTERVAL, &interval));
}

- (VO_OSMP_RETURN_CODE) stopSEINotification
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int interval = -1;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_SEI_EVENT_INTERVAL, &interval));
}

- (id) getSEIInfo:(long long)time flag:(VO_OSMP_SEI_INFO_FLAG)flag
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMP_SEI_INFO cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    cInfo.llTime = time;
    cInfo.nFlag = flag;
    if (VO_OSMP_FLAG_SEI_MAX == flag) {
        cInfo.nFlag = VOOSMP_FLAG_SEI_MAX;
    }
    
    int nRet = _player->GetParam(VOOSMP_PID_RETRIEVE_SEI_INFO, &cInfo);
    if (VOOSMP_ERR_None != nRet) {
        VOLOGE("getSEIInfo Info Err:%d", nRet);
        return nil;
    }
    
    VOOSMPSEIPicTimingImpl* info = [VOOSMPSEIPicTimingImpl alloc];
    info = [[info init:&cInfo] autorelease];
    
    return info;
}

- (id<VOOSMPImageData>) captureVideoImage
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMPImageDataImpl *image = nil;
    
    VOOSMP_IMAGE_DATA cData;
    memset(&cData, 0, sizeof(cData));
    int nRet = _player->GetParam(VOOSMP_PID_CAPTURE_VIDEO_IMAGE, &cData);
    
    if (VOOSMP_ERR_None == nRet) {
        image = [VOOSMPImageDataImpl alloc];
        image = [[image init:&cData] autorelease];
        
        _player->GetParam(VOOSMP_PID_CAPTURE_VIDEO_IMAGE, nil);
    }
    
    return image;
}

- (VO_OSMP_RETURN_CODE) updateSourceURL:(NSString *)url
{
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_UPDATE_SOURCE_URL, (void*)[url UTF8String]));
}


#pragma mark Configuration

- (VO_OSMP_RETURN_CODE) setLicenseContent:(NSData *)data
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == data) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_LICENSE_CONTENT, (void *)[data bytes]));
}

-(VO_OSMP_RETURN_CODE) setLicenseFilePath:(NSString *)path
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == path) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_LICENSE_FILE_PATH, (void *)[path UTF8String]));
}

-(VO_OSMP_RETURN_CODE) setPreAgreedLicense:(NSString *)str
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == str) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_LICENSE_TEXT, (void *)[str UTF8String]));
}

-(VO_OSMP_RETURN_CODE) enableDeblock:(bool) value
{
    return [self enableByID:VOOSMP_PID_DEBLOCK_ONOFF value:value];
}

-(VO_OSMP_RETURN_CODE) setDeviceCapabilityByFile:(NSString *)filename
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == filename) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_CAP_TABLE_PATH, (void *)[filename UTF8String]));
}

-(VO_OSMP_RETURN_CODE) setInitialBitrate:(int) bitrate
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    VOOSMP_PERF_DATA cData;
    memset(&cData, 0, sizeof(cData));
    cData.nCodecType = 5;
    cData.nBitRate = bitrate;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BA_STARTCAP, &cData));
}

-(VO_OSMP_PLAYER_ENGINE) getPlayerType
{
    if (NULL == _player) {
        return VO_OSMP_VOME2_PLAYER;
    }
    
    int nType = VOOSMP_VOME2_PLAYER;
    _player->GetParam(VOOSMP_PID_PLAYER_TYPE, &nType);
    
    return (VO_OSMP_PLAYER_ENGINE)nType;
}

- (VO_OSMP_RETURN_CODE) getParameter:(int)paramID pObj:(void *)pObj
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == pObj) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->GetParam(paramID, pObj));
}

- (VO_OSMP_RETURN_CODE) setParameter:(int)paramID pObj:(void *)pObj
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(paramID, pObj));
}

- (VO_OSMP_RETURN_CODE) setZoomMode:(VO_OSMP_ZOOM_MODE)mode rect:(Rect)rect
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_ZOOM_MODE, &mode));
}

- (VO_OSMP_RETURN_CODE) enableLiveStreamingDVRPosition:(bool)enable
{
    _enableDVRPosition = enable;
    
    return VO_OSMP_ERR_NONE;
}

- (long) getMinPosition
{
    if (NULL == _player) {
        return -1;
    }
    
    VOOSMP_SRC_DVRINFO cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    
    int nRet = _player->GetParam(VOOSMP_SRC_PID_DVRINFO, &cInfo);
    if (VOOSMP_ERR_None != nRet) {
        VOLOGE("getMinPosition Info Err:%d", nRet);
        return -1;
    }
    
    if (_enableDVRPosition && _player->IsLive()) {
        return (cInfo.uStartTime - cInfo.uLiveTime);
    }
    
    return cInfo.uStartTime;
}

- (long) getMaxPosition
{
    if (NULL == _player) {
        return -1;
    }
    
    VOOSMP_SRC_DVRINFO cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    
    int nRet = _player->GetParam(VOOSMP_SRC_PID_DVRINFO, &cInfo);
    if (VOOSMP_ERR_None != nRet) {
        VOLOGE("getMaxPosition Err:%d", nRet);
        return -1;
    }
    
    if (_enableDVRPosition && _player->IsLive()) {
        //Live head
        return 0;
    }
    
    return cInfo.uEndTime;
}

- (VO_OSMP_RETURN_CODE) setDRMLibrary:(NSString *)libName libApiName:(NSString *)libApiName
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int nRet = VO_OSMP_ERR_NONE;

    if (NULL != libName)
    {
        nRet = _player->SetParam(VOOSMP_SRC_PID_DRM_FILE_NAME, (void *)[libName UTF8String]);
    }
    
    if (NULL != libName)
    {
        nRet = _player->SetParam(VOOSMP_SRC_PID_DRM_API_NAME, (void *)[libApiName UTF8String]);
    }
    
    return (VO_OSMP_RETURN_CODE)nRet;
}

- (VO_OSMP_RETURN_CODE) enableBitrateAdaptation:(bool) value
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int iMode = VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_MANUAL;
    if (value) {
        iMode = VOOSMP_SRC_ADAPTIVE_STREAMING_BA_MODE_AUTO;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BA_WORKMODE, &iMode));
}

- (VO_OSMP_RETURN_CODE) setDRMAdapter:(void *)adapter
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == adapter) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_DRM_ADAPTER_OBJECT, adapter));
}

- (VO_OSMP_RETURN_CODE) setDRMVerificationInfo:(VOOSMPVerificationInfo *)info
{
    return [self setVerificationInfo:info paramID:VOOSMP_SRC_PID_DODRMVERIFICATION];
}

- (NSString *) getDRMUniqueIdentifier
{
    if (NULL == _player) {
        return nil;
    }
    
    char *pValue = NULL;
    int nRet = _player->GetParam(VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER, &pValue);
    
    if ((VO_OSMP_ERR_NONE == nRet) && (pValue != NULL)) {
        return [NSString stringWithFormat:@"%s", pValue];
    }
    
    return nil;
}

- (VO_OSMP_RETURN_CODE) setDRMUniqueIdentifier:(NSString *)string
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == string) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_DRM_UNIQUE_IDENTIFIER, (void *)[string UTF8String]));
}

- (NSString *) getDRMUniqueIndentifier
{
    return [self getDRMUniqueIdentifier];
}

- (VO_OSMP_RETURN_CODE) setDRMUniqueIndentifier:(NSString *)string
{
    return [self setDRMUniqueIdentifier:string];
}

- (VO_OSMP_RETURN_CODE) setInitialBufferTime:(int)time
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BUFFER_START_BUFFERING_TIME, &time));
}

- (VO_OSMP_RETURN_CODE) setAnewBufferingTime:(int)time
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME, &time));
}

- (VO_OSMP_RETURN_CODE) setPlaybackBufferingTime:(int)time
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BUFFER_BUFFERING_TIME, &time));
}

- (VO_OSMP_RETURN_CODE) setMaxBufferTime:(int)time
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BUFFER_MAX_SIZE, &time));
}

- (VO_OSMP_RETURN_CODE) enableAudioStream:(bool) value
{
    return [self enableByID:VOOSMP_PID_AUDIO_STREAM_ONOFF value:value];
}

- (VO_OSMP_RETURN_CODE) enableVideoStream:(bool) value
{
    return [self enableByID:VOOSMP_PID_VIDEO_STREAM_ONOFF value:value];
}

- (VO_OSMP_RETURN_CODE) enableLowLatencyVideo:(bool) value
{
    [self enableByID:VOOSMP_SRC_PID_LOW_LATENCY_MODE value:value];
    return [self enableByID:VOOSMP_PID_LOW_LATENCY value:value];
}

- (VO_OSMP_RETURN_CODE) enableAudioEffect:(bool) value
{
    return [self enableByID:VOOSMP_PID_AUDIO_EFFECT_ENABLE value:value];
}

- (VO_OSMP_RETURN_CODE) enablePCMOutput:(bool) value
{
    return [self enableByID:VOOSMP_PID_AUDIO_PCM_OUTPUT value:value];
}

- (VO_OSMP_RETURN_CODE) setAudioPlaybackSpeed:(float)speed
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_AUDIO_PLAYBACK_SPEED, &speed));
}

- (NSString*) getVersion:(VO_OSMP_MODULE_TYPE)module
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMP_MODULE_VERSION cVersion;
    memset(&cVersion, 0, sizeof(cVersion));
    cVersion.nModuleType = module;
    int nRet = _player->GetParam(VOOSMP_PID_MODULE_VERSION, &cVersion);
    
    if ( (VOOSMP_ERR_None == nRet) && (NULL != cVersion.pszVersion) )
    {
        return [NSString stringWithFormat:@"%s", cVersion.pszVersion];
    }
    
    return nil;
}

- (VO_OSMP_RETURN_CODE) setPresentationDelay:(int)time
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_PRESENTATION_DELAY, &time));
}

- (VO_OSMP_RETURN_CODE) enableSEI:(VO_OSMP_SEI_INFO_FLAG)flag
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int nFlag = flag;
    if (VO_OSMP_FLAG_SEI_MAX == flag) {
        nFlag = VOOSMP_FLAG_SEI_MAX;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_RETRIEVE_SEI_INFO, &nFlag));
}

- (VO_OSMP_RETURN_CODE) setPDConnectionRetryCount:(int)times
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_PD_CONNECTION_RETRY_TIMES, &times));
}

- (NSArray *) getVideoDecodingBitrate
{
    if (NULL == _player) {
        return nil;
    }
    
    NSMutableArray *array = nil;
    int bit[10] = {0};
    int nRet = _player->GetParam(VOOSMP_PID_VIDEO_DEC_BITRATE, bit);
    
    if (VOOSMP_ERR_None == nRet) {
        
        array = [[[NSMutableArray alloc] init] autorelease];
        
        for (int i = 0; i<10; i++)
        {
            NSNumber *pValue = [NSNumber numberWithInt:(bit[i] * 8)];
            [array addObject:pValue];
        }
    }
    
    return array;
}

- (NSArray *) getAudioDecodingBitrate
{
    if (NULL == _player) {
        return nil;
    }
    
    NSMutableArray *array = nil;
    int bit[10] = {0};
    int nRet = _player->GetParam(VOOSMP_PID_AUDIO_DEC_BITRATE, bit);
    
    if (VOOSMP_ERR_None == nRet) {
        
        array = [[[NSMutableArray alloc] init] autorelease];
        
        for (int i = 0; i<10; i++)
        {
            NSNumber *pValue = [NSNumber numberWithInt:(bit[i] * 8)];
            [array addObject:pValue];
        }
    }
    
    return array;
}

- (VO_OSMP_RETURN_CODE) enableCPUAdaptation:(bool)value
{
    return [self enableByID:VOOSMP_SRC_PID_DISABLE_CPU_ADAPTION value:!value];
}

- (VO_OSMP_RETURN_CODE) setBitrateThreshold:(int)upper lower:(int)lower
{
    VOOSMP_SRC_BA_THRESHOLD threshold;
    memset(&threshold, 0, sizeof(VOOSMP_SRC_BA_THRESHOLD));
    threshold.nUpper    = upper;
    threshold.nLower    = lower;
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_BITRATE_THRESHOLD, &threshold));
}

- (VO_OSMP_RETURN_CODE) setSegmentDownloadRetryCount:(int) times
{
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_SEGMENT_DOWNLOAD_RETRY_COUNT, &times));
}

- (VO_OSMP_RETURN_CODE) enableDolbyLibrary:(bool) value
{
    int nValue = value?1:0;
    _player->SetParam(VOOSMP_PID_LOAD_DOLBY_DECODER_MODULE, &nValue);
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_LOAD_AUDIO_EFFECT_MODULE, &nValue));
}



#pragma mark subtitle
- (VO_OSMP_RETURN_CODE) setSubtitlePath:(NSString *)filePath
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == filePath) {
        return VO_OSMP_ERR_POINTER;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_SUBTITLE_FILE_NAME, (void *)[filePath UTF8String]));
}

- (VO_OSMP_RETURN_CODE) enableSubtitle:(bool)value
{
    return [self enableByID:VOOSMP_PID_COMMON_CCPARSER value:value];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontColor:(int)color
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_COLOR;
    _subtitleSetting.nFontColor = color;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontOpacity:(int)alpha
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_OPACITY;
    _subtitleSetting.nFontOpacity = alpha;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontSizeScale:(int)scale
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_SIZE_SCALE;
    _subtitleSetting.nFontSizeScale = scale;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundColor:(int)color
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_COLOR;
    _subtitleSetting.nBackgroundColor = color;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBackgroundOpacity:(int)alpha
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_OPACITY;
    _subtitleSetting.nBackgroundOpacity = alpha;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundColor:(int)color
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_COLOR;
    _subtitleSetting.nWindowBackgroundColor = color;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleWindowBackgroundOpacity:(int)alpha
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_OPACITY;
    _subtitleSetting.nWindowBackgroundOpacity = alpha;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontItalic:(bool)enable
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_ITALIC;
    _subtitleSetting.bFontItalic = enable;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontBold:(bool)enable
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_BOLD;
    _subtitleSetting.bFontBold = enable;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontUnderline:(bool)enable
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_UNDERLINE;
    _subtitleSetting.bFontUnderline = enable;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontName:(NSString *)name
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == name) {
        return VO_OSMP_ERR_POINTER;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_NAME;
    
    memset(&_subtitleSetting.szFontName, 0, sizeof(_subtitleSetting.szFontName));
    
    int nLength = [name length];
    
    if (nLength > (sizeof(_subtitleSetting.szFontName) - 1)) {
        nLength = sizeof(_subtitleSetting.szFontName) - 1;
    }
    
    strncpy(_subtitleSetting.szFontName, [name UTF8String], nLength);
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeType:(int)type
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_EDGE_TYPE;
    _subtitleSetting.nEdgeType = type;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeColor:(int)color
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_EDGE_COLOR;
    _subtitleSetting.nEdgeColor = color;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) setSubtitleFontEdgeOpacity:(int)alpha
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    _subtitleSetting.nItemMask |= VOOSMP_SUBTITLE_MASK_FONT_EDGE_OPACITY;
    _subtitleSetting.nEdgeOpacity = alpha;
    
    return [self updateSubtitleSetting];
}

- (VO_OSMP_RETURN_CODE) resetSubtitleParameter
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    memset(&_subtitleSetting, 0, sizeof(_subtitleSetting));
    
    int nEnable = 0;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF, &nEnable));
}

- (VO_OSMP_RETURN_CODE) previewSubtitle:(NSString *)sampleText view:(id)view
{
    VOOSMP_PREVIEW_SUBTITLE_INFO info;
    memset(&info, 0, sizeof(VOOSMP_PREVIEW_SUBTITLE_INFO));
    info.pszSampleText  = (char*)[sampleText UTF8String];
    info.pView          = view;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_PREVIEW_SUBTITLE, &info));
}

#pragma mark Asset selection
- (int) getVideoCount
{
    if (NULL == _player) {
        return -1;
    }
    
    return _player->GetVideoCount();
}

- (int) getAudioCount
{
    if (NULL == _player) {
        return -1;
    }
    
    return _player->GetAudioCount();
}

- (int) getSubtitleCount
{
    if (NULL == _player) {
        return -1;
    }
    
    return _player->GetSubtitleCount();
}

- (VO_OSMP_RETURN_CODE) selectVideo:(int)index
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SelectVideo(index));
}

- (VO_OSMP_RETURN_CODE) selectAudio:(int)index
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SelectAudio(index));
}

- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SelectSubtitle(index));
}

- (bool) isVideoAvailable:(int)index
{
    if (NULL == _player) {
        return false;
    }
    
    return _player->IsVideoAvailable(index);
}

- (bool) isAudioAvailable:(int)index
{
    if (NULL == _player) {
        return false;
    }
    
    return _player->IsAudioAvailable(index);
}

- (bool) isSubtitleAvailable:(int)index
{
    if (NULL == _player) {
        return false;
    }
    
    return _player->IsSubtitleAvailable(index);
}

- (VO_OSMP_RETURN_CODE) commitSelection
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->CommitSelection());
}

- (VO_OSMP_RETURN_CODE) clearSelection
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->ClearSelection());
}

- (id<VOOSMPAssetProperty>) getVideoProperty:(int)index
{
    return [self getTrackProperty:index type:VO_SELECT_VIDEO];
}

- (id<VOOSMPAssetProperty>) getAudioProperty:(int)index
{
    return [self getTrackProperty:index type:VO_SELECT_AUDIO];
}

- (id<VOOSMPAssetProperty>) getSubtitleProperty:(int)index
{
    return [self getTrackProperty:index type:VO_SELECT_SUBTITLE];
}

- (id<VOOSMPAssetIndex>) getPlayingAsset
{
    if (NULL == _player) {
        return nil;
    }

    VOOSMP_SRC_CURR_TRACK_INDEX currIndex;
    memset(&currIndex, 0, sizeof(currIndex));
    VOOSMPAssetIndexImpl *assetIndex = nil;
    
    int nRet = _player->GetCurrPlayingTrackIndex(&currIndex);

    if (VOOSMP_ERR_None == nRet) {
        assetIndex = [[[VOOSMPAssetIndexImpl alloc] init] autorelease];
        [assetIndex setVideoIndex:currIndex.nCurrVideoIdx];
        [assetIndex setAudioIndex:currIndex.nCurrAudioIdx];
        [assetIndex setSubtitleIndex:currIndex.nCurrSubtitleIdx];
    }

    return assetIndex;
}

- (id<VOOSMPAssetIndex>) getCurrentSelection
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMP_SRC_CURR_TRACK_INDEX currIndex;
    memset(&currIndex, 0, sizeof(currIndex));
    VOOSMPAssetIndexImpl *assetIndex = nil;
    
    int nRet = _player->GetCurrSelectedTrackIndex(&currIndex);
    
    if (VOOSMP_ERR_None == nRet) {
        assetIndex = [[[VOOSMPAssetIndexImpl alloc] init] autorelease];
        [assetIndex setVideoIndex:currIndex.nCurrVideoIdx];
        [assetIndex setAudioIndex:currIndex.nCurrAudioIdx];
        [assetIndex setSubtitleIndex:currIndex.nCurrSubtitleIdx];
    }
    
    return assetIndex;
}

- (VO_OSMP_RETURN_CODE) setDefaultAudioLanguage:(NSString*)type
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (!type)
        return VO_OSMP_ERR_POINTER;

    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_DEFAULT_AUDIO_LANGUAGE, (void*)[type UTF8String]));
}

- (VO_OSMP_RETURN_CODE) setDefaultSubtitleLanguage:(NSString*)type
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (!type)
        return VO_OSMP_ERR_POINTER;
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_DEFAULT_SUBTITLE_LANGUAGE, (void*)[type UTF8String]));
}



#pragma mark device info
+ (VO_CPU_Info *) getCpuInfo
{
    static VO_CPU_Info info;
    
    static bool bInit = false;
    
    if (!bInit) {
        memset(&info, 0, sizeof(info));
        if (0 == get_cpu_info(&info)) {
            bInit = true;
        }
    }
    
    return &info;
}

- (int) getNumberOfCores
{
    VO_CPU_Info *pInfo = [VOCommonPlayerImplement getCpuInfo];
    
    return pInfo->mCount;
}

- (bool) hasNeon
{
    VO_CPU_Info *pInfo = [VOCommonPlayerImplement getCpuInfo];
    
#ifdef _IOS
    if (CPU_SUBTYPE_ARM_V7 <= pInfo->mFeatures) {
        return true;
    }
    else {
        return false;
    }
#endif
    
    return false;
}

- (long) getMaxCPUFrequency
{
    VO_CPU_Info *pInfo = [VOCommonPlayerImplement getCpuInfo];
    
    return (pInfo->mMaxCpuSpeed) * 1000;
}

- (VO_OSMP_DEVICE_INFO_OS_TYPE) getOSType
{
#ifdef _IOS
    return VO_OSMP_DEVICE_INFO_OS_IOS;
#else
    return VO_OSMP_DEVICE_INFO_OS_OSX;
#endif
}

- (NSString *) getOSVersion
{
#ifdef _IOS
    return [UIDevice currentDevice].systemVersion;
#else
    SInt32 versionMajor=0, versionMinor=0, versionBugFix=0;
    Gestalt(gestaltSystemVersionMajor, &versionMajor);
    Gestalt(gestaltSystemVersionMinor, &versionMinor);
    Gestalt(gestaltSystemVersionBugFix, &versionBugFix);
    return [NSString stringWithFormat:@"%ld.%ld.%ld", versionMajor, versionMinor, versionBugFix];
#endif
}

- (NSString *) getDeviceModel
{
#ifdef _IOS
    char szChar[20] = {0};
    voGetSysInfoStrByName("hw.machine", szChar, sizeof(szChar));
#else
    char szChar[20] = {0};
    voGetSysInfoStrByName("hw.model", szChar, sizeof(szChar));
#endif
    
    return [NSString stringWithFormat:@"%s", szChar];
}

#pragma mark RTSP
- (VO_OSMP_RETURN_CODE) setRTSPConnectionType:(VO_OSMP_RTSP_CONNECTION_TYPE)type
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int value = 0;
    
    if (VO_OSMP_RTSP_CONNECTION_TCP == type) {
        value = 1;
    }
    else if (VO_OSMP_RTSP_CONNECTION_UDP == type) {
        value = -1;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_SOCKET_CONNECTION_TYPE, &value));
}

- (VO_OSMP_RETURN_CODE) setRTSPConnectionPort:(VOOSMPRTSPPort *)portNum
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == portNum) {
        return VO_OSMP_ERR_POINTER;
    }
    
    VOOSMP_SRC_RTSP_CONNECTION_PORT cPort;
    memset(&cPort, 0, sizeof(cPort));
    cPort.uAudioConnectionPort = portNum.audioConnectionPort;
    cPort.uVideoConnectionPort = portNum.videoConnectionPort;
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_RTSP_CONNECTION_PORT, &cPort));
}

- (id<VOOSMPRTSPStatistics>) getRTSPStatistics
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMP_SRC_RTSP_STATS cStatus;
    memset(&cStatus, 0, sizeof(cStatus));
    VOOSMPRTSPStatisticsImpl *rtspStatistics = nil;
    
    int nRet = _player->GetParam(VOOSMP_SRC_PID_RTSP_STATS, &cStatus);
    
    if (VOOSMP_ERR_None == nRet) {
        rtspStatistics = [VOOSMPRTSPStatisticsImpl alloc];
        rtspStatistics = [[rtspStatistics init:&cStatus] autorelease];
    }
    
    return rtspStatistics;
}

- (VO_OSMP_RETURN_CODE) enableRTSPOverHTTP:(bool)enable
{
    int nVal = enable?1:0;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_ENABLE_RTSP_HTTP_TUNNELING, &nVal));
}

- (VO_OSMP_RETURN_CODE) setRTSPOverHTTPConnectionPort:(int)portNum
{
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_RTSP_OVER_HTTP_CONNECTION_PORT, &portNum));
}


#pragma mark HTTP
- (VO_OSMP_RETURN_CODE) setHTTPVerificationInfo:(VOOSMPVerificationInfo *)info
{
    return [self setVerificationInfo:info paramID:VOOSMP_SRC_PID_DOHTTPVERIFICATION];
}

- (VO_OSMP_RETURN_CODE) setHTTPHeader:(NSString *)headerName headerValue:(NSString *)headerValue
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if ((nil == headerName) || (nil == headerValue))
    {
        return VO_OSMP_ERR_POINTER;
    }
    
    VOOSMP_SRC_HTTP_HEADER cHeader;
    memset(&cHeader, 0, sizeof(cHeader));
    
    cHeader.pszHeaderName = (unsigned char *)[headerName UTF8String];
    cHeader.pszHeaderValue = (unsigned char *)[headerValue UTF8String];
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_HTTP_HEADER, &cHeader));
}

- (VO_OSMP_RETURN_CODE) setHTTPProxy:(VOOSMPHTTPProxy *)proxy
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if ((nil == proxy) || (nil == proxy.proxyHost))
    {
        return VO_OSMP_ERR_POINTER;
    }
    
    VOOSMP_SRC_HTTP_PROXY cProxy;
    memset(&cProxy, 0, sizeof(cProxy));
    cProxy.pszProxyHost = (unsigned char *)[proxy.proxyHost UTF8String];
    cProxy.nProxyPort = proxy.proxyPort;
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_HTTP_PROXY_INFO, &cProxy));
}

- (VO_OSMP_RETURN_CODE) setHTTPRetryTimeout:(int)time
{
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_SRC_PID_HTTP_RETRY_TIMEOUT, &time));
}

#pragma mark Analytics
- (VO_OSMP_RETURN_CODE) enableAnalytics:(int)cacheTime
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int iOn = 1;
    (_player->SetParam(VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF, &iOn));
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_VIDEO_PERFORMANCE_CACHE, &cacheTime));
}

- (VO_OSMP_RETURN_CODE) startAnalyticsNotification:(int)interval filter:(VOOSMPAnalyticsFilter *)filter
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int iOn = 1;
    int nRet = (_player->SetParam(VOOSMP_PID_VIDEO_PERFORMANCE_ONOFF, &iOn));
    
    if (VOOSMP_ERR_None != nRet) {
        return (VO_OSMP_RETURN_CODE)nRet;
    }
    
    if (nil != filter) {
        
        VOOSMP_PERFORMANCE_DATA cData;
        memset(&cData, 0, sizeof(cData));
        
        cData.nLastTime = filter.lastTime;
        cData.nSourceTimeNum = filter.sourceTime;
        cData.nCodecTimeNum = filter.codecTime;
        cData.nRenderTimeNum = filter.renderTime;
        cData.nJitterNum = filter.jitterTime;
        
        nRet = _player->SetParam(VOOSMP_PID_VIDEO_PERFORMANCE_OPTION, &cData);
    }
    
    if (VOOSMP_ERR_None == nRet) {
        [self createTimer:(float)interval / 1000];
    }
    
    return (VO_OSMP_RETURN_CODE)nRet;
}

#pragma mark private for asset
- (id<VOOSMPAssetProperty>) getTrackProperty:(int)index type:(VO_SELECT_TYPE)type
{
    if (NULL == _player) {
        return nil;
    }
    
    VOOSMP_SRC_TRACK_PROPERTY *pProperty = NULL;
    VOOSMPAssetInfo *pInfo = nil;
    
    int nRet = VOOSMP_ERR_Unknown;
    
    switch (type) {
        case VO_SELECT_VIDEO:
            nRet = _player->GetVideoProperty(index, &pProperty);
            break;
        case VO_SELECT_AUDIO:
            nRet = _player->GetAudioProperty(index, &pProperty);
            break;
        case VO_SELECT_SUBTITLE:
            nRet = _player->GetSubtitleProperty(index, &pProperty);
            break;
        default:
            break;
    }
    
    if (VOOSMP_ERR_None == nRet && NULL != pProperty) {
        pInfo = [VOOSMPAssetInfo alloc];
        pInfo = [[pInfo init:pProperty] autorelease];
    }
    
    return pInfo;
}

#pragma mark private for timer
- (void) handleTimer: (NSTimer *) timer
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    VOOSMP_PERFORMANCE_DATA cData;
    memset(&cData, 0, sizeof(cData));
    
    int nRet = (_player->GetParam(VOOSMP_PID_VIDEO_PERFORMANCE_OPTION, &cData));
    
    if (VOOSMP_ERR_None != nRet) {
        [pool release];
        return;
    }
    VOOSMPAnalyticsInfoImpl* obj = [[[VOOSMPAnalyticsInfoImpl alloc] init:&cData] autorelease];
    
    [self onVOEventlistener:VO_OSMP_CB_ANALYTICS_INFO withParam1:obj withParam2:NULL];
    
    [pool release];
}

- (void) createTimer:(float)interval
{
	[self.timer invalidate];
	self.timer = nil;
    
	self.timer = [NSTimer scheduledTimerWithTimeInterval: interval
                                                     target: self
                                                   selector: @selector(handleTimer:)
                                                   userInfo: nil
                                                    repeats: YES];
}

#pragma mark private for subtitle
- (VO_OSMP_RETURN_CODE) updateSubtitleSetting
{
    int nEnable = 1;
    int nRet = (_player->SetParam(VOOSMP_PID_SUBTITLE_SETTINGS_ONOFF, &nEnable));
    if (VO_OSMP_ERR_NONE != nRet) {
        return (VO_OSMP_RETURN_CODE)nRet;
    }
    
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(VOOSMP_PID_SUBTITLE_SETTINGS, &_subtitleSetting));
}

#pragma mark private for configuration
- (VO_OSMP_RETURN_CODE) setVerificationInfo:(VOOSMPVerificationInfo *)info paramID:(int)paramID
{    
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    if (nil == info) {
        return VO_OSMP_ERR_POINTER;
    }
    
    VOOSMP_SRC_VERIFICATIONINFO cInfo;
    memset(&cInfo, 0, sizeof(cInfo));
    if (nil != info.verificationString) {
        cInfo.pData = (void *)[info.verificationString UTF8String];
        cInfo.nDataSize = [info.verificationString length];
    }
    cInfo.nDataFlag = info.dataFlag;
    
    int nRet = (_player->SetParam(paramID, &cInfo));
    if (VOOSMP_ERR_None == nRet) {
        if (NULL != cInfo.pszResponse) {
            info.responseString = [NSString stringWithFormat:@"%s", cInfo.pszResponse];
        }
    }
    
    return (VO_OSMP_RETURN_CODE)nRet;
}

#pragma mark private common
- (VO_OSMP_RETURN_CODE) enableByID:(int)nID value:(bool)value
{
    if (NULL == _player) {
        return VO_OSMP_ERR_UNINITIALIZE;
    }
    
    int iEnable = value ? 1:0;
    return (VO_OSMP_RETURN_CODE)(_player->SetParam(nID, &iEnable));
}

@end
