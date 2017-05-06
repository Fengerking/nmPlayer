//
//  voAMRWBEnc_iOSAppDelegate.h
//  voAMRWBEnc_iOS
//
//  Created by Li Lin on 3/22/12.
//  Copyright 2012 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voAMRWBEnc_iOSViewController;

@interface voAMRWBEnc_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voAMRWBEnc_iOSViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voAMRWBEnc_iOSViewController *viewController;

@end

