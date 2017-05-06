/************************************************************************
 *                                                                      *
 *        VisualOn, Inc. Confidential and Proprietary, 2003-            *
 *                                                                      *
 ************************************************************************/
/*******************************************************************************
 File:        voGLRenderES2_FTU
 
 Contains:    VisualOn OpenGL ES2 Fast Texture upload Render header file
 
 Written by:  Jeff
 
 Change History (most recent first):
 2012-09-14   Jeff            Create file
 *******************************************************************************/

#ifndef __VO_GLRENDER_ES2_FTU_
#define __VO_GLRENDER_ES2_FTU_

#import "voGLRenderES2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voGLRenderES2_FTU : public voGLRenderES2 {
    
    friend class voGLRenderFactory;
    
protected:
    voGLRenderES2_FTU(EAGLContext* pContext);
    virtual ~voGLRenderES2_FTU();
    
    virtual int SetupFrameBuffer();
    virtual int DeleteFrameBuffer();
    
    virtual int SetupTexture();
    virtual int DeleteTexture();
    
    virtual int TextureSizeChange();
    
    virtual int CompileAllShaders();
    
    virtual int UploadTexture(VO_VIDEO_BUFFER *pVideoBuffer);
    virtual int RenderToScreen();
    
    virtual bool IsGLRenderReady();
    
    virtual int RedrawInner(bool bIsTryGetFrame, VO_IMAGE_DATA *pData);
    
private:
    CVOpenGLESTextureRef _lumaTexture;
    CVOpenGLESTextureRef _chromaTexture;
	CVOpenGLESTextureCacheRef videoTextureCache;
    
    GLint m_nTextureUniformY;
    GLint m_nTextureUniformUV;
    
    CVImageBufferRef pixelBuffer;
};

#ifdef _VONAMESPACE
}
#endif

#endif

