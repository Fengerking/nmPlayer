
#import "VOOSMPVerificationInfo.h"

@interface VOOSMPVerificationInfo ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPVerificationInfo

@synthesize dataFlag = _dataFlag;
@synthesize verificationString = _verificationString;
@synthesize responseString = _responseString;

- (id) init:(VO_OSMP_SRC_VERIFICATION_FLAG)dataFlag verificationString:(NSString *)verificationString
{
    if (nil != (self = [super init]))
    {
        self.dataFlag = dataFlag;
        self.verificationString = verificationString;
        self.responseString = nil;
    }
    
    return self;
}

- (void) dealloc
{
    self.verificationString = nil;
    self.responseString = nil;
    
    [super dealloc];
}

@end