//
//  NSObject+voSubTitleRender.m
//  DemoPlayer
//
//  Created by chris qian on 5/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "voSubTitleRender.h"
#import "voTextInfoEntryView.h"
#import <QuartzCore/QuartzCore.h>
#import <CoreText/CoreText.h>
#import "voSubtitleFunc.h"
#import "voRenderObjectInfo.h"
#import "voSubtitleProperty.h"

#define BASE_PERCENT                              1000
#define PIX_PERCENT                               1000
#define NO_FONT_EQUAL                             999

static const NSString *kEnterViewKey = @"EnterViewKey";
static const NSString *kImageViewKey = @"ImageViewKey";
static const NSString *kTextViewKey = @"TextViewKey";

static const NSArray *prefontNameListArray = [NSArray arrayWithObjects:@"Courier",@"Times New Roman",@"Helvetica",@"Arial",@"Dom",@"Coronet",@"Gothic", nil];

@interface voSubTitleRender ()

// Properties that don't need to be seen by the outside world.
@property (nonatomic, retain)   NSMutableArray * m_pEntryObjects;
#ifdef _IOS
@property (nonatomic, retain)   UIView *m_pParentView;
#else
@property (nonatomic, retain)   CALayer *m_pParentView;
#endif

#ifdef _IOS
- (void)setViewEffect:(voSubtitleDisPlayEffect)effect View:(UIView *)pView Rect:(CGRect)fm;
- (void)showImageView:(NSMutableArray *)pImageArray ParentView:(UIView *)pView;
- (void)showTextView:(NSMutableArray *)pTextArray ParentView:(UIView *)pView;
- (UIView *)showBackgroundView:(voRenderObjectInfo *)pInfo;
#else
- (void)setViewEffect:(voSubtitleDisPlayEffect)effect View:(CALayer *)pView Rect:(CGRect)fm;
- (void)showImageView:(NSMutableArray *)pImageArray ParentView:(CALayer *)pView;
- (void)showTextView:(NSMutableArray *)pTextArray ParentView:(CALayer *)pView;
- (CALayer *)showBackgroundView:(voRenderObjectInfo *)pInfo ZOrder:(int *)nZOrder;
#endif
- (bool)isEnableMark:(int)mark;
- (NSMutableAttributedString *)getTextInfo:(voSubtitleTextInfoEntry *)pTextInfoEntry FontSize:(int)nFontSize EdgeString:(NSMutableAttributedString **)pEdgeSting;
- (voRenderObjectInfo *)loadTextRenderObject:(voSubtitleTextRowInfo *)pTextInfo Numb:(int)numb ParentNumb:(int)parentNumb
                                  ParentRect:(CGRect)parentFrame Wrap:(int *)nWrapIndex;
- (voRenderObjectInfo *)loadImageRenderObject:(SubtitleImageInfo *)pImageInfo Numb:(int)numb ParentNumb:(int)parentNumb;
- (bool)loadRenderObject:(SubtitleInfoEntry *)pIntoEntry Numb:(int)numb;
- (void)showOnMainThread;
- (void) showViews;
- (void)removeOnMainThread;
- (void) removeViews:(BOOL)isWait;
- (BOOL) prepareSubtitle:(voSubtitleInfo *)pInfo;
- (void) setSubtitleShow:(BOOL)show;
- (void) setSettingsToSubInfo;

@end

@implementation voSubTitleRender

@synthesize m_pEntryObjects = _pEntryObjects;
@synthesize m_pParentView = _pParentView;


#pragma mark view effect
#ifdef _IOS
- (void)setViewEffect:(voSubtitleDisPlayEffect)effect View:(UIView *)pView Rect:(CGRect)fm
{
    switch (effect.nEffectType) {
        case 0:
            break;
        case 1:
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:effect.nEffectSpeed];
            pView.alpha = 0;
            [UIView commitAnimations];
            break;
        case 2:
            [UIView beginAnimations:nil context:NULL];
            [UIView setAnimationDuration:effect.nEffectSpeed];
            pView.frame = fm;
            [UIView commitAnimations];
            break;
        default:
            break;
    }
}

- (voSubtitleFontStyle)getFontStyle:(NSString *)pFontName
{
    int nName = NO_FONT_EQUAL;
    voSubtitleFontStyle style = FontStyle_Default;

    for (NSString *fontName in prefontNameListArray)
    {
        if ([fontName hasPrefix:[pFontName substringToIndex:3]]) {
            //   NSLog(@"c enter fontName %@",fontName);
            if (nName == NO_FONT_EQUAL)
            {
                nName = 0;
            }
            break;
        }
        nName += 1;
    }
    if (nName > NO_FONT_EQUAL) {
        nName -= NO_FONT_EQUAL;
    }
    switch (nName) {
        case 0:
            style = FontStyle_Courier;
            break;
        case 1:
            style = FontStyle_Times_New_Roman;
            break;
        case 2:
            style = FontStyle_Helvetica;
            break;
        case 3:
            style = FontStyle_Arial;
            break;
        case 4:
            style = FontStyle_Dom;
            break;
        case 5:
            style = FontStyle_Coronet;
            break;
        case 6:
            style = FontStyle_Gothic;
            break;
        default:
            style = FontStyle_Helvetica;
            break;
    }
    return style;
}

#else
- (void)setViewEffect:(voSubtitleDisPlayEffect)effect View:(CALayer *)pView Rect:(CGRect)fm
{
//    switch (effect.nEffectType) {
//        case 0:
//            break;
//        case 1:
//            [CALayer beginAnimations:nil context:NULL];
//            [CALayer setAnimationDuration:effect.nEffectSpeed];
//            pView.alpha = 0;
//            [CALayer commitAnimations];
//            break;
//        case 2:
//            [CALayer beginAnimations:nil context:NULL];
//            [CALayer setAnimationDuration:effect.nEffectSpeed];
//            pView.frame = fm;
//            [CALayer commitAnimations];
//            break;
//        default:
//            break;
//    }
}
#endif

#pragma mark subtitle
- (bool)isEnableMark:(int)mark
{
    bool bRet = false;
    
    if (mark == VOOSMP_SUBTITLE_MASK_DISABLE_ALL) {
        return bRet;
    }
    
    if((m_cSubTitleSettings.nItemMask & mark) == mark)
    {
        bRet = true;
    }
    return bRet;
}
#ifdef _IOS
- (NSString *)getCurFont:(voSubtitleTextInfoEntry *)pTextInfoEntry
{
    voSubtitleFontStyle style;
    
    if (![self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_NAME]) {
        style = (voSubtitleFontStyle)pTextInfoEntry->stStringInfo.stFontInfo.FontStyle;
    }
    else
    {
        style = [self getFontStyle:[NSString stringWithUTF8String:m_cSubTitleSettings.szFontName]];
    }
    NSString *pString = [voSubtitleProperty getFontNameWithStyle:style Bold:[self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BOLD] ? m_cSubTitleSettings.bFontBold : false Italic:(unsigned char)pTextInfoEntry->stStringInfo.stCharEffect.Italic == 0 ? false : true];
    return pString;
}

- (NSMutableAttributedString *)getTextInfoWithSetting:(NSString *)pText FontSize:(int)nFontSize EdgeString:(NSMutableAttributedString **)pEdgeSting
{
    if (pText == nil || [pText length] == 0) {
        return nil;
    }
    
    
    NSMutableAttributedString *pString = [[[NSMutableAttributedString alloc] initWithString:pText] autorelease];
    
    
    //CGColorRef color = [[voSubtitleProperty getColor:pTextInfoEntry->stStringInfo.stFontInfo.stFontColor] CGColor];
    NSRange range = NSMakeRange(0, [pString length]);
    
    
    voSubtitleFontStyle style = FontStyle_Default;
    if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_NAME]) {
        style = [self getFontStyle:[NSString stringWithUTF8String:m_cSubTitleSettings.szFontName]];
    }

    NSString *pFontName = [voSubtitleProperty getFontNameWithStyle:style Bold:[self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BOLD] ? m_cSubTitleSettings.bFontBold : false Italic:[self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_ITALIC] ? m_cSubTitleSettings.bFontItalic : false];
    
    CTFontRef fontType = CTFontCreateWithName((CFStringRef)pFontName, nFontSize, NULL);
    
    [pString addAttribute:(id)kCTFontAttributeName
                    value:(id)fontType
                    range:range];
    CFRelease(fontType);
    
    int number = 1;
    
    CFNumberRef num = CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt32Type,&number);
    
    [pString addAttribute:(id)kCTKernAttributeName
                    value:(id)num
                    range:range];
    
    CFRelease(num);
    
    if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_UNDERLINE] && m_cSubTitleSettings.bFontUnderline) {
        NSNumber *underline;
        underline = [NSNumber numberWithInt:kCTUnderlineStyleSingle];
        
        [pString addAttribute:(id)kCTUnderlineStyleAttributeName
                        value:underline
                        range:range];
    }
    
    voSubtitleRGBAColor color;
    int nRed = 0,nGreen = 0,nBlue = 0;
    
    if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_EDGE_TYPE]) {
        
        [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nEdgeColor Red:&nRed Green:&nGreen Blue:&nBlue];
        color.nRed = nRed;
        color.nGreen = nGreen;
        color.nBlue = nBlue;
        
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_EDGE_OPACITY]) {
            color.nTransparency = m_cSubTitleSettings.nEdgeOpacity * 255 / 100;
        }
        
        if (m_cSubTitleSettings.nEdgeType == 3) {
            
            CGFloat widthValue = -2.0;
            CFNumberRef strokeWidth = CFNumberCreate(NULL,kCFNumberFloatType,&widthValue);
            [pString addAttribute:(id)kCTStrokeWidthAttributeName
                            value:(id)strokeWidth
                            range:NSMakeRange(0, [pString length])];
            
            
            CGColorRef strokeColor = [voSubtitleProperty getCGColor:color];
            [pString addAttribute:(id)kCTStrokeColorAttributeName
                            value:(id)strokeColor
                            range:range];
            [voSubtitleProperty releaseCGColor:strokeColor];
            
            CFRelease(strokeWidth);
        }
        else if (m_cSubTitleSettings.nEdgeType != 0)
        {
            if (*pEdgeSting != nil) {
                [*pEdgeSting release];
                *pEdgeSting = nil;
            }
            *pEdgeSting = [[NSMutableAttributedString alloc] initWithAttributedString:pString];
            
            CGColorRef strokeColor = [voSubtitleProperty getCGColor:color];
            [*pEdgeSting addAttribute:(id)(kCTForegroundColorAttributeName)
                                value:(id)strokeColor
                                range:NSMakeRange(0, [*pEdgeSting length])];
            [voSubtitleProperty releaseCGColor:strokeColor];
            
        }

    }
    if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_COLOR]) {
        [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nFontColor Red:&nRed Green:&nGreen Blue:&nBlue];
        color.nRed = nRed;
        color.nGreen = nGreen;
        color.nBlue = nBlue;
        
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_OPACITY]) {
            color.nTransparency = m_cSubTitleSettings.nFontOpacity  * 255 / 100;
        }
        else
        {
            color.nTransparency = 255;
        }
        
        CGColorRef fontColor = [voSubtitleProperty getCGColor:color];
        [pString addAttribute:(id)(kCTForegroundColorAttributeName)
                        value:(id)fontColor
                        range:range];
        [voSubtitleProperty releaseCGColor:fontColor];
    }
    else
    {
        CGColorRef fontColor = [UIColor whiteColor].CGColor;
        [pString addAttribute:(id)(kCTForegroundColorAttributeName)
                        value:(id)fontColor
                        range:range];

    }

    //NSLog(@"pString:%@",*pString);
    
    return pString;
}
#endif

