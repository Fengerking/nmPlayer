//
//  AppDelegate.h
//  BrowserPluginInstaller
//
//  Created by Shawn on 4/12/13.
//  Copyright (c) 2013 Shawn. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IntroducePage.h"
#import "SummaryPage.h"
#import "SideBar.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    IntroducePage* m_pageIntroduce;
    SummaryPage* m_pageSummary;
    SideBar* m_sideBar;
	NSView * m_sidebarView;
	NSButton * m_nextButton;
	NSButton * m_prevButton;
    NSTask* task;
    NSPipe* pipe;
}

- (void)onNextPage:(id)sender;
- (void)onPrevPage:(id)sender;

@property (assign) IBOutlet NSWindow *window;

@end
