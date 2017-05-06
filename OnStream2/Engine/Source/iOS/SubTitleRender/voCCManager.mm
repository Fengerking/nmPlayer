
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2011				*
*																		*
************************************************************************/
/*******************************************************************************
	File:		voCCManager.cpp

	Contains:	voCCManager class file

	Written by:	Jim Lin 

	Change History (most recent first):
	2011-11-16		Jim			Create file

*******************************************************************************/

#include "voCCManager.h"

#include "voOSFunc.h"
#include "voLog.h"
#include "voOnStreamType.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

typedef enum{
	VO_CCTHREAD_INIT			= 0,	/*!<The status is init */
	VO_CCTHREAD_STOPPED			= 1,	/*!<The status is stopped */
	VO_CCTHREAD_RUNNING			= 2,	/*!<The status is running */
	VO_CCTHREAD_PAUSED			= 3,	/*!<The status is paused */
}VO_CCTHREAD_STATUS;

@interface voCCManager ()
// Properties that don't need to be seen by the outside world.

@end

@implementation voCCManager


@synthesize m_pSubTitle = _pSubTitle;
@synthesize m_pSubTitleID3 = _pSubTitleID3;
@synthesize viewParent = _viewParent;
@synthesize viewRender = _viewRender;
@synthesize viewRenderID3 = _viewRenderID3;


- (void) readCCAndRender
{
    // don't need lock here
    
    int nRC = VOOSMP_ERR_Retry;
    
    voSubtitleInfo cInfo;
    
    if (m_pQuery) {
        
        if (0 < m_nTimeOut) {
            int nTimeCurrent = m_pQuery(m_pUserData, CC_QT_GET_TIMESTAMP, NULL, NULL);
            
            if (m_nTimeOut <= nTimeCurrent) {
                [self.m_pSubTitle destroySubtitleInfo];
                m_nTimeOut = 0;
            }
        }
        
        memset(&cInfo, 0, sizeof(voSubtitleInfo));
        nRC = m_pQuery(m_pUserData, CC_QT_GET_BUFFER, &cInfo, NULL);
    }
    
    if ((VOOSMP_ERR_None == nRC) && (NULL != cInfo.pSubtitleEntry)) {
        
        m_nTimeOut = 0;
        
        [self.m_pSubTitle renderSubtitle:&cInfo];
        m_bRefreshScreen = false;
        
        if ((0xFFFFFFFF != cInfo.pSubtitleEntry->nDuration) && (0 < cInfo.pSubtitleEntry->nDuration)) {
            m_nTimeOut = cInfo.nTimeStamp + cInfo.pSubtitleEntry->nDuration;
        }
    }
}

- (void) RunThread
{
	while (m_nThreadStatus >= VO_CCTHREAD_RUNNING || m_bRefreshScreen)
	{
		if ((m_nThreadStatus == VO_CCTHREAD_PAUSED) && !m_bRefreshScreen)
		{
            m_bPauseDone = true;
			voOS_Sleep(5);
			continue;
		}
        
        [self readCCAndRender];
        
        int iLoop = 0;
        while (m_nThreadStatus == VO_CCTHREAD_RUNNING) {
            voOS_Sleep(5);
            if (40 == (++iLoop)) {
                break;
            }
        }
	}
    
    m_bThreadExit = true;
}

- (id) init:(void*)pUserData pQuery:(CCQueryCallback)pQuery
{
    if (nil != (self = [super init]))
    {
        m_pQuery = pQuery;
        m_pUserData = pUserData;
        m_nThreadStatus = VO_CCTHREAD_INIT;
        m_bThreadExit = true;
        m_bPauseDone = false;
        m_bRefreshScreen = false;
        m_nTimeOut = 0;
        
        self.m_pSubTitle = [[voSubTitleRender alloc] init];
        [self.m_pSubTitle release];
        
        self.m_pSubTitleID3 = [[voSubTitleRender alloc] init];
        [self.m_pSubTitleID3 release];
        
        [self.m_pSubTitleID3 setSubtitleShow:YES];
        [self.m_pSubTitleID3 setImgDispAsDrawRect:YES];
        
        self.viewParent = nil;
        self.viewRender = nil;
        self.viewRenderID3 = nil;
    }
    
    return self;
}

