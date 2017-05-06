//
//  voGLRenderES1.m
//  
//
//  Created by Jason Gao on 5/25/09.
//  Copyright 2009 VisualOn Inc. All rights reserved.
//

#import "voGLRenderES1.h"
#import <TargetConditionals.h>
#include <sys/stat.h>

#import "voOSFunc.h"
#import "voLog.h"
#import "voIndex.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

/*-------------------------------------------------------------------------------------*
 * GLRender implementation
 *-------------------------------------------------------------------------------------*/
voGLRenderES1::voGLRenderES1(EAGLContext* pContext)
:voGLRenderBase(pContext)
,m_nInputWidth(0)
,m_nInputHeight(0)
,m_nOutputWidth(0)
,m_nOutputHeight(0)
,m_nRotation(VO_GL_ROTATION_0)
,m_nRenderBuffer(0)
,m_nFrameBuffer(0)
,m_nFrameTexture(0)
,m_nRGBType(VO_GL_COLOR_RGB565)
{
    m_cAdjustPoint.x = 0;
	m_cAdjustPoint.y = 0;
    
    m_pFrameData = NULL;
    
    memset(m_fCoordinates, 0, sizeof(m_fCoordinates));
    memset(m_fVertices, 0, sizeof(m_fVertices));
    
    VOLOGI("alloc EAGLContext:%d\n", (int)m_pContext);
    
    if (!m_pContext || ![EAGLContext setCurrentContext: m_pContext])
    {
        VOLOGE("Init EAGLContext fail\n");
    }
    
    InitGL2D();
}

voGLRenderES1::~voGLRenderES1()
{
    DeleteBuffer();
    
    DestroyTexture();
    DestroyFramebuffer();
}

#pragma mark Public function.
int voGLRenderES1::SetRGBType(int type)
{
    voCAutoLock cAuto(m_pLock);
    
    if (type != m_nRGBType) {
        m_nRGBType = type;
        ReInitGLView();
        RenewBuffer();
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES1::SetRotation(VO_GL_ROTATION eType)
{
    voCAutoLock cAuto(m_pLock);
    
    if (m_nRotation != eType) {
        m_nRotation = eType;
        UpdatePosition();
    }
    
    return VO_GL_RET_OK;
}

int voGLRenderES1::SetTexture(int nWidth, int nHeight)
{
    voCAutoLock cAuto(m_pLock);
    
    m_nInputWidth = nWidth;
    m_nInputHeight = nHeight;
    
    UpdateSettings();
    return VO_GL_RET_OK;
}

int voGLRenderES1::SetOutputRect(int nLeft, int nTop, int nWidth, int nHeight)
{
    voCAutoLock cAuto(m_pLock);
    
    m_cAdjustPoint.x = nLeft;
	m_cAdjustPoint.y = nTop;
    m_nOutputWidth = nWidth;
    m_nOutputHeight = nHeight;
    UpdateSettings();
    
    return VO_GL_RET_OK;
}

int voGLRenderES1::ClearGL()
{
    voCAutoLock cAuto(m_pLock);
    
    if (NULL == m_pContext || ![EAGLContext setCurrentContext:m_pContext]) {
        return VO_GL_RET_FAIL;
    }
    
    /* Set the clear color to 100% opaque black */
	//glClearColor(250.0, 250.0, 250.0, 1.0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
    /* Clear the renderbuffer */
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	
	if (nil == m_pContext) {
		return VO_GL_RET_FAIL;
	}
	
	/* Refresh the screen */
	[m_pContext presentRenderbuffer: GL_RENDERBUFFER_OES];
    
    return VO_GL_RET_OK;
}

int voGLRenderES1::Redraw()
{
    return RenderFrameData();
}

int voGLRenderES1::RenderFrameData()
{
    voCAutoLock cAuto(m_pLock);
    
	if ((nil == m_pContext)
        || (nil == m_pVideoView)) {
		return VO_GL_RET_FAIL;
	}
    
	/* Ensure our OpenGL context is the current one */
	[EAGLContext setCurrentContext: m_pContext];
	
	// tag: 20101213 close it, no need to call it.
	// 20110425, reopen it.There is issue on iPhone 3GS(OS 3.1.2)
	glClear(GL_COLOR_BUFFER_BIT);
    
    /* Frame data buffer must be allocated before */
	if (NULL == m_pFrameData)
	{
		return VO_GL_RET_FAIL;
	}
    
	if (VO_GL_COLOR_RGB565 == m_nRGBType)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nTextureWidth, m_nTextureHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, m_pFrameData);
	else// RGB32
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_nTextureWidth, m_nTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pFrameData);
	
	glTexCoordPointer(2, GL_FLOAT, 0, m_fCoordinates);
	glVertexPointer(2, GL_FLOAT, 0, m_fVertices);		// 只有x，y坐标，没有z，所以一个点只用2个数字描述
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);			// 画四个点，矩形
    // GL_TRIANGLE_STRIP 模式,使用头两个顶点，然后对于后续顶点，它都将和之前的两个顶点组成一个三角形
    
    /* All rendered, so make render buffer appear on the screen */
	[m_pContext presentRenderbuffer: GL_RENDERBUFFER_OES];
    
    return VO_GL_RET_OK;
}

