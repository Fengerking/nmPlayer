
#import "VOOSMPAnalyticsInfoImpl.h"

@interface VOOSMPAnalyticsInfoImpl ()
// Properties that don't need to be seen by the outside world.
@end

@implementation VOOSMPAnalyticsInfoImpl

@synthesize lastTime          = _lastTime;
@synthesize sourceDropNum     = _sourceDropNum;
@synthesize codecDropNum      = _codecDropNum;
@synthesize renderDropNum     = _renderDropNum;
@synthesize decodedNum        = _decodedNum;
@synthesize renderNum         = _renderNum;
@synthesize sourceTimeNum     = _sourceTimeNum;
@synthesize codecTimeNum      = _codecTimeNum;
@synthesize renderTimeNum     = _renderTimeNum;
@synthesize jitterNum         = _jitterNum;
@synthesize codecErrorsNum    = _codecErrorsNum;
@synthesize codecErrors       = _codecErrors;
@synthesize CPULoad           = _CPULoad;
@synthesize frequency         = _frequency;
@synthesize maxFrequency      = _maxFrequency;
@synthesize worstDecodeTime   = _worstDecodeTime;
@synthesize worstRenderTime   = _worstRenderTime;
@synthesize averageDecodeTime = _averageDecodeTime;
@synthesize averageRenderTime = _averageRenderTime;
@synthesize totalCPULoad      = _totalCPULoad;
@synthesize playbackDuration  = _playbackDuration;
@synthesize totalSourceDropNum  = _totalSourceDropNum;
@synthesize totalCodecDropNum   = _totalCodecDropNum;
@synthesize totalRenderDropNum  = _totalRenderDropNum;
@synthesize totalDecodedNum     = _totalDecodedNum;
@synthesize totalRenderedNum    = _totalRenderedNum;


- (id) init:(VOOSMP_PERFORMANCE_DATA *)pValue
{
    if (NULL == pValue) {
        [self release];
        return nil;
    }
    
    if (nil != (self = [super init]))
    {
        _lastTime          = pValue->nLastTime;
        _sourceDropNum     = pValue->nSourceDropNum;
        _codecDropNum      = pValue->nCodecDropNum;
        _renderDropNum     = pValue->nRenderDropNum;
        _decodedNum        = pValue->nDecodedNum;
        _renderNum         = pValue->nRenderNum;
        _sourceTimeNum     = pValue->nSourceTimeNum;
        _codecTimeNum      = pValue->nCodecTimeNum;
        _renderTimeNum     = pValue->nRenderTimeNum;
        _jitterNum         = pValue->nJitterNum;
        _codecErrorsNum    = pValue->nCodecErrorsNum;
        _codecErrors       = NULL;
        if ((NULL != pValue->nCodecErrors) && (0 < pValue->nCodecErrorsNum)) {
            _codecErrors = new int[pValue->nCodecErrorsNum];
            memcpy(_codecErrors, pValue->nCodecErrors, sizeof(int) * pValue->nCodecErrorsNum);
        }
        _CPULoad           = pValue->nCPULoad;
        _frequency         = pValue->nFrequency;
        _maxFrequency      = pValue->nMaxFrequency;
        _worstDecodeTime   = pValue->nWorstDecodeTime;
        _worstRenderTime   = pValue->nWorstRenderTime;
        _averageDecodeTime = pValue->nAverageDecodeTime;
        _averageRenderTime = pValue->nAverageRenderTime;
        _totalCPULoad      = pValue->nTotalCPULoad;
        _playbackDuration  = pValue->nTotalPlaybackDuration;
        _totalSourceDropNum= pValue->nTotalSourceDropNum;
        _totalCodecDropNum = pValue->nTotalCodecDropNum;
        _totalRenderDropNum= pValue->nTotalRenderDropNum;
        _totalDecodedNum   = pValue->nTotalDecodedNum;
        _totalRenderedNum  = pValue->nTotalRenderNum;
    }
    
    return self;
}

- (void) dealloc
{
    if (NULL != _codecErrors) {
        delete []_codecErrors;
        _codecErrors = NULL;
    }
    
    [super dealloc];
}

@end