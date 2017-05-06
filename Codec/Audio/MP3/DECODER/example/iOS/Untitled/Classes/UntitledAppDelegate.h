//
//  UntitledAppDelegate.h
//  Untitled
//
//  Created by Li Lin on 6/29/11.
//  Copyright 2011 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UntitledAppDelegate : NSObject <UIApplicationDelegate> {
    
    UIWindow *window;
    UINavigationController *navigationController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet UINavigationController *navigationController;

@end

