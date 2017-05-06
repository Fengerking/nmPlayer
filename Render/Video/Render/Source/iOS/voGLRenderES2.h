/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voGLRenderES2
 
 Contains:    VisualOn OpenGL ES2 Render header file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-14   Jeff            Create file
 *******************************************************************************/

#ifndef __VO_GLRENDER_ES2__
#define __VO_GLRENDER_ES2__

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES2/glext.h>
#import "voGLRenderBase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voGLRenderES2 : public voGLRenderBase {
    
    friend class voGLRenderFactory;
    
public:
    // should add lock
    virtual int SetRotation(VO_GL_ROTATION eType);
    
    virtual int SetTexture(int nWidth, int nHeight);
    virtual int SetOutputRect(int nLeft, int nTop, int nWidth, int nHeight);

    virtual int ClearGL();
    
    virtual int Redraw();
    virtual int RenderYUV(VO_VIDEO_BUFFER *pVideoBuffer);
    
    virtual int GetSupportType();
    
    virtual int GetLastFrame(VO_IMAGE_DATA *pData);
    
protected:
    // should add lock
    virtual int RefreshView();
    
protected:
    voGLRenderES2(EAGLContext* pContext);
    virtual ~voGLRenderES2();
    
    virtual int init();
    
    virtual int SetupGLDisplayRect();
    virtual int SetupGlViewport();
    virtual int SetupRenderBuffer();
    virtual int SetupFrameBuffer();
    virtual int SetupTexture();
    
    virtual int TextureSizeChange();
    
    virtual int DeleteRenderBuffer();
    virtual int DeleteFrameBuffer();
    virtual int DeleteTexture();
    
    virtual int GLTexImage2D(GLuint nTexture, Byte *pDate, int nWidth, int nHeight);

    virtual int CompileAllShaders();
    virtual GLuint CompileShader(const GLchar *pBuffer, GLenum shaderType);
    
    virtual int UploadTexture(VO_VIDEO_BUFFER *pVideoBuffer);
    virtual int RenderToScreen();
    virtual int RenderCommit();
    
    virtual int RedrawInner(bool bIsTryGetFrame, VO_IMAGE_DATA *pData);
    virtual int GetLastFrameInner(bool bIsTryGetFrame, VO_IMAGE_DATA *pData);
    
    virtual bool IsGLRenderReady();
    
protected:
    int m_nPositionSlot;
    int m_nTexCoordSlot;
    
    GLuint m_nProgramHandle;
    
    GLfloat m_fTextureVertices[8];
    GLfloat m_fSquareVertices[8];
    
private:
    bool m_bInitSuccess;
    
    int m_nOutLeft;
    int m_nOutTop;
    int m_nOutWidth;
    int m_nOutHeight;
    
    int m_nRotation;
    
    GLuint m_nColorRenderBuffer;
    GLuint m_nFrameBuffer;
    
    GLuint m_nTexturePlanarY;
    GLuint m_nTexturePlanarU;
    GLuint m_nTexturePlanarV;
    
    GLint m_nTextureUniformY;
    GLint m_nTextureUniformU;
    GLint m_nTextureUniformV;
    
    unsigned char* m_pFrameData;
    
    GLubyte* m_pLastGetFrame;
    
#ifdef _VOLOG_INFO
    CFAbsoluteTime m_fTimeAllUsed;
    unsigned int m_nTimeCount;
#endif
};

#ifdef _VONAMESPACE
}
#endif

#endif