unsigned char* voGLRenderES1::GetFrameData()
{
    return m_pFrameData;
}

int voGLRenderES1::GetSupportType()
{
    return VO_GL_SUPPORT_RGB;
}

#pragma mark Protect function.
void voGLRenderES1::InitGL2D()
{
	/* Enable texturing, we do not need depth buffer or alpha handling */
    
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDisable(GL_DITHER);
	
	glEnable(GL_TEXTURE_2D);
    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

int voGLRenderES1::RefreshView()
{
    ReInitGLView();
    
    return VO_GL_RET_OK;
}

void voGLRenderES1::ReInitGLView()
{
    if (nil == m_pVideoView) {
        return;
    }
    
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
	//NSLog(@"SetView\n");
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)m_pVideoView.layer;
	eaglLayer.opaque = YES;
    
	if (m_nRGBType == VO_GL_COLOR_RGB565)
	{
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool: NO], kEAGLDrawablePropertyRetainedBacking,
										kEAGLColorFormatRGB565,
										kEAGLDrawablePropertyColorFormat,
										nil];
		
	}
	else//RGB32
	{
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool: NO], kEAGLDrawablePropertyRetainedBacking,
										kEAGLColorFormatRGBA8,
										kEAGLDrawablePropertyColorFormat,
										nil];
		
	}
    
    [pool release];
    
	if (nil == m_pContext) {
		return;
	}
	[EAGLContext setCurrentContext: m_pContext];
	
	DestroyFramebuffer();
	CreateFramebuffer();
    
	SetupView();
    
    DestroyTexture();
    CreateTexture();
}

void voGLRenderES1::SetupView()
{
	/* Extract renderbuffer's width and height. This should match layer's size, I assume */
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &m_nBackingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &m_nBackingHeight);
	
	/* Set viewport size to match the renderbuffer size */
	int glX = 0;
	int glY = 0;
	glViewport(glX, glY, m_nBackingWidth, m_nBackingHeight);
	
#ifdef _VOLOG_INFO
	//NSLog(@"GL: glX = %d, glY = %d, m_nBackingWidth = %d, m_nBackingHeight = %d \n\n", glX, glY, m_nBackingWidth, m_nBackingHeight);
#endif
	
	/* Setup orthographic projection mode */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrthof(-m_nBackingWidth, m_nBackingWidth, -m_nBackingHeight, m_nBackingHeight, -1.0, 1.0);
	glOrthof(0, m_nBackingWidth, 0, m_nBackingHeight, -1.0, 0);
	////NSLog(@"GL: glX = %d, glY = %d, m_nBackingWidth = %d, m_nBackingHeight = %d \n\n", -m_nBackingWidth, m_nBackingWidth, -m_nBackingHeight, m_nBackingHeight);
	
	/* Reset the matrix mode to model/view */
	glMatrixMode(GL_MODELVIEW);
	
	ClearGL();
}

void voGLRenderES1::CreateFramebuffer()
{
	if (0 != m_nFrameBuffer)
	{
		return;
	}
	
	VOLOGI("voGLRenderManager CreateFramebuffer\n");
	
    /* Generate handles for 1 view- and 1 m_nRenderBuffer. */
	glGenFramebuffersOES(1, &m_nFrameBuffer);
	glGenRenderbuffersOES(1, &m_nRenderBuffer);
	
	/* Bind (i.e. select) frame and render buffers. */
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_nFrameBuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_nRenderBuffer);
	
	if (nil == m_pContext) {
		return;
	}
	/* This call is equiv. of glRenderbufferStorage, that binds renderbuffer to layer's framebuffer */
	[m_pContext renderbufferStorage: GL_RENDERBUFFER_OES fromDrawable: (CAEAGLLayer *)m_pVideoView.layer];
	
	/* Attach a renderbuffer to a framebuffer, color buffer */
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, m_nRenderBuffer);
	
	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		VOLOGE("Framebuffer creation failed: %x\n", glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return;
	}
}

