#import "BoxView.h"


@implementation BoxView

- (id)initWithFrame:(NSRect)frameRect
{
	return [super initWithFrame:frameRect];
}

- (void)dealloc
{

	[super dealloc];
}

#pragma mark Drawing

- (void)drawRect:(NSRect)rect
{	
	[super drawRect:rect];

	NSRect currentFrame = [self frame];	
	currentFrame.origin.x =0;
	currentFrame.origin.y =0;
	
	[self drawSquare:currentFrame];
}

//- (void)drawImage:(NSImage *)image inFrame:(NSRect)rect
//{
//	NSSize size;
//	NSPoint position;
//	
//	if (image)
//	{
//		size = [image size];
//		
//		if (size.height > rect.size.height) size.height = rect.size.height;
//		if (size.width > rect.size.width) size.width = rect.size.width;
//		
//		position.x = NSMidX(rect) - (NSInteger)(size.width / 2);
//		position.y = NSMidY(rect) - (NSInteger)(size.height / 2);
//		
//		NSRect fromRect = NSMakeRect(0, 0, 0, 0);
//		fromRect.size = [image size];
//		
//		rect.size = size;
//		rect.origin = position;
//		
//		float fraction = 1.0;
//
//		[image drawInRect:rect fromRect:fromRect operation:NSCompositeSourceOver fraction:fraction];
//	}
//}

//- (void)drawLine:(NSPoint)startPoint toPoint:(NSPoint)endPoint
//{
//  float alpha = 0.6;
//
//	NSBezierPath * pLine = [NSBezierPath bezierPath];
//  
//	[pLine setLineCapStyle:NSSquareLineCapStyle];	
//	[pLine setLineWidth:1];
//
//	[pLine moveToPoint:startPoint];
//	[pLine lineToPoint:endPoint];
//	[pLine closePath];
//  
//	[[NSColor colorWithCalibratedRed:0.0 green:1.0 blue:0.0 alpha:alpha] setStroke];
//	[pLine stroke];
//}

- (void)drawSquare:(NSRect)rect
{
	[[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:0.5] setFill];	 
	[NSBezierPath fillRect:rect];

	[[NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:0.85] setStroke];	 
	[NSBezierPath strokeRect:rect];
}

@end
