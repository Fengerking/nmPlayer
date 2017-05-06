//
//  voSubtitleProperty.m
//  voOSEng
//
//  Created by qian_siyao on 3/11/13.
//
//

#import "voSubtitleProperty.h"
#import <CoreText/CoreText.h>
#include "voLog.h"
//static const NSArray *fontNameBoldList = [NSArray arrayWithObjects:@"Courier-Bold",@"TimesNewRomanPS-BoldMT",@"Helvetica-Bold",@"Arial-BoldMT",@"Papyrus",@"Zapfino",@"AppleGothic",nil];
//static const NSArray *fontNameItalicList = [NSArray arrayWithObjects:@"Courier-Oblique",@"TimesNewRomanPS-ItalicMT",@"Helvetica-Oblique",@"Arial-ItalicMT",@"Papyrus",@"Zapfino",@"AppleGothic",nil];
//static const NSArray *fontNameBoldItalicList = [NSArray arrayWithObjects:@"Courier-BoldOblique",@"TimesNewRomanPS-BoldItalicMT",@"Helvetica-BoldOblique",@"Arial-BoldItalicMT",@"Papyrus",@"Zapfino",@"AppleGothic",nil];

@interface voSubtitleProperty ()

@end
@implementation voSubtitleProperty

- (id)init
{
    self = [super init];
    if (self) {
    }
    return self;
}

#pragma mark set text info

+ (NSArray *) getNameListArray {
    
    static dispatch_once_t pred;
    static NSArray *pFontNameListArray = nil;
    dispatch_once(&pred, ^{
        pFontNameListArray = [[NSArray arrayWithObjects:@"Courier",@"TimesNewRomanPSMT",@"Helvetica",@"ArialMT",@"ChalkboardSE-Regular",@"Zapfino",@"AppleGothic", nil] retain];
        });
    
    return pFontNameListArray;
}

+ (NSArray *) getNameBoldList {
    static dispatch_once_t pred;
    static NSArray *pFontNameBoldList = nil;
    dispatch_once(&pred, ^{
        pFontNameBoldList = [[NSArray arrayWithObjects:@"Courier-Bold",@"TimesNewRomanPS-BoldMT",@"Helvetica-Bold",@"Arial-BoldMT",@"ChalkboardSE-Bold",@"Zapfino",@"AppleGothic",nil] retain];
    });
    
    return pFontNameBoldList;
}

+ (NSArray *) getFontNameItalicList {
    
    static dispatch_once_t pred;
    static NSArray *pFontNameItalicList = nil;
    dispatch_once(&pred, ^{
        pFontNameItalicList = [[NSArray arrayWithObjects:@"Courier-Oblique",@"TimesNewRomanPS-ItalicMT",@"Helvetica-Oblique",@"Arial-ItalicMT",@"ChalkboardSE-Regular",@"Zapfino",@"AppleGothic",nil] retain];
    });
    
    return pFontNameItalicList;
}

+ (NSArray *) getFontNameBoldItalicList {
    
    static dispatch_once_t pred;
    static NSArray *pFontNameBoldItalicList = nil;
    dispatch_once(&pred, ^{
        pFontNameBoldItalicList = [[NSArray arrayWithObjects:@"Courier-BoldOblique",@"TimesNewRomanPS-BoldItalicMT",@"Helvetica-BoldOblique",@"Arial-BoldItalicMT",@"ChalkboardSE-Bold",@"Zapfino",@"AppleGothic",nil] retain];
    });
    
    return pFontNameBoldItalicList;
}

//- (int)getColorInt:(voSubtitleRGBAColor)color
//{
//    return 65536 *color.nBlue + 256 * color.nGreen +color.nRed;
//}
//- (UIColor *)getColor:(int)nColor Opacity:(int)nOpacity
//{
//    int r = 0,g = 0,b = 0;
//    [self getColorRGB:nColor Red:&r Green:&g Blue:&b];
//
//    return [UIColor colorWithRed:(CGFloat)r/255.0 green:(CGFloat)g/255.0 blue:(CGFloat)b/255.0 alpha:(CGFloat)nOpacity/255.0];
//}

