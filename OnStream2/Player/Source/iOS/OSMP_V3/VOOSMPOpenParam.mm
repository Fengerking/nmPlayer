
#import "VOOSMPOpenParam.h"

@interface VOOSMPOpenParam ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPOpenParam

@synthesize fileSize    = _fileSize;

- (id) init
{
    if (nil != (self = [super init]))
    {
    }
    
    return self;
}

@end