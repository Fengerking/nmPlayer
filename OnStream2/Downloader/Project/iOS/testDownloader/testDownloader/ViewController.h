//
//  ViewController.h
//  testDownloader
//
//  Created by Jim Lin on 10/14/13.
//  Copyright (c) 2013 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SampleDownloader.h"


@interface ViewController : UIViewController<VOOSMPStreamingDownloaderDelegate>
{
    id<VOOSMPStreamingDownloader> _downloader;
}

//@property (nonatomic, retain) id<VOOSMPDownloader> downloader;

@end