- (void) dealloc
{
    self.m_pSubTitle = nil;
    self.m_pSubTitleID3 = nil;
    
    if (nil != self.viewRender) {
#ifdef _IOS
        [self.viewRender removeFromSuperview];
#else
        [self.viewRender removeFromSuperlayer];
#endif
        
        self.viewRender = nil;
    }
    
    if (nil != self.viewRenderID3) {
#ifdef _IOS
        [self.viewRenderID3 removeFromSuperview];
#else
        [self.viewRenderID3 removeFromSuperlayer];
#endif
        self.viewRenderID3 = nil;
    }
    
    self.viewParent = nil;
    
	[super dealloc];
}

- (int) voStart
{
    voCAutoLock cAuto(&m_cLock);
    
    m_bRefreshScreen = false;
    
    m_nThreadStatus = VO_CCTHREAD_RUNNING;
    
    if (!m_bThreadExit)
		return 0;
    
    m_bThreadExit = false;
    [NSThread detachNewThreadSelector:@selector(RunThread) toTarget:self withObject:nil];
    
    return 0;
}

- (int) voPause
{
    voCAutoLock cAuto(&m_cLock);
    
    if (m_nThreadStatus == VO_CCTHREAD_RUNNING) {
        
        m_bPauseDone = false;
        
        m_nThreadStatus = VO_CCTHREAD_PAUSED;
        
        while (!m_bPauseDone) {
            voOS_Sleep(2);
        }
    }
    
    return 0;
}

- (int) voStop
{
    voCAutoLock cAuto(&m_cLock);
    
    m_bRefreshScreen = false;
    
	m_nThreadStatus = VO_CCTHREAD_STOPPED;
    
	while (!m_bThreadExit)
	{
		voOS_Sleep(1);
	}

    [self voFlush];
        
    return 0;
}

- (int) voFlush
{
    voCAutoLock cAuto(&m_cLock);
    [self.m_pSubTitle destroySubtitleInfo];
    
    [self.m_pSubTitleID3 destroySubtitleInfo];
    
    m_nTimeOut = 0;

    return 0;
}

- (int) voFlushID3
{
    voCAutoLock cAuto(&m_cLock);
    
    [self.m_pSubTitleID3 destroySubtitleInfo];
    return 0;
}

- (void) setSubtitleShow:(bool)show
{
    voCAutoLock cAuto(&m_cLock);
    
    BOOL bShow = NO;
    if (show) {
        bShow = YES;
    }
    [self.m_pSubTitle setSubtitleShow:bShow];
}

- (int) renderID3:(voSubtitleInfo *)pID3Info
{
    voCAutoLock cAuto(&m_cLock);
    
    if (m_nThreadStatus != VO_CCTHREAD_RUNNING) {
        return 0;
    }
    
    [self.m_pSubTitleID3 renderSubtitle:pID3Info];
    return 0;
}

- (int) updateScreen
{
    voCAutoLock cAuto(&m_cLock);
    
    m_bRefreshScreen = true;
    
    if (!m_bThreadExit)
        return 0;
    
    m_bThreadExit = false;
    [NSThread detachNewThreadSelector:@selector(RunThread) toTarget:self withObject:nil];
    
    return 0;
}

