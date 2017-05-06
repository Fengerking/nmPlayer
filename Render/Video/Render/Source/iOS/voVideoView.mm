//
//  voVideoView.m
//
//  Created by Lin Jun on 09-5-30.
//  Copyright 2009 VisualOn. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "voVideoView.h"

@implementation voVideoView


+(Class) layerClass
{
	return [CAEAGLLayer class];
}


- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    
    if (nil != self) {
        // Initialization code
    }
    return self;
}


- (void)drawRect:(CGRect)rect {
    // Drawing code
}


- (void)dealloc {
    [super dealloc];
}


@end
