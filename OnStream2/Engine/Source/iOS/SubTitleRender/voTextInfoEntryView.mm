//
//  voTextInfoEntryView.m
//  DemoPlayer
//
//  Created by chris qian on 5/17/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "voTextInfoEntryView.h"
#import <CoreText/CoreText.h>
#ifdef _IOS
#import <QuartzCore/QuartzCore.h>
#endif


@interface voTextInfoEntryView()
{
}
@property (nonatomic, retain)    NSMutableAttributedString *m_pStringToDraw;
@property (nonatomic, retain)    NSMutableAttributedString *m_pStringEdge;
@end

@implementation voTextInfoEntryView
@synthesize m_pStringToDraw = _pStringToDraw;
@synthesize m_pStringEdge = _pStringEdge;

#ifdef _IOS
- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.m_pStringToDraw = nil;
        self.m_pStringEdge = nil;
        edgeType = 0;
    }

    return self;
}
#else
- (id)init
{
    self = [super init];
    if (self) {
        self.m_pStringToDraw = nil;
        self.m_pStringEdge = nil;
        edgeType = 0;
    }
    
    return self;
}
#endif


//- (UIColor *)getColor:(int)nColor Opacity:(int)nOpacity
//{
//    int r = 0,g = 0,b = 0;
//    int colRemainder = 0;
//    b = nColor /  65536;
//    colRemainder = nColor % 65536;
//    g = colRemainder / 256;
//    r = colRemainder % 256;
//    
//    return [UIColor colorWithRed:(CGFloat)r/255.0 green:(CGFloat)g/255.0 blue:(CGFloat)b/255.0 alpha:(CGFloat)nOpacity/255.0];
//}
//
//- (NSString *)getFontName:(int)index Bold:(BOOL)bold Italic:(BOOL)italic
//{
//    int nFontIndex = 0;
//    NSArray *pArray = nil;
//    
//    if (bold && !italic) {
//        pArray = [pFontNameBoldList componentsSeparatedByString:@","];
//    }
//    else if (!bold && italic) {
//        pArray = [pFontNameItalicList componentsSeparatedByString:@","];
//    }
//    else if (bold && italic)
//    {
//        pArray = [pFontNameBoldItalicList componentsSeparatedByString:@","];
//    }
//    else{
//        pArray = [pFontNameList componentsSeparatedByString:@","];
//    }
//    if (index < [pArray count]) {
//        nFontIndex = index;
//    }
//    return [pArray objectAtIndex:nFontIndex];
//}
//
////- (void)getAttributedString:(NSString *)pText InfoArray:(NSArray *)pArray
//- (void)getAttributedString:(NSArray *)pArray
//{
//    
////    NSDate* tmpStartData = [NSDate date];
//    int nBegin = 0;
//    
//    [self.m_pStringToDraw beginEditing];
//    
//    int count = [pArray count];
//    for (int i = 0; i < count; i++) {
//        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
//        NSValue *value = [pArray objectAtIndex:i];
//        TextInfoEntry entry;
//        [value getValue:&entry];
//        
//        
//        CGColorRef color = [[self getColor:entry.fontColor Opacity:entry.fontOpacity] CGColor];
//        [self.m_pStringToDraw addAttribute:(id)(kCTForegroundColorAttributeName)
//                             value:(id)color
//                             range:NSMakeRange(nBegin, entry.stringLengh)];
//        
//
//        NSString *pFontName = [self getFontName:entry.fontIndex Bold:entry.bold Italic:entry.italic];
//        //        CTFontRef fontType = CTFontCreateWithName((CFStringRef)[NSString stringWithUTF8String:entry.pFontName], entry.fontSize, NULL);
//        CTFontRef fontType = CTFontCreateWithName((CFStringRef)pFontName, entry.fontSize, NULL);
//
//        [self.m_pStringToDraw addAttribute:(id)kCTFontAttributeName
//                             value:(id)fontType
//                             range:NSMakeRange(nBegin, entry.stringLengh)];
//        CFRelease(fontType);
//        
//        long number = 1;
//        
//        CFNumberRef num = CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt8Type,&number);
//        
//        [self.m_pStringToDraw addAttribute:(id)kCTKernAttributeName
//                             value:(id)num
//                             range:NSMakeRange(nBegin,entry.stringLengh)];
//        
//        CFRelease(num);
//        
//        NSNumber *underline;
//        if (entry.underLine) {
//            underline = [NSNumber numberWithInt:kCTUnderlineStyleSingle];
//        }
//        else {
//            underline = [NSNumber numberWithInt:kCTUnderlineStyleNone];
//        }
//        [self.m_pStringToDraw addAttribute:(id)kCTUnderlineStyleAttributeName 
//                             value:underline
//                             range:NSMakeRange(nBegin, entry.stringLengh)];
//        
//        if (entry.edgeType != 0) {
//            
//            CGFloat widthValue = 0;
//            switch (entry.edgeType) {
//                case 1:
//                    widthValue = -2.0;
//                    break;
//                case 2:
//                    widthValue = 2;
//                    break;
//                case 3:
//                    widthValue = 5;
//                    break;
//                case 4:
//                    widthValue = -5;
//                default:
//                    break;
//            }
//            
//            CFNumberRef strokeWidth = CFNumberCreate(NULL,kCFNumberFloatType,&widthValue);
//            [self.m_pStringToDraw addAttribute:(id)kCTStrokeWidthAttributeName 
//                                 value:(id)strokeWidth
//                                 range:NSMakeRange(nBegin, entry.stringLengh)];
//            if (entry.edgeType != 3) {
//                CGColorRef strokeColor = [[self getColor:entry.edgeColor Opacity:entry.edgeOpacity] CGColor];
//                [self.m_pStringToDraw addAttribute:(id)kCTStrokeColorAttributeName 
//                                        value:(id)strokeColor
//                                        range:NSMakeRange(nBegin, entry.stringLengh)];
//            }
//            
//            CFRelease(strokeWidth);
//        }
//        
//        nBegin += entry.stringLengh;
////        
////        //        CTTextAlignment alignment;
////        //        switch (entry.alignment) {
////        //            case 0:
////        //                alignment = kCTLeftTextAlignment;
////        //                break;
////        //            case 1:
////        //                alignment = kCTRightTextAlignment;
////        //                break;
////        //            case 2:
////        //                alignment = kCTCenterTextAlignment;
////        //                break;
////        //            default:
////        //                break;
////        //        }
////        //        CTParagraphStyleSetting alignmentStyle;
////        //        alignmentStyle.spec=kCTParagraphStyleSpecifierAlignment;
////        //        alignmentStyle.valueSize=sizeof(alignment);
////        //        alignmentStyle.value=&alignment;
////        //        
////        //        CTParagraphStyleSetting settings[]={alignmentStyle};
////        //        CTParagraphStyleRef paragraphStyle = CTParagraphStyleCreate(settings, sizeof(settings));
////        //        
////        //        [stringToDraw addAttribute:(id)kCTParagraphStyleAttributeName
////        //                       value:(id)paragraphStyle
////        //                       range:NSMakeRange(nBegin, entry.stringLengh)];
////        
////        //CFRelease(color);
//        [pool release];
//        pool = nil;
//    }
//    [self.m_pStringToDraw endEditing];
//    
////    double deltaTime = [[NSDate date] t，himeIntervalSinceDate:tmpStartData];
////    NSLog(@">>>>>>>>>>cost time render = %f \r\n", deltaTime * 1000);
//    
//    return;
//}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
#ifdef _IOS
- (void)drawRect:(CGRect)rect
{
    
    if (self.m_pStringToDraw == nil) {
        [self setBackgroundColor:[UIColor clearColor]];
        return;
    }
    
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSaveGState(context);
    CGContextTranslateCTM(context, 0.0, self.bounds.size.height);
    CGContextScaleCTM(context, 1, -1.0);
    
    CGFloat ascent,descent,leading, fHeight;

    CTLineRef line2 = nil;
    if (self.m_pStringEdge != nil) {
        line2 = CTLineCreateWithAttributedString((CFAttributedStringRef)self.m_pStringEdge);
        CTLineGetTypographicBounds(line2, &ascent, &descent, &leading);
        fHeight = ascent + descent;
        fHeight /= 4.0;
        if (edgeType == 1) {
            CGContextSetTextPosition(context, 3.0, fHeight + fHeight * 0.1);
        }
        else if (edgeType == 2)
        {
            CGContextSetTextPosition(context, 5.0, fHeight - fHeight * 0.1);
        }
        else
        {
            CGContextSetTextPosition(context, 5.5, fHeight - fHeight * 0.1);
        }
        
        
        CTLineDraw(line2, context);
    }

    
    CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)self.m_pStringToDraw);
    
    CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
    fHeight = ascent + descent;
    CGContextSetTextPosition(context, 4.0, fHeight / 4.0);
    
    CTLineDraw(line, context);
    
    CGContextRestoreGState(context);
    CFRelease(line);
    if (line2 != nil) {
        CFRelease(line2);
    }

}
#else
- (void)drawInContext:(CGContextRef)ctx
{
    if (self.m_pStringToDraw == nil) {
        [self setBackgroundColor:[NSColor clearColor].CGColor];
        return;
    }
    CGContextSaveGState(ctx);
    CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)self.m_pStringToDraw);
    CGFloat ascent,descent,leading;
    CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
    CGFloat fHeight = ascent + descent;
    CGContextSetTextPosition(ctx, 4.0, fHeight / 4.0);
    
    CTLineDraw(line, ctx);
    CGContextRestoreGState(ctx);
    CFRelease(line);

}
#endif
//- (void)setTextInfo:(NSString *)pText InfoArray:(NSArray *)pArray Width:(double *)width
//{
//    if (pText == nil || pArray == nil) {
//        return;
//    }
//    self.m_pStringToDraw = nil;
//    self.m_pStringToDraw = [[NSMutableAttributedString  alloc] initWithString:pText];
//    [self.m_pStringToDraw release];
//    
//    //[self getAttributedString:pText InfoArray:pArray];
////    [self performSelectorInBackground:@selector(getAttributedString:) withObject:pArray];
//    [self getAttributedString:pArray];
//    if (self.m_pStringToDraw != nil) {
//        CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)self.m_pStringToDraw);
//        CGFloat ascent,descent,leading;
//        *width = CTLineGetTypographicBounds(line, &ascent, &descent, &leading);
//        *width += 8.0;
//        CFRelease(line);
//
//    }
////    printf("width: %f,ascent:%f,desent:%f,leading:%f\r\n",width,ascent,desent,leading);
//}

- (void)setTextInfo:(NSMutableAttributedString *)pText
{
    self.m_pStringToDraw = pText;
}

- (void)setEdgeTextInfo:(NSMutableAttributedString *)pText EdgeType:(int)type
{
    self.m_pStringEdge = pText;
    edgeType = type;
}

-(void)dealloc
{	
    self.m_pStringToDraw = nil;
    self.m_pStringEdge = nil;
	[super dealloc];
}

@end
