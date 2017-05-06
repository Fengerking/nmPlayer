
#import "VOOSMPImageDataImpl.h"

@interface VOOSMPImageDataImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPImageDataImpl

@synthesize imageType = _imageType;
@synthesize imageWidth = _imageWidth;
@synthesize imageHeight = _imageHeight;
@synthesize imageSize = _imageSize;
@synthesize imageData = _imageData;

- (id) init:(VOOSMP_IMAGE_DATA *)pValue
{
    if (NULL == pValue) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        _imageType = (VO_OSMP_IMAGE_TYPE)(pValue->nType);
        _imageWidth = pValue->nWidth;
        _imageHeight = pValue->nHeight;
        _imageSize = pValue->nSize;
        
        if (NULL != pValue->pPicData) {
            self.imageData = [[NSData alloc] initWithBytes:pValue->pPicData length:pValue->nSize];
            [self.imageData release];
        }
    }
    
    return self;
}

- (void) dealloc
{
    self.imageData = nil;
    
    [super dealloc];
}

@end
