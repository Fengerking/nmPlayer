#import "SummaryPage.h"


@implementation SummaryPage

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
        m_successText = [[NSTextField alloc] initWithFrame:NSMakeRect(0, rtBox.size.height - textHeight,
                                                                           rtBox.size.width,
                                                                           textHeight)];
        [m_successText setEditable:NO];
        [m_successText setBordered:NO];
        [m_successText setDrawsBackground:NO];
        [m_successText setAutoresizingMask:NSViewWidthSizable|NSViewMinYMargin];
        [m_successText setStringValue:@"Install successfully"];
        [m_boxView addSubview:m_successText];
        
    }
    
    return ret;
}

@end