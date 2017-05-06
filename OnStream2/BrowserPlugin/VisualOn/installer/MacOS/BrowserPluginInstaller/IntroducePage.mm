#import "IntroducePage.h"


@implementation IntroducePage

- (id)initWithFrame:(NSRect)frameRect;
{
    id ret = [super initWithFrame:frameRect];
    if (ret != nil)
    {
        // Content box
        NSRect rtBox = NSMakeRect(10, 20, frameRect.size.width-20, frameRect.size.height-50);
        m_boxView = [[BoxView alloc] initWithFrame:rtBox];
        [m_boxView setAutoresizesSubviews:YES];
        [m_boxView setAutoresizingMask:NSViewWidthSizable|NSViewHeightSizable];
        [self addSubview:m_boxView];
        
        // Text
        const int textHeight = 50;
        m_introductionText = [[NSTextField alloc] initWithFrame:NSMakeRect(0, rtBox.size.height - textHeight,
                                                                           rtBox.size.width,
                                                                           textHeight)];
        [m_introductionText setEditable:NO];
        [m_introductionText setBordered:NO];
        [m_introductionText setDrawsBackground:NO];
        [m_introductionText setAutoresizingMask:NSViewWidthSizable|NSViewMinYMargin];
        [m_introductionText setStringValue:@"Introduce Page"];
        [m_boxView addSubview:m_introductionText];
        
    }
    
    return ret;
}

@end