
#import "voOSEvent.h"

@interface voOSEvent ()
// Properties that don't need to be seen by the outside world.
@end

@implementation voOSEvent

@synthesize eventID = _eventID;
@synthesize param1 = _param1;
@synthesize param2 = _param2;
@synthesize obj = _obj;

- (id) init:(int)eventID param1:(int)param1 param2:(int)param2 obj:(id)obj;
{
    if (nil != (self = [super init]))
    {
        self.eventID = eventID;
        self.param1 = param1;
        self.param2 = param2;
        self.obj = obj;
    }
    
    return self;
}

- (void) dealloc
{
    self.obj = nil;
    
    [super dealloc];
}

@end
