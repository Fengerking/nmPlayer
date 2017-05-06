
#import "VOOSMPSEIClockTimeStampImpl.h"

@interface VOOSMPSEIClockTimeStampImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPSEIClockTimeStampImpl

@synthesize clockTimestampFlag = _clockTimestampFlag;
@synthesize ctType = _ctType;
@synthesize nuitFieldBasedFlag = _nuitFieldBasedFlag;
@synthesize countingType = _countingType;
@synthesize fullTimestampFlag = _fullTimestampFlag;
@synthesize discontinuityFlag = _discontinuityFlag;
@synthesize countDroppedFlag = _countDroppedFlag;
@synthesize frames = _frames;
@synthesize secondsValue = _secondsValue;
@synthesize minutesValue = _minutesValue;
@synthesize hoursValue = _hoursValue;
@synthesize secondsFlag = _secondsFlag;
@synthesize minutesFlag = _minutesFlag;
@synthesize hoursFlag = _hoursFlag;
@synthesize timeOffset = _timeOffset;

- (id) init:(VOOSMP_CLOCK_TIME_STAMP *)pValue
{
    if (NULL == pValue) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        _clockTimestampFlag = pValue->nClockTimestampFlag;
        _ctType = pValue->nCtType;
        _nuitFieldBasedFlag = pValue->nNuitFieldBasedFlag;
        _countingType = pValue->nCountingType;
        _fullTimestampFlag = pValue->nFullTimestampFlag;
        _discontinuityFlag = pValue->nDiscontinuityFlag;
        _countDroppedFlag = pValue->nCntDroppedFlag;
        _frames = pValue->nFrames;
        _secondsValue = pValue->nSecondsFlag;
        _minutesValue = pValue->nMinutesFlag;
        _hoursValue = pValue->nHoursFlag;
        _secondsFlag = pValue->nSecondsFlag;
        _minutesFlag = pValue->nMinutesFlag;
        _hoursFlag = pValue->nHoursFlag;
        _timeOffset = pValue->nTimeOffset;
    }
    
    return self;
}

- (void) dealloc
{    
    [super dealloc];
}

@end
