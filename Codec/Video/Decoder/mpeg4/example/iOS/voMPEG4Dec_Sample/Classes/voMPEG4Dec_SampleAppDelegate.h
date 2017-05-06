//
//  voMPEG4Dec_SampleAppDelegate.h
//  voMPEG4Dec_Sample
//
//  Created by Li Lin on 12/28/11.
//  Copyright 2011 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voMPEG4Dec_SampleViewController;

@interface voMPEG4Dec_SampleAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voMPEG4Dec_SampleViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voMPEG4Dec_SampleViewController *viewController;

@end

