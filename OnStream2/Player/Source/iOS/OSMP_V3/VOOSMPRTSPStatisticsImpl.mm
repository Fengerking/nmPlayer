
#import "VOOSMPRTSPStatisticsImpl.h"

@interface VOOSMPRTSPStatisticsImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPRTSPStatisticsImpl

@synthesize packetReceived      = _packetReceived;
@synthesize packetDuplicated    = _packetDuplicated;
@synthesize packetLost          = _packetLost;
@synthesize packetSent          = _packetSent;
@synthesize averageJitter       = _averageJitter;
@synthesize averageLatency      = _averageLatency;

- (id) init:(VOOSMP_SRC_RTSP_STATS *)pValue
{
    if (NULL == pValue) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        _packetReceived = pValue->uPacketRecved;
        _packetDuplicated = pValue->uPacketDuplicated;
        _packetLost = pValue->uPacketLost;
        _packetSent = pValue->uPacketSent;
        _averageJitter = pValue->uAverageJitter;
        _averageLatency = pValue->uAverageLatency;
    }
    
    return self;
}

- (void) dealloc
{
    [super dealloc];
}

@end