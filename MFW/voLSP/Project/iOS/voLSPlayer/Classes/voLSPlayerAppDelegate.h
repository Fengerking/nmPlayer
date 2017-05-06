//
//  voLSPlayerAppDelegate.h
//  voLSPlayer
//
//  Created by Lin Jun on 5/5/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voLSPlayerViewController;

@interface voLSPlayerAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voLSPlayerViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voLSPlayerViewController *viewController;

@end

