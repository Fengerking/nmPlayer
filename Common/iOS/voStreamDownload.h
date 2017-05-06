/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-    			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		voStreamDownload.h
 
 Contains:	VisualOn asynchronism download header file
 
 Written by:	Jeff Huang
 
 Change History (most recent first):
 2012-06-07		Jeff			Create file
 *******************************************************************************/

//#ifdef _VONAMESPACE
//namespace _VONAMESPACE {
//#endif

#import <Foundation/Foundation.h>

typedef enum {
    VO_DOWNLOAD_SUCCESS,
    VO_DOWNLOAD_FAIL
}DOWNLOAD_RESPOND;

typedef void (* voStreamDownloadListener) (void * pUserData, int nCode, const char * strError);
typedef struct 
{
    voStreamDownloadListener   pListener;
    void*			           pUserData;
}voStreamDownloadListenerInfo;

@protocol voStreamDownloadDelegate <NSObject>
@optional
- (void)voStreamDownloadRespond:(DOWNLOAD_RESPOND)nCode strError:(NSString *)strError;
@end


@interface voStreamDownload : NSObject <NSStreamDelegate>
{
    NSInputStream *                 _networkStream;
    NSOutputStream *                _fileStream;
    NSString *                      _filePath;
    id <voStreamDownloadDelegate>   _StreamDelegate;
    voStreamDownloadListenerInfo    _cStreamCB;
}

-(id) init;
-(void)stop;

-(void) setDelegate:(id <voStreamDownloadDelegate>)delegate;
-(void) setDelegateCB:(voStreamDownloadListenerInfo *)pInfo;

-(BOOL) requestDownLoad:(NSInputStream *)pInStream strOutFilePath:(NSString *)strOutFilePath;

@end
    
//#ifdef _VONAMESPACE
//}
//#endif
