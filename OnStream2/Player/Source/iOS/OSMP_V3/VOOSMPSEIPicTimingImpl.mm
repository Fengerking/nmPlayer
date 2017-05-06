
#import "VOOSMPSEIPicTimingImpl.h"
#import "VOOSMPSEIClockTimeStampImpl.h"

@interface VOOSMPSEIPicTimingImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPSEIPicTimingImpl

@synthesize cpbDpbDelaysPresentFlag = _cpbDpbDelaysPresentFlag;
@synthesize cpbRemovalDelay = _cpbRemovalDelay;
@synthesize dpbOutputDelay = _dpbOutputDelay;
@synthesize pictureStructurePresentFlag = _pictureStructurePresentFlag;
@synthesize pictureStructure = _pictureStructure;
@synthesize numClockTs = _numClockTs;
@synthesize clock = _clock;

- (id) init:(VOOSMP_SEI_INFO *)pValue
{
    if ((NULL == pValue) || (NULL == pValue->pInfo)) {
        [self release];
        return nil;
    }
    
    VOOSMP_SEI_PIC_TIMING* pPic = (VOOSMP_SEI_PIC_TIMING*)(pValue->pInfo);
    
    if (nil != (self = [super init]))
    {
        _cpbDpbDelaysPresentFlag = pPic->nCpbDpbDelaysPresentFlag;
        _cpbRemovalDelay = pPic->nCpbRemovalDelay;
        _dpbOutputDelay = pPic->nDpbOutputDelay;
        _pictureStructurePresentFlag = pPic->nPictureStructurePresentFlag;
        _pictureStructure = pPic->nPictureStructure;
        _numClockTs = pPic->nNumClockTs;
        
        NSMutableArray* clockArray = [NSMutableArray array];
        
        for (int i = 0; i < 3; ++i) {
            VOOSMPSEIClockTimeStampImpl *pTemp = [VOOSMPSEIClockTimeStampImpl alloc];
            pTemp = [[pTemp init:&((pPic->sClock)[i])] autorelease];
            [clockArray addObject:pTemp];
        }
        
        self.clock = clockArray;
    }
    
    return self;
}

- (void) dealloc
{
    self.clock = nil;
    
    [super dealloc];
}

@end



@interface VOOSMPSEIUserDataUnregisteredImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPSEIUserDataUnregisteredImpl

@synthesize fieldCount = _fieldCount;
@synthesize dataBuffer = _dataBuffer;

- (int) getFieldLength:(int) i
{
    if(i >= [_fieldLength count])
        return 0;
    
    NSNumber* number = [_fieldLength objectAtIndex: i];
    
    if(!number)
        return 0;
    
    return [number integerValue];
}

- (id) init:(VOOSMP_SEI_USER_DATA_UNREGISTERED *)pValue
{
    if (nil != (self = [super init]))
    {
        _fieldLength = [NSMutableArray array];
        
        int nTotalLength = 0;
        for(int n=0; n<pValue->nCount; n++)
        {
            nTotalLength += pValue->nSize[n];
            NSNumber* pTemp = [[NSNumber numberWithInt: pValue->nSize[n]] autorelease];
            [_fieldLength addObject: pTemp];
        }
        
        _fieldCount = pValue->nCount;
        
        if(pValue->pBuffer && nTotalLength>0)
            _dataBuffer = [[NSData alloc] initWithBytes:pValue->pBuffer length:nTotalLength];
    }
    
    return self;
}

- (void) dealloc
{
    if(_fieldLength)
    {
        [_fieldLength release];
        _fieldLength = nil;
    }
    
    if(_dataBuffer)
    {
        [_dataBuffer release];
        _dataBuffer = nil;
    }
    
    [super dealloc];
}

@end