- (NSMutableAttributedString *)getTextInfo:(voSubtitleTextInfoEntry *)pTextInfoEntry FontSize:(int)nFontSize EdgeString:(NSMutableAttributedString **)pEdgeSting
{
    if (pTextInfoEntry == NULL) {
        return nil;
    }
    int iLength = pTextInfoEntry->nSize/sizeof(const unichar);
    if (iLength == 0) {
        return nil;
    }
    
    
    NSMutableAttributedString *pString = [[[NSMutableAttributedString alloc] initWithString:[NSString stringWithCharacters:(const unichar *)pTextInfoEntry->pString length:iLength]] autorelease];
    
    
    //CGColorRef color = [[voSubtitleProperty getColor:pTextInfoEntry->stStringInfo.stFontInfo.stFontColor] CGColor];
    NSRange range = NSMakeRange(0, [pString length]);
    
#ifdef _IOS
    NSString *pFontName = [self getCurFont:pTextInfoEntry];
    CTFontRef fontType = CTFontCreateWithName((CFStringRef)pFontName, nFontSize, NULL);

    [pString addAttribute:(id)kCTFontAttributeName
                    value:(id)fontType
                    range:range];
    CFRelease(fontType);
#endif
    int number = 1;
    
    CFNumberRef num = CFNumberCreate(kCFAllocatorDefault,kCFNumberSInt32Type,&number);
    
    [pString addAttribute:(id)kCTKernAttributeName
                    value:(id)num
                    range:range];
    
    CFRelease(num);
    
    NSNumber *underline;
    if ((unsigned char)pTextInfoEntry->stStringInfo.stCharEffect.Underline == 1) {
        underline = [NSNumber numberWithInt:kCTUnderlineStyleSingle];
    }
    else {
        underline = [NSNumber numberWithInt:kCTUnderlineStyleNone];
    }
    [pString addAttribute:(id)kCTUnderlineStyleAttributeName
                    value:underline
                    range:range];
    
    if ((unsigned char)pTextInfoEntry->stStringInfo.stCharEffect.EdgeType == 3) {
        
        CGFloat widthValue = -2.0;
        CFNumberRef strokeWidth = CFNumberCreate(NULL,kCFNumberFloatType,&widthValue);
        [pString addAttribute:(id)kCTStrokeWidthAttributeName
                        value:(id)strokeWidth
                        range:NSMakeRange(0, [pString length])];
        CGColorRef strokeColor = [voSubtitleProperty getCGColor:pTextInfoEntry->stStringInfo.stCharEffect.EdgeColor];
        [pString addAttribute:(id)kCTStrokeColorAttributeName
                        value:(id)strokeColor
                        range:range];
        [voSubtitleProperty releaseCGColor:strokeColor];
        
        CFRelease(strokeWidth);
    }
    else if ((unsigned char)pTextInfoEntry->stStringInfo.stCharEffect.EdgeType != 0)
    {
        if (*pEdgeSting != nil) {
            [*pEdgeSting release];
            *pEdgeSting = nil;
        }
        *pEdgeSting = [[NSMutableAttributedString alloc] initWithAttributedString:pString];
        
        CGColorRef strokeColor = [voSubtitleProperty getCGColor:pTextInfoEntry->stStringInfo.stCharEffect.EdgeColor];
        [*pEdgeSting addAttribute:(id)(kCTForegroundColorAttributeName)
                            value:(id)strokeColor
                            range:NSMakeRange(0, [*pEdgeSting length])];
        [voSubtitleProperty releaseCGColor:strokeColor];
        
    }
    CGColorRef color = [voSubtitleProperty getCGColor:pTextInfoEntry->stStringInfo.stFontInfo.stFontColor];
    [pString addAttribute:(id)(kCTForegroundColorAttributeName)
                    value:(id)color
                    range:range];
    [voSubtitleProperty releaseCGColor:color];
    
    //NSLog(@"pString:%@",*pString);
    
    return pString;
}

