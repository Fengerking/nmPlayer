
#import "VOOSMPChunkInfoImpl.h"

@interface VOOSMPChunkInfoImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPChunkInfoImpl

@synthesize type = _type;
@synthesize rootURL = _rootURL;
@synthesize URL = _URL;
@synthesize startTime = _startTime;
@synthesize duration = _duration;
@synthesize timeScale = _timeScale;

- (id) init:(VOOSMP_SRC_CHUNK_INFO *)pValue
{
    if (NULL == pValue) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        _type = pValue->nType;
        
        if (NULL != pValue->szRootUrl) {
            _rootURL = [[NSString alloc] initWithFormat:@"%s", pValue->szRootUrl];
        }
        
        if (NULL != pValue->szUrl) {
            _URL = [[NSString alloc] initWithFormat:@"%s", pValue->szUrl];
        }
        
        _startTime = pValue->ullStartTime;
        _duration = pValue->ullDuration;
        _timeScale = pValue->ullTimeScale;
    }
    
    return self;
}

- (void) dealloc
{
    if (nil != _rootURL) {
        [_rootURL release];
        _rootURL = nil;
    }
    
    if (nil != _URL) {
        [_URL release];
        _URL = nil;
    }
    
    [super dealloc];
}

@end
