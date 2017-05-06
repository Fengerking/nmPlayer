#import "SideBar.h"

@implementation SideBar

- (id)initWithFrame:(NSRect)frameRect;
{
    id ret = [super initWithFrame:frameRect];
    if (ret != nil)
    {
        m_introductionText = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 300, 100, 20)];
        [m_introductionText setEditable:NO];
        [m_introductionText setBordered:YES];
        [m_introductionText setDrawsBackground:NO];
        [m_introductionText setAutoresizingMask:NSViewWidthSizable|NSViewMinYMargin];
        [m_introductionText setStringValue:@"Introduce Page"];
        [self addSubview:m_introductionText];
        
        m_summaryText = [[NSTextField alloc] initWithFrame:NSMakeRect(20, 260, 100, 20)];
        [m_summaryText setEditable:NO];
        [m_summaryText setBordered:NO];
        [m_summaryText setDrawsBackground:NO];
        [m_summaryText setAutoresizingMask:NSViewWidthSizable|NSViewMinYMargin];
        [m_summaryText setStringValue:@"Summary Page"];
        [self addSubview:m_summaryText];
    }
    
    return ret;
}

- (void)Step:(int)nStep
{
    switch (nStep)
    {
        case 1:
            [m_introductionText setBordered:YES];
            [m_summaryText setBordered:NO];
            break;
        case 2:
            [m_introductionText setBordered:NO];
            [m_summaryText setBordered:YES];
            break;
    }
}

@end