#pragma mark load subitle(text, image, background view)
- (voRenderObjectInfo *)loadTextRenderObject:(voSubtitleTextRowInfo *)pTextInfo Numb:(int)numb ParentNumb:(int)parentNumb
                  ParentRect:(CGRect)parentFrame Wrap:(int *)nWrapIndex
{
    if (pTextInfo == NULL || nWrapIndex == NULL)
    {
        return nil;
    }
    voRenderObjectInfo *pInfo = [[[voRenderObjectInfo alloc] init] autorelease];
    if (pInfo == nil) {
        return nil;
    }
    pInfo.viewType = TEXT_VIEW;
    pInfo.viewNumber = numb;
    pInfo.parentViewNumber = parentNumb;
    
    voSubtitleRectInfo *rectInfo = &pTextInfo->stTextRowDes.stDataBox;
    pInfo.viewColor = rectInfo->stRectFillColor;
    
    pInfo.viewZOrder = rectInfo->nRectZOrder;
    
    CGRect frame, fmBegin;
//        printf("------------m_cDrawRect%f, %f, %f, %f \r\n",m_cDrawRect.origin.x, m_cDrawRect.origin.y, m_cDrawRect.size.width, m_cDrawRect.size.height);
    frame.origin.x = rectInfo->stRect.nLeft * m_cDrawRect.size.width / 100 - parentFrame.origin.x +m_cDrawRect.origin.x;
#ifdef _IOS
    frame.origin.y = rectInfo->stRect.nTop * m_cDrawRect.size.height / 100 - parentFrame.origin.y +m_cDrawRect.origin.y + m_fMoveHeight;
#else
    frame.origin.y = (100 - rectInfo->stRect.nBottom) * m_cDrawRect.size.height / 100 - parentFrame.origin.y + m_cDrawRect.origin.y + m_fMoveHeight;
#endif
    frame.size.width = 100;
    frame.size.height = (rectInfo->stRect.nBottom - rectInfo->stRect.nTop) * m_cDrawRect.size.height / 100;
    
//    NSString* strN = nil;
//    if (frame.size.height < m_cDrawRect.size.height) {
//        
//        if (![self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_NAME]) {
//            strN = @"Courier";
//        }
//        else
//        {
//            strN = [NSString stringWithUTF8String:m_cSubTitleSettings.szFontName];
//        }
//        nFontSize = [voSubtitleProperty checkfontSize:[voSubtitleProperty getFontName:strN Bold:false Italic:false] String:@"ABCD" Height:frame.size.height];
//    }
    
    voSubtitleDisPlayEffect *effect = &rectInfo->stRectDisplayEffct;
    if (effect == NULL) {
        return nil;
    }
    if (effect->nEffectType == 2) {
        switch (effect->nEffectDirection) {
            case 0:
                fmBegin.origin.x = 0.0;
                fmBegin.origin.y = frame.origin.y;
                break;
            case 1:
                fmBegin.origin.x = -frame.origin.x;
                fmBegin.origin.y = frame.origin.y;
                break;
            case 2:
                fmBegin.origin.x = frame.origin.x;
                fmBegin.origin.y = 0.0;
                break;
            case 3:
                fmBegin.origin.x = frame.origin.x;
                fmBegin.origin.y = -frame.origin.y;
                break;
            default:
                break;
        }
        fmBegin.size = frame.size;
    }
    else {
        fmBegin = frame;
    }
    //    float fLWidth = pParentView.frame.size.width - fmBegin.origin.x;
    float fLWidth = m_cDrawRect.size.width - fmBegin.origin.x;
    
    //NSLog(@"Text: top:%lu,left:%lu,bottom:%lu",rectInfo->stRect.nTop,rectInfo->stRect.nLeft,rectInfo->stRect.nBottom);
    
    pvoSubtitleTextInfoEntry pSubtitleTextInfoEntry = pTextInfo->pTextInfoEntry;
    pInfo.textViewString = [[NSMutableAttributedString alloc] init];
    [pInfo.textViewString release];
    double stringWidth = 100;
    int nText = 0,  nMaxFontPerent = 0;
    float fHeight = 0;

    //get max font percent.
    while (pSubtitleTextInfoEntry != NULL)
    {
        voSubtitleFontInfo fontInfo = pSubtitleTextInfoEntry->stStringInfo.stFontInfo;
        nMaxFontPerent = nMaxFontPerent > fontInfo.FontSize ? nMaxFontPerent : fontInfo.FontSize;
        
        pSubtitleTextInfoEntry = pSubtitleTextInfoEntry->pNext;
    }
    nMaxFontPerent = nMaxFontPerent - BASE_PERCENT;
    
    pSubtitleTextInfoEntry = pTextInfo->pTextInfoEntry;
    
    int nSize = 0;
    
    while (pSubtitleTextInfoEntry != NULL)
    {
        NSMutableAttributedString *str = nil;
        NSMutableAttributedString *strEdge = nil;
        
        nSize = 0;
        
#ifdef _IOS
        NSString *pFontName = [self getCurFont:pSubtitleTextInfoEntry];
        voSubtitleFontInfo fontInfo = pSubtitleTextInfoEntry->stStringInfo.stFontInfo;
        if (fontInfo.FontSize <= BASE_PERCENT || fontInfo.FontSize > PIX_PERCENT) {
            nSize = [voSubtitleProperty checkfontSize:pFontName String:@"ABCD" Height:frame.size.height];
            switch (fontInfo.FontSize) {
                case 1:
                    nSize -= 1;
                    break;
                case 2:
                    nSize += 1;
                    break;
                default:
                    break;
            }
        }
        else
        {
            fHeight = frame.size.height / nMaxFontPerent * 100.0;
            fHeight *= ((fontInfo.FontSize - BASE_PERCENT) / 100.0);
            nSize = [voSubtitleProperty checkfontSize:pFontName String:@"ABCD" Height:fHeight];
        }
#endif
        
        str = [self getTextInfo:pSubtitleTextInfoEntry FontSize:nSize EdgeString:&strEdge];
        if (str == nil) {
            pSubtitleTextInfoEntry = pSubtitleTextInfoEntry->pNext;
            continue;
        }
        
        nText += 1;
        
        [pInfo.textViewString appendAttributedString:str];
        if (strEdge != nil) {
            if (pInfo.textEdgeViewString == nil) {
                pInfo.textEdgeViewString = [[[NSMutableAttributedString alloc] initWithAttributedString:strEdge] autorelease];
            }
            else
            {
                [pInfo.textEdgeViewString appendAttributedString:strEdge];
            }
            [strEdge release];
            strEdge = nil;
        }
        pSubtitleTextInfoEntry = pSubtitleTextInfoEntry->pNext;
        
    }
    if (nText == 0) {
        pInfo.textViewString = nil;
        pInfo.textEdgeViewString = nil;
        return nil;
    }
    
    //    printf("%s",[pString UTF8String]);
    
    if (*nWrapIndex == 0) {
        fLWidth = fLWidth < 0 ? 0 : fLWidth;
        float moveWidth = [voSubtitleProperty checkTextRowWidth:pInfo.textViewString Width:&stringWidth BreakIndex:nWrapIndex Width:fLWidth];
        if (moveWidth != -1){
            m_fMoveHeight += fmBegin.size.height;
            m_fMoveWidth = moveWidth;
            [pInfo.textViewString deleteCharactersInRange:NSMakeRange(*nWrapIndex, [[pInfo.textViewString mutableString] length] - *nWrapIndex)];
            if (pInfo.textEdgeViewString != nil) {
                [pInfo.textEdgeViewString deleteCharactersInRange:NSMakeRange(*nWrapIndex, [[pInfo.textEdgeViewString mutableString] length] - *nWrapIndex)];
            }
        }
    }
    else {
        [pInfo.textViewString deleteCharactersInRange:NSMakeRange(0, *nWrapIndex)];
        if (pInfo.textEdgeViewString != nil) {
            [pInfo.textEdgeViewString deleteCharactersInRange:NSMakeRange(0, *nWrapIndex)];
        }
        stringWidth = m_fMoveWidth;
    }
    
    if (rectInfo->nRectBorderType == 1) {
        pInfo.viewBorderType = 1;
        pInfo.viewBorderColor = rectInfo->stRectBorderColor;
    }
    
    fmBegin.size.width = stringWidth;
    
    int alignment = pTextInfo->stTextRowDes.nHorizontalJustification;
    if (alignment == 1) {
        fmBegin.origin.x = rectInfo->stRect.nRight * m_cDrawRect.size.width / 100 - parentFrame.origin.x + m_cDrawRect.origin.x - stringWidth;
    }
    else if (alignment == 2) {
//        fmBegin.origin.x = ((rectInfo->stRect.nRight - rectInfo->stRect.nLeft) * m_cDrawRect.size.width / 100 - parentFrame.origin.x + m_cDrawRect.origin.x) / 2 - stringWidth / 2;
        fmBegin.origin.x = (m_cDrawRect.size.width - fmBegin.size.width) / 2  - parentFrame.origin.x + m_cDrawRect.origin.x;
    }
    

    if (fmBegin.origin.y < 0) {
        if (m_fWindowLostHeight == 0) {
            m_fWindowLostHeight = fmBegin.origin.y;
        }
        else
        {
            if (m_fWindowLostHeight > fmBegin.origin.y) {
                m_fWindowLostHeight = fmBegin.origin.y;
            }
        }
        
    }
#ifdef _IOS
    fmBegin.origin.y -= m_fWindowLostHeight;
#else
    fmBegin.origin.y += m_fWindowLostHeight;
#endif
    //NSLog(@"stringWidth:%f",stringWidth);
    pInfo.viewFrame = [voSubtitleProperty getRect:fmBegin];
//    printf("------------fmBegin:%f, %f, %f, %f,pInfo.textViewString:%s \r\n",fmBegin.origin.x, fmBegin.origin.y, fmBegin.size.width, fmBegin.size.height,[[pInfo.textViewString string] UTF8String]);
    pInfo.viewEffect = (*effect).nEffectType;
    
    return pInfo;
}

- (voRenderObjectInfo *)loadImageRenderObject:(SubtitleImageInfo *)pImageInfo Numb:(int)numb ParentNumb:(int)parentNumb
{
    if (pImageInfo == NULL)
    {
        return nil;
    }
    voRenderObjectInfo *pInfo = [[[voRenderObjectInfo alloc] init] autorelease];
    if (pInfo == nil) {
        return nil;
    }
    pInfo.viewType = IMAGE_VIEW;
    pInfo.viewNumber = numb;
    pInfo.parentViewNumber = parentNumb;
    pInfo.imageData = pImageInfo->stImageData;
    
    voSubtitleRectInfo *rectInfo = &pImageInfo->stImageDesp.stImageRectInfo;
    
    pInfo.viewZOrder = rectInfo->nRectZOrder;
    
    if (rectInfo == NULL) {
        return nil;
    }
    CGRect frame;
    if (rectInfo->stRect.nBottom == 0 || rectInfo->stRect.nRight == 0) {
        
        frame.origin.x = 0;
        frame.origin.y = 0;
        frame.size.width =  0;
        frame.size.height = 0;
        pInfo.viewFrame = [voSubtitleProperty getRect:frame];
    }
    else
    {
        frame.origin.x = rectInfo->stRect.nLeft * m_cDrawRect.size.width / 100 + m_cDrawRect.origin.x;
#ifdef _IOS
        frame.origin.y = rectInfo->stRect.nTop * m_cDrawRect.size.height / 100 + m_cDrawRect.origin.y;
#else
        frame.origin.y = (100 - rectInfo->stRect.nBottom) * m_cDrawRect.size.height / 100 + m_cDrawRect.origin.y;
#endif
        frame.size.width = (rectInfo->stRect.nRight - rectInfo->stRect.nLeft) * m_cDrawRect.size.width / 100;
        frame.size.height = (rectInfo->stRect.nBottom - rectInfo->stRect.nTop) * m_cDrawRect.size.height / 100;
        pInfo.viewFrame = [voSubtitleProperty getRect:frame];
    }
    
    return pInfo;
}

