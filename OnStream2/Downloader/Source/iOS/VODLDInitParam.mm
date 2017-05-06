
#import "VOOSMPStreamingDownloaderType.h"

@interface VOOSMPStreamingDownloaderInitParam ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPStreamingDownloaderInitParam

@synthesize libraryPath = _libraryPath;

- (id) init
{
    if (nil != (self = [super init]))
    {
    }
    
    return self;
}

- (void) dealloc
{
    self.libraryPath    = nil;
    
    [super dealloc];
}


@end












@interface VOOSMPStreamingDownloaderProgressInfo ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPStreamingDownloaderProgressInfo

@synthesize downloadedStreamDuration    = _downloadedStreamDuration;
@synthesize totalStreamDuration         = _totalStreamDuration;


- (id) init:(int)downloadedStreamDuration totalStreamDuration:(int)totalStreamDuration;
{
    if (nil != (self = [super init]))
    {
        _downloadedStreamDuration   = downloadedStreamDuration;
        _totalStreamDuration        = totalStreamDuration;
    }
    
    return self;
}

@end