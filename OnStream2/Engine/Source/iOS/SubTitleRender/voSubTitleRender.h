//
//  voSubTitleRender.h

//  Created by qian chris on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import "voType.h"
//#import "voSubtitleType.h"
#import "SubtitleFunc.h"
#import "voOnStreamType.h"
#ifdef _IOS
#import <UIKit/UIKit.h>
#else
#import <Cocoa/Cocoa.h>
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif
    
@interface voSubTitleRender : NSObject
{
@private
    vo_allocator< VO_BYTE > m_alloc;
    voSubtitleInfo*	 m_pSubInfo;
    
    voSubtitleRGBAColor m_cWindowColor;
    voSubtitleRGBAColor m_cBackgroundColor;
    voSubtitleRGBAColor m_cFontColor;
    voSubtitleRGBAColor m_cEdgeColor;
    bool m_bSetting;
    bool m_bShowSubtitle;
    bool m_bIsDrawRect;
    int m_nFontIndex;
    CGFloat m_cScale;
    CGFloat m_fMoveHeight;
    CGFloat m_fMoveWidth;
    CGFloat m_fWindowLostHeight;
    CGRect m_cDrawRect;
    VOOSMP_SUBTITLE_SETTINGS m_cSubTitleSettings;
    
    NSMutableArray * _pEntryObjects;
#ifdef _IOS
    UIView *_pParentView;
#else
    CALayer *_pParentView;
#endif
}

- (id) init;
- (void) setParentView:(void*)pView;
- (void) setDrawRect:(CGRect *)rect;
- (void) setSubtitleShow:(BOOL)show;
- (void) setImgDispAsDrawRect:(BOOL)isDrawRect;
- (BOOL) renderSubtitle:(voSubtitleInfo *)pSubtitleInfo;
- (void) destroySubtitleInfo;
- (void) setSettingsEnable:(int)isEnable;
- (void) setSettingsInfo:(VOOSMP_SUBTITLE_SETTINGS *)settings;
- (void) previewSubtitle:(VOOSMP_PREVIEW_SUBTITLE_INFO *)subtitleInfo;

@end
