/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-    			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		voStreamDownload.mm
 
 Contains:	VisualOn test client Stream download mm file
 
 Written by:	Jeff Huang
 
 Change History (most recent first):
 2012-06-07		Jeff			Create file
 *******************************************************************************/

#import "voStreamDownload.h"
#import "voLog.h"

@interface voStreamDownload ()

// Properties that don't need to be seen by the outside world.
@property (nonatomic, retain)   NSInputStream *   networkStream;
@property (nonatomic, retain)   NSOutputStream *  fileStream;
@property (nonatomic, retain)   NSString *        filePath;
@end

@implementation voStreamDownload

@synthesize networkStream = _networkStream;
@synthesize fileStream    = _fileStream;
@synthesize filePath      = _filePath;

- (id) init
{
    self = [super init];
    
    self.networkStream = nil;
    self.fileStream = nil;
    self.filePath = nil;
    
    memset(&_cStreamCB, 0, sizeof(_cStreamCB));
    return self;
}

-(void) dealloc
{
    [self stop];
	[super dealloc];
}

- (void)setDelegate:(id <voStreamDownloadDelegate>)delegate
{
    _StreamDelegate = delegate;
}

-(void) setDelegateCB:(voStreamDownloadListenerInfo *)pInfo
{
    if (NULL == pInfo) {
        return;
    }
    memcpy(&_cStreamCB, pInfo, sizeof(_cStreamCB));
}

-(BOOL) requestDownLoad:(NSInputStream *)pInStream strOutFilePath:(NSString *)strOutFilePath
{
    if (nil == pInStream || (nil == strOutFilePath)) {
        VOLOGE("requstDownLoad NULL pointer");
        return NO;
    }
    
    if ((nil != self.networkStream) || (nil != self.fileStream) || (nil != self.filePath)) {
        VOLOGE("requstDownLoad tap receive twice in a row!");
        return NO;
    }
    
    self.filePath = strOutFilePath;
    
    // Open a stream for the file we're going to receive into.
    NSString *pDirPath = [self.filePath stringByDeletingLastPathComponent];
    NSError *error = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath:pDirPath withIntermediateDirectories:YES attributes:nil error:&error];

    self.fileStream = [NSOutputStream outputStreamToFileAtPath:self.filePath append:NO];
    
    if (nil == self.fileStream)
    {
        VOLOGE("requstDownLoad outputStreamToFileAtPath fail!");
        return NO;
    }
    
    [self.fileStream open];
    
    self.networkStream = pInStream;
    self.networkStream.delegate = self;
    [self.networkStream scheduleInRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode]; //currentRunLoop
    [self.networkStream open];
    
    return YES;
}

- (void)stop
{
    if (self.networkStream != nil) {
        [self.networkStream removeFromRunLoop:[NSRunLoop mainRunLoop] forMode:NSDefaultRunLoopMode];
        self.networkStream.delegate = nil;
        [self.networkStream close];
        self.networkStream = nil;
    }
    if (self.fileStream != nil) {
        [self.fileStream close];
        self.fileStream = nil;
    }
    
    self.filePath = nil;
}

// Shuts down the connection and displays the result (statusString == nil) 
// or the error status (otherwise).
- (void)stopWithStatus:(NSString *)statusString
{    
    if (nil != _StreamDelegate) {
        if (nil == statusString) {
            [_StreamDelegate voStreamDownloadRespond:VO_DOWNLOAD_SUCCESS strError:self.filePath];
        }
        else {
            [_StreamDelegate voStreamDownloadRespond:VO_DOWNLOAD_FAIL  strError:statusString];
        }
    }
    
    if (NULL != _cStreamCB.pListener && NULL != _cStreamCB.pUserData) {
        if (nil == statusString) {
            _cStreamCB.pListener(_cStreamCB.pUserData, VO_DOWNLOAD_SUCCESS, [self.filePath UTF8String]);
        }
        else {
            _cStreamCB.pListener(_cStreamCB.pUserData, VO_DOWNLOAD_FAIL, [statusString UTF8String]);
        }
    }
    
    [self stop];
}

- (void) writeToFile:(uint8_t *)pBuffer length:(NSInteger)length
{
    do {
        NSInteger nWrite = [self.fileStream write:pBuffer maxLength:length];
        
        if (nWrite <= 0) {
            [self stopWithStatus:@"FileStream write error"];
            return;
        }
        
        length -= nWrite;
        pBuffer = pBuffer + nWrite;
        
    } while (0 < length);
}

// An NSStream delegate callback that's called when events happen on our
// network stream.
- (void)stream:(NSStream *)aStream handleEvent:(NSStreamEvent)eventCode
{
    if (aStream != self.networkStream) {
        return;
    }
    
    switch (eventCode) {
        case NSStreamEventOpenCompleted:
            VOLOGI("NSStreamEventOpenCompleted");
            break;
        case NSStreamEventHasBytesAvailable:
        {
            uint8_t buffer[32768];
            
            // Pull some data off the network.
            NSInteger length = [self.networkStream read:buffer maxLength:sizeof(buffer)];
            
            // Read end
            if (length == 0) {
                [self stopWithStatus:nil];
                return;
            }

            if (length < 0) {
                [self stopWithStatus:@"NetworkStream read fail"];
            }
            else {
                [self writeToFile:buffer length:length];
            }
        }
            break;
            
        case NSStreamEventEndEncountered:
            [self stopWithStatus:nil];
            break;
            
        case NSStreamEventErrorOccurred:
        {
            VOLOGE("NSStreamEventErrorOccurred:%s", [[[self.networkStream streamError] localizedDescription] UTF8String]);
            NSString *str = [NSString stringWithFormat:@"NSStreamEventErrorOccurred:%d", (int)[[self.networkStream streamError] code]];
            [self stopWithStatus:str];
        }
            break;
            
        // should never happen for the output stream
        case NSStreamEventHasSpaceAvailable:
        default:
            [self stopWithStatus:@"unexpected code"];
            break;
    }
}

@end