- (bool)loadRenderObject:(SubtitleInfoEntry *)pIntoEntry Numb:(int)numb
{
    @synchronized(self){
        if (pIntoEntry == NULL)
        {
            return false;
        }
        voSubtitleDisplayInfo *subtitleDispInfo = &pIntoEntry->stSubtitleDispInfo;
        if (subtitleDispInfo == NULL) {
            return false;
        }
        if (subtitleDispInfo->pImageInfo == NULL && subtitleDispInfo->pTextRowInfo == NULL) {
            return false;
        }
        
        NSMutableDictionary *pDictonary = [[[NSMutableDictionary alloc] init] autorelease];
        if (pDictonary == nil) {
            return false;
        }
        
        voRenderObjectInfo *pInfo = [[[voRenderObjectInfo alloc] init] autorelease];
        if (pInfo == nil) {
            return false;
        }
        
        pInfo.viewType = BACKGROUN_VIEW;
        pInfo.viewNumber = numb;
        pInfo.parentViewNumber = 0;
        
        voSubtitleRectInfo *rect = &pIntoEntry->stSubtitleRectInfo;
        //printf("_______________rect->stRect.nTop:%d, %d, %d, %d \r\n",rect->stRect.nTop, rect->stRect.nLeft, rect->stRect.nRight, rect->stRect.nBottom);
        pInfo.viewColor = rect->stRectFillColor;
        pInfo.viewZOrder = rect->nRectZOrder;
        
        CGRect frame, fmBegin;
        
        frame.origin.x = rect->stRect.nLeft * m_cDrawRect.size.width / 100 + m_cDrawRect.origin.x;
#ifdef _IOS
        frame.origin.y = rect->stRect.nTop * m_cDrawRect.size.height / 100 + m_cDrawRect.origin.y;
#else
        frame.origin.y = (100 - rect->stRect.nBottom) * m_cDrawRect.size.height / 100 + m_cDrawRect.origin.y;
#endif
        frame.size.width = (rect->stRect.nRight - rect->stRect.nLeft) * m_cDrawRect.size.width / 100;
        frame.size.height = (rect->stRect.nBottom - rect->stRect.nTop) * m_cDrawRect.size.height / 100;
        
        //NSLog(@"_______________________parent:%f, %f, %f, %f setDrawRect m_cDrawRect: %f, %f, %f, %f,frame:%f, %f, %f, %f",self.m_pParentView.frame.origin.x, self.m_pParentView.frame.origin.y, self.m_pParentView.frame.size.width, self.m_pParentView.frame.size.height,m_cDrawRect.origin.x, m_cDrawRect.origin.y, m_cDrawRect.size.width, m_cDrawRect.size.height,frame.origin.x, frame.origin.y, frame.size.width, frame.size.height);
        
        voSubtitleDisPlayEffect *effect = &rect->stRectDisplayEffct;
        if (effect->nEffectType == 2) {
            switch (effect->nEffectDirection) {
                case 0:
                    fmBegin.origin.x = 0.0;
                    fmBegin.origin.y = frame.origin.y;
                    break;
                case 1:
                    fmBegin.origin.x = -frame.origin.x;
                    fmBegin.origin.y = frame.origin.y;
                    break;
                case 2:
                    fmBegin.origin.x = frame.origin.x;
                    fmBegin.origin.y = 0.0;
                    break;
                case 3:
                    fmBegin.origin.x = frame.origin.x;
                    fmBegin.origin.y = -frame.origin.y;
                    break;
                default:
                    break;
            }
            fmBegin.size = frame.size;
        }
        else {
            fmBegin = frame;
        }
        if (subtitleDispInfo->pImageInfo != NULL && subtitleDispInfo->pTextRowInfo == NULL) {
            fmBegin = CGRectMake(0, 0, 0, 0);
        }
        
        m_fWindowLostHeight = 0;
        
        
        if (rect->nRectBorderType == 1) {
            pInfo.viewBorderType = 1;
            pInfo.viewBorderColor = rect->stRectBorderColor;
        }
        
        pInfo.viewEffect = (*effect).nEffectType;
        
        pvoSubtitleImageInfo imageInfo = subtitleDispInfo->pImageInfo;
        
        NSMutableArray *pImageArray = [[[NSMutableArray alloc] init] autorelease];
        if (pImageArray == nil) {
            return false;
        }
        int nImageShow = 0;
        while (imageInfo != NULL) {
            voRenderObjectInfo *pImageInfo = [self loadImageRenderObject:imageInfo Numb:nImageShow ParentNumb:numb];
            if (pImageInfo != nil) {
                nImageShow += 1;
                [pImageArray addObject:pImageInfo];
            };
            imageInfo = imageInfo->pNext;
        }
        if ([pImageArray count] != 0) {
            [pDictonary setValue:pImageArray forKey:(NSString *)kImageViewKey];
        }
        
        int nTextShow = 0, textCount = 0, nWarp = 0;
        BOOL isWrap = NO;
        m_fMoveHeight = 0;
        
        pvoSubtitleTextRowInfo textRowInfo = subtitleDispInfo->pTextRowInfo;
        
        NSMutableArray *pTextArray = [[[NSMutableArray alloc] init] autorelease];
        if (pTextArray == nil) {
            return false;
        }
        
        while (textRowInfo != NULL) {
            voRenderObjectInfo *pTextInfo = [self loadTextRenderObject:textRowInfo Numb:nTextShow ParentNumb:numb ParentRect:fmBegin Wrap:&nWarp];
            if (pTextInfo != nil) {
                nTextShow += 1;
                [pTextArray addObject:pTextInfo];
            }
            
            if (isWrap) {
                nWarp = 0;
                isWrap = NO;
            }
            
            if (nWarp == 0) {
                textRowInfo = textRowInfo->pNext;
                m_fMoveWidth = 0;
            }
            else {
                isWrap = YES;
            }
            textCount += 1;
        }
        if ([pTextArray count] != 0) {
            [pDictonary setValue:pTextArray forKey:(NSString *)kTextViewKey];
        }
        
        if (nImageShow != 0 || nTextShow != 0) {
            
#ifdef _IOS
            fmBegin.size.height += m_fMoveHeight;
            fmBegin.origin.y += m_fWindowLostHeight;
            fmBegin.size.height -= m_fWindowLostHeight;
#endif
            pInfo.viewFrame = [voSubtitleProperty getRect:fmBegin];
            
            [pDictonary setValue:pInfo forKey:(NSString *)kEnterViewKey];
            [self.m_pEntryObjects addObject:pDictonary];
        
            return true;
        }
        return false;
    }
}

#pragma mark show subtitle (ios)
void voReleasePixels(void *info, const void *data, size_t size)
{
    if (data != NULL && size > 0) {
        free((unsigned char*)data);
    }
//    NSLog(@"____________________voReleasePixels");
}

#ifdef _IOS
- (void)showImageView:(NSMutableArray *)pImageArray ParentView:(UIView *)pView
{
    if (pView == nil || pImageArray == nil) {
        return;
    }
    
    int count = [pImageArray count];
    if (count == 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        voRenderObjectInfo *pInfo = [pImageArray objectAtIndex:i];
        if (pInfo == nil) {
            continue;
        }
        if (pInfo.viewType == IMAGE_VIEW) {
            
            voSubtitleImageInfoData imageInfo = pInfo.imageData;
            
            if (imageInfo.pPicData == NULL && imageInfo.nSize > 0) {
                continue;
            }
            
            UIImageView *pImageView = [[[UIImageView alloc] init] autorelease];
            if (pImageView == nil) {
                return;
            }
            
            [pImageView setFrame:[voSubtitleProperty getCGRect:pInfo.viewFrame]];
            
            UIImage *pCellImg = nil;
            if (imageInfo.nType == VO_IMAGE_RGBA32) {
                int width =imageInfo.nWidth;
                int height = imageInfo.nHeight;
                int stride = width * 4;
                
                VO_PBYTE pPicData = (VO_PBYTE) malloc (imageInfo.nSize);
                if (pPicData == NULL) {
                    return;
                }
                memset(pPicData, 0, imageInfo.nSize);
                memcpy(pPicData, imageInfo.pPicData,imageInfo.nSize);
                
                CGDataProviderRef provider = CGDataProviderCreateWithData(NULL,
                                                                          pPicData,
                                                                          stride*height, voReleasePixels);
                
                CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
                
                CGImageRef iref = CGImageCreate(width, height, 8, 32, stride,
                                                space, kCGImageAlphaLast | kCGBitmapByteOrder32Big,
                                                provider, NULL, true, kCGRenderingIntentDefault);
                CGDataProviderRelease(provider);
                
                CGColorSpaceRelease(space);
                
                pCellImg = [[[UIImage alloc] initWithCGImage:iref] autorelease];
                
                CGImageRelease(iref);
            }
            else
            {
                NSData *pData = [NSData dataWithBytes:imageInfo.pPicData length:imageInfo.nSize];
                pCellImg = [[[UIImage alloc] initWithData:pData] autorelease];
                
            }
            
            if (pCellImg != nil) {
                
                CGRect fm = pImageView.frame;
                bool bNeeDecrease = false;

                if (fm.size.width == 0 || fm.size.height == 0) {
                    if (pCellImg.size.width <= m_cDrawRect.size.width && pCellImg.size.height <= m_cDrawRect.size.height) {
                        fm.size.width = pCellImg.size.width;
                        fm.size.height = pCellImg.size.height;
                        pImageView.frame = fm;
                        [pImageView setCenter:CGPointMake(self.m_pParentView.frame.size.width / 2.0, self.m_pParentView.frame.size.height / 2.0)];
                    }
                    else
                    {
                        bNeeDecrease = true;
                    }
                    
                }
                if (m_bIsDrawRect || bNeeDecrease) {
                    //NSLog(@"__________m_cDrawRect: %f, %f",m_cDrawRect.size.width, m_cDrawRect.size.height);
                    
                    float ratio = pCellImg.size.width / pCellImg.size.height;
                    float drawRatio = m_cDrawRect.size.width / m_cDrawRect.size.height;
                    if (ratio >= drawRatio) {
                        fm.size.width =  m_cDrawRect.size.width;
                        fm.size.height = fm.size.width / ratio;
                    }
                    else
                    {
                        fm.size.height =  m_cDrawRect.size.height;
                        fm.size.width = fm.size.height * ratio;
                    }
                    //NSLog(@"__________pCellImg: %f, %f, %f, %f, %f, %f",pCellImg.size.width, pCellImg.size.height, ratio, fm.size.width, fm.size.height, fm.size.width/fm.size.height);
                    pImageView.frame = fm;
                    [pImageView setCenter:CGPointMake(self.m_pParentView.frame.size.width / 2.0, self.m_pParentView.frame.size.height / 2.0)];
                    
                    if (m_bIsDrawRect) {
                        [self.m_pParentView setBackgroundColor:[UIColor blackColor]];
                    }
                }
                
                [pImageView setImage:pCellImg];
                [pView insertSubview:pImageView atIndex:pInfo.viewZOrder];
//                [pView addSubview:pImageView];
                
            }
        }
    }
}
 
- (void)showTextView:(NSMutableArray *)pTextArray ParentView:(UIView *)pView
{
    if (pView == nil || pTextArray == nil) {
        return;
    }

    int count = [pTextArray count];
    if (count == 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        voRenderObjectInfo *pInfo = [pTextArray objectAtIndex:i];
        if (pInfo == nil) {
            continue;
        }
        if (pInfo.viewType == TEXT_VIEW) {
            voTextInfoEntryView *pTextView = [[[voTextInfoEntryView alloc] init] autorelease];
            if (pTextView == nil) {
                return;
            }
            
            CGRect fmBegin = [voSubtitleProperty getCGRect:pInfo.viewFrame];
            [pTextView setFrame:fmBegin];
            //printf("------------fmBegin:%f, %f, %f, %f \r\n",fmBegin.origin.x, fmBegin.origin.y, fmBegin.size.width, fmBegin.size.height);
            [pTextView setBackgroundColor:[voSubtitleProperty getColor:pInfo.viewColor]];
            //printf("________________pInfo.textViewString:%s \r\n",[[pInfo.textViewString mutableString] UTF8String]);
            if (pInfo.textEdgeViewString != nil)
            {
                [pTextView setEdgeTextInfo:pInfo.textEdgeViewString EdgeType:m_cSubTitleSettings.nEdgeType];
                pInfo.textEdgeViewString = nil;
            }
            [pTextView setTextInfo:pInfo.textViewString];
            
            if (pInfo.viewBorderType == 1) {
                [pTextView.layer setBorderWidth:1];
                //[pTextView.layer setBorderColor:[voSubtitleProperty getColor:pInfo.viewBorderColor].CGColor];
                CGColorRef color = [voSubtitleProperty getCGColor:pInfo.viewBorderColor];
                [pTextView.layer setBorderColor:color];
                [voSubtitleProperty releaseCGColor:color];
            }
//            [pTextView setHidden:NO];
            [pTextView setNeedsDisplay];
            [pView insertSubview:pTextView atIndex:pInfo.viewZOrder];
//            [pView addSubview:pTextView];
        }
    }

}

