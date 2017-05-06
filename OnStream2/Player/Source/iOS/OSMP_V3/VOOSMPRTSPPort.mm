
#import "VOOSMPRTSPPort.h"

@interface VOOSMPRTSPPort ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPRTSPPort

@synthesize audioConnectionPort    = _audioConnectionPort;
@synthesize videoConnectionPort    = _videoConnectionPort;

- (id) init:(int)audioConnectionPort videoConnectionPort:(int)videoConnectionPort
{
    if (nil != (self = [super init]))
    {
        _audioConnectionPort = audioConnectionPort;
        _videoConnectionPort = videoConnectionPort;
    }
    
    return self;
}

@end