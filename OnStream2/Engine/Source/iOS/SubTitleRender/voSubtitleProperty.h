//
//  voSubtitleProperty.h
//  voOSEng
//
//  Created by qian_siyao on 3/11/13.
//
//

#import <Foundation/Foundation.h>
#import "voSubtitleType.h"
#import "voRenderObjectInfo.h"
#import <QuartzCore/QuartzCore.h>
#ifdef _IOS
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

@interface voSubtitleProperty : NSObject
{

}

+ (NSArray *) getNameListArray;
+ (NSArray *) getNameBoldList;
+ (NSArray *) getFontNameItalicList;
+ (NSArray *) getFontNameBoldItalicList;
+ (CGRect)getCGRect:(voViewRect)rect;
+ (voViewRect)getRect:(CGRect)cRect;
+ (bool)isEqualFloatValue:(float)a Compare:(float)b;
+ (int)bytes2int:(VO_U8)b;
+ (void)getColorRGB:(int)color Red:(int *)red Green:(int *)green Blue:(int *)blue;
#ifdef _IOS
+ (UIColor *)getColor:(voSubtitleRGBAColor)color;
#else
+ (NSColor *)getColor:(voSubtitleRGBAColor)color;
#endif
+ (NSString *)getFontNameWithStyle:(voSubtitleFontStyle)style Bold:(BOOL)bold Italic:(BOOL)italic;
+ (int)checkfontSize:(NSString *)fontName String:(NSString *)pString Height:(int)height;
+ (int)checkWhiteSpace:(NSString *)pString Wrap:(int)nWrap;
+ (int)checkTextRowWidth:(NSMutableAttributedString *)pAttribtedString Width:(double *)width BreakIndex:(int *)index Width:(float)fBiggestWidth;
+ (CGColorRef)getCGColor:(voSubtitleRGBAColor)color;
+ (void)releaseCGColor:(CGColorRef)color;
@end
