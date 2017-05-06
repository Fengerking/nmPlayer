//
//  voH264EncViewController.m
//  voH264EncSampleDemo
//
//  Created by renjie yu on 12-3-16.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "voH264EncViewController.h"
#import "voH264EncSample.h"

@implementation voH264EncViewController

- (void)didReceiveMemoryWarning
{
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    NSString* documentsDir	= [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]; 
	char configFile[512];
    char* workDir	= (char *)[documentsDir UTF8String];
    strcat(workDir,"/h264enc.cfg");
	//strcpy(configFile, workDir);
	//strcat(configFile, "/real.cfg");
	beginTest(1, &workDir);
    [super viewDidLoad];

	// Do any additional setup after loading the view, typically from a nib.
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
	[super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

@end