- (UIView *)showBackgroundView:(voRenderObjectInfo *)pInfo
{
    if (pInfo == nil) {
        return nil;
    }
    
    UIView *pView = [[[UIView alloc] init] autorelease];
    
    if (pView == nil) {
        return nil;
    }
    [pView setBackgroundColor:[voSubtitleProperty getColor:pInfo.viewColor]];
    [pView setFrame:[voSubtitleProperty getCGRect:pInfo.viewFrame]];
    
    if (pInfo.viewBorderType == 1) {
        [pView.layer setBorderWidth:1];
        CGColorRef color = [voSubtitleProperty getCGColor:pInfo.viewBorderColor];
        [pView.layer setBorderColor:color];
        [voSubtitleProperty releaseCGColor:color];
    }
    [pView setTag:pInfo.viewZOrder];
    [pView setHidden:NO];
    [pView setNeedsDisplay];
    
    return pView;
}


- (void)showOnMainThread {
    @synchronized(self) {
        
        //CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();
        
        [self removeOnMainThread];
        
        if (self.m_pEntryObjects == nil) {
            return;
        }
        
        for (int i = 0; i < [self.m_pEntryObjects count]; i++) {
            
            NSMutableDictionary *pDictonary = [self.m_pEntryObjects objectAtIndex:i];
            if (pDictonary == nil) {
                continue;
            }
            
            if ([pDictonary objectForKey:kEnterViewKey] == nil) {
                continue;
            }
            UIView *pView = [self showBackgroundView:[pDictonary objectForKey:kEnterViewKey]];
            if (pView != nil) {
                [self.m_pParentView insertSubview:pView atIndex:pView.tag];
                
                
                if ([pDictonary objectForKey:kTextViewKey] != nil)
                {
                    [self showTextView:(NSMutableArray *)[pDictonary objectForKey:kTextViewKey] ParentView:pView];
                }
            
                if ([pDictonary objectForKey:kImageViewKey] != nil)
                {
                    [self showImageView:(NSMutableArray *)[pDictonary objectForKey:kImageViewKey] ParentView:pView];
                }
            }
        }
        
        //NSLog(@"********** showOnMainThread:%f", CFAbsoluteTimeGetCurrent() - startTime);
    }
}
#else
#pragma mark show subtitle (mac os)
- (void)showImageView:(NSMutableArray *)pImageArray ParentView:(CALayer *)pView
{
    if (pView == nil || pImageArray == nil) {
        return;
    }
    
    int count = [pImageArray count];
    if (count == 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        voRenderObjectInfo *pInfo = [pImageArray objectAtIndex:i];
        if (pInfo == nil) {
            continue;
        }
        if (pInfo.viewType == IMAGE_VIEW) {
            
            voSubtitleImageInfoData imageInfo = pInfo.imageData;
            
            if (imageInfo.pPicData == NULL && imageInfo.nSize > 0) {
                continue;
            }
            
            CALayer *pImageView = [CALayer layer];
            if (pImageView == nil) {
                return;
            }
            
            [pImageView setFrame:[voSubtitleProperty getCGRect:pInfo.viewFrame]];
            CGImageRef pCellImg = NULL;
            if (imageInfo.nType == VO_IMAGE_RGBA32) {
                int width =imageInfo.nWidth;
                int height = imageInfo.nHeight;
                int stride = width * 4;
                
                VO_PBYTE pPicData = (VO_PBYTE) malloc (imageInfo.nSize);
                if (pPicData == NULL) {
                    return;
                }
                memset(pPicData, 0, imageInfo.nSize);
                memcpy(pPicData, imageInfo.pPicData,imageInfo.nSize);
                
                CGDataProviderRef provider = CGDataProviderCreateWithData(NULL,
                                                                          pPicData,
                                                                          stride*height, voReleasePixels);
                
                CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
                
                pCellImg = CGImageCreate(width, height, 8, 32, stride,
                                                space, kCGImageAlphaLast | kCGBitmapByteOrder32Big,
                                                provider, NULL, true, kCGRenderingIntentDefault);
                CGDataProviderRelease(provider);
                
                CGColorSpaceRelease(space);
                
            }
            else
            {
                NSData *pData = [NSData dataWithBytes:imageInfo.pPicData length:imageInfo.nSize];
                
                CFDataRef imgData = (CFDataRef)pData;
                CGDataProviderRef imgDataProvider = CGDataProviderCreateWithCFData (imgData);
                
                // Get CGImage from CFDataRef
                pCellImg = CGImageCreateWithJPEGDataProvider(imgDataProvider, NULL, true, kCGRenderingIntentDefault);
                
                // If the image isn't a JPG Image, would be PNG file
                if (!pCellImg)
                    pCellImg = CGImageCreateWithPNGDataProvider(imgDataProvider, NULL, true, kCGRenderingIntentDefault);
                

                
            }
            
            float imgWidth = CGImageGetWidth(pCellImg);
            float imgHeight = CGImageGetHeight(pCellImg);
            
            [pImageView  setAnchorPoint:CGPointMake(0.5, 0.5)];
            CGPoint center = CGPointMake(self.m_pParentView.frame.size.width / 2.0, self.m_pParentView.frame.size.height / 2.0);
            
            if (pCellImg != nil) {
                
                CGRect fm = pImageView.frame;
                bool bNeeDecrease = false;
        
                if (fm.size.width == 0 || fm.size.height == 0) {
                    if (imgWidth <= m_cDrawRect.size.width && imgHeight <= m_cDrawRect.size.height) {
                        fm.size.width = imageInfo.nWidth;
                        fm.size.height = imageInfo.nHeight;
                        pImageView.frame = fm;
                        [pImageView setPosition:center];
                    }
                    else
                    {
                        bNeeDecrease = true;
                    }
                    
                }
                if (m_bIsDrawRect || bNeeDecrease) {
                    //NSLog(@"__________m_cDrawRect: %f, %f",m_cDrawRect.size.width, m_cDrawRect.size.height);
                    float ratio = imgWidth / imgHeight;
                    if (m_cDrawRect.size.width == 0 || m_cDrawRect.size.height == 0) {
                        fm.size.width =  imgWidth;
                        fm.size.height = fm.size.width / ratio;
                    }
                    else
                    {
                        float drawRatio = m_cDrawRect.size.width / m_cDrawRect.size.height;
                        if (ratio >= drawRatio) {
                            fm.size.width =  m_cDrawRect.size.width;
                            fm.size.height = fm.size.width / ratio;
                        }
                        else
                        {
                            fm.size.height =  m_cDrawRect.size.height;
                            fm.size.width = fm.size.height * ratio;
                        }
                    }
                    //NSLog(@"__________pCellImg: %f, %f, %f, %f, %f, %f",pCellImg.size.width, pCellImg.size.height, ratio, fm.size.width, fm.size.height, fm.size.width/fm.size.height);
                    pImageView.frame = fm;
                    [pImageView setPosition:center];
                    
                    if (m_bIsDrawRect) {
                        //[self.m_pParentView setBackgroundColor:[NSColor blackColor].CGColor];
                        voSubtitleRGBAColor color;
                        memset(&color, 0, sizeof(voSubtitleRGBAColor));
                        color.nRed              = 0;
                        color.nGreen            = 0;
                        color.nBlue             = 0;
                        color.nTransparency     = 100;
                        CGColorRef bgColor = [voSubtitleProperty getCGColor: color];
                        [self.m_pParentView setBackgroundColor: bgColor];
                        [voSubtitleProperty releaseCGColor: bgColor];
                        
                    }
                }
                
                [pImageView setContents:(id)pCellImg];
                CGImageRelease(pCellImg);
                [pView insertSublayer:pImageView atIndex:pInfo.viewZOrder];
                //                [pView addSubview:pImageView];
                
            }
        }
    }
}

- (void)showTextView:(NSMutableArray *)pTextArray ParentView:(CALayer *)pView
{
    if (pView == nil || pTextArray == nil) {
        return;
    }
    
    int count = [pTextArray count];
    if (count == 0) {
        return;
    }
    
    for (int i = 0; i < count; i++) {
        voRenderObjectInfo *pInfo = [pTextArray objectAtIndex:i];
        if (pInfo == nil) {
            continue;
        }
        if (pInfo.viewType == TEXT_VIEW) {
            voTextInfoEntryView *pTextView = [[[voTextInfoEntryView alloc] init] autorelease];
            if (pTextView == nil) {
                return;
            }
            
            CGRect fmBegin = [voSubtitleProperty getCGRect:pInfo.viewFrame];
            [pTextView setFrame:fmBegin];
            //printf("------------fmBegin:%f, %f, %f, %f \r\n",fmBegin.origin.x, fmBegin.origin.y, fmBegin.size.width, fmBegin.size.height);
            //[pTextView setBackgroundColor:[voSubtitleProperty getColor:pInfo.viewColor].CGColor];
            CGColorRef bgColor = [voSubtitleProperty getCGColor: pInfo.viewColor];
            [pTextView setBackgroundColor:bgColor];
            [voSubtitleProperty releaseCGColor: bgColor];
            //printf("________________pInfo.textViewString:%s \r\n",[[pInfo.textViewString mutableString] UTF8String]);
            [pTextView setTextInfo:pInfo.textViewString];
            
            if (pInfo.viewBorderType == 1) {
                [pTextView setBorderWidth:1];
                //[pTextView setBorderColor:[voSubtitleProperty getColor:pInfo.viewBorderColor].CGColor];
                CGColorRef viewColor = [voSubtitleProperty getCGColor: pInfo.viewColor];
                [pTextView setBorderColor: viewColor];
                [voSubtitleProperty releaseCGColor: viewColor];
            }
            //            [pTextView setHidden:NO];
            [pTextView setNeedsDisplay];
            [pView insertSublayer:pTextView atIndex:pInfo.viewZOrder];
            //            [pView addSubview:pTextView];
            
            
        }
    }
    
}

