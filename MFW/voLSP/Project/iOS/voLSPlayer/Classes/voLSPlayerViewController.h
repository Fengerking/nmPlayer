//
//  voLSPlayerViewController.h
//  voLSPlayer
//
//  Created by Lin Jun on 5/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "CvoPlayer.h"


@interface voLSPlayerViewController : UIViewController <CEventDelegate>
{
	CvoPlayer*			m_pPlayer;
	
	IBOutlet UIScrollView*				m_pUIScrollView;
	IBOutlet UILabel*					m_pInfoLabel;
	IBOutlet UILabel*					m_pInfoBuffer;
	
	IBOutlet UIActivityIndicatorView*	m_pIndicator;

	IBOutlet UIButton*					m_pStartBtn;
	IBOutlet UISegmentedControl*		m_pSeg;
}

@property (nonatomic, retain) IBOutlet UIScrollView				*m_pUIScrollView;
@property (nonatomic, retain) IBOutlet UILabel					*m_pInfoLabel;
@property (nonatomic, retain) IBOutlet UILabel					*m_pInfoBuffer;
@property (nonatomic, retain) IBOutlet UIActivityIndicatorView	*m_pIndicator;
@property (nonatomic, retain) IBOutlet UIButton					*m_pStartBtn;
@property (nonatomic, retain) IBOutlet UISegmentedControl		*m_pSeg;

- (IBAction)start:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)pause:(id)sender;
@end