void voGLRenderES1::DestroyFramebuffer()
{
	if (m_nFrameBuffer)
	{
		glDeleteFramebuffersOES(1, &m_nFrameBuffer);
		m_nFrameBuffer = 0;
	}
	if (m_nRenderBuffer)
	{
		glDeleteRenderbuffersOES(1, &m_nRenderBuffer);
		m_nRenderBuffer = 0;
	}
}

void voGLRenderES1::CreateTexture()
{
	glGenTextures(1, (GLuint*)&m_nFrameTexture);
	glBindTexture(GL_TEXTURE_2D, m_nFrameTexture);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST
}

void voGLRenderES1::DestroyTexture()
{
	if (m_nFrameTexture)
	{
		glDeleteTextures(1, &m_nFrameTexture);
		m_nFrameTexture = 0;
	}
}

int voGLRenderES1::GetTextureSize(int size)
{
	int outSize = 64;
	while (outSize < size)
		outSize <<= 1;
	return outSize;
}

void voGLRenderES1::UpdateSettings()
{
    if ((0 == m_nInputWidth) || (0 == m_nInputHeight)) {
        return;
    }
    
	int width	= GetTextureSize(m_nInputWidth);
	int height	= GetTextureSize(m_nInputHeight);
	
	bool changed = (m_nTextureWidth!=width || m_nTextureHeight!=height);
	
	if (changed)
	{
		m_nTextureWidth	= width;
		m_nTextureHeight	= height;
        RenewBuffer();
	}
	
	UpdatePosition();
}

/*-------------------------------------------------------------------------------------*
 * Internal methods
 *-------------------------------------------------------------------------------------*/
void voGLRenderES1::DeleteBuffer()
{
    if (m_pFrameData)
	{
		free(m_pFrameData);
		m_pFrameData = NULL;
	}
}

void voGLRenderES1::RenewBuffer()
{
    DeleteBuffer();
    
    if ((0 == m_nTextureWidth) || (0 == m_nTextureHeight)) {
        return;
    }
    
    int iStride = 2;
    if (m_nRGBType == VO_GL_COLOR_RGBA8) {
        iStride = 4;
    }
    
    m_pFrameData = (unsigned char* )malloc(m_nTextureWidth * m_nTextureHeight * iStride);
}