- (CALayer *)showBackgroundView:(voRenderObjectInfo *)pInfo ZOrder:(int *)nZOrder
{
    if (pInfo == nil) {
        return nil;
    }
    
    CALayer *pView = [CALayer layer];
    
    if (pView == nil) {
        return nil;
    }
    
    CGColorRef bgColor = [voSubtitleProperty getCGColor: pInfo.viewColor];
    [pView setBackgroundColor: bgColor];
    [voSubtitleProperty releaseCGColor: bgColor];
    
    [pView setFrame:[voSubtitleProperty getCGRect:pInfo.viewFrame]];
    
    if (pInfo.viewBorderType == 1) {
        [pView setBorderWidth:1];
        
        CGColorRef viewColor = [voSubtitleProperty getCGColor: pInfo.viewBorderColor];
        //[pView setBorderColor:[voSubtitleProperty getColor:pInfo.].CGColor];
        [pView setBorderColor: viewColor];
        [voSubtitleProperty releaseCGColor: viewColor];
    }
    *nZOrder = pInfo.viewZOrder;
    [pView setHidden:NO];
    [pView setNeedsDisplay];
    
    return pView;
}

- (void)showOnMainThread {
    @synchronized(self) {
        
        //CFAbsoluteTime startTime = CFAbsoluteTimeGetCurrent();
        
        [self removeOnMainThread];
        
        if (self.m_pEntryObjects == nil) {
            return;
        }
        
        for (int i = 0; i < [self.m_pEntryObjects count]; i++) {
            
            NSMutableDictionary *pDictonary = [self.m_pEntryObjects objectAtIndex:i];
            if (pDictonary == nil) {
                continue;
            }
            
            if ([pDictonary objectForKey:kEnterViewKey] == nil) {
                continue;
            }
            int nZOrder = 0;
            CALayer *pView = [self showBackgroundView:[pDictonary objectForKey:kEnterViewKey] ZOrder:&nZOrder];
            if (pView != nil) {
                [self.m_pParentView insertSublayer:pView atIndex:nZOrder];
                
                
                if ([pDictonary objectForKey:kTextViewKey] != nil)
                {
                    [self showTextView:(NSMutableArray *)[pDictonary objectForKey:kTextViewKey] ParentView:pView];
                }
                
                if ([pDictonary objectForKey:kImageViewKey] != nil)
                {
                    [self showImageView:(NSMutableArray *)[pDictonary objectForKey:kImageViewKey] ParentView:pView];
                }
            }
        }
        
        //NSLog(@"********** showOnMainThread:%f", CFAbsoluteTimeGetCurrent() - startTime);
    }
}

#endif

- (void) showViews
{
    @synchronized(self) {
        if ([NSRunLoop mainRunLoop] == [NSRunLoop currentRunLoop])
        {
            [self showOnMainThread];
        }
        else
        {
            [self performSelectorOnMainThread:@selector(showOnMainThread) withObject:nil waitUntilDone:NO];
        }
    }
}

#pragma mark remove subtitle
- (void)removeOnMainThread
{
    @synchronized(self) {
        // remove all entry view from parent window.
        if (self.m_pParentView == nil) {
            return;
        }
        
#ifdef _IOS
        if (m_bIsDrawRect) {
            [self.m_pParentView setBackgroundColor:[UIColor clearColor]];
        }

        
        NSArray *viewsToRemove = [self.m_pParentView subviews];
        
        if (viewsToRemove == nil || [viewsToRemove count] == 0) {
            return;
        }
        for (UIView *view in viewsToRemove) {
            if (view != nil) {
                [view removeFromSuperview];
            }
        }
#else
        if (m_bIsDrawRect) {
            //[self.m_pParentView setBackgroundColor:NSColor.clearColor.CGColor];
            voSubtitleRGBAColor color;
            memset(&color, 0, sizeof(voSubtitleRGBAColor));
            color.nRed              = 0;
            color.nGreen            = 0;
            color.nBlue             = 0;
            color.nTransparency     = 0;
            CGColorRef bgColor = [voSubtitleProperty getCGColor: color];
            [self.m_pParentView setBackgroundColor: bgColor];
            [voSubtitleProperty releaseCGColor: bgColor];

        }
        
        if ([self.m_pParentView sublayers] != nil) {
            [self.m_pParentView setSublayers:nil];
        }
        
//        NSArray *viewsToRemove = [self.m_pParentView sublayers];
//        
//        if (viewsToRemove == nil || [viewsToRemove count] == 0) {
//            return;
//        }
//        for (CALayer *view in viewsToRemove) {
//            if (view != nil) {
//                [view removeFromSuperlayer];
//            }
//        }
#endif
        
//        if (self.m_pEntryViews == nil) {
//            return;
//        }
//        
//        int count = [self.m_pEntryViews count];
//        if (count == 0) {
//            return;
//        }
//        for (int i = 0; i < count; i++) {
//            NSMutableDictionary *pDictonary = [self.m_pEntryViews objectAtIndex:i];
//            if (pDictonary == nil) {
//                return;
//            }
//            UIView *pView = (UIView *)[pDictonary objectForKey:kEnterViewKey];
//            if (pView == nil) {
//                return;
//            }
//            [pView setHidden:YES];
//            
//            NSMutableArray *pImageArray = [pDictonary objectForKey:kImageViewKey];
//            if (pImageArray != nil) {
//                for (int j = 0; j < [pImageArray count]; j++) {
//                    UIImageView *pImageView = (UIImageView *)[pImageArray objectAtIndex:j];
//                    if (pImageView == nil) {
//                        continue;
//                    }
//                    [pImageView setHidden:YES];
//                }
//            }
//            
//            NSMutableArray *pTextArray = [pDictonary objectForKey:kTextViewKey];
//            if (pTextArray != nil) {
//                for (int j = 0; j < [pTextArray count]; j++) {
//                    voTextInfoEntryView *pTextView = (voTextInfoEntryView *)[pTextArray objectAtIndex:j];
//                    if (pTextView == nil) {
//                        continue;
//                    }
//                    [pTextView setHidden:YES];
//                }
//            }
//
//        }
    }
    
}

- (void) removeViews:(BOOL)isWait
{
    //don't need lock because it is need to post to main thread.
    //@synchronized(self) {
    if ([NSRunLoop mainRunLoop] == [NSRunLoop currentRunLoop])
    {
        [self removeOnMainThread];
    }
    else
    {
        [self performSelectorOnMainThread:@selector(removeOnMainThread) withObject:nil waitUntilDone:isWait];
    }
    //}
}

#pragma mark render subtitle
- (BOOL) renderSubtitle:(voSubtitleInfo *)pSubtitleInfo
{
    @synchronized(self) {
        if (pSubtitleInfo == NULL)
        {
            return NO;
        }
        
        if(m_pSubInfo == NULL)
        {
            m_pSubInfo = create_subtitleinfo( (voSubtitleInfo*)pSubtitleInfo, m_alloc);
            [self resetRectWithFontSizePercent];
            [self setSettingsToSubInfo];
        }
        else
        {
            if (pSubtitleInfo->nTimeStamp != m_pSubInfo->nTimeStamp) {
                if(!isequal_subtitleinfo((voSubtitleInfo*)pSubtitleInfo, m_pSubInfo))
                {
                    destroy_subtitleinfo(m_pSubInfo, m_alloc);
                    m_pSubInfo = create_subtitleinfo( (voSubtitleInfo*)pSubtitleInfo, m_alloc);
                    [self resetRectWithFontSizePercent];
                    [self setSettingsToSubInfo];
                }
            }
        }
        
        self.m_pEntryObjects = nil;
        [self prepareSubtitle:m_pSubInfo];
    }
    
    
    return YES;
}

- (BOOL) prepareSubtitle:(voSubtitleInfo *)pInfo
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    @synchronized(self) {
        
        if (pInfo == NULL || !m_bShowSubtitle) {
            [self removeViews:NO];
            [pool release];
            return NO;
        }
        
        SubtitleInfoEntry *pIntoEntry = pInfo->pSubtitleEntry;
        if (pIntoEntry == NULL) {
            [self removeViews:NO];
            [pool release];
            return NO;
        }
        
        if (m_cDrawRect.size.width <= 0) {
            //NSLog(@"__________ prepareSubtitle m_cDrawRect: %f, %f",m_cDrawRect.size.width, m_cDrawRect.size.height);
            m_cDrawRect.origin.x = self.m_pParentView.frame.origin.x;
            m_cDrawRect.origin.y = self.m_pParentView.frame.origin.y;
            m_cDrawRect.size.width = self.m_pParentView.frame.size.width;
            m_cDrawRect.size.height = self.m_pParentView.frame.size.height;
            //NSLog(@"__________ prepareSubtitle 2 m_cDrawRect: %f, %f",m_cDrawRect.size.width, m_cDrawRect.size.height);
        }
        
        self.m_pEntryObjects = [NSMutableArray array];
        
        while (pIntoEntry != NULL)
        {
            if ([self loadRenderObject:pIntoEntry Numb:[self.m_pEntryObjects count]]) {
            }
            
            pIntoEntry = pIntoEntry->pNext;
        }
        
        [self showViews];
    }
    [pool release];
    return YES;
}

#pragma mark destroy subtitle
- (void) destroySubtitleInfo
{
    @synchronized(self) {
        if (m_pSubInfo)
        {
            destroy_subtitleinfo(m_pSubInfo, m_alloc);
            m_pSubInfo = NULL;
        }
        
        [self removeViews:NO];
        
        NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
        self.m_pEntryObjects = nil;
        [pool release];
    }
}

#pragma mark init
- (id) init
{
    if (nil != (self = [super init])) 
    {
        @synchronized(self) {
#ifdef _IOS
            m_cScale = [[UIScreen mainScreen] scale];
#else
            m_cScale = 1.0;
#endif
            self.m_pEntryObjects = nil;
            self.m_pParentView = nil;
            m_bSetting = false;
            m_bShowSubtitle = false;
            m_bIsDrawRect = false;
            m_nFontIndex = 0;
            m_fMoveHeight = 0;
            m_fMoveWidth = 0;
            m_fWindowLostHeight = 0;

            memset(&m_cDrawRect, 0, sizeof(m_cDrawRect));
            memset(&m_cBackgroundColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cFontColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cEdgeColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cWindowColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cSubTitleSettings, 0, sizeof(VOOSMP_SUBTITLE_SETTINGS));
            
        }
    }
    
    return self;
}