+ (CGRect)getCGRect:(voViewRect)rect
{
    CGRect cRect;
    cRect.origin.x = rect.left;
    cRect.origin.y = rect.top;
    cRect.size.width = rect.right - rect.left;
    cRect.size.height = rect.bottom - rect.top;
    return cRect;
}

+ (voViewRect)getRect:(CGRect)cRect
{
    voViewRect rect;
    rect.left = cRect.origin.x;
    rect.top = cRect.origin.y;
    rect.right = cRect.size.width + rect.left;
    rect.bottom = cRect.size.height + rect.top;
    return rect;
}

+ (bool)isEqualFloatValue:(float)a Compare:(float)b
{
    const static float V_RANGE = 0.000001;
    if (((a - b) > -V_RANGE) && ((a - b) < V_RANGE) ) {
        return true;
    }
    return false;
}

+ (int)bytes2int:(VO_U8)b{
    int s=(unsigned char)b;
    //NSLog(@"s:%d",s);
    return s;
}

+ (void)getColorRGB:(int)color Red:(int *)red Green:(int *)green Blue:(int *)blue
{
    if (red == NULL || green == NULL || blue == NULL) {
        return;
    }
    int colRemainder = 0;
    *blue = color /  65536;
    colRemainder = color % 65536;
    *green = colRemainder / 256;
    *red = colRemainder % 256;
}

#ifdef _IOS
+ (UIColor *)getColor:(voSubtitleRGBAColor)color
{
    return [UIColor colorWithRed:(CGFloat)[self bytes2int:color.nRed]/255.0 green:(CGFloat)[self bytes2int:color.nGreen]/255.0 blue:(CGFloat)[self bytes2int:color.nBlue]/255.0 alpha:(CGFloat)[self bytes2int:color.nTransparency]/255.0];
}
#else
+ (NSColor *)getColor:(voSubtitleRGBAColor)color
{
    return [NSColor colorWithCalibratedRed:(CGFloat)[self bytes2int:color.nRed]/255.0 green:(CGFloat)[self bytes2int:color.nGreen]/255.0 blue:(CGFloat)[self bytes2int:color.nBlue]/255.0 alpha:(CGFloat)[self bytes2int:color.nTransparency]/255.0];
}
#endif

+ (NSString *)getFontNameWithStyle:(voSubtitleFontStyle)style Bold:(BOOL)bold Italic:(BOOL)italic
{
    int fontIndex = 0;
    switch (style) {
        case FontStyle_Courier:
        case FontStyle_Monospaced:
        case FontStyle_Monospaced_with_serifs:
        case FontStyle_Default_Monospaced_without_serifs:
            fontIndex = 0;
            break;
        case FontStyle_Times_New_Roman:
            fontIndex = 1;
            break;
        case FontStyle_Helvetica:
        case FontStyle_Default_Proportionally_spaced_with_serifs:
        case FontStyle_SansSerif:
            fontIndex = 2;
            break;
        case FontStyle_Arial:
            fontIndex = 3;
            break;
        case FontStyle_Dom:
            fontIndex = 4;
            break;
        case FontStyle_Coronet:
            fontIndex = 5;
            break;
        case FontStyle_Gothic:
            fontIndex = 6;
            break;
        default:
            VOLOGI("Font Style is :%d, ui set to default.",style);
            fontIndex = 2;
            break;
    }
    NSArray *pArray = nil;
    
    if (bold && !italic) {
        pArray = [voSubtitleProperty getNameBoldList];
    }
    else if (!bold && italic) {
        pArray = [voSubtitleProperty getFontNameItalicList];
    }
    else if (bold && italic)
    {
        pArray = [voSubtitleProperty getFontNameBoldItalicList];
    }
    else{
        pArray = [voSubtitleProperty getNameListArray];
    }
    
    if (pArray == nil || [pArray count] == 0) {
        return nil;
    }
    
    return [pArray objectAtIndex:fontIndex];
}

