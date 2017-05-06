/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voGLRenderBase
 
 Contains:    VisualOn OpenGL ES Base Render header file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-17   Jeff            Create file
 *******************************************************************************/

#ifndef __VO_GLRENDER_BASE__
#define __VO_GLRENDER_BASE__

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import "voIVCommon.h"
#import "voCMutex.h"
#import "voRunRequestOnMain.h"
#import "voVideoView.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

static const int VO_GL_RET_OK         = 0;
static const int VO_GL_RET_FAIL       = 1;
static const int VO_GL_RET_IMPLEMENT  = 2;
static const int VO_GL_RET_RETRY      = 3;
static const int VO_GL_RET_ERR_STATUS = 4;

static const int VO_GL_SUPPORT_NONE   = 0;
static const int VO_GL_SUPPORT_RGB    = 0x1;
static const int VO_GL_SUPPORT_Y_U_V  = 0x1 << 1;
static const int VO_GL_SUPPORT_Y_UV   = 0x1 << 2;
    
typedef enum
{
	VO_GL_ROTATION_0,
    VO_GL_ROTATION_0FLIP,
    VO_GL_ROTATION_180,
    VO_GL_ROTATION_180FLIP,
}
VO_GL_ROTATION;

typedef enum
{
    VO_GL_COLOR_RGBA8,
    VO_GL_COLOR_RGB565,
}
VO_GL_COLOR;


// Effective C++ 42: Use private inheritance judiciously
class voGLRenderBase :private voRunRequestOnMain {
        
    friend class voGLRenderFactory;
        
public:
    
    EAGLContext *GetGLContext();
    int SetLock(voCMutex *pLock);
    
    virtual int SetView(UIView *view);
    
    virtual int SetRGBType(int type);
    
    virtual int SetRotation(VO_GL_ROTATION eType) = 0;
    
    virtual int SetTexture(int nWidth, int nHeight) = 0;
    virtual int SetOutputRect(int nLeft, int nTop, int nWidth, int nHeight) = 0;
    
    virtual int ClearGL() = 0;
    
    virtual int Redraw() = 0;
    virtual int RenderYUV(VO_VIDEO_BUFFER *pVideoBuffer);

    virtual int RenderFrameData();
    virtual int GetTextureWidth();
    virtual int GetTextureHeight();
    virtual unsigned char* GetFrameData();
    
    virtual int GetSupportType();
    
    virtual int GetLastFrame(VO_IMAGE_DATA *pData);
    
protected:
    
    voGLRenderBase(EAGLContext* pContext);
    virtual ~voGLRenderBase();
    
    virtual int init();
    
    virtual int unInitVideoView();
    virtual int initVideoView(bool bReInit);
    virtual int RefreshView() = 0;
    
    virtual bool IsViewChanged();
    
    virtual bool IsGLRenderReady();
    
    bool IsEqual(float a, float b);
    void Swap(float *a, float *b);
    
private:
    virtual void RunningRequestOnMain(int nID, void *pParam1, void *pParam2);
    
protected:
    UIDeviceOrientation m_eOrientation;
    UIViewContentMode m_eMode;
    
    EAGLContext *m_pContext;
    UIView *m_pUIViewSet;
    voVideoView *m_pVideoView;
    voCMutex *m_pLock;
    
    /* Dimensions of the backing buffer */
	int m_nBackingWidth;
    int m_nBackingHeight;
    
    /* Dimensions of the texture in pixels */
	int m_nTextureWidth;
	int m_nTextureHeight;
    
    CGAffineTransform m_cTransform;
    CGRect            m_cFrame;
};

#ifdef _VONAMESPACE
}
#endif

#endif