#pragma mark set info
- (void) setParentView:(void*)pView
{
    @synchronized(self) {
        
        if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
            return;
        }
        
        if (pView == NULL)
        {
            [self removeOnMainThread];
            self.m_pParentView = nil;
            return;
        }
#ifdef _IOS
        if (self.m_pParentView != (UIView *)pView)
        {
            [self removeOnMainThread];
            self.m_pParentView = (UIView *)pView;
        }
#else
        if (self.m_pParentView != (CALayer *)pView)
        {
            [self removeOnMainThread];
            self.m_pParentView = (CALayer *)pView;
        }
#endif
        
        if (m_pSubInfo != NULL) {
            [self prepareSubtitle:m_pSubInfo];
        }
        
    }
}

- (void) setSettingsEnable:(int)isEnable
{
    @synchronized(self) {
        
        m_bSetting = isEnable == 0 ? false :true;
        if (!m_bSetting) {
            m_cSubTitleSettings.nItemMask = VOOSMP_SUBTITLE_MASK_DISABLE_ALL;
        }
        
    }
}

- (void) setSubtitleShow:(BOOL)show
{
    @synchronized(self) {
        m_bShowSubtitle = show;
        
        if (!show) {
            [self removeViews:NO];
        }
        else
        {
            if (m_pSubInfo != NULL) {
                [self prepareSubtitle:m_pSubInfo];
            }
        }
    }
}

- (void) setImgDispAsDrawRect:(BOOL)isDrawRect
{
    m_bIsDrawRect = isDrawRect;
}

- (void) setSettingsInfo:(VOOSMP_SUBTITLE_SETTINGS *)settings
{
    @synchronized(self) {
        if (settings == NULL) {
        
            memset(&m_cDrawRect, 0, sizeof(m_cDrawRect));
            memset(&m_cBackgroundColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cFontColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cEdgeColor, 0, sizeof(voSubtitleRGBAColor));
            memset(&m_cWindowColor, 0, sizeof(voSubtitleRGBAColor));
            return;
        }
        
        
//        if (m_cSubTitleSettings == NULL) {
//            m_cSubTitleSettings = new VOOSMP_SUBTITLE_SETTINGS;
//            memset(m_cSubTitleSettings, 0, sizeof(VOOSMP_SUBTITLE_SETTINGS));
//        }
        if (!m_bSetting) {
            m_cSubTitleSettings.nItemMask = VOOSMP_SUBTITLE_MASK_DISABLE_ALL;
            return;
        }
        
        if (settings->nItemMask == 0) {
            m_cSubTitleSettings.nItemMask = settings->nItemMask;
            return;
        }
        
        
        int nRed = 0,nGreen = 0,nBlue = 0;
        float opacity = 0.0;
        
        m_cSubTitleSettings.nItemMask = settings->nItemMask;
        
        m_cSubTitleSettings.nBackgroundColor = settings->nBackgroundColor;
        m_cSubTitleSettings.nBackgroundOpacity = settings->nBackgroundOpacity;
        
        //back ground color & opacity

        [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nBackgroundColor Red:&nRed Green:&nGreen Blue:&nBlue];
        m_cBackgroundColor.nRed = nRed;
        m_cBackgroundColor.nGreen = nGreen;
        m_cBackgroundColor.nBlue = nBlue;
        opacity = m_cSubTitleSettings.nBackgroundOpacity / 100.0f;
        m_cBackgroundColor.nTransparency = opacity  * 255;
        
        m_cSubTitleSettings.nFontColor = settings->nFontColor;
        m_cSubTitleSettings.nFontOpacity = settings->nFontOpacity;
        m_cSubTitleSettings.nFontSizeScale = settings->nFontSizeScale;
        
        nRed = 0,nGreen = 0,nBlue = 0;
        //font color & opacity
        [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nFontColor Red:&nRed Green:&nGreen Blue:&nBlue];
        m_cFontColor.nRed = nRed;
        m_cFontColor.nGreen = nGreen;
        m_cFontColor.nBlue = nBlue;
        opacity = m_cSubTitleSettings.nFontOpacity / 100.0f;
        m_cFontColor.nTransparency = opacity  * 255;
        
        m_cSubTitleSettings.nEdgeType = settings->nEdgeType;
        m_cSubTitleSettings.nEdgeColor = settings->nEdgeColor;
        m_cSubTitleSettings.nEdgeOpacity = settings->nEdgeOpacity;
        //edge color &opacity
        [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nEdgeColor Red:&nRed Green:&nGreen Blue:&nBlue];
        m_cEdgeColor.nRed = nRed;
        m_cEdgeColor.nGreen = nGreen;
        m_cEdgeColor.nBlue = nBlue;
        opacity = m_cSubTitleSettings.nEdgeOpacity / 100.0f;
        m_cEdgeColor.nTransparency = opacity  * 255;
        
        m_cSubTitleSettings.nWindowBackgroundColor = settings->nWindowBackgroundColor;
        m_cSubTitleSettings.nWindowBackgroundOpacity = settings->nWindowBackgroundOpacity;
    
        //window color & opacity
        nRed = 0,nGreen = 0,nBlue = 0;
        [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nWindowBackgroundColor Red:&nRed Green:&nGreen Blue:&nBlue];
        m_cWindowColor.nRed = nRed;
        m_cWindowColor.nGreen = nGreen;
        m_cWindowColor.nBlue = nBlue;
        opacity = m_cSubTitleSettings.nWindowBackgroundOpacity / 100.0f;
        m_cWindowColor.nTransparency = opacity * 255;
        
        m_cSubTitleSettings.bFontBold = settings->bFontBold;
        m_cSubTitleSettings.bFontItalic = settings->bFontItalic;
        m_cSubTitleSettings.bFontUnderline = settings->bFontUnderline;
        memset(m_cSubTitleSettings.szFontName, 0, sizeof(m_cSubTitleSettings.szFontName) / sizeof(char));
        strncpy(m_cSubTitleSettings.szFontName, settings->szFontName, sizeof(m_cSubTitleSettings.szFontName) / sizeof(char));
//        m_nFontIndex = [self checkFontName:[NSString stringWithUTF8String:settings->szFontName]];        
//        NSString *pFontName = [self checkFontName:[NSString stringWithUTF8String:settings->szFontName] Bold:m_cSubTitleSettings->bFontBold Italic:m_cSubTitleSettings->bFontItalic];
//        if (pFontName != nil) {
//            strcpy(m_cSubTitleSettings->szFontName, [pFontName UTF8String]);
//        }
//        else {
//            strcpy(m_cSubTitleSettings->szFontName, [[[pFontNameList componentsSeparatedByString:@","] objectAtIndex:0] UTF8String]);
//        }
    }    
}

- (void) setDrawRect:(CGRect *)rect
{
    @synchronized(self) {
//        NSLog(@"_______________________setDrawRect m_cDrawRect: %f, %f, %f, %f *rect: %f, %f, %f, %f",m_cDrawRect.origin.x, m_cDrawRect.origin.y, m_cDrawRect.size.width, m_cDrawRect.size.height,(*rect).origin.x, (*rect).origin.y, (*rect).size.width, (*rect).size.height);
        if (rect == NULL)
        {
            return;
        }
        if ([voSubtitleProperty isEqualFloatValue:(m_cDrawRect.origin.x * m_cScale) Compare:(*rect).origin.x]
            && [voSubtitleProperty isEqualFloatValue:(m_cDrawRect.origin.y * m_cScale) Compare:(*rect).origin.y]
            && [voSubtitleProperty isEqualFloatValue:(m_cDrawRect.size.width * m_cScale) Compare:(*rect).size.width]
            && [voSubtitleProperty isEqualFloatValue:(m_cDrawRect.size.height * m_cScale) Compare:(*rect).size.height])
        {
            return;
        }
        
        memcpy(&m_cDrawRect, rect, sizeof(m_cDrawRect));
        m_cDrawRect.origin.x /= m_cScale;
        m_cDrawRect.origin.y /= m_cScale;
        m_cDrawRect.size.width /= m_cScale;
        m_cDrawRect.size.height /= m_cScale;
        
        if (m_pSubInfo != NULL) {
            [self prepareSubtitle:m_pSubInfo];
        }
    }
}

- (void) resetRectWithFontSizePercent
{
    voSubtitleInfoEntry *pSubtitleEntry = m_pSubInfo->pSubtitleEntry;
    int nTextBottom = 0, nTextViewCount = 0, nPercent = 0, nPreViewTop = 0, FontPercent = 0;
    while (pSubtitleEntry != NULL) {
        nTextBottom = 0;
        voSubtitleTextRowInfo *pTextRowInfo = pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo;
        
        while (pTextRowInfo != NULL) {
            nTextViewCount += 1;
            voSubtitleTextInfoEntry *pSubtitleTextInfoEntry = pTextRowInfo->pTextInfoEntry;
            while (pSubtitleTextInfoEntry != NULL) {
                voSubtitleFontInfo fontInfo = pSubtitleTextInfoEntry->stStringInfo.stFontInfo;
                //font precent
                if (fontInfo.FontSize > BASE_PERCENT && fontInfo.FontSize < PIX_PERCENT)
                {
                    nPercent = fontInfo.FontSize - BASE_PERCENT;
                    FontPercent = nPercent > FontPercent ? nPercent : FontPercent;
                    //NSLog(@"FontPercent:%d",FontPercent);
                }
                
                pSubtitleTextInfoEntry = pSubtitleTextInfoEntry->pNext;
            }
            if (FontPercent == 0) {
                break;
            }
            voSubtitleRectInfo *rectInfo = &((pTextRowInfo->stTextRowDes).stDataBox);
            if (rectInfo != NULL) {
                int nMove = (rectInfo->stRect.nBottom - rectInfo->stRect.nTop) * ((100 - FontPercent) / 100.0) - 0.5;
                if (nPreViewTop <= rectInfo->stRect.nTop) {
                    nPreViewTop = rectInfo->stRect.nTop;
                    rectInfo->stRect.nTop -= nMove * (nTextViewCount - 1);
                    rectInfo->stRect.nBottom -= nMove * nTextViewCount;
                }
                else {
                    nPreViewTop = rectInfo->stRect.nTop;
                    rectInfo->stRect.nTop += nMove * (nTextViewCount - 1);
                    rectInfo->stRect.nBottom += nMove * nTextViewCount;
                }
                if (pTextRowInfo->pTextInfoEntry->nSize != 0) {
                    nTextBottom = rectInfo->stRect.nBottom;
                }
            }

            pTextRowInfo = pTextRowInfo->pNext;
        }
        voSubtitleRectInfo *rectInfo = &(pSubtitleEntry->stSubtitleRectInfo);
        if (rectInfo != NULL) {
            if (nTextBottom > rectInfo->stRect.nTop) {
                rectInfo->stRect.nBottom = nTextBottom;
            }
        }
        pSubtitleEntry = pSubtitleEntry->pNext;
    }
}

