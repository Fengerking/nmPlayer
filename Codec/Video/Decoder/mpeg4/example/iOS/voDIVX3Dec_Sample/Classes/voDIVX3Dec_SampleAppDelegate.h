//
//  voDIVX3Dec_SampleAppDelegate.h
//  voDIVX3Dec_Sample
//
//  Created by Li Lin on 1/7/12.
//  Copyright 2012 VisualOn. All rights reserved.
//

#import <UIKit/UIKit.h>

@class voDIVX3Dec_SampleViewController;

@interface voDIVX3Dec_SampleAppDelegate : NSObject <UIApplicationDelegate> {
    UIWindow *window;
    voDIVX3Dec_SampleViewController *viewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet voDIVX3Dec_SampleViewController *viewController;

@end

