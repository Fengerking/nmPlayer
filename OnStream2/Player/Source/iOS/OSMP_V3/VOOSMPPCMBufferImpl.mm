
#import "VOOSMPPCMBufferImpl.h"

@interface VOOSMPPCMBufferImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPPCMBufferImpl

@synthesize timestamp = _timestamp;
@synthesize bufferSize = _bufferSize;
@synthesize buffer = _buffer;

- (id) init:(VOOSMP_PCMBUFFER *)pValue
{
    if (NULL == pValue) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        _timestamp = pValue->nTimestamp;
        _bufferSize = pValue->nBufferSize;
        
        _buffer = nil;
        
        if ((pValue->nBufferSize > 0) && (pValue->pBuffer != NULL)) {
            _buffer = [[NSData alloc] initWithBytes:pValue->pBuffer length:pValue->nBufferSize];
        }
    }
    
    return self;
}

- (void) dealloc
{
    if (nil != _buffer) {
        [_buffer release];
        _buffer = nil;
    }
    
    [super dealloc];
}

@end
