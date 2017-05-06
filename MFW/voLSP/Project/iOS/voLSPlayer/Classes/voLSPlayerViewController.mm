//
//  voLSPlayerViewController.m
//  voLSPlayer
//
//  Created by Lin Jun on 5/5/11.
//  Copyright 2011 VisualOn. All rights reserved.
//

#import "voLSPlayerViewController.h"

#import <OpenGLES/EAGL.h>

@implementation voLSPlayerViewController

@synthesize m_pUIScrollView;
@synthesize m_pInfoLabel;
@synthesize m_pInfoBuffer;
@synthesize m_pIndicator;
@synthesize m_pStartBtn;
@synthesize m_pSeg;

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

// Implement loadView to create a view hierarchy programmatically, without using a nib.
- (void)loadView {
    [super loadView];
    m_pPlayer = NULL;
}


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad 
{
    [super viewDidLoad];
    
	[m_pUIScrollView setHidden:YES];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    if (NULL == m_pPlayer) {
        m_pPlayer = new CvoPlayer(self.view);
        m_pPlayer->SetPlayerListener(self);
        m_pPlayer->Open();
        
        EAGLContext *pValue = NULL;
        m_pPlayer->GetParam(VOMP_PID_VIDEO_RENDER_CONTEXT, &pValue);
        
        if (NULL != pValue) {
            NSLog(@"Open EAGLContext:%d", (int32_t)pValue);
        }
        
        m_pPlayer->SetParam(VOMP_PID_LICENSE_TEXT, (void *)"VOTRUST_#20120430Tarasystem");
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *filePath = [[paths objectAtIndex:0] stringByAppendingString:@"/voVidDec.dat"];
        m_pPlayer->SetParam(VOMP_PID_LICENSE_FILE_PATH, (void *)[filePath UTF8String]);
    }
}

- (IBAction)start:(id)sender
{
	if (NULL != m_pPlayer)
	{
		bool bFile = true;
		if (self.m_pSeg.selectedSegmentIndex == 0) {
			bFile = false;
		}
		
		VOMP_STATUS status;
		m_pPlayer->GetStatus(&status);
		if (VOMP_STATUS_PAUSED == status)
		{
			m_pPlayer->Run(bFile);
			return;
		}
			
		if (VOMP_STATUS_RUNNING == status)
		{
			return;
		}
		
#if TARGET_IPHONE_SIMULATOR
		m_pPlayer->SetDrawArea(0, 0, self.view.frame.size.height, self.view.frame.size.width);
#else
		int deviceType = m_pPlayer->GetDeviceType();
		if(deviceType == IPHONE_4)
			m_pPlayer->SetDrawArea(0, 0, 960, 640);
		else if(deviceType == IPAD_1G)
		{
			 m_pPlayer->SetDrawArea(0, 0, 1024, 768);
		}
		else if(deviceType == IPAD_2G)
		{
			m_pPlayer->SetDrawArea(0, 0, 1024, 768);
		}
		else
		{
			m_pPlayer->SetDrawArea(0, 0, 480, 320);
		}
#endif
		m_pPlayer->SetDataSource(NULL);
		m_pPlayer->Run(bFile);
		
		EAGLContext *pValue = NULL;
		m_pPlayer->GetParam(VOMP_PID_VIDEO_RENDER_CONTEXT, &pValue);
		
		if (NULL != pValue) {
			NSLog(@"Start EAGLContext:%d", (int32_t)pValue);
		}
		
		[m_pSeg setHidden:YES];
		[m_pStartBtn setHidden:YES];
		[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
	}
}

- (IBAction)stop:(id)sender
{
	if(m_pPlayer)
		m_pPlayer->Stop();
	
	[m_pIndicator stopAnimating];
	[m_pSeg setHidden:NO];
	[m_pStartBtn setHidden:NO];
	[[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

- (IBAction)pause:(id)sender
{
}

- (IBAction)switchProgram0:(id)sender
{
	if(m_pPlayer)
	{
		m_pPlayer->SwitchProgram(0);
	}
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
{
	if (YES == [m_pUIScrollView isHidden]) {
		[m_pUIScrollView setHidden:NO];
	}
	else {
		[m_pUIScrollView setHidden:YES];
	}
}

// Override to allow orientations other than the default portrait orientation.
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
    // Return YES for supported orientations
	if (interfaceOrientation == UIInterfaceOrientationLandscapeRight) {
		return YES;
	}
	
	return NO;
}

- (void)didReceiveMemoryWarning {
	// Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
	// Release any cached data, images, etc that aren't in use.
}

- (void)viewDidUnload {
	// Release any retained subviews of the main view.
	// e.g. self.myOutlet = nil;
}


- (void)dealloc 
{
	if(m_pPlayer)
	{
		m_pPlayer->Stop();
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
	
	[m_pUIScrollView release];
	[m_pIndicator release];
	[m_pSeg release];
	[m_pStartBtn release];
	[m_pInfoLabel release];
	[m_pInfoBuffer release];
	
    [super dealloc];
}

- (int)handleEvent:(int)nID withParam1:(void *)pParam1 withParam2:(void * )pParam2
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	if(nID == VOMP_CB_VideoStartBuff)
	{
		//NSLog(@"updateByEvent Buffering start");
		[m_pIndicator performSelectorOnMainThread:@selector(startAnimating) withObject:nil waitUntilDone:NO];
	}
	else if(nID == VOMP_CB_VideoStopBuff)
	{
		//NSLog(@"updateByEvent Buffing stopped");
		[m_pIndicator performSelectorOnMainThread:@selector(stopAnimating) withObject:nil waitUntilDone:NO];
	}
	else if (NO == [m_pUIScrollView isHidden]) {
		if (nID == VOMP_CB_VideoFrameRate)
		{
			int decoder	=	*(int*)pParam1;
			int render	= *(int*)pParam2;
			
			NSString *strRate = [NSString stringWithFormat:@"decoder:%02d, render:%02d", decoder, render];
			[m_pInfoLabel performSelectorOnMainThread:@selector(setText:) withObject:strRate waitUntilDone:NO];
		}
		else if (nID == VOMP_CB_RenderAudio) {
			if (m_pPlayer)
			{
				int iVideoBuffer = 0;
				m_pPlayer->GetParam(VOMP_PID_VIDEO_BUFFTIME, &iVideoBuffer);
				
				int iAudioBuffer = 0;
				m_pPlayer->GetParam(VOMP_PID_AUDIO_BUFFTIME, &iAudioBuffer);
				
				NSString *strBuffer = [NSString stringWithFormat:@"VB:%06d, AB:%06d", iVideoBuffer, iAudioBuffer];
				[m_pInfoBuffer performSelectorOnMainThread:@selector(setText:) withObject:strBuffer waitUntilDone:NO];
			}
		}
		else {
			
		}
	}
	else {}
	
	[pool release];
	
	return VOMP_ERR_None;
}

@end
