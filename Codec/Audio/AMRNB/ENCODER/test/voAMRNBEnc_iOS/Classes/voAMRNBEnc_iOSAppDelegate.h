//
//  voAMRNBEnc_iOSAppDelegate.h
//  voAMRNBEnc_iOS
//
//  Created by Li Lin on 3/22/12.
//  Copyright 2012 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voAMRNBEnc_iOSViewController;

@interface voAMRNBEnc_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voAMRNBEnc_iOSViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voAMRNBEnc_iOSViewController *viewController;

@end

