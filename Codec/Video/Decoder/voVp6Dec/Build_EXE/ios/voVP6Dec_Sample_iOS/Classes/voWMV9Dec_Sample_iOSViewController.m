//
//  voWMV9Dec_Sample_iOSViewController.m
//  voWMV9Dec_Sample_iOS
//
//  Created by Li Lin on 4/15/11.
//  Copyright 2011 VisualOn. All rights reserved.
//

#import "voWMV9Dec_Sample_iOSViewController.h"
#include "voVP6DecSample.h"

@implementation voWMV9Dec_Sample_iOSViewController



/*
// The designated initializer. Override to perform setup that is required before the view is loaded.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
}
*/



// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
    [super viewDidLoad];
	
	NSString* documentsDir	= [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]; 
	//NSString* inFilePath	= [NSString stringWithFormat:@"%@/AdrenalineRush.41_2621frames.rcv", documentsDir];
	NSString* inFilePath	= [NSString stringWithFormat:@"%@/18023895_AVI_VP6_SP_739K_19s987ms_800x600_30f.vp6", documentsDir];
	char* inFile	= [inFilePath UTF8String];
	
	NSString* outFilePath	= [NSString stringWithFormat:@"%@/18023895_AVI_VP6_SP_739K_19s987ms_800x600_30f.yuv", documentsDir];
	NSString* outSpeedPath	= [NSString stringWithFormat:@"%@/frames_speed.txt", documentsDir];
	char* outFile	= [outFilePath UTF8String];
	char* outSpeedFile	= [outSpeedPath UTF8String];
	
	beginTest_vp6(inFile, outFile,1);
}



/*
// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}
*/

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc {
    [super dealloc];
}

@end
