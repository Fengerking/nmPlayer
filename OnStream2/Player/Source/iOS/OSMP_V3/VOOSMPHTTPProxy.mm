
#import "VOOSMPHTTPProxy.h"

@interface VOOSMPHTTPProxy ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPHTTPProxy

@synthesize proxyHost  = _proxyHost;
@synthesize proxyPort  = _proxyPort;

- (id) init:(NSString *)proxyHost proxyPort:(int)proxyPort
{
    if (nil != (self = [super init]))
    {
        self.proxyHost = proxyHost;
        self.proxyPort = proxyPort;
    }
    
    return self;
}

- (void) dealloc
{
    self.proxyHost = nil;
    
    [super dealloc];
}

@end