//
//  voTextInfoEntryView.h
//  DemoPlayer
//
//  Created by chris qian on 5/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifdef _IOS
#import <UIKit/UIKit.h>

@interface voTextInfoEntryView : UIView
{
@private
    int edgeType;
}

#else

#import <Cocoa/Cocoa.h>
@interface voTextInfoEntryView : CALayer
{
@private
    int edgeType;
    NSMutableAttributedString *_pStringToDraw;
    NSMutableAttributedString *_pStringEdge;
}

#endif
- (void)setTextInfo:(NSMutableAttributedString *)pText;
- (void)setEdgeTextInfo:(NSMutableAttributedString *)pText EdgeType:(int)type;

@end