- (int) setSurface:(void*)pView
{
    voCAutoLock cAuto(&m_cLock);
    
    if (NULL == pView) {
        return -1;
    }
    
    if ([NSRunLoop mainRunLoop] != [NSRunLoop currentRunLoop]) {
        return -1;
    }
    
#ifdef _IOS
    
    if (self.viewParent != pView) {
        self.viewParent = (UIView *)pView;
        
        if (nil != self.viewRender) {
            [self.viewRender removeFromSuperview];
        }
        
        CGRect cRect = self.viewParent.bounds;
        self.viewRender = [[UIView alloc] initWithFrame:cRect];
        [self.viewRender release];
        [self.viewRender setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
        [self.viewParent insertSubview:self.viewRender atIndex:1];
        
        
        if (nil != self.viewRenderID3) {
            [self.viewRenderID3 removeFromSuperview];
        }
        
        self.viewRenderID3 = [[UIView alloc] initWithFrame:cRect];
        [self.viewRenderID3 release];
        [self.viewRenderID3 setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
        [self.viewParent insertSubview:self.viewRenderID3 atIndex:1];
    }
#else
    if (self.viewParent != pView) {
        self.viewParent = (CALayer *)pView;
//        if ([self.viewParent sublayers] != nil) {
//            [self.viewParent setSublayers:nil];
//        }
        CGRect fm = ((CALayer *)pView).frame;
        if (nil != self.viewRender) {
            [self.viewRender removeFromSuperlayer];
        }
        
        self.viewRender = [CALayer layer];
        [self.viewRender setFrame:fm];
        [self.viewRender setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [self.viewParent insertSublayer:self.viewRender atIndex:1];
        
        
        if (nil != self.viewRenderID3) {
            [self.viewRenderID3 removeFromSuperlayer];
        }
        
        self.viewRenderID3 = [CALayer layer];
        [self.viewRenderID3 setFrame:fm];
        [self.viewRenderID3 setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
        [self.viewParent insertSublayer:self.viewRenderID3 atIndex:2];
        
//    if (self.viewParent != pView) {
//        self.viewParent = (CALayer *)pView;
//        CGRect fm = ((CALayer *)pView).bounds;
//
//        if (nil != self.viewRender) {
//            [self.viewRender removeFromSuperlayer];
//        }
//        self.viewRender = [CALayer layer];
//        //[self.viewRender release];
//        [self.viewRender setFrame:fm];
//        [self.viewRender setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
//        [self.viewParent insertSublayer:self.viewRender atIndex:1];
//        
//        if (nil != self.viewRenderID3) {
//            [self.viewRenderID3 removeFromSuperlayer];
//        }
//        self.viewRenderID3 = [CALayer layer];
//        //[self.viewRenderID3 release];
//        [self.viewRenderID3 setFrame:fm];
//        [self.viewRenderID3 setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];
//        [self.viewParent insertSublayer:self.viewRenderID3 atIndex:2];
    }
#endif
    
    [self.m_pSubTitle setParentView:self.viewRender];
    [self.m_pSubTitleID3 setParentView:self.viewRenderID3];
    
    return 0;
}

- (int) setDrawRect:(int)nLeft nTop:(int)nTop nRight:(int)nRight nBottom:(int)nBottom
{
    voCAutoLock cAuto(&m_cLock);
    
    CGRect cFrame;
    cFrame.origin.x = nLeft;
    cFrame.origin.y = nTop;
    cFrame.size.width = nRight - nLeft;
    cFrame.size.height = nBottom - nTop;

	[self.m_pSubTitle setDrawRect:&cFrame];
    
    // Use default value at present
    // [self.m_pSubTitleID3 setDrawRect:&cFrame];
    // set view to refresh subtitle render
    [self.m_pSubTitleID3 setParentView:self.viewRenderID3];
    
	return 0;
}

- (void) setSettingsEnable:(int)isEnable
{
    voCAutoLock cAuto(&m_cLock);
    
    [self.m_pSubTitle setSettingsEnable:isEnable];
}

- (void) setSettingsInfo:(VOOSMP_SUBTITLE_SETTINGS *)settings
{
    voCAutoLock cAuto(&m_cLock);
    
    [self.m_pSubTitle setSettingsInfo:settings];
}

- (void) previewSubtitle:(VOOSMP_PREVIEW_SUBTITLE_INFO *)subtitleInfo
{
    voCAutoLock cAuto(&m_cLock);
    
    [self.m_pSubTitle previewSubtitle:subtitleInfo];
}

@end