void voGLRenderES1::UpdatePosition()
{
	CGPoint point = {m_cAdjustPoint.x, m_cAdjustPoint.y};
	
	GLfloat maxS = (GLfloat)m_nInputWidth / m_nTextureWidth;
	GLfloat maxT = (GLfloat)m_nInputHeight / m_nTextureHeight;
	
	GLfloat *pc = m_fCoordinates;
	GLfloat *pv = m_fVertices;
	
	//m_nOutputWidth = m_nOutputWidth / 2;
	//m_nOutputHeight = m_nOutputHeight / 2;
	
	if (m_nRotation == VO_GL_ROTATION_0 ||
		m_nRotation == VO_GL_ROTATION_180 ||
		m_nRotation == VO_GL_ROTATION_0FLIP ||
		m_nRotation == VO_GL_ROTATION_180FLIP)
	{
		////NSLog(@"m_fVertices rotate 0\n");
		*pv++ = 0 + point.x;				// bottom left
		*pv++ = 0 + point.y;
		
		*pv++ = m_nOutputWidth + point.x;		// bottom right
		*pv++ = 0 + point.y;
		
		*pv++ = 0 + point.x;				// top left
		*pv++ = m_nOutputHeight + point.y;
		
		*pv++ = m_nOutputWidth + point.x;		// top right
		*pv++ = m_nOutputHeight + point.y;
	}
	else
	{
		////NSLog(@"m_fVertices not rotate 0\n");
		*pv++ = -m_nOutputHeight + point.x;
		*pv++ = -m_nOutputWidth + point.y;
		*pv++ = m_nOutputHeight + point.x;
		*pv++ = -m_nOutputWidth + point.y;
		*pv++ = -m_nOutputHeight + point.x;
		*pv++ = m_nOutputWidth + point.y;
		*pv++ = m_nOutputHeight + point.x;
		*pv++ = m_nOutputWidth + point.y;
	}
	
	//纹理的长宽范围为0～1
	switch (m_nRotation)
	{
		case VO_GL_ROTATION_0:  //3412
			////NSLog(@"m_fCoordinates VO_GL_ROTATION_0 0\n");
			*pc++ = 0;
			*pc++ = maxT;
			*pc++ = maxS;
			*pc++ = maxT;
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = maxS;
			*pc++ = 0;
			break;
            
		case VO_GL_ROTATION_180:  //2143
			////NSLog(@"m_fCoordinates VO_GL_ROTATION_180 0\n");
			*pc++ = maxS;
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = maxS;
			*pc++ = maxT;
			*pc++ = 0;
			*pc++ = maxT;
			break;
			
//		case ROTATION_90:  //4231
//			////NSLog(@"m_fCoordinates ROTATION_90 0\n");
//			*pc++ = maxS;
//			*pc++ = maxT;
//			*pc++ = maxS;
//			*pc++ = 0;
//			*pc++ = 0;
//			*pc++ = maxT;
//			*pc++ = 0;
//			*pc++ = 0;
//			break;
//			
//		case ROTATION_270:  //1324
//			////NSLog(@"m_fCoordinates ROTATION_270 0\n");
//			*pc++ = 0;
//			*pc++ = 0;
//			*pc++ = 0;
//			*pc++ = maxT;
//			*pc++ = maxS;
//			*pc++ = 0;
//			*pc++ = maxS;
//			*pc++ = maxT;
//			break;
			
		case VO_GL_ROTATION_0FLIP:  //4321
			////NSLog(@"m_fCoordinates VO_GL_ROTATION_0FLIP 0\n");
			*pc++ = maxS;
			*pc++ = maxT;
			*pc++ = 0;
			*pc++ = maxT;
			*pc++ = maxS;
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = 0;
			break;
			
		case VO_GL_ROTATION_180FLIP:  //1234
			////NSLog(@"m_fCoordinates VO_GL_ROTATION_180FLIP 0\n");
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = maxS;
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = maxT;
			*pc++ = maxS;
			*pc++ = maxT;
			break;
			
//		case ROTATION_90FLIP:  //3142
//			////NSLog(@"m_fCoordinates ROTATION_90FLIP 0\n");
//			*pc++ = 0;
//			*pc++ = maxT;
//			*pc++ = 0;
//			*pc++ = 0;
//			*pc++ = maxS;
//			*pc++ = maxT;
//			*pc++ = maxS;
//			*pc++ = 0;
//			break;
//			
//		case ROTATION_270FLIP: //2413
//			////NSLog(@"m_fCoordinates ROTATION_270FLIP 0\n");
//			*pc++ = maxS;
//			*pc++ = 0;
//			*pc++ = maxS;
//			*pc++ = maxT;
//			*pc++ = 0;
//			*pc++ = 0;
//			*pc++ = 0;
//			*pc++ = maxT;
//			break;
			
		default: // == case ROTATION_90:  //4231
			////NSLog(@"m_fCoordinates default 0\n");
			*pc++ = maxS;
			*pc++ = maxT;
			*pc++ = maxS;
			*pc++ = 0;
			*pc++ = 0;
			*pc++ = maxT;
			*pc++ = 0;
			*pc++ = 0;
			break;
	}
	
#if 0
	//NSLog(@"RenderBuffer_Width = %d, RenderBuffer_Height = %d  \n", m_nBackingWidth, m_nBackingHeight);
	
	//NSLog(@"CCRR: m_nInputWidth = %d, m_nInputHeight = %d, m_nOutputWidth = %d, m_nOutputHeight = %d, m_nTextureWidth = %d, m_nTextureHeight = %d \n\n\n",
    m_nInputWidth, m_nInputHeight, m_nOutputWidth, m_nOutputHeight, m_nTextureWidth, m_nTextureHeight);
	
	//NSLog(@"GL: maxS = %f, maxT = %f, \n\n", maxS, maxT);
	
	//NSLog(@"m_fVertices: %f, %f, %f, %f, %f, %f, %f, %f \n\n",
    m_fVertices[0], m_fVertices[1], m_fVertices[2], m_fVertices[3], m_fVertices[4], m_fVertices[5], m_fVertices[6], m_fVertices[7]);
	
	//NSLog(@"m_fCoordinates: %f, %f, %f, %f, %f, %f, %f, %f \n\n",
    m_fCoordinates[0], m_fCoordinates[1], m_fCoordinates[2], m_fCoordinates[3], m_fCoordinates[4], m_fCoordinates[5], m_fCoordinates[6], m_fCoordinates[7]);
#endif
    
}
