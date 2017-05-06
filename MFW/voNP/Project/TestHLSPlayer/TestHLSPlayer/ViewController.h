//
//  ViewController.h
//  TestHLSPlayer
//
//  Created by Lin Jun on 9/8/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#import "PlayerView.h"
#include "voNPWrap.h"
#include "CSrcCenter.h"

@interface ViewController : UIViewController
{
    bool            bRunning;
    
    IBOutlet UILabel*       playingTimeLabel;
    IBOutlet UILabel*       leftTimeLabel;
	
	IBOutlet UIButton*      playButton;
    IBOutlet UIButton*      stopButton;
    
    IBOutlet UISlider*      slider;
    id                      timeObserver;
    
    IBOutlet UISwitch*      switcher;
    IBOutlet UISwitch*      switcherLoop;
    
    CSrcCenter*         m_pSrc;
    void*               m_hPlayer;
    VO_NP_WRAPPER_API   m_funcPlayer;
}
@property (nonatomic, retain) IBOutlet PlayerView *playerView;

// event callback
int voPlayerEvent(void * pUserData, int nID, void * pParam1, void * pParam2);
- (int)handlePlayerEvent:(int)nID pram1:(void*)pParam1 pram2:(void*)pParam2;
- (int)onEOS;

// read callback
- (int) handleReadVideo:(VONP_BUFFERTYPE*) pBuffer;
- (int) handleReadAudio:(VONP_BUFFERTYPE*) pBuffer;


- (IBAction)play:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)seekTo:(id)sender;
- (bool)isRunning;

@end
