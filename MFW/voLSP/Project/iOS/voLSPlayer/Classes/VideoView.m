//
//  VideoView.m
//
//  Created by Lin Jun on 09-5-30.
//  Copyright 2009 VisualOn. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "VideoView.h"

@implementation VideoView


+(Class) layerClass
{
	//printf("layer Class\n");
	return [CAEAGLLayer class];
}


- (id)initWithFrame:(CGRect)frame {
    if (self = [super initWithFrame:frame]) {
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
