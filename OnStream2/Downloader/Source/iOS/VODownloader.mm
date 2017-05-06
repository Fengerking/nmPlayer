
#import "VODownloader.h"
#import "VOOSMPAssetInfo.h"

#include "voLog.h"


typedef enum {
    DLD_SELECT_VIDEO,
    DLD_SELECT_AUDIO,
    DLD_SELECT_SUBTITLE,
}DLD_SELECT_TYPE;


@interface VODownloader ()

// API that don't need to be seen by the outside world.
- (id<VOOSMPAssetProperty>) getTrackProperty:(int)index type:(DLD_SELECT_TYPE)type;
@end

@implementation VODownloader


- (VO_OSMP_RETURN_CODE) onDownloaderEvent:(VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID)nID param1:(void*)param1 param2:(void*)param2
{
    VO_OSMP_RETURN_CODE nRet = VO_OSMP_ERR_POINTER;
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    if(_downloadDelegate)
    {
        //it should do transition here according the event output
        void* pObj = NULL;
        
        if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_OK)
        {
            if(param1)
            {
                pObj = [NSString stringWithFormat:@"%s", (unsigned char*)param1];
            }
        }
        else if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_UPDATE)
        {
            if(param1)
            {
                DOWNLOADER_PROGRESS_INFO* info = (DOWNLOADER_PROGRESS_INFO*)param1;
                pObj = [[[VOOSMPStreamingDownloaderProgressInfo alloc] init:info->uCurrDuration totalStreamDuration:info->TotalDuration] autorelease];
            }
        }

        nRet = [_downloadDelegate onVOStreamingDownloaderEvent:nID param1:(int)param1 param2:(int)param2 pObj:pObj];
    }
    
    [pool release];
    
    return nRet;
}


int onDownloaderEventCallback (void* pUserData, unsigned int nID, void* pParam1, void* pParam2)
{
    if (pUserData == nil)
    {
        return VO_OSMP_ERR_POINTER;
    }
    
    VODownloader * pSelf = (VODownloader *)pUserData;
    
    VO_OSMP_RETURN_CODE result = [pSelf onDownloaderEvent:(VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID)nID param1:pParam1 param2:pParam2];
    
    return (int)result;
}




/**
 * Initialize a player instance.
 * <pre>This function must be called first to start a session.</pre>
 *
 * @param   delegate  [in] Refer to {@link VODownloaderDelegate}.
 * @param   initParam [in] Refer to {@link VODLDInitParam}.
 * @return  player object if successful; nil if unsuccessful
 */
- (id) init:(id<VOOSMPStreamingDownloaderDelegate>)delegate initParam:(VOOSMPStreamingDownloaderInitParam*)initParam
{
    _downloadDelegate = nil;
    
    if (nil != (self = [super init]))
    {
        _downloadDelegate   = delegate;
        _pDownloader        = new CDownloaderImpl;
        
        DOWNLOADER_API_EVENTCALLBACK callback;
        memset(&callback, 0, sizeof(DOWNLOADER_API_EVENTCALLBACK));
        callback.pUserData  = self;
        callback.SendEvent  = onDownloaderEventCallback;
        
        DOWNLOADER_API_INITPARAM init;
        memset(&init, 0, sizeof(DOWNLOADER_API_INITPARAM));
        init.uFlag          = 0;
        init.strWorkPath    = (wchar_t*)[[initParam getLibraryPath] UTF8String];
        init.pInitParam     = NULL;

        _pDownloader->Init(&callback, &init);
        
        //[self onDownloaderEvent: (VO_OSMP_CB_DLD_EVENT_ID)1 param1:0 param2:0];
    }
    
    return self;
}


