//
//  voWMV9Dec_Sample_iOSAppDelegate.h
//  voWMV9Dec_Sample_iOS
//
//  Created by Li Lin on 4/15/11.
//  Copyright 2011 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voWMV9Dec_Sample_iOSViewController;

@interface voWMV9Dec_Sample_iOSAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voWMV9Dec_Sample_iOSViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voWMV9Dec_Sample_iOSViewController *viewController;

@end

