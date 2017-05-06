
#import "VOOSMPAnalyticsFilter.h"

@interface VOOSMPAnalyticsFilter ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPAnalyticsFilter

@synthesize lastTime = _lastTime;
@synthesize sourceTime = _sourceTime;
@synthesize codecTime = _codecTime;
@synthesize renderTime = _renderTime;
@synthesize jitterTime = _jitterTime;

- (id) init:(int)lastTime sourceTime:(int)sourceTime codecTime:(int)codecTime renderTime:(int)renderTime jitterTime:(int)jitterTime
{
    if (nil != (self = [super init]))
    {
        self.lastTime = lastTime;
        self.sourceTime = sourceTime;
        self.codecTime = codecTime;
        self.renderTime = renderTime;
        self.jitterTime = jitterTime;
    }
    
    return self;
}

@end