- (void) setSettingsToSubInfo
{
    if (m_cSubTitleSettings.nItemMask == VOOSMP_SUBTITLE_MASK_DISABLE_ALL || m_pSubInfo == NULL)
    {
        return;
    }
    
    int nPreViewTop = 0, nTextViewCount = 0, nTextBottom = 0;
    voSubtitleInfoEntry *pSubtitleEntry = m_pSubInfo->pSubtitleEntry;
    while (pSubtitleEntry != NULL) {
        
        //window color & opacity
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_COLOR]) {
            pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nRed = m_cWindowColor.nRed;
            pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nGreen = m_cWindowColor.nGreen;
            pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nBlue = m_cWindowColor.nBlue;
        }
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_OPACITY]) {
            pSubtitleEntry->stSubtitleRectInfo.stRectFillColor.nTransparency = m_cWindowColor.nTransparency;
        }
        nTextBottom = 0;
        voSubtitleTextRowInfo *pTextRowInfo = pSubtitleEntry->stSubtitleDispInfo.pTextRowInfo;
        while (pTextRowInfo != NULL) {
            nTextViewCount += 1;
            //font scale
            if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_SIZE_SCALE])
            {
                voSubtitleRectInfo *rectInfo = &((pTextRowInfo->stTextRowDes).stDataBox);
                if (rectInfo != NULL) {
                    int nMove = (rectInfo->stRect.nBottom - rectInfo->stRect.nTop) * ((100 - m_cSubTitleSettings.nFontSizeScale) / 100.0) - 0.5;
                    if (nPreViewTop <= rectInfo->stRect.nTop) {
                        nPreViewTop = rectInfo->stRect.nTop;
                        rectInfo->stRect.nTop -= nMove * (nTextViewCount - 1);
                        rectInfo->stRect.nBottom -= nMove * nTextViewCount;
                    }
                    else {
                        nPreViewTop = rectInfo->stRect.nTop;
                        rectInfo->stRect.nTop += nMove * (nTextViewCount - 1);
                        rectInfo->stRect.nBottom += nMove * nTextViewCount;
                    }
                    if (pTextRowInfo->pTextInfoEntry->nSize != 0) {
                        nTextBottom = rectInfo->stRect.nBottom;
                    }
                }
            }
            //back ground color & opacity
            if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_COLOR]) {
                pTextRowInfo->stTextRowDes.stDataBox.stRectFillColor.nRed = m_cBackgroundColor.nRed;
                pTextRowInfo->stTextRowDes.stDataBox.stRectFillColor.nGreen = m_cBackgroundColor.nGreen;
                pTextRowInfo->stTextRowDes.stDataBox.stRectFillColor.nBlue = m_cBackgroundColor.nBlue;
            }
            if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_OPACITY]) {
                pTextRowInfo->stTextRowDes.stDataBox.stRectFillColor.nTransparency = m_cBackgroundColor.nTransparency;
            }
            
            voSubtitleTextInfoEntry *pSubtitleTextInfoEntry = pTextRowInfo->pTextInfoEntry;
            while (pSubtitleTextInfoEntry != NULL) {
                //font color & opacity
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_COLOR]) {
                    pSubtitleTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nRed = m_cFontColor.nRed;
                    pSubtitleTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nGreen = m_cFontColor.nGreen;
                    pSubtitleTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nBlue = m_cFontColor.nBlue;
                }
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_OPACITY]) {
                    pSubtitleTextInfoEntry->stStringInfo.stFontInfo.stFontColor.nTransparency = m_cFontColor.nTransparency;
                }
                
                // edge
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_EDGE_TYPE]) {
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.EdgeType = m_cSubTitleSettings.nEdgeType;
                }
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_EDGE_COLOR]) {
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nRed = m_cEdgeColor.nRed;
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nGreen = m_cEdgeColor.nGreen;
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nBlue = m_cEdgeColor.nBlue;
                }
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_EDGE_OPACITY]) {
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.EdgeColor.nTransparency = m_cEdgeColor.nTransparency;
                }
                
                // font under line
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_UNDERLINE]) {
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.Underline = m_cSubTitleSettings.bFontUnderline;
                }
                // font italic
                if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_ITALIC]) {
                    pSubtitleTextInfoEntry->stStringInfo.stCharEffect.Italic = m_cSubTitleSettings.bFontItalic;
                }
                pSubtitleTextInfoEntry = pSubtitleTextInfoEntry->pNext;
                
            }
            pTextRowInfo = pTextRowInfo->pNext;
        }
        
        //window scale
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_SIZE_SCALE]) {
            voSubtitleRectInfo *rectInfo = &(pSubtitleEntry->stSubtitleRectInfo);
            if (rectInfo != NULL) {
                if (nTextBottom > rectInfo->stRect.nTop) {
                    rectInfo->stRect.nBottom = nTextBottom;
                }
            }
            
            //            NSLog(@"%lu",rectInfo->stRect.nBottom);
        }
        
        pSubtitleEntry = pSubtitleEntry->pNext;
    }
    
}
#ifdef _IOS
- (void) previewSubtitle:(VOOSMP_PREVIEW_SUBTITLE_INFO *)subtitleInfo
{
    @synchronized(self) {
        // enter preview view.
        if (subtitleInfo->pView == NULL)
        {
            return;
        }
        
        NSArray *viewsToRemove = [(UIView *)subtitleInfo->pView subviews];
        
        if (viewsToRemove != nil || [viewsToRemove count] != 0) {
            for (UIView *view in viewsToRemove) {
                if (view != nil) {
                    [view removeFromSuperview];
                }
            }
        }

        
        //window color
        voSubtitleRGBAColor color;
        int nRed = 0,nGreen = 0,nBlue = 0;
        
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_COLOR]) {
            [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nWindowBackgroundColor Red:&nRed Green:&nGreen Blue:&nBlue];
            color.nRed = nRed;
            color.nGreen = nGreen;
            color.nBlue = nBlue;
            
            if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_WINDOW_BACKGROUND_OPACITY]) {
                color.nTransparency = m_cSubTitleSettings.nWindowBackgroundOpacity * 255 / 100;
            }
            [(UIView *)subtitleInfo->pView setBackgroundColor:[voSubtitleProperty getColor:color]];
            
        }
        else
        {
            [(UIView *)subtitleInfo->pView setBackgroundColor:[UIColor blackColor]];
        }
        
        //background view
        voTextInfoEntryView *pTextView = [[[voTextInfoEntryView alloc] init] autorelease];
        if (pTextView == nil) {
            return;
        }
        
        CGRect fmBegin = ((UIView *)subtitleInfo->pView).frame;
        float defaultHeight = 0.4;
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_SIZE_SCALE]) {
            defaultHeight *= (m_cSubTitleSettings.nFontSizeScale / 100.0);
        }
        if (defaultHeight > 1.0) {
            defaultHeight = 1.0;
        }
        fmBegin.origin.x = 0.0;
        fmBegin.origin.y += fmBegin.size.height * (1 - defaultHeight) / 2;
        fmBegin.size.height *= defaultHeight;
        [pTextView setFrame:fmBegin];
        
        [pTextView setBackgroundColor:[UIColor clearColor]];
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_COLOR]) {
            [voSubtitleProperty getColorRGB:m_cSubTitleSettings.nBackgroundColor Red:&nRed Green:&nGreen Blue:&nBlue];
            color.nRed = nRed;
            color.nGreen = nGreen;
            color.nBlue = nBlue;
            
            if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BACKGROUND_OPACITY]) {
                color.nTransparency = m_cSubTitleSettings.nBackgroundOpacity * 255 / 100;
            }

            [pTextView setBackgroundColor:[voSubtitleProperty getColor:color]];
        }
        
        //display font
        voSubtitleFontStyle style = FontStyle_Default;
        if ([self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_NAME]) {
            style = [self getFontStyle:[NSString stringWithUTF8String:m_cSubTitleSettings.szFontName]];
        }
        
        NSString *pFontName = [voSubtitleProperty getFontNameWithStyle:style Bold:[self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_BOLD] ? m_cSubTitleSettings.bFontBold : false Italic:[self isEnableMark:VOOSMP_SUBTITLE_MASK_FONT_ITALIC] ? m_cSubTitleSettings.bFontItalic : false];
        
        int nSize = [voSubtitleProperty checkfontSize:pFontName String:@"ABCD" Height:fmBegin.size.height];
        
        NSMutableAttributedString *pEdgeString = nil;
        NSMutableAttributedString *pDrawString = [self getTextInfoWithSetting:[NSString stringWithUTF8String:subtitleInfo->pszSampleText] FontSize:nSize EdgeString:&pEdgeString];

        if (pEdgeString != nil)
        {
            [pTextView setEdgeTextInfo:pEdgeString EdgeType:m_cSubTitleSettings.nEdgeType];
            [pEdgeString release];
            pEdgeString = nil;
        }
        [pTextView setTextInfo:pDrawString];
        
        [pTextView setNeedsDisplay];
        [(UIView *)subtitleInfo->pView insertSubview:pTextView atIndex:0];
 
    }
}
#endif

-(void) dealloc
{
    [self removeViews:YES];
    
    @synchronized(self) {
        self.m_pParentView = nil;
        self.m_pEntryObjects = nil;
        
        if (m_pSubInfo)
        {
            destroy_subtitleinfo(m_pSubInfo, m_alloc);
            m_pSubInfo = NULL;
        }
    }
    
	[super dealloc];
}

@end
