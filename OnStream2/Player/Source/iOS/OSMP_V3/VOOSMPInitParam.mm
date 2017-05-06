
#import "VOOSMPInitParam.h"

@interface VOOSMPInitParam ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPInitParam

@synthesize context     = _context;
@synthesize libraryPath = _libraryPath;
@synthesize fileSize = _fileSize;

- (id) init
{
    if (nil != (self = [super init]))
    {
    }
    
    return self;
}

- (void) dealloc
{
    self.context = nil;
    self.libraryPath = nil;
    
    [super dealloc];
}


@end