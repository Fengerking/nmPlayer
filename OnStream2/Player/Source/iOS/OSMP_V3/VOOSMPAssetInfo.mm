
#import "VOOSMPAssetInfo.h"

@interface VOOSMPAssetIndexImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPAssetIndexImpl

@synthesize videoIndex = _videoIndex;
@synthesize audioIndex = _audioIndex;
@synthesize subtitleIndex = _subtitleIndex;

- (id) init
{
    if (nil != (self = [super init]))
    {
        _videoIndex = -1;
        
        _audioIndex = -1;
        
        _subtitleIndex = -1;
    }
    
    return self;
}

-(void) setVideoIndex:(int)videoIndex
{
    _videoIndex = videoIndex;
}

-(void) setAudioIndex:(int)audioIndex
{
    _audioIndex = audioIndex;
}

-(void) setSubtitleIndex:(int)subtitleIndex
{
    _subtitleIndex = subtitleIndex;
}

@end


@interface VOOSMPAssetInfo ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPAssetInfo

@synthesize assetProperty = _assetProperty;

- (id) init:(VOOSMP_SRC_TRACK_PROPERTY *)pValue;
{
    if (NULL == pValue || 0 > pValue->nPropertyCount || NULL == pValue->ppItemProperties) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        self.assetProperty = [[NSMutableArray alloc] init];
        [self.assetProperty release];
        
        for (int i = 0; i < pValue->nPropertyCount; ++i) {
            VOOSMP_SRC_TRACK_ITEM_PROPERTY *pProperty = (pValue->ppItemProperties)[i];
            
            if (NULL == pProperty->pszProperty) {
                continue;
            }
            
            [self.assetProperty addObject:[NSArray arrayWithObjects:[NSString stringWithFormat:@"%s", pProperty->szKey],
                                           [NSString stringWithUTF8String:pProperty->pszProperty], nil]];
        }
    }
    
    return self;
}

- (void) dealloc
{
    self.assetProperty = nil;

    [super dealloc];
}

- (int) getPropertyCount
{
    if (nil == self.assetProperty) {
        return 0;
    }
    
    return [self.assetProperty count];
}

- (NSString*) getKey:(int)index
{
    if (nil == self.assetProperty
        || (index >= [self.assetProperty count])) {
        return nil;
    }
    
    return [[self.assetProperty objectAtIndex:index] objectAtIndex:0];
}

- (id) getValue:(int)index
{
    if (nil == self.assetProperty
        || (index >= [self.assetProperty count])) {
        return nil;
    }
    
    return [[self.assetProperty objectAtIndex:index] objectAtIndex:1];
}

@end