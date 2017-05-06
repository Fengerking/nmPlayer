/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2011		*
*																		*
************************************************************************/
/*******************************************************************************
	File:		voCCManager.h

	Contains:	voCCManager header file

	Written by:	Jim Lin 

	Change History (most recent first):
	2011-11-16		Jim			Create file

*******************************************************************************/

#import <Foundation/Foundation.h>
#import "voOnStreamType.h"
#import "voCMutex.h"

#include "voSubTitleRender.h"
#ifdef _IOS
#import <UIKit/UIKit.h>
#endif

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


typedef enum
{
    CC_QT_GET_BUFFER,
    CC_QT_GET_TIMESTAMP,
}CCQueryType;

typedef int (* CCQueryCallback) (void * pUserData, int nID/*CCQueryType*/, void * pParam1, void * pParam2);

@interface voCCManager : NSObject
{
    bool m_bThreadExit;
    bool m_bPauseDone;
    bool m_bRefreshScreen;
    int m_nThreadStatus;
    
    VO_U64 m_nTimeOut;
    
    void* m_pUserData;
    CCQueryCallback m_pQuery;
    
    voCMutex m_cLock;
    
    voSubTitleRender* _pSubTitle;
    voSubTitleRender* _pSubTitleID3;
    
#ifdef _IOS
    UIView* _viewParent;
    UIView* _viewRender;
    UIView* _viewRenderID3;
#else
    CALayer* _viewParent;
    CALayer* _viewRender;
    CALayer* _viewRenderID3;
#endif
}

@property (nonatomic, retain) voSubTitleRender* m_pSubTitle;
@property (nonatomic, retain) voSubTitleRender* m_pSubTitleID3;
#ifdef _IOS
@property (nonatomic, retain) UIView* viewParent;
@property (nonatomic, retain) UIView* viewRender;
@property (nonatomic, retain) UIView* viewRenderID3;
#else
@property (nonatomic, retain) CALayer* viewParent;
@property (nonatomic, retain) CALayer* viewRender;
@property (nonatomic, retain) CALayer* viewRenderID3;
#endif

- (id) init:(void*)pUserData pQuery:(CCQueryCallback)pQuery;
- (void) readCCAndRender;
- (int) voStart;
- (int) voPause;
- (int) voStop;
- (int) voFlush;
- (int) voFlushID3;
- (void) setSubtitleShow:(bool)show;
- (int) updateScreen;
- (int) renderID3:(voSubtitleInfo *)pID3Info;
- (int) setSurface:(void*)pView;
- (int) setDrawRect:(int)nLeft nTop:(int)nTop nRight:(int)nRight nBottom:(int)nBottom;
- (void) setSettingsEnable:(int)isEnable;
- (void) setSettingsInfo:(VOOSMP_SUBTITLE_SETTINGS *)settings;
- (void) previewSubtitle:(VOOSMP_PREVIEW_SUBTITLE_INFO *)subtitleInfo;

@end
