
#import "VOOSMPHTTPDownloadFailureImpl.h"

@interface VOOSMPHTTPDownloadFailureImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPHTTPDownloadFailureImpl

@synthesize URL = _url;
@synthesize reason = _reason;
@synthesize response = _response;

- (id) init
{
    if (nil != (self = [super init]))
    {
        self.URL = nil;
        self.response = nil;
        self.reason = VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX;
    }
    
    return self;
}

- (void) dealloc
{
    self.URL = nil;
    self.response = nil;
    
    [super dealloc];
}

@end
