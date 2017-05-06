/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voGLRenderBase
 
 Contains:    VisualOn OpenGL ES Base Render cpp file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-17   Jeff            Create file
 *******************************************************************************/

#import "voGLRenderBase.h"

#import "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voGLRenderBase::voGLRenderBase(EAGLContext* pContext)
:voRunRequestOnMain()
,m_eOrientation(UIDeviceOrientationUnknown)
,m_eMode(UIViewContentModeScaleToFill)
,m_pContext(pContext)
,m_pUIViewSet(NULL)
,m_pVideoView(NULL)
,m_pLock(NULL)
,m_nTextureWidth(0)
,m_nTextureHeight(0)
,m_nBackingWidth(0)
,m_nBackingHeight(0)
{
    memset(&m_cTransform, 0, sizeof(m_cTransform));
    memset(&m_cFrame, 0, sizeof(m_cFrame));
}

voGLRenderBase::~voGLRenderBase()
{
    unInitVideoView();
}

int voGLRenderBase::init()
{
    return VO_GL_RET_OK;
}

int voGLRenderBase::SetLock(voCMutex *pLock)
{
    m_pLock = pLock;
    return VO_GL_RET_OK;
}

EAGLContext* voGLRenderBase::GetGLContext()
{
    return m_pContext;
}

bool voGLRenderBase::IsEqual(float a, float b)
{
    const static float V_RANGE = 0.000001;
    if (((a - b) > -V_RANGE)
        && ((a - b) < V_RANGE) ) {
        return true;
    }
    return false;
}

void voGLRenderBase::Swap(float *a, float *b)
{
    if ((NULL == a) || (NULL == b)) {
        return;
    }
    
    float temp = *a;
    *a = *b;
    *b = temp;
}

bool voGLRenderBase::IsViewChanged()
{
    if (NULL == m_pUIViewSet) {
        return true;
    }
    
    CGAffineTransform cTransform = [m_pUIViewSet transform];
    CGRect cFrame = [m_pUIViewSet frame];
    
    if (IsEqual(m_cTransform.a, cTransform.a)
        && IsEqual(m_cTransform.b, cTransform.b)
        && IsEqual(m_cTransform.c, cTransform.c)
        && IsEqual(m_cTransform.d, cTransform.d)
        && IsEqual(m_cTransform.tx, cTransform.tx)
        && IsEqual(m_cTransform.ty, cTransform.ty)
        && IsEqual(m_cFrame.origin.x, cFrame.origin.x)
        && IsEqual(m_cFrame.origin.y, cFrame.origin.y)
        && IsEqual(m_cFrame.size.width, cFrame.size.width)
        && IsEqual(m_cFrame.size.height, cFrame.size.height)
        && m_eOrientation == [[UIDevice currentDevice] orientation]
        && m_eMode == m_pUIViewSet.contentMode) {
        return false;
    }
    
    return true;
}

int voGLRenderBase::SetView(UIView* view)
{
    bool bReInitVideoView = false;
    
    {
        voCAutoLock cAuto(m_pLock);
        
        if (view != m_pUIViewSet) {
            bReInitVideoView = true;
        }
        // return if view not be changed
        else if (!IsViewChanged()) {
            return VO_GL_RET_OK;
        }
        
        m_pUIViewSet = view;
        
        m_eOrientation = [[UIDevice currentDevice] orientation];
        m_cTransform = [m_pUIViewSet transform];
        m_cFrame = [m_pUIViewSet frame];
        m_eMode = m_pUIViewSet.contentMode;
        
        VOLOGI("view:%d, a:%f,b:%f,c:%f,d:%f,tx:%f,ty:%f\n", (int )view, m_cTransform.a, m_cTransform.b, m_cTransform.c, m_cTransform.d, m_cTransform.tx, m_cTransform.ty);
    }
    
    if ([NSRunLoop mainRunLoop] == [NSRunLoop currentRunLoop]) {
        voCAutoLock cAuto(m_pLock);
        initVideoView(bReInitVideoView);
        RefreshView();
    }
    else {
        
        PostRunOnMainRequest(false, bReInitVideoView, NULL, NULL);
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderBase::initVideoView(bool bReInit)
{
    voCAutoLock cAuto(m_pLock);
    
    if (bReInit) {
        unInitVideoView();
    }
    
    if (nil == m_pVideoView) {
        
        CGRect cRect = m_pUIViewSet.bounds;

        m_pVideoView = [[voVideoView alloc] initWithFrame:cRect];

        [m_pVideoView setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];
        
        [m_pUIViewSet insertSubview:m_pVideoView atIndex:0];
    }
    
    if (UIViewContentModeScaleToFill == m_pUIViewSet.contentMode) {
        if (m_pVideoView.bounds.size.width > m_pVideoView.bounds.size.height) {
            m_pVideoView.contentMode = UIViewContentModeScaleAspectFit;
        }
        else {
            m_pVideoView.contentMode = UIViewContentModeScaleAspectFill;
        }
    }
    else {
        m_pVideoView.contentMode = m_pUIViewSet.contentMode;
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderBase::unInitVideoView()
{
    if (nil != m_pVideoView) {
        [m_pVideoView removeFromSuperview];
        [m_pVideoView release];
        m_pVideoView = NULL;
    }
    
    return VO_GL_RET_OK;
}

void voGLRenderBase::RunningRequestOnMain(int nID, void *pParam1, void *pParam2)
{
    voCAutoLock cAuto(m_pLock);
    
    initVideoView(nID);
    RefreshView();
}

int voGLRenderBase::SetRGBType(int type)
{
    return VO_GL_RET_IMPLEMENT;
}

int voGLRenderBase::RenderYUV(VO_VIDEO_BUFFER *pVideoBuffer)
{
    return VO_GL_RET_IMPLEMENT;
}

unsigned char* voGLRenderBase::GetFrameData()
{
    return NULL;
}

int voGLRenderBase::GetTextureWidth()
{
	return m_nTextureWidth;
}

int voGLRenderBase::GetTextureHeight()
{
	return m_nTextureHeight;
}

int voGLRenderBase::RenderFrameData()
{
    return VO_GL_RET_IMPLEMENT;
}

int voGLRenderBase::GetSupportType()
{
    return VO_GL_SUPPORT_NONE;
}

bool voGLRenderBase::IsGLRenderReady() {
    return false;
}

int voGLRenderBase::GetLastFrame(VO_IMAGE_DATA *pData)
{
    return VO_GL_RET_IMPLEMENT;
}
