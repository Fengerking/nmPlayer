//
//  AppDelegate.m
//  BrowserPluginInstaller
//
//  Created by Shawn on 4/12/13.
//  Copyright (c) 2013 Shawn. All rights reserved.
//

#import "AppDelegate.h"


@implementation AppDelegate

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    NSRect whiteBoxRect = NSMakeRect(150, 50, [[self window] frame].size.width - 150, [[self window] frame].size.height - 50);
    m_pageIntroduce = [[IntroducePage alloc] initWithFrame:whiteBoxRect];
    [[[self window] contentView] addSubview:m_pageIntroduce];
    
    m_pageSummary = [[SummaryPage alloc] initWithFrame:whiteBoxRect];
    [[[self window] contentView] addSubview:m_pageSummary];
    [m_pageSummary setHidden:YES];
    
    // Sidebar
    m_sideBar = [[SideBar alloc] initWithFrame:NSMakeRect(0, 50, 150, [[self window] frame].size.height - 50)];
    [m_sideBar setAutoresizesSubviews:YES];
    [m_sideBar setAutoresizingMask:NSViewHeightSizable];
    [[[self window] contentView] addSubview:m_sideBar];

     
    // Next button
    m_nextButton = [[NSButton alloc] initWithFrame:NSMakeRect(504, 12, 120, 26)];
    [m_nextButton setTitle:@"Install"];
    [m_nextButton setBezelStyle:NSRoundedBezelStyle];
    [m_nextButton setAutoresizingMask:NSViewMinXMargin|NSViewMaxYMargin];
    //[m_nextButton setHidden:YES];
    [m_nextButton setTarget:self];
    [m_nextButton setAction:@selector(onNextPage:)];
    [[[self window] contentView] addSubview:m_nextButton];
    
    // Previous button
    m_prevButton = [[NSButton alloc] initWithFrame:NSMakeRect(384, 12, 120, 26)];
    [m_prevButton setTitle:@"Prev"];
    [m_prevButton setBezelStyle:NSRoundedBezelStyle];
    [m_prevButton setHidden:YES];
    [m_prevButton setAutoresizingMask:NSViewMinXMargin|NSViewMaxYMargin];
    [m_prevButton setTarget:self];
    [m_prevButton setAction:@selector(onPrevPage:)];
    [[[self window] contentView] addSubview:m_prevButton];

    [[[self window] contentView] setDefaultButtonCell:[m_nextButton cell]];
}

- (BOOL) Install
{
    NSError* error = 0;
    NSString* toPath = [NSHomeDirectory() stringByAppendingString:@"/Library/Internet Plug-Ins/voBrowserPlugin.plugin\0"];
    NSString* fromPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/voBrowserPlugin.bundle\0"];
    if (![[NSFileManager defaultManager] removeItemAtPath:toPath error:&error] && error.code != NSFileNoSuchFileError)
    {
        NSLog(@"Error copying files: %@", [error localizedDescription]);
        return FALSE;
    }
    
    if (![[NSFileManager defaultManager] copyItemAtPath:fromPath toPath:toPath error:&error])
    {
        NSLog(@"Error copying files: %@", [error localizedDescription]);
        return FALSE;
    }
    
//    NSString* profilePath = [NSHomeDirectory() stringByAppendingString:@"/.profile\0"];
//    NSString* strNewEnv = @"\r\nexport PATH=~/Library/Internet\\ Plug-Ins/voBrowserPlugin.plugin/Contents/Resources:$PATH";
//    NSString *contentOfFile = [NSString stringWithContentsOfFile:profilePath encoding:NSUTF8StringEncoding error:nil];
//    
//    if (contentOfFile)
//    {
//        if ([contentOfFile rangeOfString:strNewEnv].location == NSNotFound)
//        {
//            NSString* strNew = [contentOfFile stringByAppendingString:strNewEnv];
//            if (![strNew writeToFile:profilePath atomically:YES encoding:NSUTF8StringEncoding error:&error])
//            {
//                NSLog(@"Error changing profile: %@", [error localizedDescription]);
//                return FALSE;
//            }
//        }
//    }
//    else
//    {
//        strNewEnv = [@"#!/bin/bash" stringByAppendingString:strNewEnv];
//        if (![strNewEnv writeToFile:profilePath atomically:YES encoding:NSUTF8StringEncoding error:&error])
//        {
//            NSLog(@"Error changing profile: %@", [error localizedDescription]);
//            return FALSE;
//        }
//    }
    
    return TRUE;
}

- (void)onNextPage:(id)sender
{
    if ([[m_nextButton title] isEqual: @"Install"])
    {        
        [self Install];
        
        [m_pageSummary setHidden:NO];
        [m_pageIntroduce setHidden:YES];
        [m_nextButton setTitle:@"Close"];
        [m_sideBar Step:2];
    }
    else if ([[m_nextButton title] isEqual: @"Close"])
    {
        [NSApp terminate:self];
    }
}

- (void)onPrevPage:(id)sender
{
}

@end
