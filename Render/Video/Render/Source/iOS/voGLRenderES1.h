//
//  GLRender.h
//  voCCRRR
//
//  Created by Jason Gao on 5/25/09.
//  Copyright 2009 VisualOn Inc. All rights reserved.
//

#ifndef __VO_GLRENDER_EXT__
#define __VO_GLRENDER_EXT__

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/ES1/glext.h>
#import "voGLRenderBase.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voGLRenderES1 : public voGLRenderBase {
    
    friend class voGLRenderFactory;
    
public:
    virtual int SetRGBType(int type);
    virtual int SetRotation(VO_GL_ROTATION eType);
    
    virtual int SetTexture(int nWidth, int nHeight);
    virtual int SetOutputRect(int nLeft, int nTop, int nWidth, int nHeight);
    
    virtual int ClearGL();
    
    virtual int Redraw();
    
    virtual int RenderFrameData();
    virtual unsigned char* GetFrameData();
    
    virtual int GetSupportType();
    
protected:
    
    virtual int RefreshView();
    
    void InitGL2D();
    
    void SetupView();
    
    void ReInitGLView();
    void CreateFramebuffer();
	void DestroyFramebuffer();
    void CreateTexture();
    void DestroyTexture();
    
    void UpdateSettings();
    
protected:
    voGLRenderES1(EAGLContext* pContext);
    virtual ~voGLRenderES1();
    
private:
    void DeleteBuffer();
    void RenewBuffer();
    void UpdatePosition();
    
    static int GetTextureSize(int size);
    
private:
	
	/* OpenGL ES names for m_nRenderBuffer and framebuffer */
	GLuint m_nRenderBuffer;
    GLuint m_nFrameBuffer;
    
	/* Texture "name" (i.e. OpenGL ES handles) */
	unsigned int m_nFrameTexture;
    
	int m_nInputWidth;
	int m_nInputHeight;
	int m_nOutputWidth;
	int m_nOutputHeight;
	int m_nRotation;
	CGPoint m_cAdjustPoint;
	
	/* Buffer with frame data to be rendered, RGBA or RGB565(simulator) format. */
	unsigned char* m_pFrameData;
	
	GLfloat m_fCoordinates[8]; 
	GLfloat m_fVertices[8];
	
	int m_nRGBType;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif
