
#import "VODownloader.h"
#import "VOOSMPStreamingDownloaderImpl.h"

@interface VOOSMPStreamingDownloaderImpl ()

// Properties that don't need to be seen by the outside world.
@property (nonatomic, retain) id downloader;
@end

@implementation VOOSMPStreamingDownloaderImpl

@synthesize downloader = _downloader;


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
    if (nil != (self = [super init]))
    {
        _downloader = [[VODownloader alloc] init:delegate initParam:initParam];
        
        if (nil == _downloader) {
            [self release];
            return nil;
        }
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
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    
    if(!url || !localDir)
        return VO_OSMP_ERR_POINTER;
    
    return [(VODownloader*)_downloader open:url flag:flag localDir:localDir];
}


/**
 * Close media source.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) close
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader close];
}


/**
 * Start to download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) start
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader start];
}


/**
 * Stop download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) stop
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader stop];
}


/**
 * Pause download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) pause
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader pause];
}


/**
 * Resume download contents.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) resume
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader resume];
}

- (int) getVideoCount
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;

    return [(VODownloader*)_downloader getVideoCount];
}

- (int) getAudioCount
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader getAudioCount];
}

- (int) getSubtitleCount
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader getSubtitleCount];
}

- (VO_OSMP_RETURN_CODE) selectVideo:(int)index
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader selectVideo:index];
}

- (VO_OSMP_RETURN_CODE) selectAudio:(int)index
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader selectAudio:index];
}

- (VO_OSMP_RETURN_CODE) selectSubtitle:(int)index
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader selectSubtitle:index];
}

- (bool) isVideoAvailable:(int)index
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader isVideoAvailable:index];
}

- (bool) isAudioAvailable:(int)index
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader isAudioAvailable:index];
}

- (bool) isSubtitleAvailable:(int)index
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader isSubtitleAvailable:index];
}

- (VO_OSMP_RETURN_CODE) commitSelection
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader commitSelection];
}

- (VO_OSMP_RETURN_CODE) clearSelection
{
    if(!_downloader)
        return VO_OSMP_ERR_UNINITIALIZE;
    return [(VODownloader*)_downloader clearSelection];
}

- (id<VOOSMPAssetProperty>) getVideoProperty:(int)index
{
    if(!_downloader)
        return nil;
    return [(VODownloader*)_downloader getVideoProperty:index];
}

- (id<VOOSMPAssetProperty>) getAudioProperty:(int)index
{
    if(!_downloader)
        return nil;
    return [(VODownloader*)_downloader getAudioProperty:index];
}

- (id<VOOSMPAssetProperty>) getSubtitleProperty:(int)index
{
    if(!_downloader)
        return nil;
    return [(VODownloader*)_downloader getSubtitleProperty:index];
}

- (id<VOOSMPAssetIndex>) getPlayingAsset
{
    if(!_downloader)
        return nil;
    return [(VODownloader*)_downloader getPlayingAsset];
}

- (id<VOOSMPAssetIndex>) getCurrentSelection
{
    if(!_downloader)
        return nil;
    return [(VODownloader*)_downloader getCurrentSelection];
}

- (VO_OSMP_RETURN_CODE) setDefaultAudioLanguage:(NSString*)type
{
    return VO_OSMP_ERR_IMPLEMENT;
}

- (VO_OSMP_RETURN_CODE) setDefaultSubtitleLanguage:(NSString*)type
{
    return VO_OSMP_ERR_IMPLEMENT;
}



- (void) dealloc
{
    if(_downloader)
    {
        [_downloader release];
        _downloader = nil;
    }
    
    [super dealloc];
}


@end