//
//  ViewController.m
//  testDownloader
//
//  Created by Jim Lin on 10/14/13.
//  Copyright (c) 2013 VisualOn. All rights reserved.
//

#import "ViewController.h"
#import "voAdaptiveStreamHLS.h"

@interface ViewController ()

@end



@implementation ViewController

//@synthesize downloader = _downloader;

void* voGetModuleAdapterFunc(char *pszApiName)
{
    if (0 == strcmp(pszApiName, "voGetAdaptiveStreamHLSAPI")) {
		return (void *)voGetAdaptiveStreamHLSAPI;
	}
    return NULL;
}

- (VO_OSMP_RETURN_CODE) onVOStreamingDownloaderEvent:(VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID)nID param1:(int)param1 param2:(int)param2 pObj:(void *)pObj;
{
    NSLog(@"Event ID %x", nID);
    
    if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_OPEN_COMPLETE)
    {
        
    }
    else if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_OK)
    {
        
    }
    else if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_END)
    {
        
    }
    else if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_PROGRAM_INFO_CHANGE)
    {
        
    }
    else if(nID == VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_UPDATE)
    {
        
    }
    
    return VO_OSMP_ERR_NONE;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
    
    VOOSMPStreamingDownloaderInitParam* param = [[VOOSMPStreamingDownloaderInitParam alloc] init];
    [param setLibraryPath: nil];
    _downloader = [[SampleDownloader alloc] init:self initParam:param];
    [_downloader getAudioCount];

    [param release];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/"];
    [_downloader open:@"http://10.2.68.7:8082/hls/multibitrate/troy5677_clear_10min/hls.m3u8" flag:0 localDir:filePath];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)dealloc
{
    [_downloader release];
    
    [super dealloc];
}

@end