//- (int)checkFontName:(NSString *)pFontName
//{
//    if (pFontName == nil) {
//        return 0;
//    }
//    int nName = 0;
//    NSArray *pArray = [pFontNameList componentsSeparatedByString:@","];
//    
//    if (![self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_NAME]) {
//        return nName;
//    }
//    
//    for (NSString *fontName in pArray)
//    {
//        if ([fontName hasPrefix:[pFontName substringToIndex:2]]) {
//            //   NSLog(@"c enter fontName %@",fontName);
//            return nName;
//        }
//        nName += 1;
//    }
//    return 0;
//}

+ (int)checkfontSize:(NSString *)fontName String:(NSString *)pString Height:(int)height
{
    if (fontName == nil || pString == nil) {
        return 0;
    }
#ifdef _IOS
    CGRect rx = [UIScreen mainScreen].bounds;
#else
    NSRect rx = [NSScreen mainScreen].frame;
#endif
    if (height <= 0 || height > rx.size.height) {
        return 0;
    }
    int fontSize = 1;
    int fontHeight = 0;
    int lastFontHeight = 0;
    bool bIsFontSize = false;
    while (!bIsFontSize) {
#ifdef _IOS
        fontHeight = [pString sizeWithFont:[UIFont fontWithName:fontName size:fontSize]].height;
#else
        NSFont *textFont = [NSFont fontWithName:fontName size:fontSize];
        NSDictionary *attributes = [[NSDictionary alloc] initWithObjectsAndKeys:textFont, NSFontAttributeName, nil];
        fontHeight = [pString sizeWithAttributes:attributes].height;
        [attributes release];
        attributes = nil;
#endif
        if (fontHeight > height && lastFontHeight <= height)
        {
            bIsFontSize = true;
        }
        else
        {
            fontSize += 1;
            lastFontHeight = fontHeight;
        }
    }
    return fontSize;
}

+ (int)checkWhiteSpace:(NSString *)pString Wrap:(int)nWrap
{
    if (pString == nil || [pString length] <= nWrap) {
        return 0;
    }
    
    int nMove = 0;
    while (nWrap > 0) {
        
        if ([[pString substringWithRange:NSMakeRange(nWrap, 1)] isEqualToString:@" "]) {
            break;
        }
        nMove += 1;
        nWrap -= 1;
    }
    return nMove;
}

+ (int)checkTextRowWidth:(NSMutableAttributedString *)pAttribtedString Width:(double *)width BreakIndex:(int *)index Width:(float)fBiggestWidth
{
    float fMoveWidth = 0.0;
    if (pAttribtedString == nil || width == NULL || index == NULL) {
        return -1;
    }
    if (pAttribtedString != nil) {
        CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)pAttribtedString);
        *width = CTLineGetTypographicBounds(line, NULL, NULL, NULL);
        *width += 8.0;
        CFRelease(line);
    }
    if (*width <= fBiggestWidth) {
        return -1;
    }
    else {
        NSMutableString *pstring = [pAttribtedString mutableString];
        if (pstring == nil) {
            return -1;
        }
        if ([pstring length] == 0) {
            return -1;
        }
        int nWrap = fBiggestWidth / (*width / [pstring length]);
        if (nWrap % 2 != 0) {
            nWrap -= 1;
        }
        int nMove = [self checkWhiteSpace:pstring Wrap:nWrap];
        if (nMove == nWrap) {
            nMove = 0;
        }
        *index = nWrap - nMove;
        
        fMoveWidth = *width - fBiggestWidth + (*width / [pstring length]) * (nMove + 2);
        *width = fBiggestWidth;
    }
    return fMoveWidth;
}

CGColorRef CreateDeviceRGBColor(CGFloat r, CGFloat g, CGFloat b, CGFloat a)
{
    CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
    r = r / 255.0;
    g = g / 255.0;
    b = b / 255.0;
    a = a / 255.0;
    CGFloat comps[] = {r, g, b, a};
    CGColorRef color = CGColorCreate(rgb, comps);
    CGColorSpaceRelease(rgb);
    return color;
}


+ (CGColorRef)getCGColor:(voSubtitleRGBAColor)color
{
    return CreateDeviceRGBColor(color.nRed, color.nGreen, color.nBlue, color.nTransparency);
}

+ (void)releaseCGColor:(CGColorRef)color
{
    if(color)
        CGColorRelease(color);
}


- (void)dealloc
{
    [super dealloc];
}
@end
