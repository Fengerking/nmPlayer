//
//  voADPCMTest_iOSAppDelegate.h
//  voADPCMTest_iOS
//
//  Created by Li Lin on 2/8/12.
//  Copyright 2012 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voADPCMTest_iOSViewController;

@interface voADPCMTest_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voADPCMTest_iOSViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voADPCMTest_iOSViewController *viewController;

@end

