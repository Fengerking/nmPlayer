#import <Cocoa/Cocoa.h>

@interface BoxView : NSView
{
}

- (id)initWithFrame:(NSRect)frameRect;
- (void)dealloc;

- (void)drawRect:(NSRect)rect;
//- (void)drawImage:(NSImage *)image inFrame:(NSRect)rect;
//- (void)drawLine:(NSPoint)startPoint toPoint:(NSPoint)endPoint;
- (void)drawSquare:(NSRect)rect;

@end