/**
 * Open media source.
 *
 * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
 * @param   flag            [in] The flag for opening media source.
 * @param   localDir        [in] The directory to save content files.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(unsigned int)flag localDir:(NSString *)localDir
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->Open((void*)[url UTF8String], flag, (void*)[localDir UTF8String]);
}


/**
 * Close media source.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) close
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->Close();
}


/**
 * Start to download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) start
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->StartDownload();
}


/**
 * Stop download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) stop
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->StopDownload();
}


/**
 * Pause download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) pause
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->PauseDownload();
}


/**
 * Resume download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) resume
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->ResumeDownload();
}


- (int) getVideoCount
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->GetVideoCount();
}

- (int) getAudioCount
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->GetAudioCount();
}

- (int) getSubtitleCount
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->GetSubtitleCount();
}

- (VO_OSMP_RETURN_CODE) selectVideo:(int)index
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->SelectVideo(index);
}

- (VO_OSMP_RETURN_CODE) selectAudio:(int)index
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->SelectAudio(index);
}

- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->SelectSubtitle(index);
}

- (bool) isVideoAvailable:(int)index
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->IsVideoAvailable(index);
}

- (bool) isAudioAvailable:(int)index
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->IsAudioAvailable(index);
}

- (bool) isSubtitleAvailable:(int)index
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->IsSubtitleAvailable(index);
}

- (VO_OSMP_RETURN_CODE) commitSelection
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->CommitSelection();
}

- (VO_OSMP_RETURN_CODE) clearSelection
{
    if(!_pDownloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    return (VO_OSMP_RETURN_CODE)_pDownloader->ClearSelection();
}

- (id<VOOSMPAssetProperty>) getVideoProperty:(int)index
{
    if(!_pDownloader)
        return nil;
    
    return [self getTrackProperty:index type:DLD_SELECT_VIDEO];
}

- (id<VOOSMPAssetProperty>) getAudioProperty:(int)index
{
    if(!_pDownloader)
        return nil;
    return [self getTrackProperty:index type:DLD_SELECT_AUDIO];
}

- (id<VOOSMPAssetProperty>) getSubtitleProperty:(int)index
{
    if(!_pDownloader)
        return nil;
    return [self getTrackProperty:index type:DLD_SELECT_SUBTITLE];
}

- (id<VOOSMPAssetIndex>) getPlayingAsset
{
    if(!_pDownloader)
        return nil;

    VOOSMP_SRC_CURR_TRACK_INDEX currIndex;
    memset(&currIndex, 0, sizeof(currIndex));
    VOOSMPAssetIndexImpl *assetIndex = nil;
    
    int nRet = _pDownloader->GetCurrPlayingTrackIndex(&currIndex);
    
    if (VOOSMP_ERR_None == nRet)
    {
        assetIndex = [[[VOOSMPAssetIndexImpl alloc] init] autorelease];
        [assetIndex setVideoIndex:currIndex.nCurrVideoIdx];
        [assetIndex setAudioIndex:currIndex.nCurrAudioIdx];
        [assetIndex setSubtitleIndex:currIndex.nCurrSubtitleIdx];
    }
    
    return assetIndex;
}

- (id<VOOSMPAssetIndex>) getCurrentSelection
{
    if(!_pDownloader)
        return nil;

    VOOSMP_SRC_CURR_TRACK_INDEX currIndex;
    memset(&currIndex, 0, sizeof(currIndex));
    VOOSMPAssetIndexImpl *assetIndex = nil;
    
    int nRet = _pDownloader->GetCurrSelectedTrackIndex(&currIndex);
    
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
    return VO_OSMP_ERR_IMPLEMENT;
}

- (VO_OSMP_RETURN_CODE) setDefaultSubtitleLanguage:(NSString*)type
{
    return VO_OSMP_ERR_IMPLEMENT;
}

//private class method
- (id<VOOSMPAssetProperty>) getTrackProperty:(int)index type:(DLD_SELECT_TYPE)type
{
    if(!_pDownloader)
        return nil;
    
    VOOSMP_SRC_TRACK_PROPERTY *pProperty = NULL;
    VOOSMPAssetInfo *pInfo = nil;
    
    int nRet = VOOSMP_ERR_Unknown;
    
    switch (type) {
        case DLD_SELECT_VIDEO:
            nRet = _pDownloader->GetVideoProperty(index, &pProperty);
            break;
        case DLD_SELECT_AUDIO:
            nRet = _pDownloader->GetAudioProperty(index, &pProperty);
            break;
        case DLD_SELECT_SUBTITLE:
            nRet = _pDownloader->GetSubtitleProperty(index, &pProperty);
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

- (void) dealloc
{
    if(_pDownloader)
    {
        delete _pDownloader;
        _pDownloader = NULL;
    }
    
    [super dealloc];
}


@end